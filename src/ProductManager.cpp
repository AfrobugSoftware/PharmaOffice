#include "ProductManager.h"
boost::hash<boost::uuids::uuid> std::hash<pof::base::data::duuid_t>::uuid_hasher;
pof::ProductManager::ProductManager() {
	mProductData = std::make_unique<pof::DataModel>();
	mInventoryData = std::make_unique<pof::DataModel>();
	mOrderList = std::make_unique<pof::DataModel>();
	mStockCheckData = std::make_unique<pof::DataModel>();
	mSuppliers = std::make_unique<pof::DataModel>();
	mInvoices = std::make_unique<pof::DataModel>();

	mProductData->Adapt<
		pof::base::data::duuid_t, //UUID
		std::uint64_t, //SERIAL NUM
		pof::base::data::text_t, // NAME
		pof::base::data::text_t, // GENERIC NAME
		pof::base::data::text_t, //CLASS
		pof::base::data::text_t, //FORMULATION
		pof::base::data::text_t, // STRENGTH
		pof::base::data::text_t, // STRENGTH TYPE
		pof::base::data::text_t, // USAGE INFO
		pof::base::data::text_t, // PRODUCT DESCRIPTION
		pof::base::data::text_t, // PRODUCT HEALTH CONDITIONS COMMA SEPERATED
		pof::base::data::currency_t, // UNIT PRICE
		pof::base::data::currency_t, // COST PRICE
		std::uint64_t, //PACAKGE SIZE
		std::uint64_t, //STOCK COUNT
		pof::base::data::text_t, //SIDE EFFECTS
		pof::base::data::text_t, //BARCODE
		std::uint64_t, //CATEGORY ID

		//PRODUCT SETTINGS
		std::uint32_t, //MIN_STOCJ_COUNT
		pof::base::data::text_t, //EXPIRE PERIOD
		std::uint64_t //EXPIRE DATE

	>();
	
	mInventoryData->Adapt<
		std::uint64_t, //ID
		pof::base::data::duuid_t, //UUID
		pof::base::data::datetime_t, // EXPIRE DATE
		pof::base::data::datetime_t, // ADDED DATE
		std::uint64_t, // STOCK COUNT
		pof::base::data::currency_t, //CURRENCY
		pof::base::data::text_t, // MANUFACTURE NAME
		std::uint64_t,  // MANUFACTURER ADDRESS ID
		pof::base::data::text_t // LOT NUMBER/ BATCH NUMBER
	>();
	pof::base::adapt<std::uint64_t, pof::base::data::text_t>(mCategories);

	mOrderList->Adapt<
		pof::base::data::duuid_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::currency_t,
		std::uint64_t
	>();

	mStockCheckData->Adapt<
		pof::base::data::duuid_t, // prod uuid
		pof::base::data::text_t, //prod name
		std::uint64_t, //currenct stock
		std::uint64_t, //checked stock
		std::uint64_t, //status
		pof::base::data::datetime_t
	>();
	
	mSuppliers->Adapt<
		std::uint64_t, //supplier id
		pof::base::data::text_t, //supplier_name
		pof::base::data::datetime_t, //date created
		pof::base::data::datetime_t, //date modified
		pof::base::data::text_t
	>();

	mInvoices->Adapt<
		std::uint64_t, //supplier_id
		pof::base::data::text_t, //invoice identifier
		pof::base::data::duuid_t, //product uuid
		std::uint64_t //inventory id
	>();

	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::StrProductData, this), pof::DataModel::Signals::STORE);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::UpdateProductData, this), pof::DataModel::Signals::UPDATE);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::RemoveProductData, this), pof::DataModel::Signals::REMOVED);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::OnStoreProductData, this), pof::DataModel::Signals::STORE_LOAD);

	mInventoryData->ConnectSlot(std::bind_front(&pof::ProductManager::StoreInventoryData, this), pof::DataModel::Signals::STORE);
	mInventoryData->ConnectSlot(std::bind_front(&pof::ProductManager::UpdateInventoryData, this), pof::DataModel::Signals::UPDATE);
	mInventoryData->ConnectSlot(std::bind_front(&pof::ProductManager::RemoveInventoryData, this), pof::DataModel::Signals::REMOVED);

	mSuppliers->ConnectSlot(std::bind_front(&pof::ProductManager::AddSupplier, this), pof::DataModel::Signals::STORE);
	mSuppliers->ConnectSlot(std::bind_front(&pof::ProductManager::RemoveSupplier, this), pof::DataModel::Signals::REMOVED);
	mSuppliers->ConnectSlot(std::bind_front(&pof::ProductManager::UpdateSupplier, this), pof::DataModel::Signals::UPDATE);

	mInvoices->ConnectSlot(std::bind_front(&pof::ProductManager::AddInvoice, this), pof::DataModel::Signals::STORE);
	mInvoices->ConnectSlot(std::bind_front(&pof::ProductManager::RemoveInvoice, this), pof::DataModel::Signals::REMOVED);
	mInvoices->ConnectSlot(std::bind_front(&pof::ProductManager::UpdateInvoice, this), pof::DataModel::Signals::UPDATE);
}

pof::ProductManager::~ProductManager()
{
	mProductData.release();
	mInventoryData.release();
}

bool pof::ProductManager::LoadInventoryData(const pof::base::data::duuid_t& ud)
{
	if (bUsingLocalDatabase && mLocalDatabase){
		if (!InventoryLoadStmt){
			static constexpr const std::string_view sql = "SELECT * FROM inventory WHERE uuid =  ? ORDER BY input_date DESC LIMIT 300;";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			InventoryLoadStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(InventoryLoadStmt, ud, 1);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(InventoryLoadStmt);
			InventoryLoadStmt = nullptr;
			return false;
		}
		auto rel = mLocalDatabase->retrive<
			std::uint64_t, //ID
			pof::base::data::duuid_t, //UUID
			pof::base::data::datetime_t, // EXPIRE DATE
			pof::base::data::datetime_t, // ADDED DATE
			std::uint64_t, // STOCK COUNT
			pof::base::data::currency_t, //CURRENCY
			pof::base::data::text_t, // MANUFACTURE NAME
			std::uint64_t,  // MANUFACTURER ADDRESS ID
			pof::base::data::text_t // LOT NUMBER/ BATCH NUMBER
		>(InventoryLoadStmt);
		if (!rel.has_value()){
			//assume an error occured
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(InventoryLoadStmt);
			InventoryLoadStmt = nullptr;
			return false;
		}
		if (rel->empty()) {
			spdlog::info("Inventory is empty");
			return false;
		}

		mInventoryData->GetDatastore().reserve(rel->size());
		for (auto& v : *rel) {
			pof::base::data::row_t row;
			auto& f = row.first;
			f = std::move(pof::base::make_row_from_tuple(std::move(v)));
			mInventoryData->EmplaceData(std::move(row));
		}
	}
	return true;
}

bool pof::ProductManager::LoadCategories()
{
	if (bUsingLocalDatabase && mLocalDatabase){
		constexpr const std::string_view sql = "SELECT * FROM category;";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto relation = mLocalDatabase->retrive<
			std::uint64_t, 
			pof::base::data::text_t
		>(*stmt);
		if (!relation.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

		if (relation->empty()){
			spdlog::info("Categories is empty");
			return false;
		}
		for (auto& tup : *relation) {
			auto row = pof::base::make_row_from_tuple(tup);
			mCategories.insert(row);
		}
	}

	return true;
}

bool pof::ProductManager::LoadProductsFromDatabase()
{
	std::shared_ptr<pof::base::database> pd = mLocalDatabase;
	if (pd) {
		constexpr const std::string_view sql = "SELECT * FROM products WHERE uuid NOT IN (SELECT * FROM hidden) ORDER BY name ASC LIMIT 5000;";
		auto stmt = pd->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(pd->err_msg());
			return false;
		}

		auto relation = pd->retrive <
			pof::base::data::duuid_t, //UUID
			std::uint64_t, //SERIAL NUM
			pof::base::data::text_t, // NAME
			pof::base::data::text_t, // GENERIC NAME
			pof::base::data::text_t, //CLASS
			pof::base::data::text_t, //FORMULATION
			pof::base::data::text_t, // STRENGTH
			pof::base::data::text_t, // STRENGTH TYPE
			pof::base::data::text_t, // USAGE INFO
			pof::base::data::text_t, // PRODUCT DESCRIPTION
			pof::base::data::text_t, // PRODUCT HEALTH CONDITIONS COMMA SEPERATED
			pof::base::data::currency_t, // UNIT PRICE
			pof::base::data::currency_t, // COST PRICE
			std::uint64_t, //PACAKGE SIZE
			std::uint64_t, //STOCK COUNT
			pof::base::data::text_t, //SIDE EFFECTS
			pof::base::data::text_t, //BARCODE
			std::uint64_t, //CATEGORY ID
			//PRODUCT SETTINGS
			std::uint32_t, //MIN_STOCJ_COUNT
			pof::base::data::text_t, //EXPIRE PERIOD
			std::uint64_t //EXPIRE DATE
		> (stmt.value());

		if (!relation.has_value()) {
			spdlog::error(pd->err_msg());
			return false;
		}

		if (relation->empty()) {
			spdlog::info("Product database is empty");
			return false;
		}

		mProductData->Clear();
		for (auto& tup : *relation) {
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(tup));
			mProductData->EmplaceData(std::move(row));
		}
	}
	return true;
}

std::optional<pof::base::data> pof::ProductManager::GetHiddenProducts()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = "SELECT * FROM products WHERE uuid IN (SELECT * FROM hidden) ORDER BY name ASC LIMIT 5000;";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto relation = mLocalDatabase->retrive<
			pof::base::data::duuid_t, //UUID
			std::uint64_t, //SERIAL NUM
			pof::base::data::text_t, // NAME
			pof::base::data::text_t, // GENERIC NAME
			pof::base::data::text_t, //CLASS
			pof::base::data::text_t, //FORMULATION
			pof::base::data::text_t, // STRENGTH
			pof::base::data::text_t, // STRENGTH TYPE
			pof::base::data::text_t, // USAGE INFO
			pof::base::data::text_t, // PRODUCT DESCRIPTION
			pof::base::data::text_t, // PRODUCT HEALTH CONDITIONS COMMA SEPERATED
			pof::base::data::currency_t, // UNIT PRICE
			pof::base::data::currency_t, // COST PRICE
			std::uint64_t, //PACAKGE SIZE
			std::uint64_t, //STOCK COUNT
			pof::base::data::text_t, //SIDE EFFECTS
			pof::base::data::text_t, //BARCODE
			std::uint64_t, //CATEGORY ID
			//PRODUCT SETTINGS
			std::uint32_t, //MIN_STOCJ_COUNT
			pof::base::data::text_t, //EXPIRE PERIOD
			std::uint64_t //EXPIRE DATE
		> (stmt.value());
		if (!relation.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		pof::base::data ret;
		ret.get_metadata() = mProductData->GetDatastore().get_metadata();
		ret.reserve(relation->size());
		for (auto& tup : relation.value()) {
			ret.emplace(pof::base::make_row_from_tuple(tup));
		}
		return ret;
	}
	return std::nullopt;
}

