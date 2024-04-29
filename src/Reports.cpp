#include "Reports.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::ReportsDialog, wxDialog)
EVT_TOOL(pof::ReportsDialog::ID_PRINT, pof::ReportsDialog::OnPrint)
EVT_TOOL(pof::ReportsDialog::ID_EXCEL, pof::ReportsDialog::OnDownloadExcel)
EVT_MENU(pof::ReportsDialog::ID_ADD_RETURNS, pof::ReportsDialog::OnAddReturns)
EVT_MENU(pof::ReportsDialog::ID_REMOVE_SALE, pof::ReportsDialog::OnRemoveSale)
EVT_DATE_CHANGED(pof::ReportsDialog::ID_EOD_DATE, pof::ReportsDialog::OnDateChange)
EVT_BUTTON(pof::ReportsDialog::ID_SEARCH_SALEID, pof::ReportsDialog::OnSaleIdSearch)
EVT_SEARCH_CANCEL(pof::ReportsDialog::ID_SEARCH_SALEID, pof::ReportsDialog::OnSaleIdCleared)

EVT_LIST_ITEM_RIGHT_CLICK(pof::ReportsDialog::ID_REPORT_LIST, pof::ReportsDialog::OnEodRightClick)
END_EVENT_TABLE()

pof::ReportsDialog::ReportsDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSize(FromDIP(size));
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	m_panel5 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	m_panel5->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	mTools = new wxAuiToolBar(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTools->SetMaxSize(FromDIP(wxSize(-1, 30)));
	
	bSizer7->Add(mTools, 1, wxALL | wxEXPAND, FromDIP(2));
	mBook = new wxSimplebook(m_panel5, ID_BOOK, wxDefaultPosition, wxDefaultSize, 0);

	panel = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	mSPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	bSizer4->AddStretchSpacer();

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));

	mTotalQuantity = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalQuantity->SetFont(wxFont(wxFontInfo().Bold().AntiAliased()));
	mTotalQuantity->Wrap(-1);
	bSizer4->Add(mTotalQuantity, 0, wxALL, FromDIP(5));

	bSizer4->AddSpacer(FromDIP(5));

	l1 = new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer4->Add( l1 , wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));

	mTotalAmount = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmount->SetFont(wxFont(wxFontInfo().Bold().AntiAliased()));
	mTotalAmount->Wrap(-1);
	bSizer4->Add(mTotalAmount, 0, wxALL, FromDIP(5));

	bSizer4->AddSpacer(FromDIP(5));

	l2 = new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer4->Add( l2 , wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));

	mTotalAmountCash = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountCash->SetFont(wxFont(wxFontInfo().Bold().AntiAliased()));
	mTotalAmountCash->Wrap(-1);
	bSizer4->Add(mTotalAmountCash, 0, wxALL, FromDIP(5));


	bSizer4->AddSpacer(FromDIP(5));

	l3 = new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer4->Add( l3 , wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));

	mTotalAmountTransfer = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountTransfer->SetFont(wxFont(wxFontInfo().Bold().AntiAliased()));
	mTotalAmountTransfer->Wrap(-1);
	bSizer4->Add(mTotalAmountTransfer, 0, wxALL, FromDIP(5));


	bSizer4->AddSpacer(FromDIP(5));

	l4 = new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer4->Add( l4, wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));

	mTotalAmountPos = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountPos->SetFont(wxFont(wxFontInfo().Bold().AntiAliased()));
	mTotalAmountPos->Wrap(-1);
	bSizer4->Add(mTotalAmountPos, 0, wxALL, FromDIP(5));

	//bSizer4->AddSpacer(FromDIP(5));

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddStretchSpacer();

	mSPanel->SetSizer(bSizer4);
	mSPanel->Layout();
	bSizer4->Fit(mSPanel);


	mCSPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8 = new wxBoxSizer(wxHORIZONTAL);

	mTotalClosingStock = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalClosingStock->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalClosingStock->Wrap(-1);
	bSizer8->Add(mTotalClosingStock, 0, wxALL, FromDIP(5));

	bSizer8->AddSpacer(FromDIP(5));

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(FromDIP(5));

	mTotalExpiredStock = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalExpiredStock->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalExpiredStock->Wrap(-1);
	bSizer8->Add(mTotalExpiredStock, 0, wxALL, FromDIP(5));

	bSizer8->AddSpacer(FromDIP(5));

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(FromDIP(5));

	mTotalInventoryIn = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalInventoryIn->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalInventoryIn->Wrap(-1);
	bSizer8->Add(mTotalInventoryIn, 0, wxALL, 5);

	bSizer8->AddSpacer(FromDIP(5));

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(FromDIP(5));

	mTotalAmountIn = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountIn->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmountIn->Wrap(-1);
	bSizer8->Add(mTotalAmountIn, 0, wxALL, FromDIP(5));

	bSizer8->AddSpacer(FromDIP(5));

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(FromDIP(5));

	mTotalInventoryOut = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalInventoryOut->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalInventoryOut->Wrap(-1);
	bSizer8->Add(mTotalInventoryOut, 0, wxALL, FromDIP(5));

	bSizer8->AddSpacer(FromDIP(5));

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(FromDIP(5));


	mTotalAmountOut = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountOut->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmountOut->Wrap(-1);
	bSizer8->Add(mTotalAmountOut, 0, wxALL, FromDIP(5));

	mCSPanel->SetSizer(bSizer8);
	mCSPanel->Layout();
	bSizer8->Fit(mCSPanel);

	mListReport = new wxListCtrl(panel, ID_REPORT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxNO_BORDER | wxLC_HRULES | wxLC_VRULES);
	wxImageList* imageList = new wxImageList(16, 16);
	imageList->Add(wxArtProvider::GetBitmap("action_check"));
	imageList->Add(wxArtProvider::GetBitmap("action_delete"));
	mListReport->AssignImageList(imageList, wxIMAGE_LIST_NORMAL);
	mListReport->Bind(wxEVT_LIST_ITEM_SELECTED, [&](wxListEvent& evt) {
		mSelItem = evt.GetItem();
	});
	mListReport->SetFont(wxGetApp().mFontSettings.GetChosenFont());
	CreateEmptyEodPage();

	sz->Add(mListReport, 1, wxEXPAND | wxALL, 0);
	sz->Add(mSPanel, 0, wxEXPAND | wxALL, FromDIP(2));
	sz->Add(mCSPanel, 0, wxEXPAND | wxALL, FromDIP(2));

	mSPanel->Hide();
	mCSPanel->Hide();

	panel->SetSizer(sz);
	sz->SetSizeHints(panel);
	panel->Layout();

	mBook->AddPage(panel, "Report", true);
	mBook->AddPage(mEmptyEod, "Empty", false);

	bSizer7->Add(mBook, 1, wxALL | wxEXPAND, 0);
	m_panel5->SetSizer(bSizer7);
	m_panel5->Layout();
	bSizer7->Fit(m_panel5);
	bSizer6->Add(m_panel5, 1, wxEXPAND | wxALL, 0);


	this->SetSizer(bSizer6);
	this->Layout();

	this->Centre(wxBOTH);

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);

	//bind some events
	mListReport->Bind(wxEVT_MENU, [&](wxCommandEvent& evt) {
		if (!bShowSaleID) return;

		wxClipboardLocker clipLocker;
		if (!clipLocker) {
			spdlog::error("Can't open the clipboard");
			return;
		}

		auto str = mListReport->GetItemText(mSelItem.GetId(), 5);
		wxTextDataObject* tObj = new wxTextDataObject(std::move(str));
		wxTheClipboard->Clear();
		wxTheClipboard->AddData(tObj);

		}, ID_COPY_RECIEPT_ID);
	mListReport->Bind(wxEVT_MENU, std::bind_front(&pof::ReportsDialog::OnDoReturn, this), ID_RETURN_SALE);
}

