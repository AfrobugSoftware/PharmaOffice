#pragma once
#include <boost/noncopyable.hpp>
#include <ranges>
#include <date/date.h>
#include <algorithm>
#include <set>
#include <shared_mutex>
#include "DataModel.h"
#include "database.h"
/// <summary>
/// ADD PHARMACOLOGICAL UNITS AS A CLASS
///		a ratio for converting from one unit to another and illegal conversion
///		also to and from string
/// 
///		Prescriptions page called backlog
/// </summary>
/// 
/// 

namespace std
{
	template<>
	struct hash<pof::base::data::duuid_t> {
		static boost::hash<boost::uuids::uuid> uuid_hasher;
		size_t operator()(const pof::base::data::duuid_t& v) const noexcept
		{
			return uuid_hasher(v);
		}
	};
};

namespace pof {
	class ProductManager : private boost::noncopyable {
	public:
		enum : std::uint8_t {
			PRODUCT_UUID,
			PRODUCT_SERIAL_NUM,
			PRODUCT_NAME,
			PRODUCT_GENERIC_NAME, //FOR PHARMACEUTICES WITH GENERIC NAME, MIGHT CONTAIN MORE THAN ONE GENERIC NAME FOR EXAMPLE ATHERTHER/LUMEFANTRINE 
			PRODUCT_CLASS,
			PRODUCT_FORMULATION,
			PRODUCT_STRENGTH, // GIVEN IN mg, g %v/v, %m/v -> need to have a list of approved stengths,
			PRODUCT_STRENGTH_TYPE, // GIVEN as mg, g %v/v, %m/v -> need to have a list of approved stengths,
			PRODUCT_USAGE_INFO,
			PRODUCT_DESCRIP, 
			PRODUCT_HEALTH_CONDITIONS, //COMMA SEPERATED
			PRODUCT_UNIT_PRICE,
			PRODUCT_COST_PRICE,
			PRODUCT_PACKAGE_SIZE,
			PRODUCT_STOCK_COUNT, // TRACKS THE PRESENT STOCK OF THE PRODUCT
			PRODUCT_SIDEEFFECTS,
			PRODUCT_BARCODE,
			PRODUCT_CATEGORY,

			//Product setting
			PRODUCT_MIN_STOCK_COUNT,
			PRODUCT_EXPIRE_PERIOD,
			PRODUCT_TO_EXPIRE_DATE, //NUMBER OF PERIOD TO WAIT TO BE INFORMED

			PRODUCT_MAX
		};

		enum : std::uint8_t {
			INVENTORY_ID,
			INVENTORY_PRODUCT_UUID, //same UUID AS THE PRODUCT
			INVENTORY_EXPIRE_DATE, // MMYY FORMAT 
			INVENTORY_INPUT_DATE, // DATE ADDED 
			INVENTORY_STOCK_COUNT, //THE STOCK ASSOCIATED WITH THIS INVENTORY
			INVENTORY_COST, //INVENTORY COST PRICE
			INVENTORY_MANUFACTURER_NAME,
			INVENTORY_MANUFACTURER_ADDRESS_ID,
			INVENTORY_LOT_NUMBER, //HAVE MULTIPLE BACTHES -> 
			INVENTORY_MAX
		};

		enum : std::uint8_t {
			CATEGORY_ID,
			CATEGORY_NAME,
			CATEGORY_MAX
		};
		
		enum : std::uint8_t {
			ORDER_PRODUCT_UUID,
			ORDER_PRODUCT_NAME,
			ORDER_QUANTITY,
			ORDER_COST,
			ORDER_STATE,
			ORDER_MAX
		};

		enum : std::uint8_t {
			PACK_UUID,
			PACK_PROD_UUID,
			PACK_PROD_QUANTITY,
			PACK_PROD_EXT_COST,
			PACK_PROD_MAX
		};

		enum : std::uint8_t {
			PACK_UUID_DECS,
			PACK_NAME,
			PACK_DESCRIPTION
		};
		//order state
		enum : std::uint8_t {
			PENDING,
			ORDERED,
		};


		//warning level
		enum : std::uint8_t {
			SIMPLE,
			CRITICAL,
		};

		enum :std::uint8_t
		{
			STOCK_CHECKED,
			BROUGHT_FORWARD,
			CHECK_TIME,
		};
		using relation_t = pof::base::relation <
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
		> ;

		~ProductManager();
		ProductManager();
	

		bool LoadInventoryData(const pof::base::data::duuid_t& ud);
		bool LoadCategories();
		bool LoadProductsFromDatabase();
		bool LoadInventoryByDate(const pof::base::data::duuid_t& ud, const pof::base::data::datetime_t& dt);
		