bool pof::ProductManager::LoadInventoryByDate(const pof::base::data::duuid_t& ud, const pof::base::data::datetime_t& dt)
{
	if (mLocalDatabase){
		if (!LoadInventoryByDateStmt){
			constexpr const std::string_view sql = R"(SELECT * 
				FROM inventory i 
				WHERE i.uuid = ? AND Days(i.input_date) = ? LIMIT 1000;
			)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			LoadInventoryByDateStmt = *stmt;
		}
		auto dayAhead = date::floor<date::days>(dt);
		auto day = (dayAhead + date::days(1)).time_since_epoch().count();
		bool status = mLocalDatabase->bind(LoadInventoryByDateStmt, std::make_tuple(ud, static_cast<std::uint64_t>(day)));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto rel = mLocalDatabase->retrive<
			std::uint64_t, //ID
			pof::base::data::duuid_t, //UUID
			pof::base::data::datetime_t, // EXPIRE DATE
			pof::base::data::datetime_t, // ADDED DATE
			std::uint64_t, // STOCK COUNT
			pof::base::data::currency_t, //CURRENCY
			pof::base::data::text_t, // MANUFACTURE NAME
			std::uint64_t,  // MANUFACTURER ADDRESS ID
			pof::base::data::text_t // LOT NUMBER/ BATCH NUMBER
		>(LoadInventoryByDateStmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& r = rel.value();
		mInventoryData->Clear();
		mInventoryData->GetDatastore().reserve(r.size());
		for (auto& tup : r) {
			pof::base::data::row_t row;
			auto& f = row.first;
			f = std::move(pof::base::make_row_from_tuple(std::move(tup)));
			mInventoryData->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

void pof::ProductManager::CreateOrderListTable()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS order_list (prod_uuid blob primary key, quan integer, state integer);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		bool status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
	}
}

bool pof::ProductManager::LoadOrderList()
{
	if (!mOrderList->GetDatastore().empty()) mOrderList->Clear();
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT p.uuid, p.name, ol.quan, p.cost_price, ol.state
				FROM order_list ol, products p
				WHERE p.uuid = ol.prod_uuid
			    LIMIT 1000;
			)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

		auto rel = mLocalDatabase->retrive <
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t,
			std::uint64_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);

		for (auto& tup : *rel){ 
			pof::base::data::row_t row;
			auto& v = row.first;
			v = std::move(pof::base::make_row_from_tuple(tup));
			mOrderList->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

bool pof::ProductManager::ClearOrderList()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM order_list;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());
		bool status = mLocalDatabase->execute(*stmt);
		mLocalDatabase->finalise(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::CheckIfInOrderList(const pof::base::data::duuid_t& ud)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT 1 FROM order_list WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(ud));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel.has_value());

		mLocalDatabase->finalise(*stmt);
		return !rel->empty();

	}
	return false;
}

bool pof::ProductManager::AddToOrderList(const pof::base::data::duuid_t& ud, std::uint64_t quan)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO order_list (prod_uuid, quan, state)
			VALUES (:prod_uuid, :quan, :state)
			ON CONFLICT (prod_uuid) DO UPDATE SET quan = quan + 1 WHERE prod_uuid = :prod_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt.has_value());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind_para(*stmt, std::make_tuple(ud, quan, PENDING), {"prod_uuid", "quan", "state"});
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::ProductManager::AddMulipleToOrderList(const std::vector<std::reference_wrapper<pof::base::data::row_t>>& data)
{
	if (data.empty()) return false;
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(INSERT INTO order_list (prod_uuid, quan, state)
			VALUES (:prod_uuid, :quan, :state)
			ON CONFLICT (prod_uuid) DO UPDATE SET quan = quan + 1 WHERE prod_uuid = :prod_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt.has_value());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = false;
		for (auto& ref : data)
		{
			auto& row = ref.get();
			pof::base::data::duuid_t uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);

			status = mLocalDatabase->bind_para(*stmt, std::make_tuple(uuid, static_cast<std::uint64_t>(1), PENDING), {"prod_uuid", "quan", "state"});
			assert(status);
			status = mLocalDatabase->execute(*stmt);
			assert(status);
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}


bool pof::ProductManager::UpdateOrderList(const pof::base::data::duuid_t& ud, std::uint64_t quan)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE order_list set quan = ? WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(quan, ud));
		if (!status) {
			mLocalDatabase->finalise(*stmt);
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return false;
	}
	return false;
}

bool pof::ProductManager::RemvFromOrderList(const pof::base::data::duuid_t ud)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM order_list WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(*stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(ud));
		if (!status) {
			mLocalDatabase->finalise(*stmt);
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

size_t pof::ProductManager::GetOrderListCount() const
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT COUNT(prod_uuid) FROM order_list;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel.has_value());
		mLocalDatabase->finalise(*stmt);

		return (std::get<0>(*rel->begin()));
	}
	return size_t(0);
}

bool pof::ProductManager::UpdateOrderState(const pof::base::data::duuid_t& uid, std::uint64_t state)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE order_list SET state = ? WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(state, uid));
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

bool pof::ProductManager::StrProductData(pof::base::data::const_iterator iter)
{
	if (iter == mProductData->GetDatastore().end()) return false;
	auto& v = iter->first;
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!productStoreStmt) {
			static constexpr const std::string_view sql = R"(INSERT INTO products (uuid, serail_num, name, generic_name, class, formulation, strength, strength_type, usage_info, descrip, health_condition, unit_price, cost_price, package_size, stock_count, side_effects, barcode, category, min_stock_count, expire_period, expire_date )
				 VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ? , ? , ?, ? ); )";
			auto stmt = mLocalDatabase->prepare(sql);
			if (stmt.has_value()) {
				productStoreStmt = *stmt;
			}
			else {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
		}
		using tuple_t = std::tuple<
			pof::base::data::duuid_t, //UUID
			std::uint64_t, //SERIAL NUM
			pof::base::data::text_t, // NAME
			pof::base::data::text_t, // GENERIC NAME
			pof::base::data::text_t, //CLASS
			pof::base::data::text_t, //FORMULATION
			pof::base::data::text_t, // STRENGTH
			pof::base::data::text_t, // STRENGTH TYPE
			pof::base::data::text_t, // USAGE INFO
			pof::base::data::text_t, // PRODUCT DESCRIPTION
			pof::base::data::text_t, // PRODUCT HEALTH CONDITIONS COMMA SEPERATED
			pof::base::data::currency_t, // UNIT PRICE
			pof::base::data::currency_t, // COST PRICE
			std::uint64_t, //PACAKGE SIZE
			std::uint64_t, //STOCK COUNT
			pof::base::data::text_t, //SIDE EFFECTS
			pof::base::data::text_t, //BARCODE
			std::uint64_t, //CATEGORY ID
			std::uint32_t, //MIN_STOCJ_COUNT
			pof::base::data::text_t, //EXPIRE PERIOD
			std::uint64_t //EXPIRE DATE
		>;
		try {
			tuple_t tup;
			pof::base::build(tup,*iter);
			bool status = mLocalDatabase->bind(productStoreStmt, std::move(tup));
			if (!status) {
				spdlog::error(mLocalDatabase->err_msg());
				mLocalDatabase->finalise(productStoreStmt);
				productStoreStmt = nullptr;
				return false;
			}
			status = mLocalDatabase->execute(productStoreStmt);
			if (!status) {
				spdlog::error(mLocalDatabase->err_msg());
				productStoreStmt = nullptr;
				return false;
			}
		}
		catch (const std::exception& exp){
			spdlog::error(exp.what());
			return false;
		}
	}
	return true;
}

bool pof::ProductManager::UpdateProductData(pof::base::data::const_iterator iter)
{
	if (iter == mProductData->GetDatastore().end()) return false;
	auto& v = iter->first;
	auto up = iter->second.second;
	std::vector<size_t> upData;
	if (bUsingLocalDatabase && mLocalDatabase){
		std::ostringstream os;
		os << "UPDATE products set ";
		std::vector<std::string_view> textArray;
		textArray.reserve(PRODUCT_MAX);

		for (size_t i = 0; i < PRODUCT_MAX; i++) {
			if (up.test(i)) {
				upData.push_back(i);
				switch (i)
				{
				case PRODUCT_NAME:
					textArray.emplace_back("name = ?");
					break;
				case PRODUCT_GENERIC_NAME:
					textArray.emplace_back("generic_name = ?");
					break;
				case PRODUCT_CLASS:
					textArray.emplace_back("class = ?");
					break;
				case PRODUCT_FORMULATION:
					textArray.emplace_back("formulation = ?");
					break;
				case PRODUCT_STRENGTH:
					textArray.emplace_back("strength = ?");
					break;
				case PRODUCT_STRENGTH_TYPE:
					textArray.emplace_back("strength_type = ?");
					break;
				case PRODUCT_USAGE_INFO:
					textArray.emplace_back("usage_info = ?");

					break;
				case PRODUCT_DESCRIP:
					textArray.emplace_back("descrip = ?");

					break;
				case PRODUCT_HEALTH_CONDITIONS:
					textArray.emplace_back("health_conditions = ?");

					break;
				case PRODUCT_UNIT_PRICE:
					textArray.emplace_back("unit_price = ?");

					break;
				case PRODUCT_COST_PRICE:
					textArray.emplace_back("cost_price = ?");

					break;
				case PRODUCT_PACKAGE_SIZE:
					textArray.emplace_back("package_size = ?");

					break;
				case PRODUCT_STOCK_COUNT:
					textArray.emplace_back("stock_count = ?");

					break;
				case PRODUCT_SIDEEFFECTS:
					textArray.emplace_back("side_effects = ?");

					break;
				case PRODUCT_BARCODE:
					textArray.emplace_back("barcode = ?");

					break;
				case PRODUCT_CATEGORY:
					textArray.emplace_back("category = ?");

					break;
				case PRODUCT_MIN_STOCK_COUNT:
					textArray.emplace_back("min_stock_count = ?");

					break;
				case PRODUCT_EXPIRE_PERIOD:
					textArray.emplace_back("expire_period = ?");

					break;
				case PRODUCT_TO_EXPIRE_DATE:
					textArray.emplace_back("expire_date = ?");

					break;
				default:
					break;
				}
			}
		}
		os << fmt::format("{}", fmt::join(textArray, ","));
		os << "WHERE uuid = ?;";
		upData.push_back(PRODUCT_UUID);

		auto stmt = mLocalDatabase->prepare(os.str());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& meta = mProductData->GetDatastore().get_metadata();
		size_t i = 1;
		for (auto& d: upData){
			auto kind = meta[d];
			switch (kind)
			{
			case pof::base::data::kind::int32:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::int32_t>(v[d]), i);
				break;
			case pof::base::data::kind::int64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::int64_t>(v[d]), i);
				break;
			case pof::base::data::kind::uint32:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint32_t>(v[d]), i);
				break;
			case pof::base::data::kind::uint64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint64_t>(v[d]), i);
				break;
			case pof::base::data::kind::float32:
				mLocalDatabase->bind(*stmt, boost::variant2::get<float>(v[d]), i);
				break;
			case pof::base::data::kind::float64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<double>(v[d]), i);
				break;
			case pof::base::data::kind::datetime:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::datetime_t>(v[d]), i);
				break;
			case pof::base::data::kind::text:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::text_t>(v[d]), i);
				break;
			case pof::base::data::kind::blob:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::blob_t>(v[d]), i);
				break;
			case pof::base::data::kind::uuid:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::duuid_t>(v[d]), i);
				break;
			case pof::base::data::kind::currency:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::currency_t>(v[d]), i);
				break;
			case pof::base::data::kind::null:
			default:
				break;
			}
			i++; //next bind statment
		}
		
		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	else {
		//
	}

	return true;
}