pof::ReportsDialog::~ReportsDialog()
{
}

bool pof::ReportsDialog::LoadReport(ReportType repType, pof::base::data::datetime_t month)
{
	bool ret = false;
	mCurReportType = repType;
	CreateToolBar();
	if (mCurReportType == ReportType::EOD) {
		//mSelectDay = pof::base::data::clock_t::now();
	}
	else {
		auto set = pof::base::data::clock_t::now();
		mSelectDay = set;
	}

	switch (repType)
	{
	case pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN:
		ret = LoadConsumptionPattern(month);
		mCSPanel->Show();
		SetTitle("Consumption Pattern");
		break;
	case pof::ReportsDialog::ReportType::EOD:
	case pof::ReportsDialog::ReportType::EOM:
		SetTitle("Reports");
		ret = LoadEndOFDay();
		mSPanel->Show();
		break;
	case ReportType::IM:
		SetTitle("Stock inventory report");
		ret = LoadInventoryMonth();
		l2->Hide();
		l3->Hide();
		l4->Hide();
		mSPanel->Show();
		break;
	case ReportType::PSM:
		SetTitle("Products sold for month");
		ret = LoadProductSoldForMonth();
		l3->Hide();
		l4->Hide();
		mSPanel->Show();
		break;
	case ReportType::PL:
		SetTitle("Profit/Loss report");
		ret = LoadProfitLoss();
		mSPanel->Show();
		l4->Hide();
		break;
	default:
		break;
	}
	panel->Layout();
	panel->Refresh();
	return ret;
}

bool pof::ReportsDialog::LoadConsumptionPattern(pof::base::data::datetime_t month)
{
	data = wxGetApp().mProductManager.GetConsumptionPattern(month);
	if (!data.has_value()) return false;

	if (data->empty()) {
		mBook->SetSelection(REPORT_EMPTY_EOD);
		//wxMessageBox("NO Transaction has occured in your store this month", "CONSUMPTION PATTERN", wxICON_INFORMATION | wxOK);
		return true;
	}
	mSelectedMonth = month;
	auto& report = *mListReport;
	wxBusyCursor cursor;
	//create the columns
	mTools->Freeze();
	std::string tt = fmt::format("Consumption pattern for {:%m/%y}", month);
	text->SetFont(wxFontInfo().AntiAliased().Bold().Italic());
	text->SetLabelText(tt);
	textItem->SetMinSize(text->GetSize());
	//text->Layout();
	text->Update();
	mTools->Thaw();
	mTools->Realize();
	mTools->Refresh();

	report.AppendColumn("Product", wxLIST_FORMAT_LEFT, FromDIP(230));
	report.AppendColumn("Closing stock", wxLIST_FORMAT_LEFT, FromDIP(150));
	report.AppendColumn("Expired stock", wxLIST_FORMAT_LEFT, FromDIP(150));
	report.AppendColumn("Inventory in", wxLIST_FORMAT_LEFT, FromDIP(150));
	report.AppendColumn("Amount in", wxLIST_FORMAT_LEFT, FromDIP(150));
	report.AppendColumn("Inventory out", wxLIST_FORMAT_LEFT, FromDIP(150));
	report.AppendColumn("Amount out", wxLIST_FORMAT_LEFT, FromDIP(150));

	wxItemAttr attr;
	attr.SetBackgroundColour(*wxBLACK);
	attr.SetFont(wxFontInfo().Bold().AntiAliased());
	report.SetHeaderAttr(attr);

	size_t i = 0;
	
	for (auto iter = data->begin(); iter != data->end(); iter++) {
		auto& v = iter->first;
		wxListItem item;

		item.SetColumn(0);
		item.SetId(i);
		item.SetText(boost::variant2::get<pof::base::data::text_t>(v[1]));
		item.SetMask(wxLIST_MASK_TEXT);
		report.InsertItem(item);

		item.SetColumn(1);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", boost::variant2::get<std::uint64_t>(v[2])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		item.SetColumn(2);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", boost::variant2::get<std::uint64_t>(v[3])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		item.SetColumn(3);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", boost::variant2::get<std::uint64_t>(v[4])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);


		item.SetColumn(4);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(v[5])));
		item.SetMask(wxLIST_MASK_TEXT );
		report.SetItem(item);


		item.SetColumn(5);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", boost::variant2::get<std::uint64_t>(v[6])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);


		item.SetColumn(6);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(v[7])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		i++;
	}

	UpdateConsumptionTotals(data.value());
	return true;
}

bool pof::ReportsDialog::LoadEndOFDay()
{
	std::string tt; 

	if (mCurReportType == ReportType::EOD){
		data = wxGetApp().mProductManager.GetEndOfDay(mSelectDay);
		tt = fmt::format("End of day for {:%d/%m/%Y}", mSelectDay);
	} else if (mCurReportType == ReportType::EOM){
		data = wxGetApp().mProductManager.GetEndOfMonth(mSelectDay);
		tt = fmt::format("End of month for {:%m/%Y}", mSelectDay);
	}

	if (!data.has_value()) return false;

	if (data->empty()) {
		mBook->SetSelection(REPORT_EMPTY_EOD);
		if (mCurReportType == ReportType::EOD) {
			tt = fmt::format("No transaction for {:%d/%m/%Y}", mSelectDay);
		}
		else if (mCurReportType == ReportType::EOM) {
			tt = fmt::format("No transaction for {:%m/%Y}", mSelectDay);
		}
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());
		mTools->Refresh();
		//mTools->Realize();

		return true;
	}

	if (bFilterReturns){
		auto& tab = data->tab();
		auto i = std::ranges::remove_if(tab, [&](const pof::base::data::row_t& row) -> bool {
			auto& pt = boost::variant2::get<std::string>(row.first[6]);
			return pt == "Returned";
		});
		tab.erase(i.begin(), i.end());
		if (data->empty()) {
			mBook->SetSelection(REPORT_EMPTY_EOD);
			text->SetLabelText(wxEmptyString);
			textItem->SetMinSize(text->GetSize());

			return true;
		}
	}

	auto& report = *mListReport;
	{
		//load data into grid
		wxBusyCursor cursor;
		pof::base::currency totalAmount;
		std::uint64_t totalQuan = 0;

		mTools->Freeze();
		text->SetFont(wxFontInfo().AntiAliased().Bold().Italic());
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());
		text->Update();
		mTools->Thaw();
		mTools->Realize();
		mTools->Refresh();
		m_panel5->Layout();


		//make the columns
		if (mCurReportType == ReportType::EOM){
			report.AppendColumn("Day", wxLIST_FORMAT_LEFT, FromDIP(200));
		}
		else {
			report.AppendColumn("Time", wxLIST_FORMAT_LEFT, FromDIP(200));
		}
		report.AppendColumn("Product", wxLIST_FORMAT_LEFT, FromDIP(200));
		report.AppendColumn("Quantity", wxLIST_FORMAT_LEFT, FromDIP(200));
		report.AppendColumn("Amount", wxLIST_FORMAT_LEFT, FromDIP(200));
		report.AppendColumn("Payment opiton", wxLIST_FORMAT_LEFT, FromDIP(100));

		if (bShowSaleID) {
			report.AppendColumn("Reciept id", wxLIST_FORMAT_CENTER, FromDIP(205));
		}

		size_t i = 0;
		for (auto iter = data->begin(); iter != data->end(); iter++) {
			auto& v = iter->first;
			wxListItem item;

			item.SetColumn(0);
			item.SetId(i);
			if (mCurReportType == ReportType::EOM)
			{
				item.SetText(std::format("{:%d/%m/%Y}", boost::variant2::get<pof::base::data::datetime_t>(v[1])));
			}
			else {
				item.SetText(std::format("{:%H:%M}", boost::variant2::get<pof::base::data::datetime_t>(v[1])));
			}
			item.SetMask(wxLIST_MASK_TEXT);
			report.InsertItem(item);


			item.SetColumn(1);
			item.SetId(i);
			item.SetText(boost::variant2::get<pof::base::data::text_t>(v[2]));
			item.SetMask(wxLIST_MASK_TEXT);
			report.SetItem(item);

			std::uint64_t quan = boost::variant2::get<std::uint64_t>(v[3]);
			totalQuan += quan;
			item.SetColumn(2);
			item.SetId(i);
			item.SetText(fmt::format("{:d}", quan));
			item.SetMask(wxLIST_MASK_TEXT);
			report.SetItem(item);

			auto& amount = boost::variant2::get<pof::base::data::currency_t>(v[4]);
			totalAmount += amount;
			item.SetColumn(3);
			item.SetId(i);
			item.SetText(fmt::format("{:cu}", amount));
			item.SetMask(wxLIST_MASK_TEXT);
			report.SetItem(item);

			item.SetColumn(4);
			item.SetId(i);
			item.SetText(boost::variant2::get<pof::base::data::text_t>(v[6]));
			item.SetMask(wxLIST_MASK_TEXT);
			report.SetItem(item);

			if (bShowSaleID){
				item.SetColumn(5);
				item.SetId(i);
				item.SetText(boost::uuids::to_string(boost::variant2::get<boost::uuids::uuid>(v[5])));
				item.SetMask(wxLIST_MASK_TEXT);
				report.SetItem(item);
			}

			i++;
		}
	}
	if (mBook->GetSelection() == REPORT_EMPTY_EOD){
		mBook->SetSelection(REPORT_VIEW);
	}
	UpdateTotals(data.value());
	return true;
}

