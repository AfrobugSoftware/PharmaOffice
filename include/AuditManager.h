#pragma once
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <boost/noncopyable.hpp>
#include <algorithm>
#include <ranges>

#include "database.h"
#include "DataModel.h"

namespace pof
{
	class AuditManager : public boost::noncopyable
	{
	public:
		enum class auditType : std::uint8_t {
			INFORMATION,
			SALE,
			PRODUCT,
			CATEGORY
		};

		AuditManager();


	private:
		std::shared_ptr<pof::base::database> mLocalDatabase;
		std::unique_ptr<pof::DataModel> mAuditData;
	};
}