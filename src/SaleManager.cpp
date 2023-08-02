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
	Finalise();
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

bool pof::SaleManager::LoadHistoryByDate(const pof::base::data::datetime_t& dt)
{
	if (mLocalDatabase)
	{
		if (!mProductHistByDateStmt){
			constexpr const std::string_view sql = R"(SELECT s.sale_date, p.name, s.product_quantity, s.product_ext_price
					FROM sales s, products p
					WHERE s.sale_date BETWEEN ? AND ?;)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()){
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			mProductHistByDateStmt = *stmt;
		}
		auto ddt = dt + date::days(1);
		bool status = mLocalDatabase->bind(mProductHistByDateStmt, std::make_tuple(dt, ddt));
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto rel = mLocalDatabase->retrive<
			pof::base::data::datetime_t,
			pof::base::data::text_t,
			std::uint64_t,
			pof::base::data::currency_t
		>(mProductHistByDateStmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& rr = rel.value();
		ProductSaleHistory->Clear();
		ProductSaleHistory->GetDatastore().reserve(rr.size());

		for (auto& tup : rr) {
			pof::base::data::row_t row;
			auto& v = row.first;
			v.resize(HIST_MAX);

			v[HIST_DATE] = std::get<HIST_DATE>(tup);
			v[HIST_PROD_NAME] = std::get<HIST_PROD_NAME>(tup);
			v[HIST_QUANTITY] = std::get<HIST_QUANTITY>(tup);
			v[HIST_EXTPRICE] = std::get<HIST_EXTPRICE>(tup);

			ProductSaleHistory->EmplaceData(std::move(row));
		}
		return true;
	}
	return false;
}

void pof::SaleManager::LoadProductSaleHistory(const boost::uuids::uuid& productUUID)
{
	if (mLocalDatabase) {
		if (!mLoadProductHistory) {
			constexpr const std::string_view sql = R"(
				SELECT s.sale_date, p.name, s.product_quantity, s.product_ext_price
				FROM sales s, products p
				WHERE s.product_uuid = ? AND p.uuid = s.product_uuid
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
			v[HIST_EXTPRICE] = std::get<HIST_EXTPRICE>(tup);
			
			ProductSaleHistory->EmplaceData(std::move(row));
		}
	}

}

bool pof::SaleManager::CreateSaleTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view table = R"(CREATE TABLE IF NOT EXISTS sales (
		uuid blob,
		product_uuid blob,
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

bool pof::SaleManager::StoreSale()
{
	if (mLocalDatabase) {
		if (!mStoreSale) {
			constexpr const std::string_view sql = R"(INSERT INTO sales VALUES (?,?,?,?,?,?);)";
			auto stmt = mLocalDatabase->prepare(sql);
			if (!stmt.has_value()) {
				spdlog::error(mLocalDatabase->err_msg());
				return false;
			}
			mStoreSale = *stmt;
		}
		pof::base::relation<
			pof::base::data::duuid_t,
			pof::base::data::duuid_t,
			std::uint64_t,
			pof::base::data::currency_t,
			pof::base::data::datetime_t,
			pof::base::data::text_t
		> rel;
		auto& datastore = SaleData->GetDatastore();
		rel.reserve(datastore.size());

		try {
			auto saleDate = pof::base::data::clock_t::now();
			for (auto& sale : datastore) {
				auto& v = sale.first;
				rel.emplace_back(
					std::make_tuple(
						boost::variant2::get<pof::base::data::duuid_t>(v[SALE_UUID]),
						boost::variant2::get<pof::base::data::duuid_t>(v[PRODUCT_UUID]),
						boost::variant2::get<std::uint64_t>(v[PRODUCT_QUANTITY]),
						boost::variant2::get<pof::base::data::currency_t>(v[PRODUCT_EXT_PRICE]),
						saleDate,
						mCurPaymentType));
			}
			bool status = mLocalDatabase->store(mStoreSale, std::move(rel));
			if (!status) {
				spdlog::error(mLocalDatabase->err_msg());
			}
			return status;
		}
		catch (const std::exception& exp){
			spdlog::critical(exp.what());
			return false;
		}

	}
	return false;
}

