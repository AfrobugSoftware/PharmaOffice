#pragma once
#include <boost/noncopyable.hpp>
#include <ranges>
#include <algorithm>
#include <shared_mutex>
#include "DataModel.h"
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
		
		~ProductManager();
		ProductManager();
	

		bool LoadProductData();
		bool LoadInventoryData();
		bool LoadCategories();


		inline const pof::DataModel& GetBaseProductData() const { return *mProductData; }
		inline std::unique_ptr<pof::DataModel>& GetProductData() { return mProductData; }

		const pof::DataModel& GetInventoryForProduct() const { return *mInventoryData; }
		inline std::unique_ptr<pof::DataModel>& GetInventory() { return mInventoryData; }
		const pof::base::data& GetCategories() const { return mCategories; }

		void EmplaceProductData(pof::base::data&& data);
		
		//add a product from UI
		void AddProductData();


	private:

		std::shared_mutex mCategoryMutex;
		pof::base::data mCategories; 
		pof::base::packer mPacker{mCategories};
		pof::base::unpacker mUnpacker{mCategories};




		//should also contain the product view
		std::unique_ptr<pof::DataModel> mProductData;
		std::unique_ptr<pof::DataModel> mInventoryData;
	};

};