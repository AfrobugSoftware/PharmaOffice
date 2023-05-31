#include "ProductManager.h"

pof::ProductManager::ProductManager() {
	mProductData = std::make_unique<pof::DataModel>();
	mInventoryData = std::make_unique<pof::DataModel>();
	mProductData->Adapt<
		pof::base::data::uuid_t, //UUID
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
		pof::base::data::uuid_t, //UUID
		pof::base::data::datetime_t, // EXPIRE DATE
		pof::base::data::datetime_t, // ADDED DATE
		std::uint64_t, // STOCK COUNT
		pof::base::data::currency_t, //CURRENCY
		pof::base::data::text_t, // MANUFACTURE NAME
		std::uint64_t,  // MANUFACTURER ADDRESS ID
		pof::base::data::text_t // LOT NUMBER/ BATCH NUMBER
	>();



}

pof::ProductManager::~ProductManager()
{
	mProductData.release();
	mInventoryData.release();
}

bool pof::ProductManager::LoadProductData()
{
	return false;
}

bool pof::ProductManager::LoadInventoryData()
{
	return false;
}

bool pof::ProductManager::LoadCategories()
{
	return false;
}

void pof::ProductManager::EmplaceProductData(pof::base::data&& data)
{
	mProductData->Emplace(std::forward<pof::base::data>(data));

}

void pof::ProductManager::AddProductData()
{
}