bool pof::ReportsDialog::LoadEndOfMonth()
{
	return false;
}

bool pof::ReportsDialog::LoadInventoryMonth()
{
	data = wxGetApp().mProductManager.GetInventoryForMonth(mSelectDay);
	if(!data.has_value()) return false;
	std::string tt;
	if (data->empty()) {
		tt = fmt::format("No transaction for {:%m/%Y}", mSelectDay);
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());

		mBook->SetSelection(REPORT_EMPTY_EOD);
		return true;
	}
	else {
		tt = fmt::format("Inventory purchase for {:%m/%Y}", mSelectDay);
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());
	}

	auto& report = *mListReport;
	report.AppendColumn("Product", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Quantity", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Amount (Cost)", wxLIST_FORMAT_LEFT, FromDIP(200));

	size_t i = 0;
	for (auto iter = data->begin(); iter != data->end(); iter++) {
		auto& v = iter->first;
		wxListItem item;

		item.SetColumn(0);
		item.SetId(i);
		item.SetText(boost::variant2::get<pof::base::data::text_t>(v[1]));
		item.SetMask(wxLIST_MASK_TEXT);
		report.InsertItem(item);

		std::uint64_t quan = boost::variant2::get<std::uint64_t>(v[2]);
		item.SetColumn(1);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", quan));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		auto& amount = boost::variant2::get<pof::base::data::currency_t>(v[3]);
		item.SetColumn(2);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", amount));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);
		
		i++;
	}
	if (mBook->GetSelection() == REPORT_EMPTY_EOD) {
		mBook->SetSelection(REPORT_VIEW);
	}
	UpdateTotals(data.value());
	return true;
}

