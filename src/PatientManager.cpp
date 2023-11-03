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
		std::uint64_t, // AGE
		pof::base::data::text_t, //PHONE NUMBER
		pof::base::data::text_t, // ADDRESS
		std::uint64_t, // BMI
		std::uint64_t, // WEIGHT
		std::uint64_t, //HR
		std::uint64_t, // BP
		std::uint64_t, // RR
		std::uint64_t, // TEMPT
		pof::base::data::datetime_t, // ENTERED DATE
		pof::base::data::datetime_t  // MODIFIED DATE
	>();

}

pof::PatientManager::~PatientManager()
{
}

bool pof::PatientManager::CreatePatientTable()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS patients 
		(
			uuid blob,
			name text,
			lastname text,
			age integer,
			phonenumber text,
			address text,
			bmi integer,
			weight integer,
			bpsys integer,
			bpdys integer,
			rr integer,
			tempreture integer,
			createdate integer,
			modifieddate integer	
		);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (stmt.has_value()){
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
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS patient_med
		(
			product_uuid blob,
			patient_uuid blob,
			purpose text,
			dirforuse text,
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
			std::uint64_t, // AGE
			pof::base::data::text_t, //PHONE NUMBER
			pof::base::data::text_t, // ADDRESS
			std::uint64_t, // BMI
			std::uint64_t, // WEIGHT
			std::uint64_t, //HR
			std::uint64_t, // BP
			std::uint64_t, // RR
			std::uint64_t, // TEMPT
			pof::base::data::datetime_t, // ENTERED DATE
			pof::base::data::datetime_t  // MODIFIED DATE
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
		constexpr const std::string_view sql = R"(SELECT *
		 FROM patient_med pm 
		 WHERE pm.stopdate > ? AND pm.patient_uuid = ?;)";


	}
	return false;
}

bool pof::PatientManager::LoadPatientHistory(const pof::base::data::duuid_t& pid)
{
	return false;
}
