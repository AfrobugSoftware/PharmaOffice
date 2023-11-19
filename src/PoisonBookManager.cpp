#include "PoisonBookManager.h"
#include "PofPch.h"

pof::PoisonBookManager::PoisonBookManager()
{
}

pof::PoisonBookManager::~PoisonBookManager()
{
}

void pof::PoisonBookManager::CreatePoisonBookTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql =
			R"(CREATE TABLE IF NOT EXISTS posion_book (
			puid blob,
			patient_name text,
			patient_addy text,
			pharmacist_name text,
			is_verified integer,
			quantity integer,
			start_stock integer,
			running_balance integer,
			date integer, 
			);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
	}
}
