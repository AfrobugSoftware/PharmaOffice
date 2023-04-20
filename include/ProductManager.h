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
			PRODUCT_NAME,
			PRODUCT_CLASS,
			PRODUCT_UNIT_TYPE,
			PRODUCT_DOSAGE, // in figures, 
			PRODUCT_DOSAGE_WORDS, //In words
			PROUDCT_STRENGTH, // GIVEN IN mg, g %v/v, %m/v -> need to have a list of approved stengths
			PRODUCT_GENERIC_NAME, //FOR PHARMACEUTICES WITH GENERIC NAME
			PRODUCT_USAGE_INFO,
			PRODUCT_UNIT_PRICE,

			PRODUCT_SIDEEFFECTS,
			PRODUCT_BARCODE,
		};

		enum : std::uint8_t {
			INVENTORY_ID,
			INVENTORY_PRODUCT_UUID, //same UUID AS THE PRODUCT
			INVENTORY_EXPIRE_DATE, // MMYY FORMAT 
			INVENTORY_INPUT_DATE, // DATE ADDED 
			INVENTORY_STOCK_COUNT, //TRACK THE PRESENT STOCK
			INVENTORY_MANUFACTURER_NAME,
			INVENTORY_MANUFACTURER_ADDRESS_ID,
			INVENTORY_LOT_NUMBER, //HAVE MULTIPLE BACTHES -> 
		};


		ProductManager();
		~ProductManager() {}

		bool LoadProductData();
		bool LoadInventoryData();
		bool LoadCategories();


		inline const pof::DataModel& GetBaseProductData() const { return *mProductData; }
		inline std::unique_ptr<pof::DataModel>& GetProductData() { return mProductData; }

		const pof::DataModel& GetInventoryForProduct() const { return *mInventoryData; }
		inline std::unique_ptr<pof::DataModel>& GetInventory() { return mInventoryData; }
		const pof::base::data& GetCategories() const { return mCategories; }

		void EmplaceProductData(pof::base::data&& data);

	private:

		pof::base::data mCategories; 
		pof::base::packer mPacker{mCategories};
		pof::base::unpacker mUnpacker{mCategories};


		//should also contain the product view
		std::shared_mutex mProductDataMutex;
		std::unique_ptr<pof::DataModel> mProductData;
		std::shared_mutex mInventoryDataMutex;
		std::unique_ptr<pof::DataModel> mInventoryData;
	};

};