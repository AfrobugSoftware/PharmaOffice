#include "PofPch.h"
#include "PatientManager.h"

pof::PatientManager::PatientManager()
{
	mPaitnets = std::make_unique<pof::DataModel>();
	mPatientHistory = std::make_unique<pof::DataModel>();
	mPatientMedications = std::make_unique<pof::DataModel>();

	mPaitnets->Adapt<
		pof::base::data::duuid_t, //UUID
		pof::base::data::text_t, //NAME
		pof::base::data::text_t, //LAST NAME
		pof::base::data::text_t, // DOB
		pof::base::data::text_t,
		pof::base::data::text_t, //PHONE NUMBER
		pof::base::data::text_t, // ADDRESS
		std::uint64_t, // BMI
		std::uint64_t, // WEIGHT
		std::uint64_t, //HR
		std::uint64_t, // BP SYS
		std::uint64_t, // BP DYS
		std::uint64_t, // RR
		std::uint64_t, // TEMPT
		pof::base::data::datetime_t, // ENTERED DATE
		pof::base::data::datetime_t,  // MODIFIED DATE
		pof::base::data::text_t
	>();

	mPatientMedications->Adapt<
		pof::base::data::duuid_t,
		pof::base::data::duuid_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		std::uint64_t,
		std::uint64_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::datetime_t,
		pof::base::data::datetime_t
	>();

	mPatientHistory->Adapt<
		pof::base::data::duuid_t,
		pof::base::data::duuid_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		std::uint64_t,
		std::uint64_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::datetime_t,
		pof::base::data::datetime_t
	>();

	//set up crud slots
	mPatientMedications->ConnectSlot(std::bind_front(&pof::PatientManager::OnAddMedication, this), pof::DataModel::Signals::STORE);
	mPatientMedications->ConnectSlot(std::bind_front(&pof::PatientManager::OnRemoveMedication, this), pof::DataModel::Signals::REMOVED);
	mPatientMedications->ConnectSlot(std::bind_front(&pof::PatientManager::OnUpdateMedication, this), pof::DataModel::Signals::UPDATE);

	//curd slots
	mPaitnets->ConnectSlot(std::bind_front(&pof::PatientManager::OnAddPatient, this), pof::DataModel::Signals::STORE);
	mPaitnets->ConnectSlot(std::bind_front(&pof::PatientManager::OnRemovePatient, this), pof::DataModel::Signals::REMOVED);
	mPaitnets->ConnectSlot(std::bind_front(&pof::PatientManager::OnUpdatePatient, this), pof::DataModel::Signals::UPDATE);

}

pof::PatientManager::~PatientManager()
{
}

