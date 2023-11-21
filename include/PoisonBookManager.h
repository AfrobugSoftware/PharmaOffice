#pragma once
#include <boost/noncopyable.hpp>
#include <ranges>
#include <date/date.h>
#include <algorithm>
#include <set>
#include <shared_mutex>
#include "DataModel.h"
#include "database.h"

namespace pof {
	class PoisonBookManager : private boost::noncopyable
	{
	public:
		enum {
			PUID,
			PNAME,
			PADDY,
			PHARMNAME,
			ISVERIFED,
			QUAN,
			STARTSTOCK,
			RUNBALANCE,
			DATE,
			MAX
		};

		PoisonBookManager();
		~PoisonBookManager();

		void CreatePoisonBookTable();

		//crud
		bool CreateNewBook(pof::base::data::row_t&& row);
		bool LoadBook(const pof::base::data::duuid_t& puid);
		bool LoadBook(const pof::base::data::duuid_t& puid, const pof::base::data::datetime_t& d);
		bool OnAddRecord(pof::base::data::const_iterator iter);
		bool OnRemoveRecord(pof::base::data::const_iterator iter);
		bool OnUpdateRecord(pof::base::data::const_iterator iter);
		std::optional<pof::base::relation<std::string, std::string, std::string>> GetBooks();
	
		bool IsBookCreated(const pof::base::data::duuid_t& puid);

		std::shared_ptr<pof::base::database> mLocalDatabase;

		inline std::unique_ptr<pof::DataModel>& GetBook() { return mPosionBook; }
	private:
		std::unique_ptr<pof::DataModel> mPosionBook;
	};

};