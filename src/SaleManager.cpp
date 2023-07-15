#include "SaleManager.h"

pof::SaleManager::SaleManager()
{
	SaleData = std::make_unique<pof::DataModel>();
	ProductSaleHistory = std::make_unique<pof::DataModel>();

	SaleData->Adapt<pof::base::data::duuid_t,
		pof::base::data::duuid_t,
		pof::base::data::text_t,
		pof::base::data::currency_t,
		std::uint64_t,
		pof::base::data::currency_t,
		pof::base::data::datetime_t>();

	ProductSaleHistory->Adapt<pof::base::data::datetime_t,
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::data::currency_t>();

}

pof::SaleManager::~SaleManager()
{
	SaleData.release();
	//ProductSaleHistory.release();
}

void pof::SaleManager::DoSale()
{
	wxBusyCursor cursor;
	auto& datastore = SaleData->GetDatastore();
	pof::base::data::currency_t totalAmount;
	for (auto& saleProduct : datastore){
		totalAmount += boost::variant2::get<pof::base::data::currency_t>(saleProduct.first[PRODUCT_EXT_PRICE]);
	}
	//SHOW PAYMENT DAILOG
	//paymentDialogData data
	// data.mTotalAmount = totalAmount;
	//PaymentDaialog payment(data);
	//if(payment.ShowModal == wxID_OK) {
	// collect payment data details,
	// write sale data to the database
}

void pof::SaleManager::LoadProductSaleHistory(const boost::uuids::uuid& productUUID)
{
	if (mLocalDatabase) {
		if (!mLoadProductHistory) {
			constexpr const std::string_view sql = R"(
				SELECT s.sale_date, s.product_name, s.product_quantity, s.product_ext_price
				FROM sales s
				WHERE s.product_uuid = ?
				ORDER BY s.sale_date DESC;
			)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return;
			}
			mLoadProductHistory = *stmt;
		}
		bool status = mLocalDatabase->bind(mLoadProductHistory, std::make_tuple(productUUID));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::datetime_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t
		>(mLoadProductHistory);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return;
		}
		using relation_t = std::decay_t<decltype(*rel)>;
		using tup_t = typename relation_t::tuple_t;
		for (auto& tup : *rel) {
			pof::base::data::row_t row;
			auto& v = row.first;
			v.resize(HIST_MAX);

			v[HIST_DATE] = std::get<HIST_DATE>(tup);
			v[HIST_PROD_NAME] = std::get<HIST_PROD_NAME>(tup);
			v[HIST_QUANTITY] = std::get<HIST_QUANTITY>(tup);
			v[HIST_DATE] = std::get<HIST_EXTPRICE>(tup);
			
			ProductSaleHistory->EmplaceData(std::move(row));
		}
	}

}

bool pof::SaleManager::CreateSaleTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view table = R"(CREATE TABLE IF NOT EXSITS sales (
		uuid blob,
		product_uuid blob,
		product_name text,
		product_price blob,
		product_quantity integer,
		product_ext_price blob,
		sale_date integer,
		sale_payment_type text
		); )";
		auto stmt = mLocalDatabase->prepare(table);
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}

		mLocalDatabase->finalise(*stmt);
		return true;
	}
	return false;
}

bool pof::SaleManager::DoPrintReceipt()
{
	/*
	* 
  <div id="invoice-POS">
    
    <center id="top">
      <div class="logo"></div>
      <div class="info"> 
        <h2>SBISTechs Inc</h2>
      </div><!--End Info-->
    </center><!--End InvoiceTop-->
    
    <div id="mid">
      <div class="info">
        <h2>Contact Info</h2>
        <p> 
            Address : street city, state 0000</br>
            Email   : JohnDoe@gmail.com</br>
            Phone   : 555-555-5555</br>
        </p>
      </div>
    </div><!--End Invoice Mid-->
    
    <div id="bot">

					<div id="table">
						<table>
							<tr class="tabletitle">
								<td class="item"><h2>Item</h2></td>
								<td class="Hours"><h2>Qty</h2></td>
								<td class="Rate"><h2>Sub Total</h2></td>
							</tr>

							<tr class="service">
								<td class="tableitem"><p class="itemtext">Communication</p></td>
								<td class="tableitem"><p class="itemtext">5</p></td>
								<td class="tableitem"><p class="itemtext">$375.00</p></td>
							</tr>

							<tr class="service">
								<td class="tableitem"><p class="itemtext">Asset Gathering</p></td>
								<td class="tableitem"><p class="itemtext">3</p></td>
								<td class="tableitem"><p class="itemtext">$225.00</p></td>
							</tr>

							<tr class="service">
								<td class="tableitem"><p class="itemtext">Design Development</p></td>
								<td class="tableitem"><p class="itemtext">5</p></td>
								<td class="tableitem"><p class="itemtext">$375.00</p></td>
							</tr>

							<tr class="service">
								<td class="tableitem"><p class="itemtext">Animation</p></td>
								<td class="tableitem"><p class="itemtext">20</p></td>
								<td class="tableitem"><p class="itemtext">$1500.00</p></td>
							</tr>

							<tr class="service">
								<td class="tableitem"><p class="itemtext">Animation Revisions</p></td>
								<td class="tableitem"><p class="itemtext">10</p></td>
								<td class="tableitem"><p class="itemtext">$750.00</p></td>
							</tr>


							<tr class="tabletitle">
								<td></td>
								<td class="Rate"><h2>tax</h2></td>
								<td class="payment"><h2>$419.25</h2></td>
							</tr>

							<tr class="tabletitle">
								<td></td>
								<td class="Rate"><h2>Total</h2></td>
								<td class="payment"><h2>$3,644.25</h2></td>
							</tr>

						</table>
					</div><!--End Table-->

					<div id="legalcopy">
						<p class="legal"><strong>Thank you for your business!</strong>  Payment is expected within 31 days; please process this invoice within that time. There will be a 5% interest charge per month on late invoices. 
						</p>
					</div>

				</div><!--End InvoiceBot-->
  </div><!--End Invoice-->

	*/

	html::Document doc("....");

	html::Div invoiceDiv("invoice");
	html::Div top("top");

	top << html::Div("logo");


	doc << std::move(invoiceDiv);
	return false;
}