bool pof::PatientManager::CreatePatientTable() {
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS patients 
		(
			uuid blob,
			name text,
			lastname text,
			dob integer,
			gender text,
			phonenumber text,
			address text,
			bmi integer,
			weight integer,
			hr integer,
			bpsys integer,
			bpdys integer,
			rr integer,
			tempreture integer,
			createdate integer,
			modifieddate integer,
			clinical text	
		);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::CreatePatientMedicationTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS medications
		(
			product_uuid blob,
			patient_uuid blob,
			purpose text,
			outcome text,
			stock integer,
			dirforusequan integer,
			dirforusesten text,
			duration integer,
			startdate integer,
			stopdate integer
		);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::CreatePatientAddInfoTable()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS patient_addinfo (puid blob unique, info text);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::CreateDatabaseFunctions()
{
	if (mLocalDatabase)
	{
		pof::base::func_aggregate isRemindAgg;
		isRemindAgg.name = "CheckReminded"s;
		isRemindAgg.arg_count = 1;
		isRemindAgg.func = &pof::PatientManager::DBFuncISReminded;


		pof::base::func_aggregate AddDayDuration;
		AddDayDuration.name = "AddDays"s;
		AddDayDuration.arg_count = 2;
		AddDayDuration.func = &pof::PatientManager::DBFuncAddDuration;


		pof::base::func_aggregate isPinned;
		isPinned.name = "IsPinned"s;
		isPinned.arg_count = 1;
		isPinned.func = &pof::PatientManager::DBFuncIsPinned;

		pof::base::func_aggregate InSale;
		InSale.name = "InSale"s;
		InSale.arg_count = 2;
		InSale.func = &pof::PatientManager::DBFuncInSale;

		pof::base::func_aggregate getDob;
		getDob.name = "GetDob"s;
		getDob.arg_count = 1;
		getDob.func = &pof::PatientManager::DBFuncGetDateOfBirth;

		mLocalDatabase->register_func(std::move(isRemindAgg));
		mLocalDatabase->register_func(std::move(AddDayDuration));
		mLocalDatabase->register_func(std::move(InSale));
		mLocalDatabase->register_func(std::move(isPinned));
		mLocalDatabase->register_func(std::move(getDob));
	}
	return false;
}

std::unique_ptr<pof::DataModel>& pof::PatientManager::GetPatientData()
{
	return mPaitnets;
}

std::unique_ptr<pof::DataModel>& pof::PatientManager::GetPatientMedData()
{
	return mPatientMedications;
}

std::unique_ptr<pof::DataModel>& pof::PatientManager::GetPatientHistotyData()
{
	return mPatientHistory;
}

bool pof::PatientManager::LoadPatients()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT 
		p.uuid, p.name, p.lastname, GetDob(pi.info), p.gender, p.phonenumber, p.address, p.bmi, p.weight, p.hr, p.bpsys, p.bpdys, p.rr, p.tempreture, p.createdate, p.modifieddate, p.clinical 
		FROM patients p, patient_addinfo pi WHERE p.uuid = pi.puid ORDER BY p.lastname ASC LIMIT 1000;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t, //UUID
			pof::base::data::text_t, //NAME
			pof::base::data::text_t, //LAST NAME
			pof::base::data::text_t, // AGE
			pof::base::data::text_t, //GENDER
			pof::base::data::text_t, //PHONE NUMBER
			pof::base::data::text_t, // ADDRESS
			std::uint64_t, // BMI
			std::uint64_t, // WEIGHT
			std::uint64_t, //HR
			std::uint64_t, // BP_sys
			std::uint64_t, // BP_dys
			std::uint64_t, // RR
			std::uint64_t, // TEMPT
			pof::base::data::datetime_t, // ENTERED DATE
			pof::base::data::datetime_t,  // MODIFIED DATE
			pof::base::data::text_t //clincial indication
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		mPaitnets->Clear();
		for (auto& tup : rel.value()) {
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(tup));
			mPaitnets->EmplaceData(std::move(row));
		}
		
		return true;
	}
	return false;
}

