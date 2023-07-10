#pragma once
#include "DataModel.h"
#include <boost/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>
#include "database.h"
#include "Account.h"


//still need to thing
namespace pof {
	class SaleManager : public boost::noncopyable {

	public:
		enum {
			PRODUCT_UUID,
			PRODUCT_SERIAL_NUM,
			PRODUCT_NAME,
			PRODUCT_CATEGORY,
			PRODUCT_PRICE,
			PRODUCT_QUANTITY,
			PRODUCT_EXT_PRICE,
			MAX
		};

		SaleManager();
		~SaleManager();
		void DoSale();

		std::shared_ptr<pof::base::database> mLocalDatabase;
		std::shared_ptr<pof::Account> mCurAccount;

		void LoadProductSaleHistory(const boost::uuids::uuid& productUUID);
		inline std::unique_ptr<pof::DataModel>& GetSaleData() { return SaleData; }
		inline std::unique_ptr<pof::DataModel>& GetProductHistory() { return ProductSaleHistory; }

		bool CreateSaleTable();

		boost::uuids::random_generator_mt19937 sUidGen;
	private:
		
		
		std::unique_ptr<pof::DataModel> SaleData;
		std::unique_ptr<pof::DataModel> ProductSaleHistory; //loaded in per product

		pof::base::database::stmt_t mLoadProductHistory = nullptr;
		pof::base::database::stmt_t mStoreSale = nullptr;


	};
};