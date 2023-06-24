#include "ProductManager.h"

pof::ProductManager::ProductManager() {
	mProductData = std::make_unique<pof::DataModel>();
	mInventoryData = std::make_unique<pof::DataModel>();
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

	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::StoreProductData, this), pof::DataModel::Signals::ADDED);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::UpdateProductData, this), pof::DataModel::Signals::UPDATE);
	mProductData->ConnectSlot(std::bind_front(&pof::ProductManager::RemoveProductData, this), pof::DataModel::Signals::REMOVED);
}

pof::ProductManager::~ProductManager()
{
	mProductData.release();
	mInventoryData.release();
}


bool pof::ProductManager::LoadProductData()
{
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!productLoadStmt) {
			constexpr const std::string_view sql = "SELECT * FROM products LIMIT 300;";
			auto stmtopt = mLocalDatabase->prepare(sql);
			if (!stmtopt) {
				spdlog::error("{}", mLocalDatabase->err_msg());
				return false;
			}
			productLoadStmt = *stmtopt;
		}
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
			std::uint64_t //CATEGORY ID
		>(productLoadStmt);
		if (!rel.has_value()) {
			spdlog::error("Error reading products from the database {}", mLocalDatabase->err_msg());
			return false;
		}
		if (rel->empty()) {
			spdlog::info("Data loaded from the database is empty");
			return false;
		}
		pof::base::data pdata;
		pdata.reserve(rel->size());
		for (auto& tup : *rel) {
			pof::base::data::row_t row;
			auto& v = row.first;
			v = std::move(pof::base::make_row_from_tuple(tup));
			v.emplace_back(static_cast<std::uint64_t>(0));
			v.emplace_back(pof::base::data::text_t{});
			v.emplace_back(pof::base::data::datetime_t{});
			pdata.insert(std::move(row));
		}
		mProductData->Emplace(std::move(pdata));
	}
	return true;
}

bool pof::ProductManager::LoadInventoryData()
{
	return false;
}

bool pof::ProductManager::LoadCategories()
{
	return false;
}

bool pof::ProductManager::StoreProductData(pof::base::data::const_iterator iter)
{
	if (iter == mProductData->GetDatastore().end()) return false;
	auto& v = iter->first;
	if (bUsingLocalDatabase && mLocalDatabase) {
		if (!productStoreStmt) {
			constexpr const std::string_view sql =
				"INSERT INTO products (uuid, serial_num, name, generic_name, class, formulation, strength, strength_type, usage_info, descrip, health_condition, unit_price, cost_price, package_size, stock_count, side_effect, barcode, category )"
				" VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? ); ";
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
			mLocalDatabase->finalise(productStoreStmt);
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
		for (auto& i: upData){
			auto kind = meta[i];
			switch (kind)
			{
			case pof::base::data::kind::int32:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::int32_t>(v[i]), i);
				break;
			case pof::base::data::kind::int64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::int64_t>(v[i]), i);
				break;
			case pof::base::data::kind::uint32:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint32_t>(v[i]), i);
				break;
			case pof::base::data::kind::uint64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint64_t>(v[i]), i);
				break;
			case pof::base::data::kind::float32:
				mLocalDatabase->bind(*stmt, boost::variant2::get<float>(v[i]), i);
				break;
			case pof::base::data::kind::float64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<double>(v[i]), i);
				break;
			case pof::base::data::kind::datetime:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::datetime_t>(v[i]), i);
				break;
			case pof::base::data::kind::text:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::text_t>(v[i]), i);
				break;
			case pof::base::data::kind::blob:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::blob_t>(v[i]), i);
				break;
			case pof::base::data::kind::uuid:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::duuid_t>(v[i]), i);
				break;
			case pof::base::data::kind::currency:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::currency_t>(v[i]), i);
				break;
			case pof::base::data::kind::null:
			default:
				break;
			}
		}
		
		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
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

	}

	return true;
}

void pof::ProductManager::EmplaceProductData(pof::base::data&& data)
{
	mProductData->Emplace(std::forward<pof::base::data>(data));

}

void pof::ProductManager::AddProductData()
{
}

void pof::ProductManager::Finialize()
{
	if (bUsingLocalDatabase) {
		mLocalDatabase->finalise(productLoadStmt);
		mLocalDatabase->finalise(InventoryLoadStmt);
		mLocalDatabase->finalise(productUpdateStmt);
		mLocalDatabase->finalise(InventoryUpdateStmt);
		mLocalDatabase->finalise(productStoreStmt);
	}
}