void pof::ProductManager::CreateHideTable()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS hidden (uuid blob UNIQUE);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

bool pof::ProductManager::HideProduct(const pof::base::data::duuid_t& uid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT OR IGNORE INTO hidden (uuid) VALUES (?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
}

bool pof::ProductManager::ShowProduct(const pof::base::data::duuid_t& uid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM hidden WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
}

std::optional<size_t> pof::ProductManager::GetHiddenCount()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT COUNT(*) FROM hidden;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		return rel->empty() ? std::nullopt : std::make_optional(std::get<0>(*(rel->begin())));
	}	
	return std::nullopt;
}

bool pof::ProductManager::CreateSupplierInvoiceTable()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS supplier (id integer, name text, date_created integer, date_modified integer, info text);)";
		constexpr const std::string_view sql2 = R"(CREATE TABLE IF NOT EXISTS invoice (supp_id integer, invoice_id text, prod_uuid blob, inventory_id integer);)";

		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);

		stmt = mLocalDatabase->prepare(sql2);
		assert(stmt);
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;

	}
	return false;
}

bool pof::ProductManager::LoadSuplliers()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT * FROM supplier;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto rel = mLocalDatabase->retrive<
			std::uint64_t, //supplier id
			pof::base::data::text_t, //supplier_name
			pof::base::data::datetime_t, //date created
			pof::base::data::datetime_t, //date modified
			pof::base::data::text_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}

		mLocalDatabase->finalise(*stmt);
		auto& data = mSuppliers->GetDatastore();
		data.reserve(rel->size());
		for (auto& r : *rel){
			auto p = pof::base::make_row_from_tuple(r);
			data.emplace(std::move(p));
		}
		//reload the itemIds 
		mSuppliers->Reload();
	}
	return false;
}



std::optional<std::vector<std::string>> pof::ProductManager::GetSupplierNames()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT name FROM supplier;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto rel = mLocalDatabase->retrive<std::string>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		std::vector<std::string> ret;
		ret.reserve(rel->size());
		for (auto&& tup : *rel) {
			ret.emplace_back(std::move(std::get<0>(tup)));
		}
		return ret;
	}
	return std::nullopt;
}

bool pof::ProductManager::AddSupplier(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(INSERT INTO supplier (id, name, date_created, date_modified, info) VALUES (?,?,?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		std::tuple<std::uint64_t, pof::base::data::text_t, pof::base::data::datetime_t, pof::base::data::datetime_t, pof::base::data::text_t> tup;
		pof::base::build(tup, *iter);
		bool status = mLocalDatabase->bind(*stmt, tup);
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status)
		{
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;

	}
	return false;
}

bool pof::ProductManager::RemoveSupplier(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM supplier WHERE id =?;)";
		constexpr const std::string_view sql2 = R"(DELETE FROM invoice WHERE supp_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		std::uint64_t id = boost::variant2::get<std::uint64_t>(iter->first[SUPPLIER_ID]);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(id));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status)
		{
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);

		stmt = mLocalDatabase->prepare(sql2);
		assert(stmt);
		status = mLocalDatabase->bind(*stmt, std::make_tuple(id));
		assert(status);
		status = mLocalDatabase->execute(*stmt);
		if (!status)
		{
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::ProductManager::UpdateSupplier(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase){
		constexpr static std::array<std::string_view, SUPPLIER_MAX> colName = {
			"id", "name", "date_created", "date_modified", "info"
		};
		std::ostringstream os;
		std::vector<size_t> upIdx;
		os << "UPDATE supplier SET ";
		auto& updateFlag = iter->second.second;
		for (int i = 0; i < SUPPLIER_MAX; i++) {
			if (updateFlag.test(i)) {
				if (upIdx.size() != 0) {
					os << ", ";
				}
				upIdx.push_back(i);
				os << colName[i] << "= ?";
			}
		}
		os << " WHERE id = ?;";
		upIdx.push_back(SUPPLIER_ID);
		auto stmt = mLocalDatabase->prepare(os.str());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		std::uint64_t id = boost::variant2::get<std::uint64_t>(iter->first[SUPPLIER_ID]);
		size_t i = 1;
		auto& meta = mSuppliers->GetDatastore().get_metadata();
		auto& v = iter->first;
		for (size_t d : upIdx) {
			auto kind = meta[d];
			switch (kind)
			{
			case pof::base::data::kind::uint64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint64_t>(v[d]), i);
				break;
			case pof::base::data::kind::datetime:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::datetime_t>(v[d]), i);
				break;
			case pof::base::data::kind::text:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::text_t>(v[d]), i);
				break;
			default:
				break;
			}
			i++;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status)
		{
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::RemoveProductFromInvoice(const boost::uuids::uuid& puid,std::uint64_t supid,  const std::string& invoiceid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(DELETE FROM invoice WHERE prod_uuid = ? AND invoice_id = ? AND supp_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid, invoiceid, supid));
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

std::optional<std::vector<std::string>> pof::ProductManager::GetInvoices(std::uint64_t suppId)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT DISTINCT invoice_id FROM invoice WHERE supp_id = ? LIMIT 1000;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(suppId));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::string>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		std::vector<std::string> ret;
		ret.reserve(rel->size());

		for (auto&& t : *rel) {
			ret.emplace_back(std::move(std::get<0>(t)));
		}
		return ret;
	}

	return std::nullopt;
}
bool pof::ProductManager::LoadInvoices(std::uint64_t sid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT supp_id, invoice_id, prod_uuid, inventory_id  FROM invoice WHERE supp_id = ? ORDER BY invoice_id ASC;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(sid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			std::uint64_t, //supplier_id
			pof::base::data::text_t, //invoice identifier
			pof::base::data::duuid_t, //product uuid
			std::uint64_t //inventory id
		>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);

		mInvoices->Clear();

		if (rel->empty()) return false;

		//make unique, this is O(n) which is very bad
		//need to come up with a better solution
		auto end = std::unique(rel->begin(), rel->end(), [&](const auto& a, const auto& b) -> bool {
			return std::get<1>(a) == std::get<1>(b);
		});
		rel->erase(end, rel->end());

		auto& datastore = mInvoices->GetDatastore();
		datastore.reserve(rel->size());
		for (auto&& tup : rel.value()){
			auto v = pof::base::make_row_from_tuple(std::move(tup));
			datastore.emplace(std::move(v));
		}

		mInvoices->Reload();
	}
	return false;
}
std::optional<pof::base::data> pof::ProductManager::LoadInvoiceDates(std::uint64_t sid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT ii.invoice_id, MIN(i.input_date) 
		FROM inventory i, invoice ii
		WHERE i.id = ii.inventory_id AND ii.supp_id = ? GROUP BY ii.invoice_id ORDER BY ii.invoice_id ASC;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(sid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::string, pof::base::data::datetime_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		pof::base::data ret;
		ret.get_metadata() = { pof::base::data::kind::text, pof::base::data::kind::datetime };
		ret.reserve(rel->size());
		for (auto&& tup : rel.value()) {
			auto v = pof::base::make_row_from_tuple(std::move(tup));
			ret.emplace(std::move(v));
		}
		return ret;
	}
	return std::nullopt;
}
bool pof::ProductManager::AddInvoice(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(INSERT INTO invoice (supp_id, invoice_id, prod_uuid, inventory_id) 
		VALUES (?,?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		std::tuple<
			std::uint64_t, //supplier_id
			pof::base::data::text_t, //invoice identifier
			pof::base::data::duuid_t, //product uuid
			std::uint64_t //inventory id
		> tup;
		pof::base::build(tup, *iter);
		bool status = mLocalDatabase->bind(*stmt, tup);
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::RemoveInvoice(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM invoice WHERE supp_id = ? AND invoice_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		const std::string& invoice_id = boost::variant2::get<std::string>(iter->first[pof::ProductManager::INVOICE_ID]);
		const std::uint64_t supp_id = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::INVOICE_SUPP_ID]);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(supp_id, invoice_id));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::UpdateInvoice(pof::base::data::const_iterator iter)
{
	return false;
}

bool pof::ProductManager::RemoveInventoryFromInvoice(pof::base::data::const_iterator iter, 
		std::uint64_t suppid, const std::string& inoice)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(DELETE FROM invoice WHERE prod_id = ? AND invoice_id = ? AND inventory_id = ? AND supp_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		const auto& v = iter->first;
		const auto& prod_id = boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::INVENTORY_PRODUCT_UUID]);
		const auto& inven_id = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::INVENTORY_ID]);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(prod_id, inoice, inven_id, suppid));
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

std::optional<pof::base::relation<pof::base::data::text_t, std::uint64_t, pof::base::currency, pof::base::data::datetime_t, pof::base::data::duuid_t>> 
	pof::ProductManager::GetProductsInInvoice(std::uint64_t suppid, const std::string& in)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT p.name, i.stock_count, CostMulti(i.cost, i.stock_count), i.input_date, p.uuid
		FROM products p, inventory i, invoice ii 
		WHERE p.uuid = ii.prod_uuid AND i.id = ii.inventory_id AND i.uuid = p.uuid AND ii.supp_id = ? AND ii.invoice_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(suppid, in));
		assert(stmt);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::text_t, 
			std::uint64_t, 
			pof::base::currency, 
			pof::base::data::datetime_t,
			pof::base::data::duuid_t>(*stmt);
		if (!rel.has_value()) {
			mLocalDatabase->finalise(*stmt);
		}
		mLocalDatabase->finalise(*stmt);
		return rel;
	}
	return std::nullopt;
}

std::optional<bool> pof::ProductManager::CheckIfProductInInvoice(std::uint64_t suppid, const std::string& in, const pof::base::data::duuid_t& puid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT 1 FROM invoice WHERE supp_id = ? AND invoice_id = ? AND prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(suppid, in, puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		return !rel->empty(); // if select it means product is in this invoice in this supplier
	}
	return std::nullopt;
}

bool pof::ProductManager::RemoveProductData(pof::base::data::const_iterator iter)
{
	if (iter == mProductData->GetDatastore().end()) return false;
	auto& v = iter->first;
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!productRemoveStmt) {
			static constexpr const std::string_view sql = "DELETE FROM products WHERE uuid = ?;";
			auto stmt = mLocalDatabase->prepare(sql);
			if (stmt.has_value()) productRemoveStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(productRemoveStmt, boost::variant2::get<pof::base::data::duuid_t>(v[PRODUCT_UUID]), 1);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		
		status = mLocalDatabase->execute(productRemoveStmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

	}
	return true;
}

