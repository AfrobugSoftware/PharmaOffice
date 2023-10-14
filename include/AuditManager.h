#pragma once
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <boost/noncopyable.hpp>
#include <algorithm>
#include <ranges>
#include <array>

#include "database.h"
#include "DataModel.h"
#include "Account.h"

namespace pof
{
	class AuditManager : public boost::noncopyable
	{
	public:
		constexpr const static size_t maxRows = 300;
		enum class auditType : std::uint8_t {
			INFORMATION,
			SALE,
			PRODUCT,
			CATEGORY,
			ALL,
			MAX
		};

		constexpr const static std::array<std::string_view, static_cast<size_t>(auditType::MAX)> types = {
			"INFORMAITON",
			"SALE",
			"PRODUCT",
			"CATEGORY",
			"ALL"
		};

		static std::array<std::shared_ptr<wxDataViewItemAttr>, static_cast<size_t>(auditType::MAX)> auditAttr;

		enum : std::uint8_t {
			AUDIT_ID,
			AUDIT_DATE,
			AUDIT_TYPE,
			AUDIT_USER_NAME,
			AUDIT_MESSAGE,
		};

		AuditManager();
		~AuditManager();

		void LoadCache(size_t from, size_t to);
		void LoadDate(const pof::base::data::datetime_t& date, size_t from, size_t to);
		void LoadType(auditType type, size_t from, size_t to);
		void LoadDate(auditType type, const pof::base::data::datetime_t& date, size_t from, size_t to);
		void WriteAudit(auditType type, const std::string& message);
		void Refresh();
		void CreateAuditTable();
		void CreateAuditDataView();
		std::optional<size_t> GetDataSize(auditType type) const; 
		std::optional<size_t> GetDataSize() const;

		std::shared_ptr<pof::base::database> mLocalDatabase;
		std::shared_ptr<pof::Account> mCurrentAccount;

		std::unique_ptr<pof::DataModel>& GetAuditData() { return mAuditData; }

		//settings
		bool bUseAudit = true; //setting parameters to allow auditting
		bool bColourAuditTypes = false;
		bool bHighlightOne = false;
		int sHighlightType = -1;
	private:
		void CreateSpeicalCols();
		void CreateTypeAttributes();

		pof::base::database::stmt_t mWriteStatement = nullptr;
		pof::base::database::stmt_t mLoadCacheStatement = nullptr;
		mutable pof::base::database::stmt_t mDataSizeStatement = nullptr;

		std::pair<size_t, size_t> mCacheRange = {0,0};
		std::unique_ptr<pof::DataModel> mAuditData;

	};
}