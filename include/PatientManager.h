#pragma once
#include <boost/noncopyable.hpp>
#include <ranges>
#include <date/date.h>
#include <algorithm>
#include <set>
#include <shared_mutex>
#include "DataModel.h"
#include "database.h"
#include "Account.h"

namespace pof {
	class PatientManager : private boost::noncopyable
	{
	public:
		//Patients
		enum : std::uint8_t {
			PATIENT_UUID,
			PATIENT_NAME,
			PATIENT_LAST_NAME,
			PATIENT_AGE,
			PATIENT_PHONE_NUMBER,
			PATIENT_ADDRESS,

			//EMERGENCY CONTACT

			//HEALTH
			PATIENT_BMI,
			PATIENT_WEIGHT,
			PATIENT_HR,
			PATIENT_BP,
			PATIENT_RR,
			PATIENT_TEMPT,
			
			PATIENT_MAX
		};
		
		//Patient medicaitons
		enum : std::uint8_t {
			MED_PRODUCT_UUID,
			MED_PATIENT_UUID,
			MED_DURATION,
			MED_PURPOSE,
			MED_DIR_FOR_USE,
			MED_OUTCOME,
			MED_STOCK,
			MED_START_DATE,
			MED_STOP_DATE,
			MED_MAX,
		};

		//Patient History
		enum : std::uint8_t {
			PATIENT_HIST_PROD_UUID,
			PAITENT_MED_UUID,


		};

	private:
		std::unique_ptr<pof::DataModel> mPaitnets;
		std::unique_ptr<pof::DataModel> mPatientMedications;
		std::unique_ptr<pof::DataModel> mPatientHistory;
	};

};