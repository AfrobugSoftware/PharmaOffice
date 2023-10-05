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
	Finalise();
	SaleData.release();
	//ProductSaleHistory.release();
}

void pof::SaleManager::DoSale()
{
	wxBusyCursor cursor;
	auto& datastore = SaleData->GetDatastore();
	pof::base::data::currency_t totalAmount;
	for (auto& saleProduct : datastore){
		totalAmount += boost::variant2::get<pof::base::data::currency_t>(saleProduct.first[PRODUCT_EXT_PRICE]);
	}
	//SHOW PAYMENT DAILOG
	//paymentDialogData data
	// data.mTotalAmount = totalAmount;
	//PaymentDaialog payment(data);
	//if(payment.ShowModal == wxID_OK) {
	// collect payment data details,
	// write sale data to the database
}

bool pof::SaleManager::LoadHistoryByDate(const pof::base::data::duuid_t& ud,const pof::base::data::datetime_t& dt)
{
	if (mLocalDatabase)
	{
		if (!mProductHistByDateStmt){
			constexpr const std::string_view sql = R"(SELECT s.sale_date, p.name, s.product_quantity, s.product_ext_price
					FROM sales s, products p
					WHERE s.product_uuid = ? AND s.product_uuid = p.uuid AND Days(s.sale_date) = ?;)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()){
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			mProductHistByDateStmt = *stmt;
		}
		auto dayAhead = date::floor<date::days>(dt);
		auto day = (dayAhead + date::days(1)).time_since_epoch().count();
		bool status = mLocalDatabase->bind(mProductHistByDateStmt, std::make_tuple(ud,static_cast<std::uint64_t>(day)));
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::datetime_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t
		>(mProductHistByDateStmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& rr = rel.value();
		ProductSaleHistory->Clear();
		ProductSaleHistory->GetDatastore().reserve(rr.size());

		for (auto& tup : rr) {
			pof::base::data::row_t row;
			auto& v = row.first;
			v.resize(HIST_MAX);

			v[HIST_DATE] = std::get<HIST_DATE>(tup);
			v[HIST_PROD_NAME] = std::get<HIST_PROD_NAME>(tup);
			v[HIST_QUANTITY] = std::get<HIST_QUANTITY>(tup);
			v[HIST_EXTPRICE] = std::get<HIST_EXTPRICE>(tup);

			ProductSaleHistory->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

void pof::SaleManager::LoadProductSaleHistory(const boost::uuids::uuid& productUUID)
{
	if (mLocalDatabase) {
		if (!mLoadProductHistory) {
			constexpr const std::string_view sql = R"(
				SELECT s.sale_date, p.name, s.product_quantity, s.product_ext_price
				FROM sales s, products p
				WHERE s.product_uuid = ? AND p.uuid = s.product_uuid
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
			v[HIST_EXTPRICE] = std::get<HIST_EXTPRICE>(tup);
			
			ProductSaleHistory->EmplaceData(std::move(row));
		}
	}

}

bool pof::SaleManager::CreateSaleTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view table = R"(CREATE TABLE IF NOT EXISTS sales (
		uuid blob,
		product_uuid blob,
		product_quantity integer,
		product_ext_price blob,
		sale_date integer,
		sale_payment_type text
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

bool pof::SaleManager::StoreSale()
{
	if (mLocalDatabase) {
		if (!mStoreSale) {
			constexpr const std::string_view sql = R"(INSERT INTO sales VALUES (?,?,?,?,?,?);)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			mStoreSale = *stmt;
		}
		pof::base::relation<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::datetime_t,
			pof::base::data::text_t
		> rel;
		auto& datastore = SaleData->GetDatastore();
		rel.reserve(datastore.size());

		try {
			for (auto& sale : datastore) {
				auto& v = sale.first;
				rel.emplace_back(
					std::make_tuple(
						boost::variant2::get<pof::base::data::duuid_t>(v[SALE_UUID]),
						boost::variant2::get<pof::base::data::duuid_t>(v[PRODUCT_UUID]),
						boost::variant2::get<std::uint64_t>(v[PRODUCT_QUANTITY]),
						boost::variant2::get<pof::base::data::currency_t>(v[PRODUCT_EXT_PRICE]),
						boost::variant2::get<pof::base::data::datetime_t>(v[SALE_DATE]),
						mCurPaymentType));
			}
			bool status = mLocalDatabase->store(mStoreSale, std::move(rel));
			if (!status) {
				spdlog::error(mLocalDatabase->err_msg());
			}
			return status;
		}
		catch (const std::exception& exp){
			spdlog::critical(exp.what());
			return false;
		}

	}
	return false;
}


void pof::SaleManager::Finalise()
{
}
