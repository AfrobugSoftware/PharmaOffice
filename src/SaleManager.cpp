#include "SaleManager.h"

pof::SaleManager::SaleManager()
{
	SaleData = std::make_unique<pof::DataModel>();
	ProductSaleHistory = std::make_unique<pof::DataModel>();

	SaleData->Adapt<std::uint64_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::text_t>();

	ProductSaleHistory->Adapt<std::uint64_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::text_t>();

}

pof::SaleManager::~SaleManager()
{
	SaleData.release();
	//ProductSaleHistory.release();
}

void pof::SaleManager::DoSale()
{
}

void pof::SaleManager::LoadProductSaleHistory(const boost::uuids::uuid& productUUID)
{

}
