#pragma once
#include <boost/noncopyable.hpp>
#include <ranges>
#include <date/date.h>
#include <algorithm>
#include <array>
#include <set>
#include <shared_mutex>
#include <nlohmann/json.hpp>

#include "DataModel.h"
#include "database.h"
#include "Account.h"
#include "Pharmacy.h"

namespace nl = nlohmann;
namespace pof {
	class PatientManager : private boost::noncopyable
	{
	public:
		//Patients
		static constexpr const std::array<std::string_view, 10> strengthType = {
			"g",
			"mg",
			"mcg",
			"L",
			"ml",
			"%v/v",
			"%w/v",
			"mol",
			"mmol",
			"NOT SPECIFIED" };

		enum : std::uint8_t {
			PATIENT_UUID,
			PATIENT_NAME,
			PATIENT_LAST_NAME,
			PATIENT_AGE,
			PATIENT_GENDER,
			PATIENT_PHONE_NUMBER,
			PATIENT_ADDRESS,

			//EMERGENCY CONTACT

			//HEALTH
			PATIENT_BMI,
			PATIENT_WEIGHT,
			PATIENT_HR,
			PATIENT_BP_SYS,
			PATIENT_BP_DYS,
			PATIENT_RR,
			PATIENT_TEMPT,
			PATIENT_ENTERED_DATE,
			PATIENT_MODIFIED_DATE,
			PATIENT_CLINICAL_INDICATION,
			PATIENT_MAX
		};
		
		//Patient medicaitons
		enum : std::uint8_t {
			MED_PRODUCT_UUID,
			MED_PATIENT_UUID,
			MED_NAME,
			MED_PURPOSE,
			MED_OUTCOME,
			MED_STOCK,
			MED_DIR_FOR_USE_QUANTITY,
			MED_DIR_FOR_USE_STRENGTH,
			MED_DURATION, //IN DAYS COUNT FOR EXAMPLE 3 TIMES A DAY
			MED_START_DATE,
			MED_STOP_DATE,
			MED_MAX,
		};


		//data is a json object that holds the forever changing data for a patient
		struct AddInfo {
			pof::base::data::duuid_t mPatientUid;
			nl::json mData;
		};

		//public shared globals
		std::shared_ptr<pof::base::database> mLocalDatabase;
		std::shared_ptr<pof::Account> mCurAccount;
		std::shared_ptr<pof::Pharmacy> mCurPharmacy;
		
		PatientManager();
		~PatientManager();

		bool CreatePatientTable();
		bool CreatePatientMedicationTable();
		bool CreatePatientAddInfoTable();
		bool CreateDatabaseFunctions();

		std::unique_ptr<pof::DataModel>& GetPatientData();
		std::unique_ptr<pof::DataModel>& GetPatientMedData();
		std::unique_ptr<pof::DataModel>& GetPatientHistotyData();

		bool LoadPatients();
		bool LoadPatientMedication(const pof::base::data::duuid_t& pid);
		bool LoadPatientHistory(const pof::base::data::duuid_t& pid);

		bool IsPatientActive(const pof::base::data::duuid_t& pid);
		//crud
		bool OnAddPatient(pof::base::data::const_iterator iter);
		bool OnRemovePatient(pof::base::data::const_iterator iter);
		bool OnUpdatePatient(pof::base::data::const_iterator iter);
		
		//crud medication
		bool OnAddMedication(pof::base::data::const_iterator iter);
		bool OnRemoveMedication(pof::base::data::const_iterator iter);
		bool OnUpdateMedication(pof::base::data::const_iterator iter);

		//curd for add info
		bool HasAddInfo(const pof::base::data::duuid_t& puid);
		std::optional<AddInfo> GetAddInfo(const pof::base::data::duuid_t& puid) const;
		bool SetAddInfo(const AddInfo& info) const;
		bool UpdateAddInfo(const AddInfo& info) const; 
		bool RemoveAddInfo(const AddInfo& info) const;

		//queries that are yet to test 
		bool CheckIfReminded(pof::base::data::duuid_t& puid);
		std::optional<pof::base::relation<pof::base::data::duuid_t,
				pof::base::data::text_t>> GetRecentlyStopMedications(const pof::base::data::duuid_t& puid) const;

		std::optional<pof::base::relation<pof::base::data::text_t, 
				pof::base::data::datetime_t,
				std::uint64_t,
				pof::base::currency>> GetSaleForPatient(const pof::base::data::duuid_t& puid);

	private:
		static void DBFuncISReminded(pof::base::database::conn_t conn,
			int arg, pof::base::database::value_arr_t values);

		static void DBFuncAddDuration(pof::base::database::conn_t conn,
			int arg, pof::base::database::value_arr_t values);

		static void DBFuncInSale(pof::base::database::conn_t conn,
			int arg, pof::base::database::value_arr_t values);

		std::unique_ptr<pof::DataModel> mPaitnets;
		std::unique_ptr<pof::DataModel> mPatientMedications;
		std::unique_ptr<pof::DataModel> mPatientHistory;

	};

};