bool pof::PatientManager::LoadPatientMedication(const pof::base::data::duuid_t& pid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT 
	pm.product_uuid,
	pm.patient_uuid,
	p.name,
	pm.purpose,
	pm.outcome,
	pm.stock,
	pm.dirforusequan,
	pm.dirforusesten,
	pm.duration,
	pm.startdate,
	pm.stopdate 
	FROM medications pm, products p 
	WHERE (Days(pm.stopdate) > ? OR Days(pm.stopdate) = ?) AND pm.patient_uuid = ? AND p.uuid = pm.product_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto dayAhead = date::floor<date::days>(pof::base::data::clock_t::now());
		auto day = static_cast<std::uint64_t>((dayAhead).time_since_epoch().count());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(day, day, pid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::datetime_t,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mPatientMedications->Clear();
		mLocalDatabase->finalise(*stmt);
		for (auto& tup : rel.value()) {
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(std::move(tup)));
			mPatientMedications->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

bool pof::PatientManager::LoadPatientHistory(const pof::base::data::duuid_t& pid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT 
	pm.product_uuid,
	pm.patient_uuid,
	p.name,
	pm.purpose,
	pm.outcome,
	pm.stock,
	pm.dirforusequan,
	pm.dirforusesten,
	pm.duration,
	pm.startdate,
	pm.stopdate 
	FROM medications pm, products p 
	WHERE Days(pm.stopdate) < ? AND pm.patient_uuid = ? AND p.uuid = pm.product_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

		auto dayAhead = date::floor<date::days>(pof::base::data::clock_t::now());
		auto day = (dayAhead).time_since_epoch().count();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(day), pid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::datetime_t,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		mPatientHistory->Clear();
		for (auto& tup : rel.value()) {
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(std::move(tup)));
			mPatientHistory->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

bool pof::PatientManager::IsPatientActive(const pof::base::data::duuid_t& pid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT 1 FROM medications WHERE patient_uuid = :uuid AND (Days(stopdate) > :dt OR Days(stopdate) = :dt);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		//on some computers it does not work with the day ahead
		auto dayAhead = date::floor<date::days>(pof::base::data::clock_t::now());
		auto day = (dayAhead).time_since_epoch().count();
		bool status = mLocalDatabase->bind_para(*stmt, std::make_tuple(pid, static_cast<std::uint64_t>(day)), {"uuid", "dt"});
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		return (!rel->empty());
	}
	return false;
}

bool pof::PatientManager::OnAddPatient(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(INSERT INTO patients 
		(uuid, name, lastname, gender, phonenumber, address, bmi, weight, hr, bpsys, bpdys, rr, tempreture, createdate, modifieddate, clinical)	
		VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);)";

		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& v = iter->first;
		//skip age
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(
			boost::variant2::get<pof::base::data::duuid_t>(v[pof::PatientManager::PATIENT_UUID]),
			boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_NAME]),
			boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_LAST_NAME]),
			boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_GENDER]),
			boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_PHONE_NUMBER]),
			boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_ADDRESS]),
			boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_BMI]),
			boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_WEIGHT]),
			boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_HR]),
			boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_BP_SYS]),
			boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_BP_DYS]),
			boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_RR]),
			boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_TEMPT]),
			boost::variant2::get<pof::base::data::datetime_t>(v[pof::PatientManager::PATIENT_ENTERED_DATE]),
			boost::variant2::get<pof::base::data::datetime_t>(v[pof::PatientManager::PATIENT_MODIFIED_DATE]),
			boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_CLINICAL_INDICATION])
		));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::OnRemovePatient(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM patients WHERE uuid = :uuid; DELETE FROM medications WHERE patient_uuid = :uuid;)";
		auto stmtArr = mLocalDatabase->prepare_multiple<2>(sql);
		if (!stmtArr.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[PATIENT_UUID]);
		bool status = mLocalDatabase->bind_para(stmtArr.value()[0], std::make_tuple(uuid), { "uuid" });
		assert(status);
		status = mLocalDatabase->bind_para(stmtArr.value()[1], std::make_tuple(uuid), { "uuid" });
		assert(status);

		status = mLocalDatabase->execute(stmtArr.value()[0])
		&& mLocalDatabase->execute(stmtArr.value()[1]);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(stmtArr.value()[0]);
		mLocalDatabase->finalise(stmtArr.value()[1]);
		return status;
	}
	return false;
}