bool pof::ReportsDialog::LoadProfitLoss()
{
	data = wxGetApp().mSaleManager.GetProfitloss(mSelectDay);
	if (!data.has_value()) return false;
	
	std::string tt;
	if (data->empty()) {
		tt = fmt::format("No transaction for {:%m/%Y}", mSelectDay);
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());

		mBook->SetSelection(REPORT_EMPTY_EOD);
		return true;
	}
	else {
		tt = fmt::format("Profit/Loss for {:%m/%Y}", mSelectDay);
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());
	}

	auto& report = *mListReport;
	report.AppendColumn("Date", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Product", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Quantity", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Amount (Sell)", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Amount (Cost)", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Profit/Loss", wxLIST_FORMAT_LEFT, FromDIP(200));

	size_t i = 0;
	for (auto iter = data->begin(); iter != data->end(); iter++) {
		auto& v = iter->first;
		wxListItem item;

		item.SetColumn(0);
		item.SetId(i);
		item.SetText(fmt::format("{:%d/%m/%Y}", boost::variant2::get<pof::base::data::datetime_t>(v[0])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.InsertItem(item);


		item.SetColumn(1);
		item.SetId(i);
		item.SetText(boost::variant2::get<pof::base::data::text_t>(v[1]));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		std::uint64_t quan = boost::variant2::get<std::uint64_t>(v[2]);
		item.SetColumn(2);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", quan));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		auto amount = boost::variant2::get<pof::base::data::currency_t>(v[3]);
		item.SetColumn(3);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", amount));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		amount = boost::variant2::get<pof::base::data::currency_t>(v[4]);
		item.SetColumn(4);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", amount));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);


		amount = (boost::variant2::get<pof::base::data::currency_t>(v[3]) - boost::variant2::get<pof::base::data::currency_t>(v[4]));
		item.SetColumn(5);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", amount));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		i++;
	}
	if (mBook->GetSelection() == REPORT_EMPTY_EOD) {
		mBook->SetSelection(REPORT_VIEW);
	}
	UpdateTotals(data.value());
	return true;
}

bool pof::ReportsDialog::LoadProductSoldForMonth()
{
	data = wxGetApp().mSaleManager.GetProductSoldForMonth(mSelectDay);
	if (!data.has_value()) return false; //this makes no sense
	std::string tt;
	if (data->empty()) {
		tt = fmt::format("No transaction for {:%m/%Y}", mSelectDay);
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());

		mBook->SetSelection(REPORT_EMPTY_EOD);
		return true;
	}
	else {
		tt = fmt::format("Product sold for {:%m/%Y}", mSelectDay);
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());
	}


	auto& report = *mListReport;
	report.AppendColumn("Product", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Quantity", wxLIST_FORMAT_LEFT, FromDIP(200));
	report.AppendColumn("Amount", wxLIST_FORMAT_LEFT, FromDIP(200));

	size_t i = 0;
	for (auto iter = data->begin(); iter != data->end(); iter++) {
		auto& v = iter->first;
		wxListItem item;

		item.SetColumn(0);
		item.SetId(i);
		item.SetText(boost::variant2::get<pof::base::data::text_t>(v[1]));
		item.SetMask(wxLIST_MASK_TEXT);
		report.InsertItem(item);

		std::uint64_t quan = boost::variant2::get<std::uint64_t>(v[2]);
		item.SetColumn(1);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", quan));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		auto& amount = boost::variant2::get<pof::base::data::currency_t>(v[3]);
		item.SetColumn(2);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", amount));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		i++;
	}
	if (mBook->GetSelection() == REPORT_EMPTY_EOD) {
		mBook->SetSelection(REPORT_VIEW);
	}
	UpdateTotals(data.value());
	return true;
}

void pof::ReportsDialog::OnPrint(wxCommandEvent& evt)
{
}

void pof::ReportsDialog::OnDownloadExcel(wxCommandEvent& evt)
{
	switch (mCurReportType)
	{
	case ReportType::COMSUMPTION_PATTARN:
		ConsumptionPatternExcel(mSelectedMonth);
		break;
	case ReportType::EOM:
	case ReportType::EOD:
		EODExcel();
	case ReportType::IM:
		InventoryMonthReportExcel();
		break;
	case ReportType::PL:
		ProfitLossExcel();
		break;
	case ReportType::PSM:
		ProductSoldExcel();
		break;
	default:
		break;
	}
}

void pof::ReportsDialog::OnReportSelectSelected(wxListEvent& evt)
{
}

void pof::ReportsDialog::OnDateChange(wxDateEvent& evt)
{
	auto dd = evt.GetDate();
	std::chrono::month month = static_cast<std::chrono::month>(dd.GetMonth() + 1);
	std::chrono::year year = static_cast<std::chrono::year>(dd.GetYear());
	std::chrono::day day = static_cast<std::chrono::day>(dd.GetDay());
	const std::chrono::year_month_day ymd{ month / day / year};
	mSelectDay = static_cast<std::chrono::sys_days>(ymd);

	m_panel5->Freeze();
	mListReport->Freeze();
	mListReport->ClearAll();
	switch (mCurReportType)
	{
	case pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN:
		break;
	case pof::ReportsDialog::ReportType::EOD:
	case pof::ReportsDialog::ReportType::EOM:
		LoadEndOFDay();
		break;
	case pof::ReportsDialog::ReportType::IM:
		LoadInventoryMonth();
		break;
	case pof::ReportsDialog::ReportType::PL:
		LoadProfitLoss();
		break;
	case pof::ReportsDialog::ReportType::PSM:
		LoadProductSoldForMonth();
		break;
	default:
		break;
	}
	mListReport->Thaw();
	mListReport->Refresh();
	m_panel5->Thaw();
	m_panel5->Refresh();
}

void pof::ReportsDialog::OnEodRightClick(wxListEvent& evt)
{
	if (mCurReportType == ReportType::EOD || mCurReportType == ReportType::EOM){
		constexpr const std::array<std::string_view, 3> opts = {
		"Cash", "Transfer", "POS"
		};
		
		wxMenu* menu = new wxMenu;
		size_t id = mSelItem.GetId();
		auto& sale_type = boost::variant2::get<std::string>(data.value()[id].first[6]);
		if (sale_type == "Returned"){
			auto x = menu->Append(ID_ADD_RETURNS, "Resell item", nullptr);
		}
		else {
			auto x = menu->Append(ID_RETURN_SALE, "Return sale", nullptr);
		}
		

		menu->AppendSeparator();
		if(bShowSaleID) auto p = menu->Append(ID_COPY_RECIEPT_ID, "Copy receipt ID", nullptr);

		wxMenu* submenu = new wxMenu;
		submenu->Append(0, opts[0].data(), nullptr);
		submenu->Append(1, opts[1].data(), nullptr);
		submenu->Append(2, opts[2].data(), nullptr);

		submenu->Bind(wxEVT_MENU, std::bind_front(&pof::ReportsDialog::OnChangePaymentOption, this), 0);
		submenu->Bind(wxEVT_MENU, std::bind_front(&pof::ReportsDialog::OnChangePaymentOption, this), 1);
		submenu->Bind(wxEVT_MENU, std::bind_front(&pof::ReportsDialog::OnChangePaymentOption, this), 2);


		auto a = menu->Append(ID_CHANGE_PAYMENT_OPT, "Change payment option", submenu);
		menu->AppendSeparator();

		if (mCurReportType == ReportType::EOD){
			auto ip = menu->Append(ID_REMOVE_SALE, "Remove sale from databaase", nullptr, "This removes the sale from the system, requires admin previlages");
		}

		mListReport->PopupMenu(menu);
	}
}

void pof::ReportsDialog::OnSaleIdSearch(wxCommandEvent& evt)
{
	auto string = mSaleIdSearch->GetValue().ToStdString();
	if (string.empty()) return;
	bool fail = false;
	try {
		boost::uuids::uuid suid = boost::lexical_cast<boost::uuids::uuid>(string);
		auto dt = wxGetApp().mSaleManager.GetSaleDate(suid);
		if (!dt.has_value()) {
			wxMessageBox("Invalid receipt id", "Reports", wxICON_INFORMATION | wxOK);
			return;
		}
		mSelectDay = dt.value() - date::days(1); //why is it one day ahead

		m_panel5->Freeze();
		mListReport->Freeze();
		mListReport->ClearAll();
		
		LoadEndOFDay();
		mEodDate->SetValue(pof::base::data::clock_t::to_time_t(mSelectDay));
		mListReport->Thaw();
		mListReport->Refresh();
		m_panel5->Thaw();
		m_panel5->Refresh();
		mSaleIdSearch->Clear();
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		fail = true;
	}
	if(fail) 
		wxMessageBox("Invalid receipt id", "Reports", wxICON_INFORMATION | wxOK);
	
}

void pof::ReportsDialog::OnSaleIdCleared(wxCommandEvent& evt)
{
	evt.Skip();
}

void pof::ReportsDialog::OnChangePaymentOption(wxCommandEvent& evt)
{
	constexpr const std::array<std::string_view, 3> opts = {  
		"Cash", "Transfer", "POS"
	};

	size_t id = mSelItem.GetId();
	auto str = mListReport->GetItemText(id, 4).ToStdString();
	if (str == "Returned") {
		wxMessageBox("Cannot change the payment option of a returned item");
		return;
	}
	

	wxBusyCursor cursor;
	//std::optional<pof::base::data> data = std::nullopt;
	//if (mCurReportType == ReportType::EOD) {
	//	data = wxGetApp().mProductManager.GetEndOfDay(mSelectDay);
	//}
	//else if (mCurReportType == ReportType::EOM) {
	//	data = wxGetApp().mProductManager.GetEndOfMonth(mSelectDay);
	//}

	if (!data.has_value()) {
		wxMessageBox("Error in getting data", "Report", wxICON_ERROR | wxOK);
		return;
	}

	if (data->empty()) return;

	auto& v = data.value()[id].first;
	auto& pid = boost::variant2::get<boost::uuids::uuid>(v[0]);
	auto& saleid = boost::variant2::get<boost::uuids::uuid>(v[5]);
	auto& name = boost::variant2::get<std::string>(v[2]);



	auto newstr = std::string(opts[evt.GetId()]);
	if (newstr == str) return; //no change


	bool status = wxGetApp().mSaleManager.ChangePaymentOption(saleid, pid, newstr);
	if (status) {
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE,
			fmt::format("Changed sale payment option from {} to {} for {} in sale {}",
				str, newstr, name, boost::lexical_cast<std::string>(saleid)));

		mSelItem.SetColumn(4);
		mSelItem.SetText(newstr);
		mSelItem.SetMask(wxLIST_MASK_TEXT);
		mListReport->SetItem(mSelItem);

		v[6] = std::move(newstr);
		UpdateTotals(data.value());
	}
	else wxMessageBox("Failed to change payment option", "Report", wxICON_ERROR | wxOK);
}