bool pof::ProductManager::RemoveProductInOrderListData(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM order_list WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInWarningsData(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM warning WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInventoryData(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM inventory WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInStockCheckData(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM stock_check WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInPacks(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM pack_product WHERE product_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInExpiredStock(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM expired_stock WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInMedication(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(DELETE FROM medications WHERE product_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInPoisonBook(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM poison_book WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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

bool pof::ProductManager::RemoveProductInInvoice(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(DELETE FROM invoice WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(boost::variant2::get<boost::uuids::uuid>(iter->first[pof::ProductManager::PRODUCT_UUID])));
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


bool pof::ProductManager::OnStoreProductData(pof::base::data::const_iterator iter)
{
	if (!productOnStoreStmt) {
		static constexpr const std::string_view sql =
			"INSERT INTO products (uuid, serail_num, name, generic_name, class, formulation, strength, strength_type, usage_info, descrip, health_condition, unit_price, cost_price, package_size, stock_count, side_effects, barcode, category, min_stock_count, expire_period, expire_date )"
			" VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ? , ? , ?, ? ); ";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		productOnStoreStmt = *stmt;
	}
	//iter is the start of where the data is loaded from

	pof::ProductManager::relation_t rel;
	rel.reserve(std::distance(iter, std::cend(mProductData->GetDatastore())));
	while (iter != mProductData->GetDatastore().end()) {
		relation_t::tuple_t tup;
		auto& v = iter->first;
		try {
			pof::base::build(tup, *iter);
			rel.emplace_back(std::move(tup));
		}
		catch (const std::exception& exp) {
			spdlog::critical(exp.what());
			return false;
		}
		iter++;
	}
	bool status = mLocalDatabase->store(productOnStoreStmt, std::move(rel));
	if (!status) {
		spdlog::error(mLocalDatabase->err_msg());
		mLocalDatabase->finalise(productOnStoreStmt);
		productOnStoreStmt = nullptr;
		return false;
	}
	return true;
}

bool pof::ProductManager::StoreInventoryData(pof::base::data::const_iterator iter)
{
	if (iter == mInventoryData->GetDatastore().end()) return false;
	auto& v = iter->first;
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!InventoryStoreStmt) {
			static constexpr const std::string_view sql = 
				"INSERT INTO inventory (id, uuid, expire_date, input_date, stock_count, cost, manufacturer_name, manufacturer_address_id, lot_number) VALUES (?,?,?,?,?,?,?,?,?);";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			InventoryStoreStmt = *stmt;
		}
		using tuple_t = std::tuple<
			std::uint64_t, //ID
			pof::base::data::duuid_t, //UUID
			pof::base::data::datetime_t, // EXPIRE DATE
			pof::base::data::datetime_t, // ADDED DATE
			std::uint64_t, // STOCK COUNT
			pof::base::data::currency_t, //CURRENCY
			pof::base::data::text_t, // MANUFACTURE NAME
			std::uint64_t,  // MANUFACTURER ADDRESS ID
			pof::base::data::text_t // LOT NUMBER/ BATCH NUMBER
		>;
		tuple_t tup;
		bool status = pof::base::build(tup, *iter);
		status = mLocalDatabase->bind(InventoryStoreStmt, std::move(tup));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(InventoryStoreStmt);
			InventoryStoreStmt = nullptr;
			return false;
		}
		status = mLocalDatabase->execute(InventoryStoreStmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			InventoryStoreStmt = nullptr;
			return false;
		}
	}
	return true;
}

bool pof::ProductManager::UpdateInventoryData(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase){
		constexpr const std::array<std::string_view, pof::ProductManager::INVENTORY_MAX> names
			= { "id", "uuid", "expire_date", "input_date", "stock_count", "cost", "manufacturer_name", "manufacturer_address_id", "lot_number"};
		
		std::ostringstream str;
		std::vector<size_t> upIdx;
		auto& updateFlag = iter->second.second;
		
		str << "UPDATE inventory SET ";
		for (int i = 1; i < names.size(); i++) {
			if (updateFlag.test(i)) {
				if (upIdx.size() != 0) {
					str << ", ";
				}
				upIdx.push_back(i);
				str << names[i] << "= ?";
			}
		}
		str << " WHERE id = ?;";
		upIdx.push_back(pof::ProductManager::INVENTORY_ID);
		auto stmt = mLocalDatabase->prepare(str.str());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		std::uint64_t id = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::INVENTORY_ID]);
		size_t i = 1;
		auto& meta = mInventoryData->GetDatastore().get_metadata();
		auto& v = iter->first;
		for (size_t d : upIdx) {
			auto kind = meta[d];
			switch (kind)
			{
			case pof::base::data::kind::uuid:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::duuid_t>(v[d]), i);
				break;
			case pof::base::data::kind::currency:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::currency_t>(v[d]), i);
				break;
			case pof::base::data::kind::uint64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint64_t>(v[d]), i);
				break;
			case pof::base::data::kind::datetime:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::datetime_t>(v[d]), i);
				break;
			case pof::base::data::kind::text:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::text_t>(v[d]), i);
				break;
			default:
				break;
			}
			i++;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status)
		{
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}

	return false;
}

bool pof::ProductManager::RemoveInventoryData(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM inventory WHERE id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		auto& row = *iter;
		auto& id = boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::INVENTORY_ID]);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(id));
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

std::optional<std::uint64_t> pof::ProductManager::GetLastInventoryId(const pof::base::data::duuid_t& uid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT id, MAX(input_date) FROM inventory WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t, pof::base::data::datetime_t>(*stmt);
		mLocalDatabase->finalise(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		return std::get<0>(*rel->begin());
	}
	return std::nullopt;
}

std::optional<pof::base::data::datetime_t> pof::ProductManager::GetLastInventoryDate(const pof::base::data::duuid_t& uid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT Min(Months(input_date)) FROM inventory WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uid));
		assert(status);
	
		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return std::nullopt;

		return std::get<0>(*(rel->begin()));
	}
	return std::nullopt;
}

std::optional<pof::base::data::text_t> pof::ProductManager::GetLastInventoryBatchNumber(const pof::base::data::duuid_t& uid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT lot_number, MAX(input_date) FROM inventory WHERE uuid = ? LIMIT 1;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uid));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::text_t, pof::base::data::datetime_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return pof::base::data::text_t{}; //return an empty string on empty load
		return std::get<0>(*(rel->begin()));
	}
	return std::nullopt;
}

std::optional<pof::base::data::text_t> pof::ProductManager::GetLastInventorySupplierName(const pof::base::data::duuid_t& uid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT manufacturer_name, MAX(input_date) FROM inventory WHERE uuid = ? LIMIT 1;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(uid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::string, pof::base::data::datetime_t>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return std::nullopt; //should never be empty
		return std::get<0>(*(rel->begin()));

	}
	return std::nullopt;
}

void pof::ProductManager::AddCategory(const std::string& name)
{
	if (name.empty()) return;
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!CategoryStoreStmt) {
			constexpr const std::string_view sql = "INSERT INTO category (id, name) VALUES (? ,?);";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return;
			}
			CategoryStoreStmt = *stmt;
		}
		std::uint64_t id = 0;
		if (!mCategories.empty()) {
			auto& r = mCategories.back().first;
			//std::uint64_t id = boost::variant2::get<std::uint64_t>(r[CATEGORY_ID]) + 1; 
			id = boost::variant2::get<std::uint64_t>(r[CATEGORY_ID]);
		}
		bool status = mLocalDatabase->bind(CategoryStoreStmt, std::tie(id, name));
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(CategoryStoreStmt);
			CategoryLoadStmt = nullptr;
			return;
		}
		status = mLocalDatabase->execute(CategoryStoreStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			CategoryStoreStmt = nullptr;
			return;
		}
	}
	else {
		//network
	}
}

void pof::ProductManager::RemoveCategory(const std::string& name)
{
	auto iter = std::ranges::find(mCategories, name, [&](const pof::base::data::row_t& row) -> pof::base::data::text_t {
		return boost::variant2::get<pof::base::data::text_t>(row.first[CATEGORY_NAME]);
	});
	if (iter == std::end(mCategories)) return; //does not exists
	if (mLocalDatabase){
		constexpr const std::string_view sql = "UPDATE products set category = ? WHERE category = ?;";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		std::uint64_t id = boost::variant2::get<std::uint64_t>(iter->first[CATEGORY_ID]);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(0, id));
		assert(status);

		std::ranges::for_each(mProductData->GetDatastore(), [&](pof::base::data::row_t& row) {
			if (boost::variant2::get<std::uint64_t>(row.first[PRODUCT_CATEGORY]) == id) {
				row.first[PRODUCT_CATEGORY] = static_cast<std::uint64_t>(0);
			}
		});
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return;
		}
		mLocalDatabase->finalise(*stmt);

		constexpr const std::string_view ss = "DELETE FROM category WHERE id = ?;";
		stmt = mLocalDatabase->prepare(ss);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}

		status = mLocalDatabase->bind(*stmt, std::make_tuple(id));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return;
		}
		mLocalDatabase->finalise(*stmt);
		mCategories.erase(iter);
	}
}

void pof::ProductManager::UpdateCategory(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase) {
		//can only name can change
		if (!CategoryUpdateStmt) {
			constexpr const std::string_view sql = "UPDATE category set name = ? WHERE id = ?;";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return;
			}
			CategoryUpdateStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(CategoryUpdateStmt, 
			std::make_tuple(boost::variant2::get<pof::base::data::text_t>(iter->first[CATEGORY_NAME]),
				boost::variant2::get<std::uint64_t>(iter->first[CATEGORY_ID])));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		status = mLocalDatabase->execute(CategoryUpdateStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
	}
}

size_t pof::ProductManager::GetInentoryCount(const pof::base::data::duuid_t& ud)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT COUNT(id) FROM inventory WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return 0;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(ud));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return 0;
		}
		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return 0;
		}
		if (rel->empty()) return 0;
		else {
			return std::get<0>((*rel)[0]);
		}
	}
	return size_t(0);
}

void pof::ProductManager::UpdateProductQuan(const std::vector<std::tuple<pof::base::data::duuid_t, std::uint64_t>>& prodQuans)
{
	if (mLocalDatabase)
	{
		if (!UpdateProductQuanStmt) {
			constexpr const std::string_view sql = R"(UPDATE products set stock_count = ? WHERE uuid = ?;)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return;
			}
			UpdateProductQuanStmt = *stmt;
		}
		mLocalDatabase->begin_trans();
		for (auto& tup : prodQuans) {
			bool status = mLocalDatabase->bind(UpdateProductQuanStmt, std::make_tuple(std::get<1>(tup), std::get<0>(tup)));
			if (!status) {
				spdlog::error(mLocalDatabase->err_msg());
				break;
			}
			status = mLocalDatabase->execute(UpdateProductQuanStmt);
			if (!status) {
				spdlog::error(mLocalDatabase->err_msg());
				break;
			}
		}
		mLocalDatabase->end_trans();
	}

}