bool pof::PatientManager::OnUpdatePatient(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr static std::array<std::string_view, PATIENT_MAX> colName = {
			"uuid", "name", "lastname", "dob", "gender" ,"phonenumber", "address", "bmi", "weight", "hr", "bpsys", "bpdys", "rr", "tempreture", "createdate", "modifieddate", "clinical"
		};
		std::ostringstream os;
		std::vector<size_t> upIdx;
		os << "UPDATE patients SET ";
		auto& updateFlag = iter->second.second;
		for (int i = 0; i < PATIENT_MAX; i++) {
			if (updateFlag.test(i)) {
				if (upIdx.size() != 0) {
					os << ", ";
				}
				upIdx.push_back(i);
				os << colName[i] << "= ?";
			}
		}
		os << " WHERE uuid = ?;";
		upIdx.push_back(PATIENT_UUID);

		auto stmt = mLocalDatabase->prepare(os.str());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[PATIENT_UUID]);
		size_t i = 1;
		auto& meta = mPaitnets->GetDatastore().get_metadata();
		auto& v = iter->first;
		for (size_t d : upIdx) {
			auto kind = meta[d];
			switch (kind)
			{
			case pof::base::data::kind::uint64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint64_t>(v[d]), i);
				break;
			case pof::base::data::kind::datetime:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::datetime_t>(v[d]), i);
				break;
			case pof::base::data::kind::text:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::text_t>(v[d]), i);
				break;
			case pof::base::data::kind::blob:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::blob_t>(v[d]), i);
				break;
			case pof::base::data::kind::uuid:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::duuid_t>(v[d]), i);
				break;
			default:
				break;
			}
			i++;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::OnAddMedication(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(INSERT INTO medications VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		//i have to copy it
		auto v = iter->first;
		auto remvIter = std::next(v.begin(), MED_NAME);
		v.erase(remvIter);
		std::tuple<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::datetime_t,
			pof::base::data::datetime_t
		> tup;

		pof::base::data::row_t row;
		row.first = std::move(v);
		bool status = pof::base::build(tup, row);
		assert(status);

		status = mLocalDatabase->bind(*stmt, std::move(tup));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::OnRemoveMedication(pof::base::data::const_iterator iter)
{
	if(mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM medications WHERE patient_uuid = ? AND product_uuid = ? AND Days(stopdate) > ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[MED_PATIENT_UUID]);
		auto& puuid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[MED_PRODUCT_UUID]);
		auto dayAhead = date::floor<date::days>(pof::base::data::clock_t::now());
		auto day = (dayAhead).time_since_epoch().count();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uuid, puuid, static_cast<std::uint64_t>(day)));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::OnUpdateMedication(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr static const std::array<std::string_view, 11> colNames = {
			"product_uuid","patient_uuid", "name", "purpose", "outcome", "stock", "dirforusequan", "dirforusesten", 
			"duration", "startdate", "stopdate"
		};
		std::ostringstream os;
		std::vector<size_t> upIdx;
		os << "UPDATE medications SET ";
		auto& updateFlag = iter->second.second;
		for (int i = 0; i < MED_MAX; i++) {
			if (updateFlag.test(i)) {
				if (upIdx.size() != 0) {
					os << ", ";
				}
				upIdx.push_back(i);
				os << colNames[i] << "= ?";
			}
		}
		os << " WHERE patient_uuid = ? AND product_uuid = ? AND Days(stopdate) > ?;";
		upIdx.push_back(MED_PATIENT_UUID);
		upIdx.push_back(MED_PRODUCT_UUID);
		//upIdx.push_back(MED_STOP_DATE); //only update active products

		auto stmt = mLocalDatabase->prepare(os.str());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		size_t i = 1;
		auto& meta = mPatientMedications->GetDatastore().get_metadata();
		auto& v = iter->first;
		for (size_t d : upIdx) {
			auto kind = meta[d];
			switch (kind)
			{
			case pof::base::data::kind::uint64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint64_t>(v[d]), i);
				break;
			case pof::base::data::kind::datetime:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::datetime_t>(v[d]), i);
				break;
			case pof::base::data::kind::text:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::text_t>(v[d]), i);
				break;
			case pof::base::data::kind::blob:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::blob_t>(v[d]), i);
				break;
			case pof::base::data::kind::uuid:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::duuid_t>(v[d]), i);
				break;
			default:
				break;
			}
			i++;
		}

		//bind today to only work on medication that has not being stopped 
		auto dayAhead = date::floor<date::days>(pof::base::data::clock_t::now());
		auto day = static_cast<std::uint64_t>((dayAhead).time_since_epoch().count());
		mLocalDatabase->bind(*stmt,day, i);

		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::HasAddInfo(const pof::base::data::duuid_t& puid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT 1 FROM patient_addinfo WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel);
		mLocalDatabase->finalise(*stmt);
		return !rel->empty();
	}
	return false;
}