void pof::ReportsDialog::OnDoReturn(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to return this product?", "Return", wxICON_WARNING | wxYES_NO) == wxNO) return;
	
	size_t id = mSelItem.GetId();
	if (!data.has_value() || data->empty()) return; //not possible but still

	wxBusyCursor cur;
	auto& row = data.value()[id];
	auto& pid = boost::variant2::get<pof::base::data::duuid_t>(row.first[0]);
	auto& sid = boost::variant2::get<pof::base::data::duuid_t>(row.first[5]);
	if (wxGetApp().mSaleManager.CheckReturned(sid, pid)) {
		wxMessageBox("Product has already been returned", "Return", wxICON_WARNING | wxOK);
		return;
	}
	auto retQuan = wxGetApp().mSaleManager.GetReturnedProductQuan(pid, sid);
	if (!retQuan.has_value()) {
		wxMessageBox("Invalid return quantity", "Return", wxICON_ERROR | wxOK);
		return;
	}
	bool status = wxGetApp().mProductManager.ReturnToInventory(pid, retQuan.value());
	if (!status) {
		wxMessageBox("Failed to return product", "Return", wxICON_ERROR | wxOK);
		return;
	}
	status = wxGetApp().mSaleManager.ReturnFromSales(sid, pid);
	if (!status) {
		wxMessageBox("Failed to return product", "Return", wxICON_ERROR | wxOK);
		return;
	}
	std::string newstr = "Returned";
	status = wxGetApp().mSaleManager.ChangePaymentOption(sid, pid, newstr);
	if (status){

		mSelItem.SetColumn(4);
		mSelItem.SetText(newstr);
		mSelItem.SetMask(wxLIST_MASK_TEXT);
		mListReport->SetItem(mSelItem);

		row.first[6] = std::move(newstr);
		UpdateTotals(data.value());
	}

	//update the product
	auto& prodDatastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto prodIter = std::ranges::find_if(prodDatastore,
		[&](auto& r) -> bool {
			return boost::variant2::get<pof::base::data::duuid_t>(r.first[pof::ProductManager::PRODUCT_UUID])
			== pid;
		});

	if (std::end(prodDatastore) != prodIter) {
		wxGetApp().mProductManager.RefreshRowFromDatabase(pid, *prodIter);
	}

	auto name = boost::variant2::get<pof::base::data::text_t>(prodIter->first[pof::ProductManager::PRODUCT_NAME]);
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE, fmt::format("Returned {} from {}", name, boost::lexical_cast<std::string>(sid)));
}

void pof::ReportsDialog::OnFilterReturns(wxCommandEvent& evt)
{
	bFilterReturns = evt.IsChecked();
	m_panel5->Freeze();
	mListReport->Freeze();
	mListReport->ClearAll();
	LoadEndOFDay();
	mListReport->Thaw();
	mListReport->Refresh();
	m_panel5->Thaw();
	m_panel5->Refresh();
}

void pof::ReportsDialog::OnAddReturns(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to resell this product?", "Reports", wxICON_WARNING | wxYES_NO) == wxNO) return;
	
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}
	wxCredentialEntryDialog dialog(this, "User credentials are required resell the item", "Reports");
	dialog.Center(wxBOTH);
	dialog.SetBackgroundColour(*wxWHITE);
	while (1) {
		if (dialog.ShowModal() == wxID_CANCEL) return;
		auto cred = dialog.GetCredentials();
		if (!wxGetApp().MainAccount->ValidateCredentials(cred.GetUser().ToStdString(),
			cred.GetPassword().GetAsString().ToStdString())) {
			wxMessageBox("Invalid user name or password", "Reports", wxICON_WARNING | wxOK);
			continue;
		}
		break;
	}
	size_t id = mSelItem.GetId();
	auto& v = data.value()[id].first;
	const auto& pid = boost::variant2::get<boost::uuids::uuid>(v[0]);
	const auto& saleid = boost::variant2::get<boost::uuids::uuid>(v[5]);
	const auto& quan = boost::variant2::get<std::uint64_t>(v[3]);

	auto& prodDatastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto pIter = std::ranges::find_if(prodDatastore, [&](const pof::base::data::row_t& row)->bool {
		return (pid == boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]));
		});
	
	if (boost::variant2::get<std::uint64_t>(pIter->first[pof::ProductManager::PRODUCT_STOCK_COUNT]) < quan) {
		wxMessageBox("Cannot resell this item, there is no stock for the item", "Resell item", wxICON_WARNING | wxOK);
		return;
	}

	//update the quantity
	pIter->first[pof::ProductManager::PRODUCT_STOCK_COUNT] =
		boost::variant2::get<std::uint64_t>(pIter->first[pof::ProductManager::PRODUCT_STOCK_COUNT]) - quan;

	
	if (wxGetApp().mSaleManager.ResellItem(saleid, pid, quan)){
		std::vector<std::tuple<pof::base::data::duuid_t, std::uint64_t>> quans;
		
		quans.emplace_back(std::make_tuple(pid, boost::variant2::get<std::uint64_t>(pIter->first[pof::ProductManager::PRODUCT_STOCK_COUNT])));
		const std::string newstr = "Cash"s;

		wxGetApp().mProductManager.UpdateProductQuan(quans);
		mSelItem.SetColumn(4);
		mSelItem.SetText(newstr);
		mSelItem.SetMask(wxLIST_MASK_TEXT);
		mListReport->SetItem(mSelItem);

		v[6] = std::move(newstr);
		UpdateTotals(data.value());
	}
}

void pof::ReportsDialog::OnRemoveSale(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to remove sale of this product?", "Reports", wxICON_WARNING | wxYES_NO) == wxNO) return;

	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}
	wxCredentialEntryDialog dialog(this, "Admin credentials are required remove this item", "Reports");
	dialog.Center(wxBOTH);
	dialog.SetBackgroundColour(*wxWHITE);
	while (1) {
		if (dialog.ShowModal() == wxID_CANCEL) return;
		auto cred = dialog.GetCredentials();
		if (cred.GetUser() != "Admin") {
			wxMessageBox("You need admin privilages to remove a sale, contact D-GLOPA admin to continue", "Reports", wxICON_WARNING | wxOK);
			return;
		}
		if (!wxGetApp().MainAccount->ValidateCredentials(cred.GetUser().ToStdString(),
			cred.GetPassword().GetAsString().ToStdString())) {
			wxMessageBox("Invalid username or password", "Reports", wxICON_WARNING | wxOK);
			continue;
		}
		break;
	}
	const size_t id = mSelItem.GetId();
	const auto& v = data.value()[id].first;
	std::uint64_t rowid = boost::variant2::get<std::uint64_t>(v[7]);
	const auto& pid = boost::variant2::get<boost::uuids::uuid>(v[0]);
	const auto& saleid = boost::variant2::get<boost::uuids::uuid>(v[5]);

	if(wxGetApp().mSaleManager.RemoveProductFromSale(rowid, pid, saleid)){
		data.value().erase(std::next(data.value().begin(), id));

		mListReport->Freeze();
		mListReport->ClearAll();
		LoadEndOFDay();
		mListReport->Thaw();
	}

}

