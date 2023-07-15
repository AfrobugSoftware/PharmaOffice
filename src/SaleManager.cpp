#include "SaleManager.h"

pof::SaleManager::SaleManager()
{
	SaleData = std::make_unique<pof::DataModel>();
	ProductSaleHistory = std::make_unique<pof::DataModel>();

	SaleData->Adapt<pof::base::data::duuid_t,
		pof::base::data::duuid_t,
		pof::base::data::text_t,
		pof::base::data::currency_t,
		std::uint64_t,
		pof::base::data::currency_t,
		pof::base::data::datetime_t>();

	ProductSaleHistory->Adapt<pof::base::data::datetime_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::currency_t>();

}

pof::SaleManager::~SaleManager()
{
	SaleData.release();
	//ProductSaleHistory.release();
}

void pof::SaleManager::DoSale()
{
}

void pof::SaleManager::LoadProductSaleHistory(const boost::uuids::uuid& productUUID)
{
	if (mLocalDatabase) {
		if (!mLoadProductHistory) {
			constexpr const std::string_view sql = R"(
				SELECT s.sale_date, s.product_name, s.product_quantity, s.product_ext_price
				FROM sales s
				WHERE s.product_uuid = ?
				ORDER BY s.sale_date DESC;
			)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return;
			}
			mLoadProductHistory = *stmt;
		}
		bool status = mLocalDatabase->bind(mLoadProductHistory, std::make_tuple(productUUID));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::datetime_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t
		>(mLoadProductHistory);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		using relation_t = std::decay_t<decltype(*rel)>;
		using tup_t = typename relation_t::tuple_t;
		for (auto& tup : *rel) {
			pof::base::data::row_t row;
			auto& v = row.first;
			v.resize(HIST_MAX);

			v[HIST_DATE] = std::get<HIST_DATE>(tup);
			v[HIST_PROD_NAME] = std::get<HIST_PROD_NAME>(tup);
			v[HIST_QUANTITY] = std::get<HIST_QUANTITY>(tup);
			v[HIST_DATE] = std::get<HIST_EXTPRICE>(tup);
			
			ProductSaleHistory->EmplaceData(std::move(row));
		}
	}

}

bool pof::SaleManager::CreateSaleTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view table = R"(CREATE TABLE IF NOT EXSITS sales (
		uuid blob,
		product_uuid blob,
		product_name text,
		product_price blob,
		product_quantity integer,
		product_ext_price blob,
		sale_date integer
		); )";
		auto stmt = mLocalDatabase->prepare(table);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

		mLocalDatabase->finalise(*stmt);
		return true;
	}
	return false;
}
