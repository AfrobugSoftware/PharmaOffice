#include "ProductManager.h"

pof::ProductManager::ProductManager() {
	mProductData.Adapt<
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
		pof::base::data::text_t
	>();
	
	mInventoryData.Adapt<
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