void pof::ReportsDialog::CreateToolBar()
{
	text = new wxStaticText(mTools, wxID_ANY, wxEmptyString);
	text->SetBackgroundColour(*wxWHITE);
	textItem = mTools->AddControl(text, wxEmptyString);
	mTools->AddStretchSpacer();

	switch (mCurReportType)
	{
	case ReportType::COMSUMPTION_PATTARN:
		CreateComsumptionPatternToolBar();
		break;
	case ReportType::EOM:
	case ReportType::EOD:
		CreateEODToolBar();
		break;
	case ReportType::IM:
	case ReportType::PSM:
	case ReportType::PL:
		CreateIMToolBar();
	default:
		break;
	}
	mTools->AddSpacer(10);
	mTools->AddTool(ID_EXCEL, wxT("Download Excel"), wxArtProvider::GetBitmap("download"));
	mTools->AddSpacer(2);
	mTools->Realize();
}

void pof::ReportsDialog::CreateComsumptionPatternToolBar()
{
	
}

void pof::ReportsDialog::CreateEODToolBar()
{
	mSaleIdSearch = new wxTextCtrl(mTools, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(300, -1)));
	mSaleIdSearch->SetHint("Search eod by receipt id");

	mTools->AddControl(mSaleIdSearch);
	mTools->AddControl(new wxButton(mTools, ID_SEARCH_SALEID, "Search"), "Text");
	mTools->AddTool(ID_REMOVE_RETURNS, "Filter returns", wxNullBitmap, "Remove returns from report", wxITEM_CHECK);
	mTools->AddTool(ID_SHOW_SALE_ID, "Receipt ID", wxNullBitmap, "Show/Hide the receipt ID", wxITEM_CHECK);
	mTools->AddSpacer(FromDIP(5));
	mEodDate = new wxDatePickerCtrl(mTools, ID_EOD_DATE, wxDateTime::Now(), wxDefaultPosition, FromDIP(wxSize(200, -1)), wxDP_DROPDOWN);
	mEodDate->SetRange(wxDateTime{}, wxDateTime::Now());

	auto dd = mEodDate->GetValue();
	std::chrono::month month = static_cast<std::chrono::month>(dd.GetMonth() + 1);
	std::chrono::year year = static_cast<std::chrono::year>(dd.GetYear());
	std::chrono::day day = static_cast<std::chrono::day>(dd.GetDay());
	const std::chrono::year_month_day ymd{ month / day / year };
	mSelectDay = static_cast<std::chrono::sys_days>(ymd);
	mTools->AddControl(mEodDate);


	this->Bind(wxEVT_TOOL, [&](wxCommandEvent& evt) {

		bShowSaleID = evt.IsChecked();
		m_panel5->Freeze();
		mListReport->Freeze();
		mListReport->ClearAll();
		LoadEndOFDay();
		mListReport->Thaw();
		mListReport->Refresh();
		m_panel5->Thaw();
		m_panel5->Refresh();

	}, ID_SHOW_SALE_ID);

	this->Bind(wxEVT_TOOL, std::bind_front(&pof::ReportsDialog::OnFilterReturns, this), ID_REMOVE_RETURNS);
}

void pof::ReportsDialog::CreateIMToolBar()
{
	mEodDate = new wxDatePickerCtrl(mTools, ID_EOD_DATE, wxDateTime::Now(), wxDefaultPosition, FromDIP(wxSize(200, -1)), wxDP_DROPDOWN);
	mEodDate->SetRange(wxDateTime{}, wxDateTime::Now());
	//mSelectDay = pof::base::data::clock_t::from_time_t(mEodDate->GetValue().GetTicks());

	auto dd = mEodDate->GetValue();
	std::chrono::month month = static_cast<std::chrono::month>(dd.GetMonth() + 1);
	std::chrono::year year = static_cast<std::chrono::year>(dd.GetYear());
	std::chrono::day day = static_cast<std::chrono::day>(dd.GetDay());
	const std::chrono::year_month_day ymd{ month / day / year };
	mSelectDay = static_cast<std::chrono::sys_days>(ymd);
	mTools->AddControl(mEodDate);

	std::string t;
	switch (mCurReportType)
	{
		case ReportType::IM:
			t = fmt::format("Stock report for the month of {:%m/%Y}", mSelectDay);
			break;
		case ReportType::PSM:
			t = fmt::format("Product sold for {:%m/%Y}", mSelectDay);
			break;
		case ReportType::PL:
			t = fmt::format("Profit/Loss for {:%m/%Y}", mSelectDay);
			break;
		default:
			break;
	}
	//text->SetLabelText(t);
	//textItem->SetMinSize(text->GetSize());
}

void pof::ReportsDialog::CreateEmptyEodPage()
{
	mEmptyEod = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmptyEod, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, FromDIP(5));

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, FromDIP(5));

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No transaction available"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, FromDIP(5));


	bSizer9->Add(0, 0, 1, wxEXPAND, FromDIP(5));


	m7->SetSizer(bSizer9);
	m7->Layout();
	bSizer9->Fit(m7);
	bSizer8->Add(m7, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));


	bSizer8->Add(0, 0, 1, wxEXPAND, FromDIP(5));


	m5->SetSizer(bSizer8);
	m5->Layout();
	bSizer8->Fit(m5);
	bSizer6->Add(m5, 1, wxEXPAND | wxALL, FromDIP(5));


	mEmptyEod->SetSizer(bSizer6);
	mEmptyEod->Layout();
}