void pof::ProductManager::RefreshRowFromDatabase(const pof::base::data::duuid_t& pid, pof::base::data::row_t& row)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT * FROM products WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t, //UUID
			std::uint64_t, //SERIAL NUM
			pof::base::data::text_t, // NAME
			pof::base::data::text_t, // GENERIC NAME
			pof::base::data::text_t, //CLASS
			pof::base::data::text_t, //FORMULATION
			pof::base::data::text_t, // STRENGTH
			pof::base::data::text_t, // STRENGTH TYPE
			pof::base::data::text_t, // USAGE INFO
			pof::base::data::text_t, // PRODUCT DESCRIPTION
			pof::base::data::text_t, // PRODUCT HEALTH CONDITIONS COMMA SEPERATED
			pof::base::data::currency_t, // UNIT PRICE
			pof::base::data::currency_t, // COST PRICE
			std::uint64_t, //PACAKGE SIZE
			std::uint64_t, //STOCK COUNT
			pof::base::data::text_t, //SIDE EFFECTS
			pof::base::data::text_t, //BARCODE
			std::uint64_t, //CATEGORY ID
			//PRODUCT SETTINGS
			std::uint32_t, //MIN_STOCJ_COUNT
			pof::base::data::text_t, //EXPIRE PERIOD
			std::uint64_t //EXPIRE DATE
		>(*stmt);
		assert(rel);

		if (rel->empty()) return;
		auto& tup = *(rel->begin());
		row.first = std::move(pof::base::make_row_from_tuple(tup));
	}
}

void pof::ProductManager::EmplaceProductData(pof::base::data&& data)
{
	mProductData->Emplace(std::forward<pof::base::data>(data));
}

void pof::ProductManager::StoreProductData(pof::base::data&& data)
{
	mProductData->Store(std::forward<pof::base::data>(data));
}
// AND uuid NOT IN (SELECT uuid FROM inventory WHERE Months(input_date) = ?)
//this does not make any sense
void pof::ProductManager::InventoryBroughtForward()
{
	if (mLocalDatabase){
		//why do you need to bring anything forward, to start the inventory with the opening stock, the current stock count
		constexpr const std::string_view sql = R"(SELECT p.uuid,
		CASE 
		WHEN sc.status = 1 THEN sc.check_stock
		ELSE p.stock_count
		END
		FROM products p, stock_check sc
		WHERE p.uuid = sc.prod_uuid AND Months(sc.date) = ?;)"; //stock check for the last month

		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		auto lastMonth = std::chrono::duration_cast<date::months>(pof::base::data::clock_t::now().time_since_epoch()) - date::months(1);
		mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(lastMonth)));

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t, std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->roll_back(*stmt);
			mLocalDatabase->finalise(*stmt);
			return;
		}
		mLocalDatabase->finalise(*stmt);

		 constexpr const std::string_view isql = R"(INSERT INTO inventory
         (id, uuid, expire_date, input_date, stock_count, cost, manufacturer_name, manufacturer_address_id, lot_number) 
         VALUES (? , ? , ? , ? , ? , ? , ? , ? , ? ); )";
		stmt = mLocalDatabase->prepare(isql);
		assert(stmt);

		bool status = false;
		for (auto& tup : rel.value()) {
			//get the next lot number 
			std::string lotNumber = "0"s;
			auto expireDate = GetCurrentExpireDate(std::get<0>(tup));
			if (!expireDate.has_value()) continue;

			if (wxGetApp().bAutomaticBatchNumber) {
				auto lotNum = GetLastInventoryBatchNumber(std::get<0>(tup));
				//only work if we get valid data from the database
				if (lotNum) {
					auto& v = lotNum.value();
					if (std::ranges::all_of(v, [&](char c) -> bool {return std::isdigit(c); })) {
						//if it a digit
						try {
							size_t c = boost::lexical_cast<size_t>(v);
							c++;
							lotNumber = std::move(std::to_string(c));
						}
						catch (std::exception& exp) {
							spdlog::error(exp.what());
						}
					}
				}
			}
			status = mLocalDatabase->bind(*stmt, 
					std::make_tuple(pof::GenRandomId(), std::get<0>(tup), expireDate.value(),
						pof::base::data::clock_t::now(), std::get<1>(tup), pof::base::data::currency_t{}, "B/F"s, 0, std::move(lotNumber)));
			if (!status) continue;
			mLocalDatabase->execute(*stmt);
		}
	}
}

//markup is a pecentage in float so markUp = markup/100
void pof::ProductManager::MarkUpProducts()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE products SET unit_price = ScaleCost(cost_price,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(gMarkup));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

void pof::ProductManager::MarkUpProducts(const pof::base::data::duuid_t& uid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(UPDATE products SET unit_price = ScaleCost(cost_price, ?) WHERE uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(gMarkup, uid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

void pof::ProductManager::AddProductData()
{
}

bool pof::ProductManager::CreateExpiredStockTable()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS expired_stock (prod_uuid blob, stock integer, date integer);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::ProductManager::MoveStockToExpire(const pof::base::data::duuid_t& pid, std::uint64_t stock)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO expired_stock (prod_uuid, stock, date) VALUES (?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, stock, pof::base::data::clock_t::now()));
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

std::optional<std::uint64_t> pof::ProductManager::GetTotalExpired(const pof::base::data::duuid_t& pid, pof::base::data::datetime_t date)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT SUM(stock) FROM expired_stock 
		WHERE Months(date) = ? AND prod_uuid = ?
		GROUP BY prod_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		auto month = std::chrono::duration_cast<date::months>(date.time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(month), pid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		return (std::get<0>(*rel->begin()));
	}
	return std::nullopt;
}

std::optional<std::vector<std::pair<pof::base::data::duuid_t, std::uint64_t>>> pof::ProductManager::GetExpiredProductsStock(pof::base::data::datetime_t m)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT prod_uuid, SUM(stock) 
		FROM  expired_stock 
		WHERE Months(date) = ?
		GROUP BY prod_uuid;)";

		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto month = std::chrono::duration_cast<date::months>(m.time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(m));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t, std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		std::vector<std::pair<pof::base::data::duuid_t, std::uint64_t>> ret;
		ret.reserve(rel->size());

		for (auto& tup : *rel){
			ret.emplace_back(std::make_pair(std::get<0>(tup), std::get<1>(tup)));
		}
		ret.shrink_to_fit();
		return ret;
	}
	return std::nullopt;
}

std::optional<pof::base::data> pof::ProductManager::GetEndOfDay(pof::base::data::datetime_t dt)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.uuid, s.sale_date, p.name,s.product_quantity, s.product_ext_price, s.uuid, s.sale_payment_type, s.ROWID
		FROM sales s, products p
		WHERE s.product_uuid = p.uuid AND Days(s.sale_date) = ? ORDER BY s.sale_date;  )";
		
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		auto days = std::chrono::floor<std::chrono::days>(dt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::int64_t>(days.time_since_epoch().count())));
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::datetime_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			std::uint64_t
		>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		
		pof::base::data data;
		data.set_metadata({
			pof::base::data::kind::uuid,
			pof::base::data::kind::datetime,
			pof::base::data::kind::text,
			pof::base::data::kind::uint64,
			pof::base::data::kind::currency,
			pof::base::data::kind::uuid,
			pof::base::data::kind::text,
			pof::base::data::kind::uint64
		});
		auto& v = rel.value();
		data.reserve(v.size());
		for (auto& tup : v) {
			data.insert(pof::base::make_row_from_tuple(tup));
		}
		data.shrink_to_fit();
		return data;
	}
	return std::nullopt;
}

//customization points to allow consumption pattern for differend months, product formulation
std::optional<pof::base::data> pof::ProductManager::GetConsumptionPattern(pof::base::data::datetime_t m)
{
	if (mLocalDatabase){
		//product name 
		//openning stock, first stock entry of the month
		//total outgoing stock
		//total incoming came in, this is plus opening stock
		//total amount that left the pharmacy,
		//total amount spent on the product
		//use aggregate functions
		constexpr const std::string_view sql = R"(SELECT p.uuid, p.name, sc.stock_count,
		    0,
			SUM(iv.stock_count), 
			SumCost(CostMulti(iv.cost, iv.stock_count))
			FROM inventory iv, products p, stock_check sc
			WHERE Months(iv.input_date) = :mo 
			AND Months(sc.date) = :mo
			AND p.uuid = iv.uuid
			AND p.uuid = sc.prod_uuid
			AND sc.status = 1
			GROUP BY p.uuid;)";
		
		constexpr const std::string_view salesql = R"(SELECT p.uuid, SUM(s.product_quantity), SumCost(s.product_ext_price)
			FROM products p, sales s
			WHERE Months(s.sale_date) = ? AND s.product_uuid = p.uuid
			GROUP BY p.uuid;)";

		constexpr const std::string_view expiresql = R"(SELECT prod_uuid, SUM(stock) 
		FROM  expired_stock 
		WHERE Months(date) = ?
		GROUP BY prod_uuid;)";

		auto stmt = mLocalDatabase->prepare(sql);
		auto stmt2 = mLocalDatabase->prepare(salesql);
		auto stmt3 = mLocalDatabase->prepare(expiresql);

		if (!stmt.has_value() || !stmt2.has_value() || !stmt3.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}


		auto month = std::chrono::duration_cast<date::months>(m.time_since_epoch());
		spdlog::info("month count {:d}", month.count());
		bool status = mLocalDatabase->bind_para(*stmt, std::make_tuple(pof::base::data::datetime_t(month)), { "mo" });
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::currency_t
		>(*stmt);

		status = mLocalDatabase->bind(*stmt2, std::make_tuple(pof::base::data::datetime_t(month)));
		assert(status);

		auto rel2 = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			std::uint64_t,
			pof::base::data::currency_t
		>(*stmt2);

		status = mLocalDatabase->bind(*stmt3, std::make_tuple(pof::base::data::datetime_t(month)));
		assert(status);

		auto rel3 = mLocalDatabase->retrive<pof::base::data::duuid_t, std::uint64_t>(*stmt3);

		if (!rel.has_value() || !rel2.has_value() || !rel3.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			mLocalDatabase->finalise(*stmt2);
			mLocalDatabase->finalise(*stmt3);
			return std::nullopt;
		}

		pof::base::data data;
		data.set_metadata({
			pof::base::data::kind::uuid,
			pof::base::data::kind::text, //name
			pof::base::data::kind::uint64, //closing stock
			pof::base::data::kind::uint64, //expire
			pof::base::data::kind::uint64, // inventory in
			pof::base::data::kind::currency, //amount in
			pof::base::data::kind::uint64, //inventory out
			pof::base::data::kind::currency //amount out 
		});
		spdlog::info("The size is : {:d}", rel->size());
		std::unordered_map<pof::base::data::duuid_t, size_t> mSumMap;
		data.reserve(rel->size());
		size_t i = 0;
		for(auto& tup : *rel){
			pof::base::data::row_t::first_type row;
			row.resize(8);

			pof::base::data::row_t::first_type row2 = std::move(pof::base::make_row_from_tuple(tup));
			
			std::move(row2.begin(), row2.end(), row.begin());
			row[6] = static_cast<std::uint64_t>(0);
			row[7] = pof::base::data::currency_t{};
			data.insert(std::move(row));

			mSumMap.insert({ std::get<0>(tup), i });
			i++;
		}

		//get sale data
		for (auto& tup : *rel2) {
			auto iter = mSumMap.find(std::get<0>(tup));
			if (iter == mSumMap.end()) continue;
			auto& v = data[iter->second];
			v.first[6] = (std::get<1>(tup));
			v.first[7] = (std::get<2>(tup));
		}

		//get expire data
		for (auto& tup : *rel3) {
			auto iter = mSumMap.find(std::get<0>(tup));
			if (iter == mSumMap.end()) continue;
			auto& v = data[iter->second];
			v.first[3] = (std::get<1>(tup));
		}

		data.shrink_to_fit();

		mLocalDatabase->finalise(*stmt);
		mLocalDatabase->finalise(*stmt2);
		mLocalDatabase->finalise(*stmt3);

		return data;
	}
	return std::nullopt;
}

