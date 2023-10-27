#include "AuditManager.h"
//#include "Application.h"
#include "PofPch.h"
/*
		wxColour(253,245,230), // old lace
		wxColour(255,239,213), // papaya whip
		wxColour(255,245,238), // sea shell
		wxColour(245,255,250), // mint cream
*/

std::array<std::shared_ptr<wxDataViewItemAttr>, static_cast<size_t>(pof::AuditManager::auditType::MAX)> pof::AuditManager::auditAttr = {};

pof::AuditManager::AuditManager()
{
	mAuditData = std::make_unique<pof::DataModel>();
	mAuditData->Adapt<
		std::uint64_t,
		pof::base::data::datetime_t,
		std::uint64_t,
		pof::base::data::text_t,
		pof::base::data::text_t
	>();
	CreateSpeicalCols();
	CreateTypeAttributes();
}

pof::AuditManager::~AuditManager() {
	if (mLocalDatabase) {
		mLocalDatabase->finalise(mWriteStatement);
		mLocalDatabase->finalise(mLoadCacheStatement);
		mLocalDatabase->finalise(mDataSizeStatement);
	}
}

void pof::AuditManager::Refresh()
{
	if (mCacheRange.first != 0 && mCacheRange.second != 0) return;

	LoadCache(mCacheRange.first, mCacheRange.second);
}

bool pof::AuditManager::LoadCache(size_t from, size_t limit)
{
	if (mLocalDatabase) {
		//select where id is less than or equal to from and limit (to  -  from)
		if (!mLoadCacheStatement) {
			constexpr const std::string_view sql = "SELECT * FROM audit WHERE rowid BETWEEN 0 AND ? ORDER BY date DESC LIMIT ?;";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}		
			mLoadCacheStatement = *stmt;
		}
		bool status = mLocalDatabase->bind(mLoadCacheStatement, std::make_tuple(from, limit));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto rel = mLocalDatabase->retrive<
			std::uint64_t,
			pof::base::data::datetime_t,
			std::uint64_t,
			pof::base::data::text_t,
			pof::base::data::text_t
		>(mLoadCacheStatement);

		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

		auto& relation = rel.value();
		using tuple_t = std::decay_t<decltype(relation)>;
		if (relation.empty()) {
			//empty
			return false;
		}
		mAuditData->Clear();
		for (size_t i = 0; i < relation.size(); i++) {
			pof::base::data::row_t row;
			row.first.resize(std::tuple_size_v<typename tuple_t::tuple_t>);
			auto& v = row.first;

			v[AUDIT_ID] = std::get<AUDIT_ID>(relation[i]);
			v[AUDIT_TYPE] = std::get<AUDIT_TYPE>(relation[i]);
			v[AUDIT_MESSAGE] = std::get<AUDIT_MESSAGE>(relation[i]);
			v[AUDIT_USER_NAME] = std::get<AUDIT_USER_NAME>(relation[i]);
			v[AUDIT_DATE] = std::get<AUDIT_DATE>(relation[i]);

			if (bColourAuditTypes) {
				std::uint64_t type = std::get<AUDIT_TYPE>(relation[i]);
				mAuditData->AddAttr(wxDataViewItem{ reinterpret_cast<void*>(i + 1) }, auditAttr[type]);
			}

			mAuditData->EmplaceData(std::move(row));
		}
		return true;
	}
}

void pof::AuditManager::LoadDate(const pof::base::data::datetime_t& date, size_t from, size_t to)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT * FROM audit WHERE date = ? AND rowid BETWEEN ? and ? LIMIT 1000;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(date, from, to));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			std::uint64_t,
			pof::base::data::datetime_t,
			std::uint64_t,
			pof::base::data::text_t,
			pof::base::data::text_t
		>(*stmt);
		mLocalDatabase->finalise(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		size_t i = 0;
		for (auto& tup : *rel) {
			if (bColourAuditTypes) {
				auto item = pof::DataModel::GetItemFromIdx(i);
				mAuditData->AddAttr(item, auditAttr[std::get<AUDIT_TYPE>(tup)]);
			}
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(tup));
			mAuditData->EmplaceData(std::move(row));
			i++;
		}
	}
}

