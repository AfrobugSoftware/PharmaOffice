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

std::optional<pof::base::relation<pof::base::data::datetime_t, std::uint64_t>> pof::SaleManager::GetProductSaleHistoryChart(const boost::uuids::uuid& puid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(
		SELECT s.sale_date, SUM(s.product_quantity)
		FROM sales s
		WHERE s.product_uuid = ?
		GROUP BY Days(s.sale_date);
		)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t, std::uint64_t>(*stmt);
		if (!rel.has_value()) spdlog::error(mLocalDatabase->err_msg());
		mLocalDatabase->finalise(*stmt);

		return rel;
	}
	return std::nullopt;
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
	mLocalDatabase->finalise(mStoreSale);
	mLocalDatabase->finalise(mLoadProductHistory);
	mLocalDatabase->finalise(mProductHistByDateStmt);
}

bool pof::SaleManager::RestoreSale(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT s.uuid, s.product_uuid, p.name, p.unit_price, 
		s.product_quantity, s.product_ext_price, s.sale_date 
		FROM sales s, products p WHERE s.uuid = ? AND s.product_uuid = p.uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::currency_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return false;

		//reset the sale
		SaleData->Clear();
		for (auto& tup : *rel) {
			pof::base::data::row_t row;
			row.first = pof::base::make_row_from_tuple(tup);
			SaleData->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

bool pof::SaleManager::CheckIfSaleExists(const boost::uuids::uuid& saleId)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT 1 FROM sales WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleId));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel);

		return (!rel->empty());
	}
	return false;
}

