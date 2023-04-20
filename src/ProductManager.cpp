#include "ProductManager.h"

pof::ProductManager::ProductManager() {
	mProductData = std::make_unique<pof::DataModel>();
	mInventoryData = std::make_unique<pof::DataModel>();
	mProductData->Adapt<
		pof::base::data::uuid_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		std::uint32_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t
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