std::optional<pof::PatientManager::AddInfo>
		pof::PatientManager::GetAddInfo(const pof::base::data::duuid_t& puid) const
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT * FROM patient_addinfo WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t, std::string>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return std::nullopt;

		AddInfo info;
		info.mPatientUid = puid;
		info.mData = nl::json::parse(std::get<1>(*(rel->begin())));
		return info;
	}
	return std::nullopt;
}

bool pof::PatientManager::SetAddInfo(const AddInfo& info) const
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(INSERT INTO patient_addinfo VALUES (?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(info.mPatientUid, info.mData.dump()));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::UpdateAddInfo(const AddInfo& info) const
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE patient_addinfo SET info = ? WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(info.mData.dump(), info.mPatientUid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::RemoveAddInfo(const AddInfo& info) const
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(DELTE FROM patient_addinfo WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(info.mPatientUid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PatientManager::CheckIfReminded(pof::base::data::duuid_t& puid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT CheckRemineded(info) FROM patient_addinfo WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		mLocalDatabase->finalise(*stmt);
		if (!rel || rel->empty()) {
			return false;
		}
		return (std::get<0>(*(rel->begin())) == 1);
	}
	return false;
}

std::optional<pof::base::relation<pof::base::data::duuid_t, pof::base::data::text_t>> 
	pof::PatientManager::GetRecentlyStopMedications(const pof::base::data::duuid_t& puid) const
{
	if (mLocalDatabase){
		constexpr const date::days checkduration = date::days(1);
		constexpr const std::string_view sql = R"(SELECT p.uuid, p.name 
		FROM patients pt, products p, medications md
		WHERE Days(md.stopdate) > ? AND AddDays(md.stopdate, ?) < ? AND p.uuid = md.product_uuid AND pt.uuid = md.patient_uuid AND md.patient_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		auto today = pof::base::data::clock_t::now();
		auto dayAhead = date::floor<date::days>(today);
		auto day = (dayAhead + date::days(1)).time_since_epoch().count();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(day),
			static_cast<std::uint64_t>(checkduration.count()), static_cast<std::uint64_t>(day), puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t, pof::base::data::text_t>(*stmt);
		if (!rel) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		return rel;
	}
	return std::nullopt;
}

std::optional<std::vector<pof::base::data::duuid_t>> pof::PatientManager::GetRemindedList()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT pai.puid FROM patient_addinfo pai WHERE CheckReminded(pai.info) = 1;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		std::vector<pof::base::data::duuid_t> ret;
		ret.reserve(rel->size());
		for (auto&& p : rel.value()) {
			ret.emplace_back(std::get<0>(p));
		}
		return ret;
	}
	return std::nullopt;
}

std::optional<pof::base::relation<pof::base::data::text_t, pof::base::data::datetime_t, std::uint64_t, pof::base::currency>> pof::PatientManager::GetSaleForPatient(const pof::base::data::duuid_t& puid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.name, s.sale_date, s.product_quantity, s.product_ext_price
		FROM products p, sales s, (SELECT info FROM patient_addinfo WHERE puid = ?) AS pai
		WHERE s.product_uuid = p.uuid AND InSale(pai.info, s.uuid) = 1;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::text_t,
			pof::base::data::datetime_t,
			std::uint64_t,
			pof::base::currency
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		return rel;
	}
	return std::nullopt;
}

