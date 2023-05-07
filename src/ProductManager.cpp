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
		pof::base::data::text_t, //UNIT TYPE
		std::uint32_t, // DOSAGE
		pof::base::data::text_t, //DOSEAGE WORDS
		pof::base::data::text_t, // STRENGTH
		pof::base::data::text_t, // USAGE INFO
		pof::base::data::text_t, // UNIT PRICE
		std::uint64_t, //PACAKGE SIZE
		std::uint64_t, //STOCK COUNT
		pof::base::data::text_t, //SIDE EFFECTS
		pof::base::data::text_t, //BARCODE
		std::uint64_t //CATEGORY ID
	>();
	
	mInventoryData->Adapt<
		std::uint64_t,
		pof::base::data::uuid_t,
		pof::base::data::datetime_t,
		pof::base::data::datetime_t,
		std::uint64_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::text_t
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