bool pof::AuditManager::LoadType(auditType type, size_t from, size_t limt)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT * FROM audit 
		WHERE type = :type AND rowid BETWEEN 0 AND :from  ORDER BY date DESC LIMIT :limit;)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind_para(*stmt, std::make_tuple(static_cast<std::uint64_t>(type), from, limt), {"type", "from", "limit"});
		assert(status);

		auto rel = mLocalDatabase->retrive<
			std::uint64_t,
			pof::base::data::datetime_t,
			std::uint64_t,
			pof::base::data::text_t,
			pof::base::data::text_t
		>(*stmt);
		mLocalDatabase->finalise(*stmt);
		if(!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		if (rel.value().empty()){
			//empty
			return false;
		}
		mAuditData->Clear();
		size_t i = 0;
		for (auto& tup : *rel){
			if (bColourAuditTypes){
				auto item = pof::DataModel::GetItemFromIdx(i);
				mAuditData->AddAttr(item, auditAttr[std::get<AUDIT_TYPE>(tup)]);
			}
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(tup));
			mAuditData->EmplaceData(std::move(row));
			i++;
		}
		return true;
	}
}

void pof::AuditManager::LoadDate(auditType type, const pof::base::data::datetime_t& date, size_t from, size_t to)
{
}

std::optional<size_t> pof::AuditManager::GetDataSize() const
{
	size_t size = 0;
	if (mLocalDatabase) {
		if (!mDataSizeStatement) {
			constexpr const std::string_view sql = "SELECT COUNT(date) FROM audit;";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return std::nullopt;
			}
			mDataSizeStatement = *stmt;
		}
		auto rel = mLocalDatabase->retrive<std::uint64_t>(mDataSizeStatement);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		if (!rel->empty()) {
			size = std::get<0>((*rel)[0]);
		}
	}
	return size;
}
void pof::AuditManager::WriteAudit(auditType type, const std::string& message)
{
	std::uint64_t at = static_cast<std::uint64_t>(type);
	if (mLocalDatabase) {
		if (!mWriteStatement) {
			constexpr const std::string_view sql = "INSERT INTO audit (date, type, user_name, message) VALUES (?,?,?,?);";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return;
			}
			mWriteStatement = *stmt;
		}

		bool status = mLocalDatabase->bind(mWriteStatement, std::make_tuple(pof::base::data::clock_t::now(), at, mCurrentAccount->name, message));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return; 
		}
		status = mLocalDatabase->execute(mWriteStatement);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
	}
}

void pof::AuditManager::CreateTypeAttributes()
{
	auditAttr[static_cast<size_t>(auditType::INFORMATION)] = std::make_shared<wxDataViewItemAttr>();
	auditAttr[static_cast<size_t>(auditType::INFORMATION)]->SetBackgroundColour(wxColour(253, 245, 230));

	auditAttr[static_cast<size_t>(auditType::SALE)] = std::make_shared<wxDataViewItemAttr>();
	auditAttr[static_cast<size_t>(auditType::SALE)]->SetBackgroundColour(wxColour(255, 239, 213));

	auditAttr[static_cast<size_t>(auditType::PRODUCT)] = std::make_shared<wxDataViewItemAttr>();
	auditAttr[static_cast<size_t>(auditType::PRODUCT)]->SetBackgroundColour(wxColour(255, 245, 238));

	auditAttr[static_cast<size_t>(auditType::CATEGORY)] = std::make_shared<wxDataViewItemAttr>();
	auditAttr[static_cast<size_t>(auditType::CATEGORY)]->SetBackgroundColour(wxColour(245, 255, 250));
}

void pof::AuditManager::CreateAuditTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = 
			"CREATE TABLE IF NOT EXISTS audit (date integer, type integer, user_name text, message text);";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return; //cannot create audit table
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		mLocalDatabase->finalise(*stmt);
		CreateAuditDataView(); //create the view 
	}
}

void pof::AuditManager::CreateAuditDataView()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"( CREATE VIEW IF NOT EXISTS audit_view (date, type, user_name, message) AS SELECT * FROM audit ORDER BY date DESC;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		bool status = mLocalDatabase->execute(*stmt);
		assert(status);
		mLocalDatabase->finalise(*stmt);
	}
}

std::optional<size_t> pof::AuditManager::GetDataSize(auditType type) const
{

	size_t size = 0;
	if (mLocalDatabase) {
		constexpr const std::string_view sql = "SELECT COUNT(date) FROM audit WHERE type = ?;";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(type)));
		assert(status);
		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		if (!rel->empty()) {
			size = std::get<0>((*rel)[0]);
		}
		mLocalDatabase->finalise(*stmt);
	}
	return size;
}

void pof::AuditManager::CreateSpeicalCols()
{
	pof::DataModel::SpeicalColHandler_t handler;
	handler.first = [&](size_t row, size_t col) -> wxVariant {
		auto& datastore = mAuditData->GetDatastore();
		std::uint64_t type = boost::variant2::get<std::uint64_t>(datastore[row].first[col]);

		return wxVariant(std::string(types[type]));
	};

	mAuditData->SetSpecialColumnHandler(AUDIT_TYPE, std::move(handler));
}