		//orderlist
		void CreateOrderListTable();
		bool LoadOrderList();
		bool ClearOrderList();
		bool CheckIfInOrderList(const pof::base::data::duuid_t& ud);
		bool AddToOrderList(const pof::base::data::duuid_t& ud, std::uint64_t quan);
		bool UpdateOrderList(const pof::base::data::duuid_t& ud, std::uint64_t quan);
		bool RemvFromOrderList(const pof::base::data::duuid_t ud);
		size_t GetOrderListCount() const;
		bool UpdateOrderState(const pof::base::data::duuid_t& uid, std::uint64_t state);

		bool StrProductData(pof::base::data::const_iterator iter);
		bool UpdateProductData(pof::base::data::const_iterator iter);
		bool RemoveProductData(pof::base::data::const_iterator iter);
		
		//the first arg is the uuid
		template<typename... Args>
		bool UpdatePD(const std::tuple<Args...>& args, std::array<std::string_view, sizeof...(Args)>&& names) {
			if (mLocalDatabase) {
				std::ostringstream str;
				str << "UPDATE products ";
				for (int i = 1; i < names.size(); i++) {
					if (i != 1) {
						str << ",";
					}
					str << fmt::format("SET {s:} = :{s:} ", fmt::arg("s", names[i]));
				}
				str << "WHERE uuid = :uuid;";
				auto stmt = mLocalDatabase->prepare(str.str());
				if (!stmt.has_value()){
					spdlog::error(mLocalDatabase->err_msg());
					return false;
				}

				bool status = mLocalDatabase->bind_para(*stmt, args, std::move(names));
				if (!status) {
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

		//load from a different source directly to the database
		bool OnStoreProductData(pof::base::data::const_iterator iter);

		bool StoreInventoryData(pof::base::data::const_iterator iter);
		bool UpdateInventoryData(pof::base::data::const_iterator iter);
		bool RemoveInventoryData(pof::base::data::const_iterator iter);
		std::optional<std::uint64_t> GetLastInventoryId(const pof::base::data::duuid_t& uid);
		std::optional<pof::base::data::datetime_t> GetLastInventoryDate(const pof::base::data::datetime_t& uid);

		void AddCategory(const std::string& name);
		void RemoveCategory(const std::string& name);
		void UpdateCategory(pof::base::data::const_iterator iter);

		size_t GetInentoryCount(const pof::base::data::duuid_t& ud);
		inline const pof::DataModel& GetBaseProductData() const { return *mProductData; }
		inline std::unique_ptr<pof::DataModel>& GetProductData() { return mProductData; }
		inline std::unique_ptr<pof::DataModel>& GetOrderList() { return mOrderList; }

		const pof::DataModel& GetInventoryForProduct() const { return *mInventoryData; }
		inline std::unique_ptr<pof::DataModel>& GetInventory() { return mInventoryData; }
		const pof::base::data& GetCategories() const { return mCategories; }
		pof::base::data& GetCategories() { return mCategories; }

		void UpdateProductQuan(const std::vector<std::tuple<pof::base::data::duuid_t, std::uint64_t>>& prodQuans);
		void RefreshRowFromDatabase(const pof::base::data::duuid_t& pid, pof::base::data::row_t& row);
		void EmplaceProductData(pof::base::data&& data);
		void StoreProductData(pof::base::data&& data);
		void InventoryBroughtForward(); 
		void MarkUpProducts();
		void MarkUpProducts(const pof::base::data::duuid_t& uid);

		//add a product from UI
		void AddProductData();

		bool CreateExpiredStockTable();
		bool MoveStockToExpire(const pof::base::data::duuid_t& pid, std::uint64_t stock);
		std::optional<std::uint64_t> GetTotalExpired(const pof::base::data::duuid_t& pid, pof::base::data::datetime_t date);
		std::optional<std::vector<std::pair<pof::base::data::duuid_t, std::uint64_t>>> GetExpiredProductsStock(pof::base::data::datetime_t m);
 		//product uuid generators
		bool bUsingLocalDatabase = false;
		float gMarkup = 0.3f;
		std::shared_ptr<pof::base::database> mLocalDatabase;

		boost::uuids::random_generator_mt19937 UuidGen;


		//reports
		std::optional<pof::base::data> GetEndOfDay();
		std::optional<pof::base::data> GetConsumptionPattern();

		//data functions
		std::optional<std::vector<wxDataViewItem>> DoExpireProductPeriod();
		std::optional<std::vector<wxDataViewItem>> DoExpiredProducts();
		std::optional<std::vector<wxDataViewItem>> DoOutOfStock();
		std::optional<pof::base::data::datetime_t> GetCurrentExpireDate(const pof::base::data::duuid_t& prod);

		//stock check
		bool CaptureStock(const pof::base::data::duuid_t& pid);
		bool CreateStockCheckTable();
		std::unique_ptr<pof::DataModel>& GetStockCheckData();
		void LoadStockCheckDate(pof::base::data::datetime_t month);
		void SaveStockCheckData();
		bool CheckIfAdded(const pof::base::data::duuid_t& pid);
		bool IsStockCheckComplete();
		bool IsStockCheckComplete(const pof::base::data::datetime_t& month);
		void UpdateStockCheck(const pof::base::data::duuid_t& pid, std::uint64_t stock);
		bool InsertProductInStockCheck(const pof::base::data::duuid_t& pid);
		void LoadStockDataByCategory(pof::base::data::datetime_t month, std::uint64_t catID);
		bool RemoveStockEntry(const pof::base::data::duuid_t& pid, const pof::base::data::datetime_t& date);
		std::optional<pof::base::data::datetime_t> GetFirstStockMonth();
		std::optional<std::vector<std::tuple<pof::base::data::datetime_t, std::uint64_t, bool>>> GetStockMonthStatus();


		using packDescType = std::tuple<pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t>;
		using packType = std::tuple<pof::base::data::duuid_t,
					pof::base::data::duuid_t,
					pof::base::data::text_t,
					std::uint64_t, //quantity
					std::uint64_t, //package size
					pof::base::data::currency_t, //current price
					pof::base::data::currency_t>; //recalculaed, ext price
		void CreatePackTable();
		std::optional<std::vector<packDescType>> GetPackDesc();
		std::optional<std::vector<packType>> GetProductPack(const pof::base::data::duuid_t& packId);

		bool CreatePack(const packDescType& packDesc);
		bool AddProductPack(const packType& packProduct);
		bool RemovePack(const pof::base::data::duuid_t& packId);
		bool RemoveProductPack(const pof::base::data::duuid_t& packId,
			const pof::base::data::duuid_t& productId);
		bool UpdateProductPack(const pof::base::data::duuid_t& packID, const packType& product);
		bool UpdatePackDesc(const packDescType& packDesc);
		bool ExistsInPack(const pof::base::data::duuid_t& packID, const pof::base::data::duuid_t& productID);

		//warning system
		bool CreateWarningTable();
		bool InsertWarning(const pof::base::data::duuid_t& pid, size_t level, const std::string& message);
		std::optional<std::vector<std::pair<size_t, std::string>>> GetWarning(const pof::base::data::duuid_t& pid);
		bool RemoveWarning(const pof::base::data::duuid_t& pid, const std::string& message); //uses both as the key
		bool UpdateWarnLevel(const pof::base::data::duuid_t& pid, std::uint64_t level, const std::string& message);

		//product search filters

		//actions are things done in the month
		void CreateActionTable();
		bool AddAction(size_t actionType); //writes an action for the currenct date
		bool CheckAction(size_t actionType, pof::base::data::datetime_t date);
		
		std::optional<pof::base::data::datetime_t> GetLastCheckedTime();
		bool UpdateTimeCheck(pof::base::data::datetime_t time);
	private:
		std::shared_mutex mCategoryMutex;
		pof::base::data mCategories; 
		pof::base::packer mPacker{mCategories};
		pof::base::unpacker mUnpacker{mCategories};


		//localdatabase statement
		void Finialize();
		pof::base::database::stmt_t productLoadStmt = nullptr;
		pof::base::database::stmt_t productStoreStmt = nullptr;
		pof::base::database::stmt_t productRemoveStmt = nullptr;
		pof::base::database::stmt_t productOnStoreStmt = nullptr;

		pof::base::database::stmt_t InventoryLoadStmt = nullptr;
		pof::base::database::stmt_t InventoryStoreStmt = nullptr;
		pof::base::database::stmt_t InventoryRemoveStmt = nullptr;
		
		
		pof::base::database::stmt_t CategoryLoadStmt = nullptr;
		pof::base::database::stmt_t CategoryStoreStmt = nullptr;
		pof::base::database::stmt_t CategoryRemoveStmt = nullptr;
		pof::base::database::stmt_t CategoryUpdateStmt = nullptr;


		//data statements
		pof::base::database::stmt_t ExpireProductStmt = nullptr;
		pof::base::database::stmt_t CurExpireDateStmt = nullptr;
		pof::base::database::stmt_t ProductPackStmt = nullptr;
		pof::base::database::stmt_t CreatePackStmt = nullptr;
		pof::base::database::stmt_t GetPackStmt = nullptr;
		pof::base::database::stmt_t AddProductPackStmt = nullptr;
		pof::base::database::stmt_t RemovePackStmt = nullptr;
		pof::base::database::stmt_t RemoveProductPackStmt = nullptr;
		pof::base::database::stmt_t RemoveProductInPackStmt = nullptr;
		pof::base::database::stmt_t UpdateProductPackStmt = nullptr;
		pof::base::database::stmt_t UpdatePackDescStmt = nullptr;
		pof::base::database::stmt_t ExistsInPackStmt = nullptr;
		pof::base::database::stmt_t UpdateProductQuanStmt = nullptr;
		pof::base::database::stmt_t LoadInventoryByDateStmt = nullptr;




		//should also contain the product view
		std::unique_ptr<pof::DataModel> mProductData;
		std::unique_ptr<pof::DataModel> mInventoryData;
		std::unique_ptr<pof::DataModel> mOrderList;
		std::unique_ptr<pof::DataModel> mStockCheckData;


	};

};