void pof::ReportsDialog::ConsumptionPatternExcel(pof::base::data::datetime_t month){

	auto datastore = wxGetApp().mProductManager.GetConsumptionPattern(month);
	if (!datastore.has_value()){
		wxMessageBox("Cannot get consumption pattern from database, call administrator", "Reports", wxICON_ERROR | wxOK);
		return;
	}
	if (datastore->empty()){
		wxMessageBox("No transaction to save to excel file", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}
	wxFileDialog dialog(this, "Save Consumption pattern Excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}

	wxBusyCursor cursor;
	excel::XLDocument doc;
	doc.create(fullPath.string());
	if (!doc.isOpen()) {
		spdlog::error("Canont open xlsx file");
		return;
	}

	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName("Consumption pattern");
	const size_t colSize = mListReport->GetColumnCount();
	const size_t rowSize = datastore.value().size() + 2; //plus the title and total row
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};

	writeHeader("PRODUCT NAME");
	writeHeader("CLOSING STOCK");
	writeHeader("EXPIRED STOCK");
	writeHeader("INVENTORY IN");
	writeHeader("AMOUNT IN");
	writeHeader("INVENTORY OUT");
	writeHeader("AMOUNT OUT");
	auto& v = datastore.value();
	for (auto it = v.begin(); it != v.end() && iter != range.end(); it++){
		auto& row = it->first;
		iter->value().set(boost::variant2::get<pof::base::data::text_t>(row[1]));
		iter++;

		iter->value().set(boost::variant2::get<std::uint64_t>(row[2]));
		iter++;

		iter->value().set(boost::variant2::get<std::uint64_t>(row[3]));
		iter++;

		iter->value().set(boost::variant2::get<std::uint64_t>(row[4]));
		iter++;

		iter->value().set(fmt::format("{:cu}",boost::variant2::get<pof::base::data::currency_t>(row[5])));
		iter++;

		iter->value().set(boost::variant2::get<std::uint64_t>(row[6]));
		iter++;
		
		iter->value().set(fmt::format("{:cu}",boost::variant2::get<pof::base::data::currency_t>(row[7])));
		iter++;
	}

	//total
	iter++;

	iter->value().set(mTotalClosingStock->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalExpiredStock->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalInventoryIn->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmountIn->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalInventoryOut->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmountOut->GetLabel().ToStdString());
	iter++;

	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Reports", wxICON_INFORMATION | wxOK);
}

void pof::ReportsDialog::EODExcel()
{
	std::optional<pof::base::data> datastore = data;
	if (!datastore.has_value()) {
		wxMessageBox("Cannot get EOD OR EOM from database, call administrator", "Reports", wxICON_ERROR | wxOK);
		return;
	}
	if (datastore->empty()) {
		wxMessageBox("No transaction to save", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}

	wxFileDialog dialog(this, "Save End of day excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}

	wxBusyCursor cursor;
	excel::XLDocument doc;
	doc.create(fullPath.string());
	if (!doc.isOpen()) {
		spdlog::error("Canont open xlsx file");
		return;
	}

	auto wks = doc.workbook().worksheet("Sheet1");
	if(mCurReportType == ReportType::EOD)
		 wks.setName(fmt::format("EOD for {:%d/%m/%Y}", mSelectDay));
	else wks.setName(fmt::format("EOM for {:%m/%Y}", mSelectDay));

	const size_t colSize = mListReport->GetColumnCount();
	const size_t rowSize = datastore.value().size() + 3; //plus title and total row
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};

	writeHeader("Date/Time");
	writeHeader("Product");
	writeHeader("Quantity");
	writeHeader("Amount");
	writeHeader("Payment type");


	auto& v = datastore.value();
	for (auto it = v.begin(); it != v.end() && iter != range.end(); it++) {
		auto& row = it->first;
		iter->value().set(fmt::format("{:%d/%m/%Y %H:%M:%S}", boost::variant2::get<pof::base::data::datetime_t>(row[1])));
		iter++;

		iter->value().set(boost::variant2::get<pof::base::data::text_t>(row[2]));
		iter++;

		iter->value().set(boost::variant2::get<std::uint64_t>(row[3]));
		iter++;

		iter->value().set(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(row[4])));
		iter++;

		iter->value().set(fmt::format("{}", boost::variant2::get<pof::base::data::text_t>(row[6])));
		iter++;
	}
	//totals
	//iter++; //skip two columns
	//iter++; //skip two columns


	iter->value().set(mTotalQuantity->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmount->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmountCash->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmountTransfer->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmountPos->GetLabel().ToStdString());
	iter++;


	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Reports", wxICON_INFORMATION | wxOK);

}

void pof::ReportsDialog::InventoryMonthReportExcel()
{
	std::optional<pof::base::data> datastore = data;
	if (!datastore.has_value()) {
		wxMessageBox("Cannot Inventory month report from database, call administrator", "Reports", wxICON_ERROR | wxOK);
		return;
	}
	if (datastore->empty()) {
		wxMessageBox("No transaction to save", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}

	wxFileDialog dialog(this, "Save inventory month report excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}
	wxBusyCursor cursor;
	excel::XLDocument doc;
	try {
		doc.create(fullPath.string());
		if (!doc.isOpen()) {
			spdlog::error("Canont open xlsx file");
			return;
		}
	}
	catch (excel::XLException& exp) {
		spdlog::error(exp.what());
		wxMessageBox("Error in creating and excel file, check if file with the same name is opened.", "Export excel", wxICON_ERROR | wxOK);
		return;
	}
	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName(fmt::format("INVENTORY for {:%m/%Y}", mSelectDay));


	const size_t colSize = mListReport->GetColumnCount();
	const size_t rowSize = datastore.value().size() + 3; //plus title and total row
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};
	writeHeader("Product");
	writeHeader("Quantity");
	writeHeader("Amount");

	auto& v = datastore.value();
	for (auto it = v.begin(); it != v.end() && iter != range.end(); it++) {
		auto& row = it->first;
		
		iter->value().set(boost::variant2::get<pof::base::data::text_t>(row[1]));
		iter++;

		iter->value().set(boost::variant2::get<std::uint64_t>(row[2]));
		iter++;

		iter->value().set(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(row[3])));
		iter++;
	}


	iter->value().set(mTotalQuantity->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmount->GetLabel().ToStdString());
	iter++;


	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Reports", wxICON_INFORMATION | wxOK);
}

void pof::ReportsDialog::ProfitLossExcel()
{
	std::optional<pof::base::data> datastore = data;
	if (!datastore.has_value()) {
		wxMessageBox("Cannot create Profit/loss report from database, call administrator", "Reports", wxICON_ERROR | wxOK);
		return;
	}
	if (datastore->empty()) {
		wxMessageBox("No transaction to save", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}

	wxFileDialog dialog(this, "Save Profit/Loss report excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}
	wxBusyCursor cursor;
	excel::XLDocument doc;
	try {
		doc.create(fullPath.string());
		if (!doc.isOpen()) {
			spdlog::error("Canont open xlsx file");
			return;
		}
	}
	catch (excel::XLException& exp) {
		spdlog::error(exp.what());
		wxMessageBox("Error in creating and excel file, check if file with the same name is opened.", "Export excel", wxICON_ERROR | wxOK);
		return;
	}
	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName(fmt::format("PROFIT LOSS for {:%m/%Y}", mSelectDay));

	const size_t colSize = 5;
	const size_t rowSize = datastore.value().size() + 4; //plus title and total row and date
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeexecel = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};
	auto dt = pof::base::data::clock_t::now();
	writeexecel(fmt::format("Report generated on {:%d/%m/%Y %H:%M:%S}", dt));
	iter++;
	iter++;
	iter++;
	iter++;


	writeexecel("Product");
	writeexecel("Quantity");
	writeexecel("Amount Sell");
	writeexecel("Amount Cost");
	writeexecel("Amount Profit/Loss");

	pof::base::currency totalAmountSell;
	pof::base::currency totalAmountCost;
	pof::base::currency totalAmountPL;
	for (auto& f : data.value()) {
		const auto& name = boost::variant2::get<std::string>(f.first[1]);
		const auto& quan = boost::variant2::get<std::uint64_t>(f.first[2]);
		const auto& sellamount = boost::variant2::get<pof::base::currency>(f.first[3]);
		const auto& costamount = boost::variant2::get<pof::base::currency>(f.first[4]);
		const pof::base::currency pl = sellamount - costamount;

		totalAmountCost += costamount;
		totalAmountSell += sellamount;
		totalAmountPL += pl;

		writeexecel(name);
		iter->value().set(quan);
		iter++;

		writeexecel(fmt::format("{:cu}", sellamount));
		writeexecel(fmt::format("{:cu}", costamount));
		writeexecel(fmt::format("{:cu}", pl));
	}
	
	writeexecel("TOTAL");
	iter++;

	writeexecel(fmt::format("{:cu}", totalAmountSell));
	writeexecel(fmt::format("{:cu}", totalAmountCost));
	writeexecel(fmt::format("{:cu}", totalAmountPL));

	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Reports", wxICON_INFORMATION | wxOK);
}

void pof::ReportsDialog::ProductSoldExcel()
{
	if (!data.has_value()) {
		wxMessageBox("Cannot create Products sold report from database, call administrator", "Reports", wxICON_ERROR | wxOK);
		return;
	}
	if (data->empty()) {
		wxMessageBox("No transaction to save", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}

	wxFileDialog dialog(this, "Save Product sold report excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}
	wxBusyCursor cursor;
	excel::XLDocument doc;
	try {
		doc.create(fullPath.string());
		if (!doc.isOpen()) {
			spdlog::error("Canont open xlsx file");
			return;
		}
	}
	catch (excel::XLException& exp) {
		spdlog::error(exp.what());
		wxMessageBox("Error in creating and excel file, check if file with the same name is opened.", "Export excel", wxICON_ERROR | wxOK);
		return;
	}
	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName(fmt::format("PROFIT LOSS for {:%m/%Y}", mSelectDay));

	const size_t colSize = 3;
	const size_t rowSize = data.value().size() + 4; //plus title and total row and date
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeexecel = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};
	auto dt = pof::base::data::clock_t::now();
	writeexecel(fmt::format("Report generated on {:%d/%m/%Y %H:%M:%S}", dt));
	iter++;
	iter++;


	writeexecel("Product");
	writeexecel("Quantity");
	writeexecel("Amount Sell");

	pof::base::currency totalAmountSell;
	for (auto& f : data.value()) {
		const auto& name = boost::variant2::get<std::string>(f.first[1]);
		const auto& quan = boost::variant2::get<std::uint64_t>(f.first[2]);
		const auto& sellamount = boost::variant2::get<pof::base::currency>(f.first[3]);

		totalAmountSell += sellamount;

		writeexecel(name);
		iter->value().set(quan);
		iter++;

		writeexecel(fmt::format("{:cu}", sellamount));
	}

	writeexecel("TOTAL");
	iter++;

	writeexecel(fmt::format("{:cu}", totalAmountSell));

	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Reports", wxICON_INFORMATION | wxOK);
}

void pof::ReportsDialog::UpdateTotals(const pof::base::data& data)
{
	if (data.empty()) return;
	
	if (mCurReportType == ReportType::EOD ||
		mCurReportType == ReportType::EOM) {
		pof::base::currency totalAmount;
		pof::base::currency totalAmountCash;
		pof::base::currency totalAmountTransfer;
		pof::base::currency totalAmountPos;
		std::uint64_t totalQuantity = 0;


		for (const auto& d : data) {
			//skip end of day that are returned
			if (boost::variant2::get<pof::base::data::text_t>(d.first[6]) == "Returned") continue;

			totalAmount += boost::variant2::get<pof::base::currency>(d.first[4]);
			totalQuantity += boost::variant2::get<std::uint64_t>(d.first[3]);
			if (boost::variant2::get<pof::base::data::text_t>(d.first[6]) == "Cash") {
				totalAmountCash += boost::variant2::get<pof::base::currency>(d.first[4]);

			}
			else if (boost::variant2::get<pof::base::data::text_t>(d.first[6]) == "Transfer") {
				totalAmountTransfer += boost::variant2::get<pof::base::currency>(d.first[4]);

			}
			else if (boost::variant2::get<pof::base::data::text_t>(d.first[6]) == "POS") {
				totalAmountPos += boost::variant2::get<pof::base::currency>(d.first[4]);
			}
		}
		mSPanel->Freeze();
		mTotalQuantity->SetLabelText(fmt::format("Total Quantity:   {:d}", totalQuantity));
		mTotalAmount->SetLabelText(fmt::format("Total Amount:   {:cu}", totalAmount));
		mTotalAmountCash->SetLabelText(fmt::format("Total Cash Amount:   {:cu}", totalAmountCash));
		mTotalAmountTransfer->SetLabelText(fmt::format("Total Transfer Amount:   {:cu}", totalAmountTransfer));
		mTotalAmountPos->SetLabelText(fmt::format("Total POS Amount:   {:cu}", totalAmountPos));
		mSPanel->Thaw();
		mSPanel->Layout();
		mSPanel->Refresh();
	}
	else if (mCurReportType == ReportType::IM ||  mCurReportType == ReportType::PSM){
		if (data.empty()) return;
		pof::base::currency totalAmount;
		std::uint64_t totalQuantity = 0;

		for (const auto& d : data) {
			//skip end of day that are returned

			totalAmount += boost::variant2::get<pof::base::currency>(d.first[3]);
			totalQuantity += boost::variant2::get<std::uint64_t>(d.first[2]);
		}
		totalAmount.nearest_hundred();
		mSPanel->Freeze();
		mTotalQuantity->SetLabelText(fmt::format("Total Quantity:   {:d}", totalQuantity));
		mTotalAmount->SetLabelText(fmt::format("Total Amount:   {:cu}", totalAmount));
		mSPanel->Thaw();
		mSPanel->Layout();
		mSPanel->Refresh();

	}
	else if (mCurReportType == ReportType::PL){
		pof::base::currency totalAmountSell;
		pof::base::currency totalAmountCost;
		pof::base::currency totalAmountPL;
		std::uint64_t totalQuantity = 0;

		for (const auto& d : data) {
			totalAmountSell += boost::variant2::get<pof::base::data::currency_t>(d.first[3]);
			totalAmountCost += boost::variant2::get<pof::base::data::currency_t>(d.first[4]);
			totalQuantity += boost::variant2::get<std::uint64_t>(d.first[2]);

			totalAmountPL += (boost::variant2::get<pof::base::data::currency_t>(d.first[3]) - 
				boost::variant2::get<pof::base::data::currency_t>(d.first[4]));
		}
		mSPanel->Freeze();
		mTotalQuantity->SetLabelText(fmt::format("Total Quantity:   {:d}", totalQuantity));
		mTotalAmount->SetLabelText(fmt::format("Total Sale Amount:   {:cu}", totalAmountSell));
		mTotalAmountCash->SetLabelText(fmt::format("Total Cost Amount:   {:cu}", totalAmountCost));
		mTotalAmountTransfer->SetLabelText(fmt::format("Total Profit/Loss:   {:cu}", totalAmountPL));
		mSPanel->Thaw();
		mSPanel->Layout();
		mSPanel->Refresh();
	}
}

void pof::ReportsDialog::UpdateConsumptionTotals(const pof::base::data& data)
{
	if (data.empty()) return;
	auto exStock = wxGetApp().mProductManager.GetExpiredProductsStock(mSelectedMonth);

	std::uint64_t totalClosingStock = 0;
	std::uint64_t totalExpiredStock = 0;
	std::uint64_t totalInventoryIn = 0;
	std::uint64_t totalInventoryOut = 0;
	pof::base::currency totalAmountOut;
	pof::base::currency totalAmountIn;

	for (const auto& d: data) {
		auto& v = d.first;
		totalClosingStock += boost::variant2::get<std::uint64_t>(v[2]);
		totalExpiredStock += boost::variant2::get<std::uint64_t>(v[3]);
		totalInventoryIn += boost::variant2::get<std::uint64_t>(v[4]);
		totalAmountIn += boost::variant2::get<pof::base::currency>(v[5]);
		totalInventoryOut += boost::variant2::get<std::uint64_t>(v[6]);
		totalAmountOut += boost::variant2::get<pof::base::currency>(v[7]);
	}

	mCSPanel->Freeze();
	mTotalClosingStock->SetLabelText(fmt::format("Total Closing stock:   {:d}", totalClosingStock));
	mTotalExpiredStock->SetLabelText(fmt::format("Total Expired stock:   {:d}", totalExpiredStock));
	mTotalInventoryIn->SetLabelText(fmt::format("Total Inventory in:   {:d}", totalInventoryIn));
	mTotalAmountIn->SetLabelText(fmt::format("Total Amount in:   {:cu}", totalAmountIn));
	mTotalInventoryOut->SetLabelText(fmt::format("Total Inventory out:   {:d}", totalExpiredStock));
	mTotalAmountOut->SetLabelText(fmt::format("Total Amount out:   {:cu}", totalAmountOut));
	mCSPanel->Thaw();
	mCSPanel->Layout();
	mCSPanel->Refresh();
}


