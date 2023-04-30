#include "SaleManager.h"

pof::SaleManager::SaleManager()
{
	SaleData = std::make_unique<pof::DataModel>();
	ProductSaleHistory = std::make_unique<pof::DataModel>();


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