bool pof::SaleManager::DoPrintReceipt(const pof::base::data::currency_t& totalAmount)
{
	const std::string style = R"(
		#invoice-POS{
				box-shadow: 0 0 1in -0.25in rgba(0, 0, 0, 0.5);
				padding:2mm;
				margin: 0 auto;
				width: 44mm;
				background: #FFF;
  
  
				::selection {background: #f31544; color: #FFF;}
				::moz-selection {background: #f31544; color: #FFF;}
		h1{
				font-size: 1.5em;
				color: #222;
		}
		h2{font-size: .9em;}
		h3{
			font-size: 1.2em;
			font-weight: 300;
			line-height: 2em;
		}
		p{
			font-size: .7em;
			color: #666;
			line-height: 1.2em;
		}
 
		#top, #mid,#bot{ /* Targets all id with 'col-' */
		border-bottom: 1px solid #EEE;
		}

		#top{min-height: 100px;}
		#mid{min-height: 80px;} 
		#bot{ min-height: 50px;}

		#top .logo{
			//float: left;
			height: 60px;
			width: 60px;
			background: url(http://michaeltruong.ca/images/logo1.png) no-repeat;
			background-size: 60px 60px;
		}
		.clientlogo{
			float: left;
			height: 60px;
			width: 60px;
			background: url(http://michaeltruong.ca/images/client.jpg) no-repeat;
			background-size: 60px 60px;
			border-radius: 50px;
		}
		.info{
			display: block;
			//float:left;
			margin-left: 0;
		}
		.title{
			float: right;
		}
		.title p{text-align: right;} 
		table{
			width: 100%;
			border-collapse: collapse;
		}
		td{
			//padding: 5px 0 5px 15px;
			//border: 1px solid #EEE
		}
		.tabletitle{
			//padding: 5px;
			font-size: .5em;
			background: #EEE;
		}
		.service{border-bottom: 1px solid #EEE;}
		.item{width: 24mm;}
		.itemtext{font-size: .5em;}

		#legalcopy{
			margin-top: 5mm;
		}

		}	
	)";

	html::Document doc("....");
	doc << html::Style(style);
	html::Div invoiceDiv("invoice");
	invoiceDiv.cls("Invoice-POS");
	html::Div top("top");
	top << html::Div("logo");
	html::Div info1("info");
	info1 << html::Header2("PAYMENT RECIEPT");
	top << std::move(info1);
	
	html::Div mid("mid");
	html::Div info2("info");
	info2 << html::Header2("Contact Info");

	std::ostringstream os;
	os << mCurPharmacy->name << "<br>\n";
	os << fmt::format("{} {} {} {} {} <br>\n", mCurPharmacy->addy.number,
			mCurPharmacy->addy.street, 
			mCurPharmacy->addy.city,
			mCurPharmacy->addy.state,
			mCurPharmacy->addy.country);
	os << fmt::format("{} {} <br>\n", mCurPharmacy->contact.phoneNumber, 
			mCurPharmacy->contact.email);
	
	info2 << html::Paragraph(os.str());
	mid << std::move(info2);

	html::Div bot("bot");
	html::Div tabDiv("table");
	html::Table tab;

	html::Row rTitle;
	rTitle.cls("tableTitle");
	html::Col colHeaders[3];
	colHeaders[0].cls("item");
	colHeaders[0].cls("Hours");
	colHeaders[0].cls("Rate");

	rTitle << (colHeaders[0] << html::Header2("Item"));
	rTitle << (colHeaders[1] << html::Header2("Qty"));
	rTitle << (colHeaders[2] << html::Header2("Price"));

	tab << std::move(rTitle);
	
	auto& datastore = SaleData->GetDatastore();
	for (const auto& row : datastore) {
		auto& v = row.first;
		html::Row r2;
		r2.cls("service");
		html::Col colBody[3];

		colBody[0].cls("tableitem");
		colBody[1].cls("tableitem");
		colBody[2].cls("tableitem");

		const pof::base::data::text_t& productName = boost::variant2::get<pof::base::data::text_t>(v[PRODUCT_NAME]);
		const std::uint64_t quantity = boost::variant2::get<std::uint64_t>(v[PRODUCT_QUANTITY]);
		const pof::base::data::currency_t& productCurreny = boost::variant2::get<pof::base::data::currency_t>(v[PRODUCT_EXT_PRICE]);

		colBody[0] << html::Paragraph(productName).cls("itemtext");
		colBody[1] << html::Paragraph(fmt::format("{:d}", quantity)).cls("itemtext");
		colBody[2] << html::Paragraph(fmt::format("{:cu}", productCurreny));

		for (int i = 0; i < 3; i++)
			r2 << std::move(colBody[i]);
		tab << std::move(r2);
	}

	html::Row r3;
	r3.cls("tabletitle");
	r3 << html::Col();

	html::Col vatC("VAT");
	vatC.cls("Rate");
	r3 << std::move(vatC);

	html::Col vatPayment(fmt::format("{:.1f}", vat));
	vatPayment.cls("payment");
	r3 << std::move(vatPayment);

	tab << std::move(r3);

	html::Row r4;
	r4.cls("tabletitle");
	r4 << html::Col();

	html::Col totl("TOTAL ");
	totl.cls("Rate");
	r4 << std::move(totl);

	html::Col totlAmt(fmt::format("{:cu}", totalAmount));
	totlAmt.cls("payment");
	r4 << std::move(totlAmt);

	tab << std::move(r4);
	tabDiv << std::move(tab);
	bot << std::move(tabDiv);

	invoiceDiv << std::move(top);
	invoiceDiv << std::move(mid);
	invoiceDiv << std::move(bot);

	doc << std::move(invoiceDiv);

	std::fstream fs("test.html", std::ios::out);
	if (!fs.is_open()) {
		spdlog::error("cannot open html file for test");
		return false;
	}
	fs << doc;
	return true;
}

void pof::SaleManager::Finalise()
{
}