std::optional<pof::base::data> pof::ProductManager::GetEndOfMonth(pof::base::data::datetime_t m)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.uuid, s.sale_date, p.name,s.product_quantity, s.product_ext_price, s.uuid, s.sale_payment_type
		FROM sales s, products p
		WHERE s.product_uuid = p.uuid AND Months(s.sale_date) = ? ORDER BY s.sale_date;)";

		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		const std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(m) };
		int out = (static_cast<int>(ymd.year()) - 1970) * 12 + (static_cast<unsigned>(ymd.month()) - 1);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(out)));
		assert(status);


		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::datetime_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);

		pof::base::data data;
		data.set_metadata({
			pof::base::data::kind::uuid,
			pof::base::data::kind::datetime,
			pof::base::data::kind::text,
			pof::base::data::kind::uint64,
			pof::base::data::kind::currency,
			pof::base::data::kind::uuid,
			pof::base::data::kind::text
			});
		auto& v = rel.value();
		data.reserve(v.size());
		for (auto& tup : v) {
			data.insert(pof::base::make_row_from_tuple(tup));
		}
		data.shrink_to_fit();
		return data;

	}
	return std::nullopt;
}

std::optional<std::vector<wxDataViewItem>> pof::ProductManager::DoExpireProductPeriod()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.uuid 
		FROM products p, inventory i 
		WHERE (p.uuid, i.input_date) IN (
			SELECT i.uuid, MAX(i.input_date)
			FROM inventory i
			WHERE i.uuid NOT NULL
			GROUP BY i.uuid
		)
		AND ? + p.expire_date > in.expire_date;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		pof::base::data::datetime_t today = pof::base::data::clock_t::now();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(today));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t>(*stmt);
		if (!rel) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}

		std::set<pof::base::data::duuid_t> set;
		std::ranges::transform(*rel, std::inserter(set, set.end()), [&](auto& tup) -> pof::base::data::duuid_t {
			return std::get<0>(tup);
			});
		auto& datastore = mProductData->GetDatastore();
		std::vector<wxDataViewItem> items;
		items.reserve(rel->size());
		for (size_t i = 0; i < datastore.size(); i++) {
			auto& uid = boost::variant2::get<pof::base::data::duuid_t>(datastore[i].first[PRODUCT_UUID]);
			if (set.find(uid) != set.end()) {
				items.emplace_back(pof::DataModel::GetItemFromIdx(i));
			}
		}
		items.shrink_to_fit();
		return items;
	}
	return std::nullopt;
}

auto pof::ProductManager::DoExpiredProducts() -> std::optional<std::vector<wxDataViewItem>>
{
	if (mLocalDatabase) {
		if (!ExpireProductStmt) {
			constexpr const std::string_view sql = R"(
			SELECT p.uuid
		    FROM products p, inventory i
			WHERE (p.uuid, i.input_date) IN 
					(
						SELECT i.uuid, MAX(i.input_date)
						FROM inventory i
						WHERE i.uuid NOT NULL
						GROUP BY i.uuid
					)  
			AND Days(?) > Days(i.expire_date) AND p.stock_count > 0 AND p.uuid = i.uuid; )";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return std::nullopt;
			}
			ExpireProductStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(ExpireProductStmt, std::make_tuple(pof::base::data::clock_t::now()));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return  std::nullopt;
		}

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t>(ExpireProductStmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		if (rel->empty()) return std::vector<wxDataViewItem>{};
		
		std::set<pof::base::data::duuid_t> set;
		std::ranges::transform(*rel, std::inserter(set, set.end()), [&](auto& tup) -> pof::base::data::duuid_t {
			return std::get<0>(tup);
		});
		auto& datastore = mProductData->GetDatastore();
		std::vector<wxDataViewItem> items;
		items.reserve(rel->size());
		for (size_t i = 0; i < datastore.size(); i++){
			auto& uid = boost::variant2::get<pof::base::data::duuid_t>(datastore[i].first[PRODUCT_UUID]);
			if (set.find(uid) != set.end()) {
				auto& name = boost::variant2::get<pof::base::data::text_t>(datastore[i].first[PRODUCT_NAME]);
				//spdlog::info("{} has expired", name);
				items.emplace_back(pof::DataModel::GetItemFromIdx(i));
			}
		}
		items.shrink_to_fit();
		return items;
	}
	return std::nullopt;
}

std::optional<std::vector<wxDataViewItem>> pof::ProductManager::DoExpiredProducts(std::chrono::months months)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(
			SELECT p.uuid
		    FROM products p, inventory i
			WHERE (p.uuid, i.input_date) IN 
					(
						SELECT i.uuid, MAX(i.input_date)
						FROM inventory i
						WHERE i.uuid NOT NULL
						GROUP BY i.uuid
					)  
			AND (:m = Months(i.expire_date)) AND p.stock_count > 0 AND p.uuid = i.uuid; )";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		auto m = std::chrono::system_clock::now() + months;
		spdlog::info("expire month to check {:%m/%Y}", m);
		const std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(m) };
		int out = (static_cast<int>(ymd.year()) - 1970) * 12 + (static_cast<unsigned>(ymd.month()) - 1);

		bool status = mLocalDatabase->bind_para(*stmt, std::make_tuple(static_cast<std::uint64_t>(out)), {"m"});
		assert(status);
		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t>(*stmt);

		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);

		if (rel->empty()) return std::vector<wxDataViewItem>{};

		std::set<pof::base::data::duuid_t> set;
		std::ranges::transform(*rel, std::inserter(set, set.end()), [&](auto& tup) -> pof::base::data::duuid_t {
			return std::get<0>(tup);
			});
		auto& datastore = mProductData->GetDatastore();
		std::vector<wxDataViewItem> items;
		items.reserve(rel->size());
		for (size_t i = 0; i < datastore.size(); i++) {
			auto& uid = boost::variant2::get<pof::base::data::duuid_t>(datastore[i].first[PRODUCT_UUID]);
			if (set.find(uid) != set.end()) {
				auto& name = boost::variant2::get<pof::base::data::text_t>(datastore[i].first[PRODUCT_NAME]);
				spdlog::info("{} has expired", name);
				items.emplace_back(pof::DataModel::GetItemFromIdx(i));
			}
		}
		items.shrink_to_fit();
		return items;
	}
}

std::optional<std::vector<wxDataViewItem>> pof::ProductManager::DoOutOfStock()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.uuid FROM products p WHERE p.stock_count = 0; )";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto rel = mLocalDatabase->retrive<pof::base::data::duuid_t>(*stmt);
		assert(rel);

		if (rel->empty()) return std::vector<wxDataViewItem>{};

		std::set<pof::base::data::duuid_t> set;
		std::ranges::transform(*rel, std::inserter(set, set.end()), [&](auto& tup) -> pof::base::data::duuid_t {
			return std::get<0>(tup);
		});
		auto& datastore = mProductData->GetDatastore();
		std::vector<wxDataViewItem> items;
		items.reserve(rel->size());
		for (size_t i = 0; i < datastore.size(); i++) {
			auto& uid = boost::variant2::get<pof::base::data::duuid_t>(datastore[i].first[PRODUCT_UUID]);
			if (set.find(uid) != set.end()) {
				items.emplace_back(pof::DataModel::GetItemFromIdx(i));
			}
		}
		items.shrink_to_fit();
		return items;

	}
	return std::nullopt;
}

std::optional<pof::base::data::datetime_t> pof::ProductManager::GetCurrentExpireDate(const pof::base::data::duuid_t& prod)
{
	if (mLocalDatabase) {
		if (!CurExpireDateStmt) {
			constexpr const std::string_view sql = R"(
					SELECT i.expire_date
					FROM inventory i
					WHERE i.input_date = 
						(
							SELECT MAX(i.input_date)
							FROM inventory i
							WHERE i.uuid = ? 
						);
			)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return std::nullopt;
			}
			CurExpireDateStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(CurExpireDateStmt, std::make_tuple(prod));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t>(CurExpireDateStmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		if (rel->empty()) return std::nullopt; //or return what ?? 
		return std::get<0>(*rel->begin());
	}
	return std::nullopt;
}

std::optional<pof::base::currency> pof::ProductManager::GetTotalStockCost()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT SumCost(CostMulti(p.unit_price, p.stock_count)) FROM products p;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		auto rel = mLocalDatabase->retrive<pof::base::currency>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return std::nullopt;
		return std::get<0>(*(rel->begin()));
	}
	return std::nullopt;
}

bool pof::ProductManager::CaptureStock(const pof::base::data::duuid_t& pid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE stock_check SET stock_count = 
			(SELECT stock_count FROM products WHERE uuid = :pid) WHERE prod_uuid = :pid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind_para(*stmt, std::make_tuple(pid), { "pid" });
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

bool pof::ProductManager::CreateStockCheckTable()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS stock_check (prod_uuid blob, check_stock integer, stock_count, status integer, date integer);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

std::unique_ptr<pof::DataModel>& pof::ProductManager::GetStockCheckData()
{
	// TODO: insert return statement here
	return mStockCheckData;
}

void pof::ProductManager::LoadStockCheckDate(pof::base::data::datetime_t m)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT p.uuid, p.name, 
		CASE 
		 WHEN sc.status = 1 THEN sc.stock_count
		 WHEN sc.status = 0 THEN p.stock_count
		END AS stock,
		CASE
		 WHEN sc.status = 1 THEN sc.check_stock 
		 WHEN sc.status = 0 AND (sc.check_stock < p.stock_count OR sc.check_stock = p.stock_count) THEN sc.check_stock
		 ELSE 0
		END AS checkcol, 
		sc.status, 
		sc.date
		FROM products p, stock_check sc
		WHERE p.uuid = sc.prod_uuid AND Months(sc.date) = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		auto month = date::floor<date::months>(m);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(month.time_since_epoch().count())));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t, // product uuid
			pof::base::data::text_t, //product name
			std::uint64_t, //captured stock
			std::uint64_t, //checked stock
			std::uint64_t, //status
			pof::base::data::datetime_t
		>(*stmt);
		assert(rel);
		auto& value = rel.value();
		mStockCheckData->Clear();
		for (auto& tup : value){
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(tup));
			mStockCheckData->EmplaceData(std::move(row));
		}
	}
}

void pof::ProductManager::SaveStockCheckData()
{
}

bool pof::ProductManager::CheckIfAdded(const pof::base::data::duuid_t& pid)
{
	if (mLocalDatabase)
	{
		//figure how to do this with tiggers 
		constexpr const std::string_view sql = R"(SELECT 1 FROM stock_check WHERE prod_uuid = ? AND Months(date) = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		auto month = std::chrono::duration_cast<date::months>(pof::base::data::clock_t::now().time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, pof::base::data::datetime_t(month)));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel);
		mLocalDatabase->finalise(*stmt);
		return !rel->empty();
	}
	return false;
}

