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
		PoisonBookManager();
		~PoisonBookManager();

		void CreatePoisonBookTable();


		std::shared_ptr<pof::base::database> mLocalDatabase;
	};

};