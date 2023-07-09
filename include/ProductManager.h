#pragma once
#include <boost/noncopyable.hpp>
#include <ranges>
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
			ORDER_ID,
			ORDER_PRODUCT_UUID,
			ORDER_PRODUCT_NAME,
			ORDER_QUANTITY,
			ORDER_COST
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
			pof::base::data::datetime_t //EXPIRE DATE
		> ;

		~ProductManager();
		ProductManager();
	

		bool LoadInventoryData(const pof::base::data::duuid_t& ud);
		bool LoadCategories();
		bool LoadProductsFromDatabase();


		bool StrProductData(pof::base::data::const_iterator iter);
		bool UpdateProductData(pof::base::data::const_iterator iter);
		bool RemoveProductData(pof::base::data::const_iterator iter);
		
		//load from a different source directly to the database
		bool OnStoreProductData(pof::base::data::const_iterator iter);

		bool StoreInventoryData(pof::base::data::const_iterator iter);
		bool UpdateInventoryData(pof::base::data::const_iterator iter);
		bool RemoveInventoryData(pof::base::data::const_iterator iter);

		void AddCategory(const std::string& name);
		void RemoveCategory(const std::string& name);
		void UpdateCategory(pof::base::data::const_iterator iter);


		inline const pof::DataModel& GetBaseProductData() const { return *mProductData; }
		inline std::unique_ptr<pof::DataModel>& GetProductData() { return mProductData; }
		inline std::unique_ptr<pof::DataModel>& GetOrderList() { return mOrderList; }

		const pof::DataModel& GetInventoryForProduct() const { return *mInventoryData; }
		inline std::unique_ptr<pof::DataModel>& GetInventory() { return mInventoryData; }
		const pof::base::data& GetCategories() const { return mCategories; }
		pof::base::data& GetCategories() { return mCategories; }

		void EmplaceProductData(pof::base::data&& data);
		void StoreProductData(pof::base::data&& data);
		
		//add a product from UI
		void AddProductData();

		//product uuid generators
		bool bUsingLocalDatabase = false;
		std::shared_ptr<pof::base::database> mLocalDatabase;

		boost::uuids::random_generator_mt19937 UuidGen;

		//data functions
		std::optional<std::vector<wxDataViewItem>> DoExpiredProducts();

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

		

		//should also contain the product view
		std::unique_ptr<pof::DataModel> mProductData;
		std::unique_ptr<pof::DataModel> mInventoryData;
		std::unique_ptr<pof::DataModel> mOrderList;

	};

};