bool pof::ProductManager::IsStockCheckComplete()
{
	//check if all the 
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT Count(pid)
		FROM stock_check WHERE Months(date) = ? AND status = 1;)"; // 1 means done
		
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		//let us do this, check if we are in this month, 
		auto month = std::chrono::duration_cast<date::months>(pof::base::data::clock_t::now().time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(month)));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);

		//all products have been accounted for this month
		return (mProductData->GetDatastore().size() == std::get<0>(*(rel->begin())));

	}
	return false;
}

bool pof::ProductManager::IsStockCheckComplete(const pof::base::data::datetime_t& month)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT Count(pid)
		FROM stock_check WHERE Months(date) = ? AND status = 1;)"; // 1 means done

		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		//let us do this, check if we are in this month, 
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(month)));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);

		//all products have been accounted for this month
		return (mProductData->GetDatastore().size() == std::get<0>(*(rel->begin())));

	}
	return false;
}

void pof::ProductManager::UpdateStockCheck(const pof::base::data::duuid_t& pid, std::uint64_t stock)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE stock_check SET check_stock = ? WHERE prod_uuid = ? AND Months(date) = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt){
			spdlog::error(mLocalDatabase->err_msg());
			return; 
		}
		auto month = date::floor<date::months>(pof::base::data::clock_t::now());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(stock, pid, month.time_since_epoch().count()));
		assert(status);
		status = mLocalDatabase->execute(*stmt);
		if (!status) spdlog::error(mLocalDatabase->err_msg());
		mLocalDatabase->finalise(*stmt);
		return;
	}
}

bool pof::ProductManager::InsertProductInStockCheck(const pof::base::data::duuid_t& pid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO stock_check (prod_uuid, check_stock, stock_count, status, date) VALUES (?, 0, 0, 0, ?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto today = pof::base::data::clock_t::now();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, today));
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

void pof::ProductManager::LoadStockDataByCategory(pof::base::data::datetime_t m, std::uint64_t catID)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT p.uuid, p.name, 
		CASE 
		 WHEN sc.status = 1 THEN sc.stock_count
		 WHEN sc.status = 0 THEN p.stock_count
		END AS stock,
		CASE
		 WHEN sc.status = 1 THEN sc.check_stock 
		 WHEN sc.status = 0 AND (sc.check_stock < p.stock_count OR sc.check_stock = p.stock_count) THEN sc.check_stock
		 ELSE 0
		END AS checkcol,
		sc.status, 
		sc.date
		FROM products p, stock_check sc
		WHERE  Months(sc.date) = ? AND p.category = ? AND p.uuid = sc.prod_uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		auto month = std::chrono::duration_cast<date::months>(m.time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(month), catID));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t, // product uuid
			pof::base::data::text_t, //product name
			std::uint64_t, //currenct stock
			std::uint64_t, //checked stock
			std::uint64_t, //status
			pof::base::data::datetime_t
		>(*stmt);
		auto& value = rel.value();
		mStockCheckData->Clear();
		for (auto& tup : value) {
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(tup));
			mStockCheckData->EmplaceData(std::move(row));
		}
	}
}

bool pof::ProductManager::RemoveStockEntry(const pof::base::data::duuid_t& pid, const pof::base::data::datetime_t& date)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM stock_check 
		WHERE prod_uuid = ? AND date = ?;)";
		
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, date));
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

bool pof::ProductManager::MarkAllAsDone(pof::base::data::datetime_t month)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE stock_check SET status = 1 WHERE Months(date) = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		//month should be in system_clock duratiion 
		auto mth = std::chrono::duration_cast<date::months>(month.time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(mth)));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::ProductManager::MarkStockCheckAsDone(pof::base::data::duuid_t pid, pof::base::data::datetime_t month)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE stock_check SET status = 1 WHERE Months(date) = ? AND prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);

		//month should be in system_clock duratiion 
		auto mth = std::chrono::duration_cast<date::months>(month.time_since_epoch());

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(mth), pid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		mLocalDatabase->finalise(*stmt);
		return status && CaptureStock(pid);
	}
	return false;
}

bool pof::ProductManager::UnmarkStockCheckAsDone(pof::base::data::duuid_t pid, pof::base::data::datetime_t month)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(UPDATE stock_check SET status = 0 WHERE Months(date) = ? AND prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		auto mth = std::chrono::duration_cast<date::months>(month.time_since_epoch());
		
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(mth), pid));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		mLocalDatabase->finalise(*stmt);

		return status;
	}
	return false;
}

bool pof::ProductManager::CheckIfMonthStarted(const pof::base::data::datetime_t& month)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT 1 FROM stock_check WHERE Months(date) = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);

		auto mth = std::chrono::duration_cast<date::months>(month.time_since_epoch());

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(mth)));
		assert(status);
	
		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel);
		mLocalDatabase->finalise(*stmt);

		return !rel->empty();
	}
	return false;
}

bool pof::ProductManager::CheckIfDone(pof::base::data::duuid_t pid, pof::base::data::datetime_t month)
{
	if (mLocalDatabase) {
		constexpr const std::string_view checksql = R"(SELECT 1 
		FROM stock_check 
		WHERE prod_uuid = ? AND date = ? AND status = 1;)";

		auto cstmt = mLocalDatabase->prepare(checksql);
		if (!cstmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*cstmt, std::make_tuple(pid, month));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*cstmt);
		if (!rel.has_value())
		{
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*cstmt);
			return false;
		}

		mLocalDatabase->finalise(*cstmt);
		return !rel->empty();

	}
	return false;
}

std::optional<pof::base::currency> pof::ProductManager::GetShortageCost(pof::base::data::datetime_t dt)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT 
		SumCost((
			CostMulti(
				p.unit_price,
				CASE 
					WHEN sc.status = 1 THEN sc.stock_count
					WHEN sc.status = 0 THEN p.stock_count
				END
			)
		)) 
		FROM stock_check sc, products p WHERE sc.prod_uuid = p.uuid AND Months(sc.date) = ?;)";

		constexpr const std::string_view sql2 = R"(SELECT
		SumCost(CostMulti(p.unit_price, sc.check_stock)) FROM stock_check sc, products p
		WHERE sc.prod_uuid = p.uuid AND Months(sc.date) = ?;)";

		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		auto stmt2 = mLocalDatabase->prepare(sql2);
		if (!stmt2.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}


		auto month = date::floor<date::months>(dt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(month.time_since_epoch().count())));
		assert(status);


		status = mLocalDatabase->bind(*stmt2, std::make_tuple(static_cast<std::uint64_t>(month.time_since_epoch().count())));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::currency>(*stmt);

		if (!rel.has_value() || rel->empty()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}

		auto rel2 = mLocalDatabase->retrive<pof::base::currency>(*stmt2);
		if (!rel2.has_value() || rel2->empty()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}


		mLocalDatabase->finalise(*stmt);
		return std::get<0>(*(rel->begin())) - std::get<0>(*rel2->begin());
	}
	return std::nullopt;
}

std::optional<size_t> pof::ProductManager::GetProductCheckedCount(pof::base::data::datetime_t month)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT Count(prod_uuid) FROM stock_check WHERE status = 1 AND Months(date) = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(month));
		assert(status);
		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		if (!rel.has_value() || rel->empty()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		return std::get<0>(*(rel->begin()));
	}
	return std::nullopt;
}

std::optional<pof::base::data::datetime_t> pof::ProductManager::GetFirstStockMonth()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT MIN(Months(date)) FROM stock_check;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) {
			return std::nullopt;
		}
		return (std::get<0>(*(rel->begin())));
	}
	return std::nullopt;
}

std::optional<std::vector<std::tuple<pof::base::data::datetime_t, std::uint64_t, bool>>> pof::ProductManager::GetStockMonthStatus()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT date, COUNT(prod_uuid) 
		FROM stock_check WHERE 1 GROUP BY Months(date);)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t, std::uint64_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		std::vector<std::tuple<pof::base::data::datetime_t, std::uint64_t, bool>> ret;
		ret.reserve(rel->size());
		size_t size = mProductData->GetDatastore().size();
		for (const auto& tup : rel.value()){
			ret.emplace_back(std::make_tuple(std::get<0>(tup),
				std::get<1>(tup), (size == std::get<1>(tup))));
		}
		return ret;
	}
	return std::nullopt;
}

void pof::ProductManager::CreatePackTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS packs (id blob,
			name text, desc text);)";
		constexpr const std::string_view sql2 = R"(CREATE TABLE IF NOT EXISTS pack_product (id blob, 
		product_id blob, quantity integer, ext_price blob);)";

		auto table1 = mLocalDatabase->prepare(sql);
		auto table2 = mLocalDatabase->prepare(sql2);

		if (!table1.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}

		if (!table2.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}

		bool status = mLocalDatabase->execute(*table1);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}

		status = mLocalDatabase->execute(*table2);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
	}
}

std::optional<std::vector<pof::ProductManager::packDescType>> pof::ProductManager::GetPackDesc()
{
	if (mLocalDatabase) {
		if (!GetPackStmt) {
			constexpr const std::string_view sql = R"(SELECT * FROM packs;)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return std::nullopt;
			}
			GetPackStmt = *stmt;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t
		>(GetPackStmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		return rel;
	}
	return std::nullopt;
}

std::optional<std::vector<pof::ProductManager::packType>> pof::ProductManager::GetProductPack(const pof::base::data::duuid_t& packId)
{
	if (mLocalDatabase) {
		if (!ProductPackStmt){
			constexpr const std::string_view sql = R"( SELECT p.id, prod.uuid, prod.name,
				p.quantity, prod.package_size, prod.unit_price, p.ext_price   
				FROM pack_product p, products prod
				WHERE p.id = ? AND p.product_id = prod.uuid;
			)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return std::nullopt;
			}
			ProductPackStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(ProductPackStmt, std::make_tuple(packId));
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::currency_t
		>(ProductPackStmt);

		if(!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
		}

		return rel;
	}
	return std::nullopt;
}