bool pof::SaleManager::RestoreLastSale()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT s.uuid, s.product_uuid, p.name, p.unit_price, 
		s.product_quantity, s.product_ext_price, s.sale_date 
		FROM sales s, products p 
		WHERE s.product_uuid = p.uuid AND s.uuid = (SELECT s.uuid FROM sales s ORDER BY s.sale_date DESC LIMIT 1);)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::currency_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return false;

		//reset the sale
		SaleData->Clear();
		for (auto& tup : *rel) {
			pof::base::data::row_t row;
			row.first = pof::base::make_row_from_tuple(tup);
			SaleData->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

void pof::SaleManager::CreateSaveSaleTable()
{
	if (mLocalDatabase)
	{
		//takes the same data as the table sale
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS save_sale (
		uuid blob,
		product_uuid blob,
		product_quantity integer,
		product_ext_price blob,
		sale_date integer,
		sale_payment_type text
		);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

void pof::SaleManager::CreateSaleLabelTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS sale_label (uuid blob, labels text);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

void pof::SaleManager::CreateSaleInfoTable()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS sale_info (uuid blob, info text);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

void pof::SaleManager::CreateSaleCostTable()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS sale_cost (suid blob, puid blob, date integer, cost blob);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

void pof::SaleManager::CreateDatabaseFunctions()
{
	pof::base::func_aggregate yearfunc;
	yearfunc.name = "Years"s;
	yearfunc.arg_count = 1;
	yearfunc.func = &pof::SaleManager::DBFuncYear;

	mLocalDatabase->register_func(std::move(yearfunc));
}

//add the current sale items to cost
void pof::SaleManager::AddSaleCost()
{
	if (!SaleData || SaleData->GetDatastore().empty()) return;

	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO sale_cost (suid, puid, date, cost) VALUES 
		(?, ?, ?, (SELECT p.cost_price FROM products p WHERE p.uuid = ?));)";

		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt){
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		bool status = false;
		mLocalDatabase->begin_trans();
		for (const auto& s : SaleData->GetDatastore()) {
			auto& suid = boost::variant2::get<pof::base::data::duuid_t>(s.first[pof::SaleManager::SALE_UUID]);
			auto& puid = boost::variant2::get<pof::base::data::duuid_t>(s.first[pof::SaleManager::PRODUCT_UUID]);
			auto& dt = boost::variant2::get<pof::base::data::datetime_t>(s.first[pof::SaleManager::SALE_DATE]);

			status = mLocalDatabase->bind(*stmt, std::make_tuple(suid, puid, dt, puid));
			assert(status);

			status = mLocalDatabase->execute(*stmt);
			if (!status){
				spdlog::error(mLocalDatabase->err_msg());
				mLocalDatabase->end_trans();
				mLocalDatabase->finalise(*stmt);
				return;
			}
		}
		mLocalDatabase->end_trans();
		mLocalDatabase->finalise(*stmt);
	}
}

std::optional<pof::base::data> pof::SaleManager::GetProfitloss(const pof::base::data::datetime_t& dt)
{
	if (mLocalDatabase) {
		std::optional<pof::base::database::stmt_t> stmt = std::nullopt;
		if (wxGetApp().bUseSavedCost) {
			constexpr const std::string_view sql = R"(SELECT s.sale_date, p.name, s.product_quantity, s.product_ext_price, sc.cost 
			FROM products p, sales s, sale_cost sc
			WHERE p.uuid = s.product_uuid AND s.uuid = sc.suid AND s.product_uuid = sc.puid AND s.sale_payment_type IS NOT 'Returned' AND Months(s.sale_date) = ? ORDER BY s.sale_date;)";
			stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return std::nullopt;
			}
		}
		else {
			constexpr const std::string_view ss = R"(SELECT s.sale_date, p.name, s.product_quantity, s.product_ext_price, p.cost_price 
			FROM products p, sales s
			WHERE p.uuid = s.product_uuid AND s.sale_payment_type IS NOT 'Returned' AND Months(s.sale_date) = ? ORDER BY s.sale_date;)";
			stmt = mLocalDatabase->prepare(ss);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return std::nullopt;
			}
		}

		auto month = date::floor<date::months>(dt);
		auto dur = month.time_since_epoch().count();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(month.time_since_epoch().count())));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::datetime_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::currency,
			pof::base::currency
		>(*stmt);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);

		pof::base::data ret;
		ret.get_metadata() = {
			pof::base::data::kind::datetime,
			pof::base::data::kind::text,
			pof::base::data::kind::uint64,
			pof::base::data::kind::currency,
			pof::base::data::kind::currency
		};
		ret.reserve(rel->size());
		for (auto& tup : rel.value()) {
			auto v = pof::base::make_row_from_tuple(std::move(tup));
			auto c = boost::variant2::get<pof::base::currency>(v[4]) *
				static_cast<double>(boost::variant2::get<std::uint64_t>(v[2]));
			c.nearest_hundred();
			v[4] = c;

			ret.emplace(std::move(v));
		}
		return ret;
	}
	return std::nullopt;
}

bool pof::SaleManager::RestoreSaveSale(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT s.uuid, s.product_uuid, p.name, p.unit_price, 
		s.product_quantity, s.product_ext_price, s.sale_date 
		FROM save_sale s, products p WHERE s.uuid = ? AND s.product_uuid = p.uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::currency_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return false;
		
		//reset the sale
		SaleData->Clear();
		for (auto& tup : *rel){
			pof::base::data::row_t row;
			row.first = pof::base::make_row_from_tuple(tup);
			SaleData->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

bool pof::SaleManager::RemoveSaveSale(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM save_sale WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::SaleManager::SaveLabels(const boost::uuids::uuid& saleID, const std::string& labelData)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(INSERT INTO sale_label VALUES (?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID, labelData));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

std::optional<std::string> pof::SaleManager::LoadLabels(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT labels FROM sale_label WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::string>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return std::nullopt;
		else return std::get<0>(*(rel->begin()));
	}

	return std::nullopt;
}

bool pof::SaleManager::RemoveLabels(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM sale_label WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}


bool pof::SaleManager::SaveSale(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(INSERT INTO save_sale (uuid,
		product_uuid,
		product_quantity,
		product_ext_price,
		sale_date,
		sale_payment_type) VALUES (?,?,?,?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
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
						pof::base::data::clock_t::now(),
						mCurPaymentType));
			}
			bool status = mLocalDatabase->store(*stmt, std::move(rel));
			if (!status) {
				spdlog::error(mLocalDatabase->err_msg());
			}
			mLocalDatabase->finalise(*stmt);
			return status;
		}
		catch (const std::exception& exp) {
			spdlog::critical(exp.what());
			mLocalDatabase->finalise(*stmt);
			return false;
		}

	}
	return false;
}

