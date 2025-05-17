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
		std::optional<pof::base::relation<pof::base::data::datetime_t, std::uint64_t>> GetProductSaleHistoryChart(const boost::uuids::uuid& puid);
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
		void CreateSaleCostTable();
		void CreateDatabaseFunctions();

		//sale cost
		void AddSaleCost();
		std::optional<pof::base::data> GetProfitloss(const pof::base::data::datetime_t& dt);

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
		bool ChangePaymentOption(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid,
		 const std::string& option);
		bool ResellItem(const boost::uuids::uuid& saleID, const boost::uuids::uuid& puid, size_t quan);

		bool SaveInfo(const boost::uuids::uuid& saleID, const std::string& info);
		std::optional<std::string> GetInfo(const boost::uuids::uuid& saleID);
		bool RemoveInfo(const boost::uuids::uuid& saleID);
		bool UpdateInfo(const boost::uuids::uuid& saleID, const std::string& info);

		//sale analytics
		//gets the rank of products sold for that month
		std::optional<pof::base::data> GetProductSoldForMonth(const pof::base::data::datetime_t& dt);
		std::vector<pof::base::data::datetime_t> GetStoreYears();
		std::optional<pof::base::currency> GetYearTotalRevenue(const pof::base::data::datetime_t& dt);
		//removes all sale history for a product
		bool RemoveProductSaleHistory(pof::base::data::const_iterator iterator);
		bool RemoveProductSaveSale(pof::base::data::const_iterator iterator);

		bool RemoveProductFromSale(const pof::base::data::duuid_t& pid, const pof::base::data::duuid_t& sid);
		bool RemoveProductFromSale(size_t rowid, const pof::base::data::duuid_t& pid, const pof::base::data::duuid_t& sid);
		std::optional<std::tuple<std::uint64_t, pof::base::data::currency_t>> GetReturnedProductQuan(const pof::base::data::duuid_t& pid,
			const pof::base::data::duuid_t& sid);
		std::optional<pof::base::data::datetime_t> GetSaleDate(const pof::base::data::duuid_t& saleID);

		std::optional<pof::base::data> GetLastSale();
		std::optional<pof::base::data> GetWeeklySales(const pof::base::data::datetime_t& dt);
		std::optional<pof::base::data> GetSalesFor(const std::vector<pof::base::data::duuid_t>& prods,
			const std::pair<pof::base::data::datetime_t, pof::base::data::datetime_t>& dts);
		std::optional<std::pair<pof::base::currency, pof::base::currency>> GetTotalPL(const pof::base::data::datetime_t& dt);
		std::optional<pof::base::data> GetMonthlySales(const pof::base::data::datetime_t& dt);
		std::optional<std::uint64_t> SumOfSalesFrom(const pof::base::data::duuid_t& puid, pof::base::data::datetime_t dt);


		//transfers
		struct transfer {
			std::uint32_t transferID;
			std::string productName;
			pof::base::currency amount;
			std::uint32_t quantity;
			pof::base::data::datetime_t date;
			pof::base::data::duuid_t puid;
		};
		void CreateTransferTable() const;
		void AddTransfer(const std::vector<transfer>& products) const;
		void RemoveTransfer(const std::vector<transfer>& products) const;
		void UpdateTransferQuantity(std::uint32_t id, std::uint32_t quantity) const;
		std::optional<std::vector<transfer>> GetTransferByDate(const pof::base::data::datetime_t& time) const;




		boost::uuids::random_generator_mt19937 sUidGen;
		double vat = 0.0;
		double discount = 0.0;
		std::string mCurPaymentType;
	private:
		
		std::unique_ptr<pof::DataModel> SaleData;
		std::unique_ptr<pof::DataModel> ProductSaleHistory; //loaded in per product

		static void DBFuncYear(pof::base::database::conn_t conn,
			int arg, pof::base::database::value_arr_t values);

		static void DBFuncWeek(pof::base::database::conn_t conn,
			int arg, pof::base::database::value_arr_t values);

		static void DBFuncSubCost(pof::base::database::conn_t conn,
			int arg, pof::base::database::value_arr_t values);

		pof::base::database::stmt_t mLoadProductHistory = nullptr;
		pof::base::database::stmt_t mStoreSale = nullptr;
		pof::base::database::stmt_t mProductHistByDateStmt = nullptr;


	};
};