#include "ProductManager.h"

pof::ProductManager::ProductManager() {
	mProductData = std::make_unique<pof::DataModel>();
	mInventoryData = std::make_unique<pof::DataModel>();
	mOrderList = std::make_unique<pof::DataModel>();
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
		pof::base::data::datetime_t //EXPIRE DATE

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
		std::uint64_t, //id
		pof::base::data::duuid_t, //product uuid
		pof::base::data::text_t, //product name
		std::uint64_t, //quantity
		pof::base::data::currency_t
	>();

	

	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::StrProductData, this), pof::DataModel::Signals::STORE);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::UpdateProductData, this), pof::DataModel::Signals::UPDATE);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::RemoveProductData, this), pof::DataModel::Signals::REMOVED);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::OnStoreProductData, this), pof::DataModel::Signals::STORE_LOAD);

	mInventoryData->ConnectSlot(std::bind_front(&pof::ProductManager::StoreInventoryData, this), pof::DataModel::Signals::STORE);
	mInventoryData->ConnectSlot(std::bind_front(&pof::ProductManager::UpdateInventoryData, this), pof::DataModel::Signals::UPDATE);
	mInventoryData->ConnectSlot(std::bind_front(&pof::ProductManager::RemoveInventoryData, this), pof::DataModel::Signals::REMOVED);
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
		}
		pof::base::data inven;
		inven.reserve(rel->size());
		for (auto& v : *rel) {
			pof::base::data::row_t row;
			auto& f = row.first;
			f = std::move(pof::base::make_row_from_tuple(std::move(v)));
			inven.insert(std::move(row));
		}
		mInventoryData->Emplace(std::move(inven));
	}
	return true;
}

bool pof::ProductManager::LoadCategories()
{
	if (bUsingLocalDatabase && mLocalDatabase){
		constexpr const std::string_view sql = "SELECT * FROM categories;";
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
		constexpr const std::string_view sql = "SELECT * FROM products LIMIT 300;";
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
			pof::base::data::datetime_t //EXPIRE DATE
		> (stmt.value());

		if (!relation.has_value()) {
			spdlog::error(pd->err_msg());
			return false;
		}

		if (relation->empty()) {
			spdlog::info("Product database is empty");
			return false;
		}

		for (auto& tup : *relation) {
			pof::base::data::row_t row;
			row.first = std::move(pof::base::make_row_from_tuple(tup));
			mProductData->EmplaceData(std::move(row));
		}
	}
	return true;
}

bool pof::ProductManager::StrProductData(pof::base::data::const_iterator iter)
{
	if (iter == mProductData->GetDatastore().end()) return false;
	auto& v = iter->first;
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!productStoreStmt) {
			static constexpr const std::string_view sql =
				"INSERT INTO products (uuid, serial_num, name, generic_name, class, formulation, strength, strength_type, usage_info, descrip, health_condition, unit_price, cost_price, package_size, stock_count, side_effect, barcode, category, min_stock_count, expire_period, expire_date )"
				" VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?, ? , ? , ? ); ";
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
			std::uint64_t //CATEGORY ID
		>;
		auto tup = pof::base::make_tuple_from_row<tuple_t>(v);
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
		os << "UPDATE products ";
		std::vector<std::string_view> textArray;
		textArray.reserve(PRODUCT_MAX);

		for (size_t i = 0; i < PRODUCT_MAX; i++) {
			if (up.test(i)) {
				upData.push_back(i);
				switch (i)
				{
				case PRODUCT_NAME:
					textArray.emplace_back("set name = ?");
					break;
				case PRODUCT_GENERIC_NAME:
					textArray.emplace_back("set generic_name = ?");
					break;
				case PRODUCT_CLASS:
					textArray.emplace_back("set class = ?");
					break;
				case PRODUCT_FORMULATION:
					textArray.emplace_back("set formulation = ?");
					break;
				case PRODUCT_STRENGTH:
					textArray.emplace_back("set strength = ?");
					break;
				case PRODUCT_STRENGTH_TYPE:
					textArray.emplace_back("set strength_type = ?");
					break;
				case PRODUCT_USAGE_INFO:
					textArray.emplace_back("set usuage_info = ?");

					break;
				case PRODUCT_DESCRIP:
					textArray.emplace_back("set descrip = ?");

					break;
				case PRODUCT_HEALTH_CONDITIONS:
					textArray.emplace_back("set health_conditions = ?");

					break;
				case PRODUCT_UNIT_PRICE:
					textArray.emplace_back("set unit_price = ?");

					break;
				case PRODUCT_COST_PRICE:
					textArray.emplace_back("set cost_price = ?");

					break;
				case PRODUCT_PACKAGE_SIZE:
					textArray.emplace_back("set package_size = ?");

					break;
				case PRODUCT_STOCK_COUNT:
					textArray.emplace_back("set stock_count = ?");

					break;
				case PRODUCT_SIDEEFFECTS:
					textArray.emplace_back("set side_effects = ?");

					break;
				case PRODUCT_BARCODE:
					textArray.emplace_back("set barcode = ?");

					break;
				case PRODUCT_CATEGORY:
					textArray.emplace_back("set category = ?");

					break;
				case PRODUCT_MIN_STOCK_COUNT:
					textArray.emplace_back("set min_stock_count = ?");

					break;
				case PRODUCT_EXPIRE_PERIOD:
					textArray.emplace_back("set expire_period = ?");

					break;
				case PRODUCT_TO_EXPIRE_DATE:
					textArray.emplace_back("set expire_date = ?");

					break;
				default:
					break;
				}
			}
		}
		os << fmt::format("{}", fmt::join(textArray, " AND "));
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
			return status;
		}
		mLocalDatabase->finalise(*stmt);
	}
	else {
		//
	}

	return true;
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
		auto tup = pof::base::make_tuple_from_row<tuple_t>(v);
		bool status = mLocalDatabase->bind(InventoryStoreStmt, std::move(tup));
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
	return false;
}

bool pof::ProductManager::RemoveInventoryData(pof::base::data::const_iterator iter)
{
	return false;
}

void pof::ProductManager::AddCategory(const std::string& name)
{
	if (name.empty()) return;
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!CategoryStoreStmt) {
			static constexpr const std::string_view sql = "INSERT INTO category (id, name) VALUES (? ,?);";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return;
			}
			CategoryStoreStmt = *stmt;
		}
		auto& r = mCategories.back().first;
		std::uint64_t id = boost::variant2::get<std::uint64_t>(r[CATEGORY_ID]) + 1; 
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

void pof::ProductManager::EmplaceProductData(pof::base::data&& data)
{
	mProductData->Emplace(std::forward<pof::base::data>(data));
}

void pof::ProductManager::StoreProductData(pof::base::data&& data)
{
	mProductData->Store(std::forward<pof::base::data>(data));
}

void pof::ProductManager::AddProductData()
{
}

void pof::ProductManager::Finialize()
{
	if (bUsingLocalDatabase) {
		mLocalDatabase->finalise(productLoadStmt);
		mLocalDatabase->finalise(productRemoveStmt);
		mLocalDatabase->finalise(productStoreStmt);
		mLocalDatabase->finalise(InventoryLoadStmt);
		mLocalDatabase->finalise(productOnStoreStmt);
	}
}