std::optional<pof::base::relation<pof::base::data::datetime_t, boost::uuids::uuid, pof::base::currency>> pof::SaleManager::GetSavedSales()
{
	if (mLocalDatabase){
		//max date is so we pick on entry in each group
		constexpr const std::string_view sql = R"(SELECT sale_date, uuid, SumCost(product_ext_price) 
		FROM save_sale WHERE 1 GROUP BY uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t, boost::uuids::uuid, pof::base::currency>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return rel;
	}
	return std::nullopt;
}

bool pof::SaleManager::CheckIfSaved(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT COUNT(uuid) FROM save_sale WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);
		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return false;
		return (std::get<0>(*rel->begin()) != 0); 
	}
	return false;
}

bool pof::SaleManager::ReturnFromSales(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE sales SET sale_payment_type = 'Returned' WHERE uuid = ? AND product_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID, puid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::SaleManager::CheckReturned(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT 1 FROM sales 
		WHERE uuid = ? AND product_uuid = ? AND sale_payment_type = 'Returned';)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID, puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return false;

		return std::get<0>(*(rel->begin())) == 1;
	}
	return false;
}

bool pof::SaleManager::ChangePaymentOption(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid, const std::string& option)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE sales SET sale_payment_type = ?
		WHERE uuid = ? AND product_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(option, saleID, puid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::SaleManager::ResellItem(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid, size_t quan)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE sales SET sale_payment_type = ?
		WHERE uuid = ? AND product_uuid = ? AND product_quantity = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple("Cash"s, saleID, puid, quan));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::SaleManager::SaveInfo(const boost::uuids::uuid& saleID, const std::string& info)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(INSERT INTO sale_info (uuid, info) VALUES (?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID, info));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

std::optional<std::string> pof::SaleManager::GetInfo(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT info FROM sale_info WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::string>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return std::string{}; //return empty string

		return std::get<0>(*(rel->begin()));

	}
	return std::nullopt;
}

bool pof::SaleManager::RemoveInfo(const boost::uuids::uuid& saleID)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM sale_info WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(saleID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::SaleManager::UpdateInfo(const boost::uuids::uuid& saleID, const std::string& info)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE sale_info SET info = ? WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(info, saleID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

std::optional<pof::base::data> pof::SaleManager::GetProductSoldForMonth(const pof::base::data::datetime_t& dt)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.uuid, p.name, SUM(s.product_quantity) as quan, SumCost(s.product_ext_price), s.sale_date
		FROM sales s, products p 
		WHERE s.product_uuid = p.uuid AND s.sale_payment_type IS NOT 'Returned' AND Months(s.sale_date) = ?
		GROUP BY p.uuid
		ORDER BY quan DESC;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		
		auto month = date::floor<date::months>(dt);
		auto dur = month.time_since_epoch().count();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(month.time_since_epoch().count())));
		assert(status);
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::currency,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		pof::base::data ret;
		ret.reserve(rel->size());
		for (auto&& tup : *rel){
			ret.emplace(pof::base::make_row_from_tuple(std::move(tup)));
		}
		ret.shrink_to_fit();
		return ret;
	}
	return std::nullopt;
}

std::optional<pof::base::currency> pof::SaleManager::GetYearTotalRevenue(const pof::base::data::datetime_t& dt)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT SumCost(s.product_ext_price) 
		FROM sales s
		WHERE Years(s.sale_date) = ? AND s.sale_payment_type IS NOT 'Returned';)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto year = date::floor<date::years>(dt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(year.time_since_epoch().count())));
		assert(status);
		
		auto rel = mLocalDatabase->retrive<pof::base::currency>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		if (rel->empty()) return pof::base::currency{};
		else {
			return std::get<0>(*rel->begin());
		}
	}
	return std::nullopt;
}