std::optional<std::vector<std::tuple<pof::base::data::duuid_t, std::string>>> pof::PatientManager::GetPinnedList()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.uuid, p.name, p.lastname  
		FROM patients p, patient_addinfo pai
		WHERE p.uuid = pai.puid AND isPinned(pai.info) = 1;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t
		>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		std::vector < std::tuple<pof::base::data::duuid_t, std::string>> ret;
		ret.reserve(rel->size());
		for (auto& tup : *rel){
			ret.emplace_back(std::make_tuple(
				std::get<0>(tup),
				std::get<2>(tup) + " " + std::get<1>(tup)
			));
		}
		return ret;
	}
	return std::nullopt;
}

void pof::PatientManager::DBFuncISReminded(pof::base::database::conn_t conn, int arg, pof::base::database::value_arr_t values)
{
	auto text = pof::base::database::arg<pof::base::data::text_t>(conn, values);
	nl::json obj = nl::json::parse(text);
	auto iter = obj.find("isRemineded");
	if (iter == obj.end()) pof::base::database::result(conn, static_cast<std::uint64_t>(0));
	else {
		std::uint64_t ret = 0;
		if (*iter) ret = 1;
		pof::base::database::result(conn, ret);
	}
}

//day duration
void pof::PatientManager::DBFuncAddDuration(pof::base::database::conn_t conn, int arg, pof::base::database::value_arr_t values)
{
	auto timepoint = pof::base::database::arg<pof::base::data::datetime_t, 0>(conn, values);
	auto duration = pof::base::database::arg<std::uint64_t, 1>(conn, values);

	timepoint += date::days(duration);

	spdlog::info("Duration: {:d}", duration);
	spdlog::info("timepoint: {:d}", timepoint.time_since_epoch().count());

	auto day = std::chrono::duration_cast<date::days>(timepoint.time_since_epoch());
	spdlog::info("Day: {:d}", day.count());
	pof::base::database::result(conn,static_cast<std::uint64_t>(day.count()));
}

void pof::PatientManager::DBFuncInSale(pof::base::database::conn_t conn, int arg, pof::base::database::value_arr_t values)
{
	auto json = pof::base::database::arg<std::string>(conn, values);
	auto saleid = pof::base::database::arg<pof::base::data::duuid_t, 1>(conn, values);
	std::uint64_t ret = 0;
	try {
		auto obj = nl::json::parse(json);
		auto iter = obj.find("saleIds");
		if (iter == obj.end()) goto exit;
		auto& arr = *iter;
		for (auto it = arr.begin(); it != arr.end(); it++) {
			if (saleid ==
				boost::lexical_cast<pof::base::data::duuid_t>(static_cast<std::string>(*it))){
				ret = 1;
				goto exit;
			}
		}
	}
	catch (std::exception& exp){
		spdlog::error(exp.what());
	}
exit:
	pof::base::database::result(conn, ret);
}

void pof::PatientManager::DBFuncIsPinned(pof::base::database::conn_t conn, int arg, pof::base::database::value_arr_t values)
{
	auto json = pof::base::database::arg<std::string>(conn, values);
	std::uint64_t ret = 0;
	try {
		auto obj = nl::json::parse(json);
		auto iter = obj.find("isPinned");
		if (iter == obj.end()) goto exit;
		ret = static_cast<bool>(*iter) ? 1 : 0 ;
	}
	catch (const std::exception& exp){
		spdlog::error(exp.what());
	}
exit:
	pof::base::database::result(conn, ret);
}

void pof::PatientManager::DBFuncGetDateOfBirth(pof::base::database::conn_t conn, int arg, pof::base::database::value_arr_t values)
{
	auto json = pof::base::database::arg<std::string>(conn, values);
	std::string ret;
	try {
		auto obj = nl::json::parse(json);
		auto iter = obj.find("dob");
		if (iter == obj.end()) goto exit;
		else {
			ret = static_cast<std::string>(*iter);
		}
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
	}
exit:
	pof::base::database::result(conn, ret);
}




