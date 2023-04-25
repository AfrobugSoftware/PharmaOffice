#pragma once
#include "DataModel.h"
#include <boost/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>


//still need to thing
namespace pof {
	class SaleManager : public boost::noncopyable {

	public:
		enum {
			PRODUCT_NAME,
			PRODUCT_CATEGORY,
			PRODUCT_PRICE,
			PRODUCT_QUANTITY,
		};

		SaleManager();
		void DoSale();

		void LoadProductSaleHistory(const boost::uuids::uuid& productUUID);
	private:
		std::unique_ptr<pof::DataModel> SaleData;
		std::unique_ptr<pof::DataModel> ProductSaleHistory; //loaded in per product
	};
};