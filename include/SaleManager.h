#pragma once
#include "DataModel.h"
#include <boost/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>
#include <fstream>
#include <sstream>
#include <date/date.h>

#include "database.h"
#include "Account.h"
#include "Pharmacy.h"


#include <wx/html/htmprint.h>
#include <dep/HTML/HTML.h>

namespace html = HTML;
//still need to thing
namespace pof {
	class SaleManager : public boost::noncopyable {

	public:
		enum {
			SALE_UUID,
			PRODUCT_UUID,
			PRODUCT_NAME,
			PRODUCT_PRICE,
			PRODUCT_QUANTITY,
			PRODUCT_EXT_PRICE,
			SALE_DATE,
			MAX
		};

		enum {
			HIST_DATE,
			HIST_PROD_NAME,
			HIST_QUANTITY,
			HIST_EXTPRICE,
			HIST_MAX
		};

		//sale payment type
		enum {
			BANK_TRANSFER,
			BANK_CARD,
			CASH,
		};
		constexpr static const std::array<std::string_view, 3> sPaymentType = { "BANK TRANSFER", "BANK CARD", "CASH" };

		SaleManager();
		~SaleManager();
		void DoSale();

		std::shared_ptr<pof::base::database> mLocalDatabase;
		std::shared_ptr<pof::Account> mCurAccount;
		std::shared_ptr<pof::Pharmacy> mCurPharmacy;

		bool LoadHistoryByDate(const pof::base::data::duuid_t& ud, const pof::base::data::datetime_t& dt);
		void LoadProductSaleHistory(const boost::uuids::uuid& productUUID);
		inline std::unique_ptr<pof::DataModel>& GetSaleData() { return SaleData; }
		inline std::unique_ptr<pof::DataModel>& GetProductHistory() { return ProductSaleHistory; }

		bool CreateSaleTable();
		bool StoreSale();
		void Finalise();
		bool RestoreSale(const boost::uuids::uuid& saleID);
		bool CheckIfSaleExists(const boost::uuids::uuid& saleId);
		bool RestoreLastSale();

		//SAVE SALE
		void CreateSaveSaleTable();
		void CreateSaleLabelTable();
		void CreateSaleInfoTable();

		bool RestoreSaveSale(const boost::uuids::uuid& saleID);
		bool RemoveSaveSale(const boost::uuids::uuid& saleID);
		bool SaveLabels(const boost::uuids::uuid& saleID, const std::string& labelData);
		std::optional<std::string> LoadLabels(const boost::uuids::uuid& saleID);
		bool RemoveLabels(const boost::uuids::uuid& saleID);
		bool SaveSale(const boost::uuids::uuid& saleID); //pushes the current sale in mSaleData; 
		std::optional<pof::base::relation<pof::base::data::datetime_t, boost::uuids::uuid, pof::base::currency>> GetSavedSales();
		bool CheckIfSaved(const boost::uuids::uuid& saleID);
		bool ReturnFromSales(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid);
		bool CheckReturned(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid);
		
		bool SaveInfo(const boost::uuids::uuid& saleID, const std::string& info);
		std::optional<std::string> GetInfo(const boost::uuids::uuid& saleID);
		bool RemoveInfo(const boost::uuids::uuid& saleID);
		bool UpdateInfo(const boost::uuids::uuid& saleID, const std::string& info);


		//removes all sale history for a product
		bool RemoveProductSaleHistory(pof::base::data::const_iterator iterator);
		bool RemoveProductSaveSale(pof::base::data::const_iterator iterator);

		bool RemoveProductFromSale(const pof::base::data::duuid_t& pid, const pof::base::data::duuid_t& sid);
		std::optional<std::tuple<std::uint64_t, pof::base::data::currency_t>> GetReturnedProductQuan(const pof::base::data::duuid_t& pid,
			const pof::base::data::duuid_t& sid);
		std::optional<pof::base::data::datetime_t> GetSaleDate(const pof::base::data::duuid_t& saleID);

		std::optional<pof::base::data> GetLastSale();

		boost::uuids::random_generator_mt19937 sUidGen;
		double vat = 0.0;
		double discount = 0.0;
		std::string mCurPaymentType;
	private:
		
		std::unique_ptr<pof::DataModel> SaleData;
		std::unique_ptr<pof::DataModel> ProductSaleHistory; //loaded in per product

		pof::base::database::stmt_t mLoadProductHistory = nullptr;
		pof::base::database::stmt_t mStoreSale = nullptr;
		pof::base::database::stmt_t mProductHistByDateStmt = nullptr;


	};
};