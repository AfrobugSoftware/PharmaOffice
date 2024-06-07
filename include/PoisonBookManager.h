#pragma once
#include <boost/noncopyable.hpp>
#include <ranges>
#include <date/date.h>
#include <algorithm>
#include <set>
#include <shared_mutex>
#include "DataModel.h"
#include "database.h"

#include <boost/signals2/signal.hpp>

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

		enum {
			ADDED = 0,
			REMOVED,
			UPDATED,
		};

		boost::signals2::signal<void(int)> gPoisonBookChanged;

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
		std::optional<pof::base::relation<pof::base::data::duuid_t, std::string, std::string, std::string>> GetBooks();
	
		bool IsBookCreated(const pof::base::data::duuid_t& puid);

		std::shared_ptr<pof::base::database> mLocalDatabase;
		void ReloadPoisonBook();

		inline std::unique_ptr<pof::DataModel>& GetBook() { return mPoisonBook; }
	private:
		std::unique_ptr<pof::DataModel> mPoisonBook;
	};

};