bool pof::SaleManager::RemoveProductSaleHistory(pof::base::data::const_iterator iterator)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM sales WHERE product_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iterator->first[pof::ProductManager::PRODUCT_UUID])));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::SaleManager::RemoveProductSaveSale(pof::base::data::const_iterator iterator)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM save_sale WHERE product_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iterator->first[pof::ProductManager::PRODUCT_UUID])));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::SaleManager::RemoveProductFromSale(const pof::base::data::duuid_t& pid, const pof::base::data::duuid_t& sid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM sales WHERE product_uuid = ? AND uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, sid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;

	}
	return false;
}

bool pof::SaleManager::RemoveProductFromSale(size_t rowid, const pof::base::data::duuid_t& pid, const pof::base::data::duuid_t& sid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM sales WHERE product_uuid = ? AND uuid = ? AND ROWID = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, sid, rowid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;

	}
	return false;
}

std::optional<std::tuple<std::uint64_t, pof::base::data::currency_t>> pof::SaleManager::GetReturnedProductQuan(const pof::base::data::duuid_t& pid, const pof::base::data::duuid_t& sid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT product_quantity, product_ext_price FROM sales WHERE product_uuid = ? AND uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, sid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t, pof::base::data::currency_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return std::nullopt;
		return (*(rel->begin()));
	}
	return std::nullopt;
}

std::optional<pof::base::data::datetime_t> pof::SaleManager::GetSaleDate(const pof::base::data::duuid_t& suid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT sale_date FROM sales WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(suid));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
		}
		mLocalDatabase->finalise(*stmt);
		
		return rel->empty() ? std::nullopt : std::make_optional(std::get<0>(*(rel->begin())));

	}
	return std::nullopt;
}

std::optional<pof::base::data> pof::SaleManager::GetLastSale()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT 
		s.uuid,
		s.product_uuid,
		s.product_quantity,
		s.product_ext_price,
		s.sale_date,
		s.sale_payment_type 
		FROM sales s, (SELECT uuid, MAX(sale_date) FROM sales LIMIT 1) as last
		WHERE  s.uuid = last.uuid AND s.sale_payment_type IS NOT 'Returned';)";

		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::datetime_t,
			pof::base::data::text_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
		}

		pof::base::data ret;
		ret.reserve(rel->size());
		for (const auto& tup : *rel){
			ret.emplace(pof::base::make_row_from_tuple(tup));
		}
		return ret;
	}

	return std::nullopt;
}

void pof::SaleManager::DBFuncYear(pof::base::database::conn_t conn, int arg, pof::base::database::value_arr_t values)
{
	std::uint64_t dur = pof::base::database::arg<std::uint64_t>(conn, values);
	auto tt = pof::base::data::datetime_t(pof::base::data::datetime_t::duration(dur));
	auto year = date::floor<date::years>(tt);

	pof::base::database::result(conn, static_cast<std::uint64_t>(year.time_since_epoch().count()));
}

std::optional<pof::base::data> pof::SaleManager::GetWeeklySales(const pof::base::data::datetime_t& dt)
{
	if(mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT Days(s.sale_date), s.sale_date, SumCost(s.product_ext_price)
		FROM sales s
		WHERE Weeks(s.sale_date) = ?
        GROUP BY Days(s.sale_date)
		ORDER BY Days(s.sale_date);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto week = date::floor<date::weeks>(dt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(week.time_since_epoch().count())));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t,
			pof::base::data::datetime_t,
			pof::base::currency>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}

		mLocalDatabase->finalise(*stmt);
		pof::base::data ret;
		ret.reserve(rel->size());
		for (auto&& tup : rel.value()) {
			ret.emplace(pof::base::make_row_from_tuple(tup));
		}
		ret.shrink_to_fit();
		return ret;
	}
	return std::nullopt;
}