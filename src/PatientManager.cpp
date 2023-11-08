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
		pof::base::data::datetime_t, // DOB
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
		constexpr const std::string_view sql = R"(SELECT * FROM patients LIMIT 1000;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t, //UUID
			pof::base::data::text_t, //NAME
			pof::base::data::text_t, //LAST NAME
			pof::base::data::datetime_t, // AGE
			pof::base::data::text_t,
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
	WHERE pm.stopdate > ? AND pm.patient_uuid = ? AND p.uuid = pm.product_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto today = pof::base::data::clock_t::now();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(today, pid));
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
	WHERE pm.stopdate < ? AND pm.patient_uuid = ? AND p.uuid = pm.product_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto today = pof::base::data::clock_t::now();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(today, pid));
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
		constexpr const std::string_view sql = R"(SELECT 1 FROM medications WHERE patient_uuid = ? AND Days(stopdate) > ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		//on some computers it does not work with the day ahead
		auto dayAhead = date::floor<date::days>(pof::base::data::clock_t::now());
		auto day = (dayAhead + date::days(1)).time_since_epoch().count();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, static_cast<std::uint64_t>(day)));
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
		constexpr const std::string_view sql = R"(INSERT INTO patients VALUES (?,?,?, ?,?,?,?,?,?,?,?,?,?,?,?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		std::tuple<
			pof::base::data::duuid_t, //UUID
			pof::base::data::text_t, //NAME
			pof::base::data::text_t, //LAST NAME
			pof::base::data::datetime_t, // AGE
			pof::base::data::text_t, //GENDER
			pof::base::data::text_t, //PHONE NUMBER
			pof::base::data::text_t, // ADDRESS
			std::uint64_t, // BMI
			std::uint64_t, // WEIGHT
			std::uint64_t, //HR
			std::uint64_t, // BP
			std::uint64_t, // BP_sys
			std::uint64_t, // RR
			std::uint64_t, // TEMPT
			pof::base::data::datetime_t, // ENTERED DATE
			pof::base::data::datetime_t,  // MODIFIED DATE
			pof::base::data::text_t //CLINICAL
		> tup;
		pof::base::build(tup, *iter);
		bool status = mLocalDatabase->bind(*stmt, tup);
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
		constexpr static std::array<std::string_view, 16> colName = {
			"uuid", "lastname", "dob", "gender" ,"phonenumber", "address", "bmi", "weight", "bpsys", "bpdys", "rr", "tempreture", "createdate", "modifieddate", "clinical"
		};
		std::ostringstream os;
		std::vector<size_t> upIdx;
		os << "UPDATE patients ";
		auto& updateFlag = iter->second.second;
		for (int i = 0; i < PATIENT_MAX; i++) {
			if (updateFlag.test(i)) {
				upIdx.push_back(i);
				os << "SET " << colName[i] << "= ?";
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
		constexpr const std::string_view sql = R"(DELETE FROM medications WHERE patient_uuid = ? AND product_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[MED_PATIENT_UUID]);
		auto& puuid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[MED_PATIENT_UUID]);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uuid, puuid));
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
		constexpr static const std::array<std::string_view, 10> colNames = {
			"product_uuid","patient_uuid", "purpose", "outcome", "stock", "dirforusequan", "dirforusesten", 
			"duration", "startdate", "stopdate"
		};
		std::ostringstream os;
		std::vector<size_t> upIdx;
		os << "UPDATE medications ";
		auto& updateFlag = iter->second.second;
		for (int i = 0; i < MED_MAX; i++) {
			if (updateFlag.test(i)) {
				upIdx.push_back(i);
				os << "SET " << colNames[i] << "= ?";
			}
		}
		os << " WHERE patient_uuid = ? AND product_uuid = ?;";
		upIdx.push_back(MED_PATIENT_UUID);
		upIdx.push_back(MED_PRODUCT_UUID);

		auto stmt = mLocalDatabase->prepare(os.str());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
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


