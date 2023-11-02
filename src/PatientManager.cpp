#include "PofPch.h"
#include "PatientManager.h"

pof::PatientManager::PatientManager()
{
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
			age,
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
	// TODO: insert return statement here
}

std::unique_ptr<pof::DataModel>& pof::PatientManager::GetPatientMedData()
{
	// TODO: insert return statement here
}

std::unique_ptr<pof::DataModel>& pof::PatientManager::GetPatientHistotyData()
{
	// TODO: insert return statement here
}

bool pof::PatientManager::LoadPatients()
{

	return false;
}

bool pof::PatientManager::LoadPatientMedication(const pof::base::data::duuid_t& pid)
{
	return false;
}

bool pof::PatientManager::LoadPatientHistory(const pof::base::data::duuid_t& pid)
{
	return false;
}