bool pof::ProductManager::CreatePack(const packDescType& packDesc)
{
	if (mLocalDatabase) {
		if (!CreatePackStmt) {
			constexpr const std::string_view sql = R"(INSERT INTO packs VALUES (?, ?, ?);)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			CreatePackStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(CreatePackStmt, packDesc);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->execute(CreatePackStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::AddProductPack(const packType& packProduct)
{
	if (mLocalDatabase) {
		if (!AddProductPackStmt) {
			constexpr const std::string_view sql = R"(INSERT INTO pack_product VALUES (?,?,?,?);)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			AddProductPackStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(AddProductPackStmt, std::make_tuple(
			std::get<0>(packProduct),
			std::get<1>(packProduct),
			std::get<3>(packProduct),
			std::get<6>(packProduct)
		));
		status = mLocalDatabase->execute(AddProductPackStmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::RemovePack(const pof::base::data::duuid_t& packId)
{
	if (mLocalDatabase) {
		if (!RemovePackStmt || !RemoveProductPackStmt) {
			constexpr const std::string_view sql = R"(DELETE FROM packs WHERE id = ?;)";
			constexpr const std::string_view sql2 = R"(DELETE FROM pack_product WHERE id = ?;)";

			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			RemovePackStmt = *stmt;
			stmt = mLocalDatabase->prepare(sql2);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			RemoveProductPackStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(RemovePackStmt, std::make_tuple(packId));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->bind(RemoveProductPackStmt, std::make_tuple(packId));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		mLocalDatabase->begin_trans();
		status = mLocalDatabase->execute(RemovePackStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->end_trans();
			return false;
		}
		status = mLocalDatabase->execute(RemoveProductPackStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->end_trans();
			return false;
		}
		mLocalDatabase->end_trans();
		return status;
	}
	return false;
}

bool pof::ProductManager::RemoveProductPack(const pof::base::data::duuid_t& packId, const pof::base::data::duuid_t& productId)
{
	if (mLocalDatabase) {
		if (!RemoveProductInPackStmt) {
			constexpr const std::string_view sql = R"(DELETE FROM pack_product WHERE id = ? AND product_id = ?;)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			RemoveProductInPackStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(RemoveProductInPackStmt, std::make_tuple(packId, productId));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->execute(RemoveProductInPackStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::UpdateProductPack(const pof::base::data::duuid_t& packID, const packType& product)
{
	if (mLocalDatabase)
	{
		if (!UpdateProductPackStmt){
			constexpr const std::string_view sql = R"(UPDATE pack_product set quantity = ?, ext_price = ? WHERE id = ? AND product_id = ?;)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			UpdateProductPackStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(UpdateProductPackStmt, std::make_tuple(
			std::get<3>(product),
			std::get<6>(product),
			std::get<0>(product),
			std::get<1>(product)
		));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->execute(UpdateProductPackStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::UpdatePackDesc(const packDescType& packDesc)
{
	if (mLocalDatabase) {
		if (!UpdatePackDescStmt) {
			constexpr const std::string_view sql = R"(UPDATE packs SET name = ?, desc = ? WHERE id = ?;)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			UpdatePackDescStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(UpdatePackDescStmt, std::make_tuple(
			std::get<1>(packDesc),
			std::get<2>(packDesc),
			std::get<0>(packDesc)
		));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->execute(UpdatePackDescStmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		return status;
	}
	return false;
}

bool pof::ProductManager::ExistsInPack(const pof::base::data::duuid_t& packID,const pof::base::data::duuid_t& productID)
{
	if (mLocalDatabase) {
		if (!ExistsInPackStmt) {
			constexpr const std::string_view sql = R"(SELECT COUNT(product_id) FROM pack_product WHERE pack_id = ? AND product_id = ?; )";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			ExistsInPackStmt = *stmt;
		}
		bool status = mLocalDatabase->bind(ExistsInPackStmt, std::make_tuple(packID, productID));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto rel = mLocalDatabase->retrive<std::int32_t>(ExistsInPackStmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& v = rel.value();
		if (v.empty()) return false;
		return (std::get<0>(v[0]) >= 1);
	}
	return false;
}

bool pof::ProductManager::CreateWarningTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS warning (prod_uuid blob, level integer, message text);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		if (!status)
		{
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::ProductManager::InsertWarning(const pof::base::data::duuid_t& pid, size_t level, const std::string& message)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO warning (prod_uuid, level, message) VALUES (?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, level, message));
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

std::optional<std::vector<std::pair<size_t, std::string>>> pof::ProductManager::GetWarning(const pof::base::data::duuid_t& pid)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT level, message FROM warning WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			std::uint64_t,
			pof::base::data::text_t
		>(*stmt);
		mLocalDatabase->finalise(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}
		std::vector<std::pair<size_t, std::string>> ret;
		ret.reserve(rel->size());

		for (const auto& tup : rel.value()){
			ret.emplace_back(std::make_pair(std::get<0>(tup), std::get<1>(tup)));
		}
		return ret;
	}
	return std::nullopt;
}

bool pof::ProductManager::RemoveWarning(const pof::base::data::duuid_t& pid, const std::string& message)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(DELETE FROM warning WHERE prod_uuid = ? AND message LIKE ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, message));
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

bool pof::ProductManager::UpdateWarnLevel(const pof::base::data::duuid_t& pid, std::uint64_t level, const std::string& message)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE warning SET level = ? WHERE prod_uuid = ? AND message LIKE ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(level, pid, message));
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

bool pof::ProductManager::DuplicateWarning(const pof::base::data::duuid_t& frompid, pof::base::data::duuid_t& topid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT * FROM warning WHERE prod_uuid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(frompid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			std::uint64_t,
			pof::base::data::text_t
		>(*stmt);
		assert(rel);
		mLocalDatabase->finalise(*stmt);
		stmt.emplace();

		if (rel->empty()) return false;

		constexpr const std::string_view sql1 = R"(INSERT INTO warning VALUES (?,?,?);)";
		for (auto& tup : *rel){
			std::get<0>(tup) = topid;
		}
		stmt = mLocalDatabase->prepare(sql1);
		assert(stmt);

		status = mLocalDatabase->store(*stmt, std::move(rel.value()));
		mLocalDatabase->finalise(*stmt);

		return status;
	}
	return false;
}

void pof::ProductManager::CreateActionTable()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS actions (date integer, action integer);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
	}
}

bool pof::ProductManager::AddAction(size_t actionType)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO actions VALUES (?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(
		pof::base::data::clock_t::now(), static_cast<std::uint64_t>(actionType)));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::ProductManager::CheckAction(size_t actionType, pof::base::data::datetime_t date)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT 1 
		FROM actions WHERE Months(date) = ? AND action = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		

		auto month = std::chrono::duration_cast<date::months>(date.time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pof::base::data::datetime_t(month), static_cast<std::uint64_t>(actionType)));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel);
		
		mLocalDatabase->finalise(*stmt);
		return !rel->empty();
	
	}
	return false;
}

std::optional<pof::base::data::datetime_t> pof::ProductManager::GetLastCheckedTime()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT date FROM actions WHERE action = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(CHECK_TIME)));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::datetime_t>(*stmt);
		assert(rel);

		mLocalDatabase->finalise(*stmt);
		if (rel->empty()){
			return pof::base::data::datetime_t{};
		}
		return std::get<0>(*(rel->begin()));
	}
	return std::nullopt;
}

bool pof::ProductManager::UpdateTimeCheck(pof::base::data::datetime_t time)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE actions SET date = ? WHERE action = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(time, static_cast<std::uint64_t>(CHECK_TIME)));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		assert(status);
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::ProductManager::ReturnToInventory(const pof::base::data::duuid_t& pid, std::tuple<std::uint64_t, pof::base::data::currency_t> quan)
{
	if (mLocalDatabase){
		//get the most recent inserted inventory for this product
		constexpr const std::string_view qSql = R"(SELECT i.uuid, id, MAX(i.input_date), i.expire_date, p.stock_count
			 FROM inventory i, products p
			 WHERE i.uuid = ? AND p.uuid = ?  LIMIT 1;)";

		constexpr const std::string_view sql = R"(INSERT INTO inventory VALUES (?,?,?,?,?,?,?,?,?);)";
		
		auto stmt = mLocalDatabase->prepare(qSql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(pid, pid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			std::uint64_t,
			pof::base::data::datetime_t,
			pof::base::data::datetime_t,
			std::uint64_t
		>(*stmt);
		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) {
			return false; //cannot return to an empty inventory
		}
		auto& tup = *(rel->begin());
		stmt.emplace();
		stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto today = pof::base::data::clock_t::now();
		spdlog::info("Product cost {:cu}", std::get<1>(quan));
		std::string lotNumber = "0"s;
		if (wxGetApp().bAutomaticBatchNumber) {
			auto lotNum = wxGetApp().mProductManager.GetLastInventoryBatchNumber(std::get<0>(tup));
			//only work if we get valid data from the database
			if (lotNum) {
				auto& v = lotNum.value();
				if (std::ranges::all_of(v, [&](char c) -> bool {return std::isdigit(c); })) {
					//if it a digit
					try {
						size_t c = boost::lexical_cast<size_t>(v);
						c++;
						lotNumber = std::move(std::to_string(c));
					}
					catch (std::exception& exp) {
						spdlog::error(exp.what());
					}
				}
			}
		}
		status = mLocalDatabase->bind(*stmt, std::make_tuple(std::get<1>(tup) + 1, std::get<0>(tup),std::get<3>(tup), today, std::get<0>(quan) , std::get<1>(quan), "RETURN"s, 0, std::move(lotNumber)));
		assert(status);
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		
		//update the product stock
		auto stock = std::get<4>(tup) + std::get<0>(quan);
		status = UpdatePD(std::make_tuple(pid, stock), { "uuid", "stock_count" });
		return status;	
	}
	return false;
}

std::optional<pof::base::data> pof::ProductManager::GetInventoryForMonth(const pof::base::data::datetime_t& dt)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT 
		p.uuid, p.name, SUM(i.stock_count),  SumCost(CostMulti(i.cost, i.stock_count))
		FROM products p, inventory i
		WHERE p.uuid = i.uuid AND Months(i.input_date) = ? AND i.manufacturer_name IS NOT 'RETURN'
		GROUP BY p.uuid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		//assert(stmt);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return std::nullopt;
		}

		const std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(dt) };
		int out = (static_cast<int>(ymd.year()) - 1970) * 12 + (static_cast<unsigned>(ymd.month()) - 1);

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(static_cast<std::uint64_t>(out)));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return std::nullopt;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return pof::base::data{};
		pof::base::data ret;
		ret.get_metadata() = {
			pof::base::data::kind::uuid,
			pof::base::data::kind::text,
			pof::base::data::kind::uint64,
			pof::base::data::kind::currency,
		};
		ret.reserve(rel->size());
		for (auto&& tup : rel.value()) {
			ret.emplace(pof::base::make_row_from_tuple(tup));
			/*auto& b = ret.tab().back();
			boost::variant2::get<pof::base::currency>(b.first[3]) = boost::variant2::get<pof::base::currency>(b.first[3])
				 * static_cast<double>(boost::variant2::get<std::uint64_t>(b.first[2]));*/

		}
		return ret;
	}
	return std::nullopt;
}

void pof::ProductManager::Finialize()
{
	if (bUsingLocalDatabase) {
		mLocalDatabase->finalise(productLoadStmt);
		mLocalDatabase->finalise(productRemoveStmt);
		mLocalDatabase->finalise(productStoreStmt);
		mLocalDatabase->finalise(InventoryLoadStmt);
		mLocalDatabase->finalise(productOnStoreStmt);
		mLocalDatabase->finalise(CategoryLoadStmt);
		mLocalDatabase->finalise(CategoryStoreStmt);
		mLocalDatabase->finalise(CategoryRemoveStmt);
		mLocalDatabase->finalise(CategoryUpdateStmt);
		mLocalDatabase->finalise(ExpireProductStmt);
		mLocalDatabase->finalise(CurExpireDateStmt);
		mLocalDatabase->finalise(CreatePackStmt);
		mLocalDatabase->finalise(ProductPackStmt);
		mLocalDatabase->finalise(GetPackStmt);
		mLocalDatabase->finalise(ExistsInPackStmt);
		mLocalDatabase->finalise(UpdateProductQuanStmt);
	}
}


