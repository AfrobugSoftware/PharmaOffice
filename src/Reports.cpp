#include "Reports.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::ReportsDialog, wxDialog)
EVT_TOOL(pof::ReportsDialog::ID_PRINT, pof::ReportsDialog::OnPrint)
EVT_TOOL(pof::ReportsDialog::ID_EXCEL, pof::ReportsDialog::OnDownloadExcel)
EVT_DATE_CHANGED(pof::ReportsDialog::ID_EOD_DATE, pof::ReportsDialog::OnDateChange)
EVT_BUTTON(pof::ReportsDialog::ID_SEARCH_SALEID, pof::ReportsDialog::OnSaleIdSearch)
EVT_SEARCH_CANCEL(pof::ReportsDialog::ID_SEARCH_SALEID, pof::ReportsDialog::OnSaleIdCleared)

EVT_LIST_ITEM_RIGHT_CLICK(pof::ReportsDialog::ID_REPORT_LIST, pof::ReportsDialog::OnEodRightClick)
END_EVENT_TABLE()

pof::ReportsDialog::ReportsDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	m_panel5 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	m_panel5->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	mTools = new wxAuiToolBar(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTools->SetMaxSize(wxSize(-1, 30));
	
	bSizer7->Add(mTools, 1, wxALL | wxEXPAND, 2);
	mBook = new wxSimplebook(m_panel5, ID_BOOK, wxDefaultPosition, wxDefaultSize, 0);

	panel = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	mSPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	mTotalQuantity = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalQuantity->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalQuantity->Wrap(-1);
	bSizer4->Add(mTotalQuantity, 0, wxALL, 5);

	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

	mTotalAmount = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmount->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmount->Wrap(-1);
	bSizer4->Add(mTotalAmount, 0, wxALL, 5);

	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

	mTotalAmountCash = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountCash->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmountCash->Wrap(-1);
	bSizer4->Add(mTotalAmountCash, 0, wxALL, 5);


	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

	mTotalAmountTransfer = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountTransfer->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmountTransfer->Wrap(-1);
	bSizer4->Add(mTotalAmountTransfer, 0, wxALL, 5);


	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

	mTotalAmountPos = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountPos->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmountPos->Wrap(-1);
	bSizer4->Add(mTotalAmountPos, 0, wxALL, 5);

	mSPanel->SetSizer(bSizer4);
	mSPanel->Layout();
	bSizer4->Fit(mSPanel);

	mCSPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8 = new wxBoxSizer(wxHORIZONTAL);

	mTotalClosingStock = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalClosingStock->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalClosingStock->Wrap(-1);
	bSizer8->Add(mTotalClosingStock, 0, wxALL, 5);

	bSizer8->AddSpacer(5);

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(5);

	mTotalExpiredStock = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalExpiredStock->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalExpiredStock->Wrap(-1);
	bSizer8->Add(mTotalExpiredStock, 0, wxALL, 5);

	bSizer8->AddSpacer(5);

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(5);

	mTotalInventoryIn = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalInventoryIn->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalInventoryIn->Wrap(-1);
	bSizer8->Add(mTotalInventoryIn, 0, wxALL, 5);

	bSizer8->AddSpacer(5);

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(5);

	mTotalAmountIn = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountIn->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmountIn->Wrap(-1);
	bSizer8->Add(mTotalAmountIn, 0, wxALL, 5);

	bSizer8->AddSpacer(5);

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(5);

	mTotalInventoryOut = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalInventoryOut->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalInventoryOut->Wrap(-1);
	bSizer8->Add(mTotalInventoryOut, 0, wxALL, 5);

	bSizer8->AddSpacer(5);

	bSizer8->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer8->AddSpacer(5);


	mTotalAmountOut = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountOut->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mTotalAmountOut->Wrap(-1);
	bSizer8->Add(mTotalAmountOut, 0, wxALL, 5);

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
	CreateEmptyEodPage();

	sz->Add(mListReport, 1, wxEXPAND | wxALL, 0);
	sz->Add(mSPanel, 0, wxEXPAND | wxALL, 2);
	sz->Add(mCSPanel, 0, wxEXPAND | wxALL, 2);

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
}

pof::ReportsDialog::~ReportsDialog()
{
}

bool pof::ReportsDialog::LoadReport(ReportType repType, pof::base::data::datetime_t month)
{
	bool ret = false;
	mCurReportType = repType;
	CreateToolBar();
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
	default:
		break;
	}
	panel->Layout();
	panel->Refresh();
	return ret;
}

bool pof::ReportsDialog::LoadConsumptionPattern(pof::base::data::datetime_t month)
{
	auto data = wxGetApp().mProductManager.GetConsumptionPattern(month);
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

	report.AppendColumn("Product", wxLIST_FORMAT_LEFT, 230);
	report.AppendColumn("Closing stock", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("Expired stock", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("Inventory in", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("Amount in", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("Inventory out", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("Amount out", wxLIST_FORMAT_LEFT, 150);

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

	std::optional<pof::base::data> data = std::nullopt;
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
		text->SetLabelText(wxEmptyString);
		textItem->SetMinSize(text->GetSize());

		//wxMessageBox("No end of day, No sale has happened yet.", "END OF DAY", wxICON_INFORMATION | wxOK);
		return true;
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
			report.AppendColumn("Day", wxLIST_FORMAT_LEFT, 200);
		}
		else {
			report.AppendColumn("Time", wxLIST_FORMAT_LEFT, 200);
		}
		report.AppendColumn("Product", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("Quantity", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("Amount", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("Payment opiton", wxLIST_FORMAT_LEFT, 100);

		if (bShowSaleID) {
			report.AppendColumn("Reciept id", wxLIST_FORMAT_CENTER, 205);
		}

		size_t i = 0;
		for (auto iter = data->begin(); iter != data->end(); iter++) {
			auto& v = iter->first;
			wxListItem item;

			item.SetColumn(0);
			item.SetId(i);
			if (mCurReportType == ReportType::EOM)
			{
				item.SetText(fmt::format("{:%d/%m/%Y}", boost::variant2::get<pof::base::data::datetime_t>(v[1])));
			}
			else {
				item.SetText(fmt::format("{:%H:%M:%S}", boost::variant2::get<pof::base::data::datetime_t>(v[1])));
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
	case ReportType::EOD:
		EODExcel();
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
	if (mCurReportType == ReportType::EOD) {
		mSelectDay = pof::base::data::clock_t::from_time_t(evt.GetDate().GetTicks());
		m_panel5->Freeze();
		mListReport->Freeze();
		mListReport->ClearAll();
		LoadEndOFDay();
		mListReport->Thaw();
		mListReport->Refresh();
		m_panel5->Thaw();
		m_panel5->Refresh();
	}
	else if (mCurReportType == ReportType::EOM){
		auto set = pof::base::data::clock_t::from_time_t(evt.GetDate().GetTicks());
		set += date::days(1); //correct for time zone
		
		if (date::floor<date::months>(mSelectDay) == date::floor<date::months>(set)) return;
		auto w = date::floor<date::months>(mSelectDay);
		auto q = date::floor<date::months>(set);

		mSelectDay = set;
		m_panel5->Freeze();
		mListReport->Freeze();
		mListReport->ClearAll();
		LoadEndOFDay();
		mListReport->Thaw();
		mListReport->Refresh();
		m_panel5->Thaw();
		m_panel5->Refresh();
	}
}

void pof::ReportsDialog::OnEodRightClick(wxListEvent& evt)
{
	if (mCurReportType == ReportType::EOD){
		wxMenu* menu = new wxMenu;
		auto p = menu->Append(ID_COPY_RECIEPT_ID, "Copy receipt ID", nullptr);

		mListReport->Bind(wxEVT_MENU, [&](wxCommandEvent& evt) {
			if (!bShowSaleID) return;

			wxClipboardLocker clipLocker;
			if (!clipLocker) {
				spdlog::error("Can't open the clipboard");
				return;
			}
			
			auto str = mListReport->GetItemText(mSelItem.GetId(), 4); 
			wxTextDataObject *tObj = new wxTextDataObject(std::move(str));
			wxTheClipboard->Clear();
			wxTheClipboard->AddData(tObj);

		}, ID_COPY_RECIEPT_ID);

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
	mSaleIdSearch = new wxTextCtrl(mTools, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(300, -1));
	mSaleIdSearch->SetHint("Search eod by receipt id");

	mTools->AddControl(mSaleIdSearch);
	mTools->AddControl(new wxButton(mTools, ID_SEARCH_SALEID, "Search"), "Text");
	mTools->AddStretchSpacer();

	mTools->AddTool(ID_SHOW_SALE_ID, "Receipt ID", wxArtProvider::GetBitmap("application"), "Show/Hide the receipt ID", wxITEM_CHECK);
	mTools->AddSpacer(5);
	mEodDate = new wxDatePickerCtrl(mTools, ID_EOD_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	mEodDate->SetRange(wxDateTime{}, wxDateTime::Now());
	mSelectDay = pof::base::data::clock_t::from_time_t(mEodDate->GetValue().GetTicks());
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
}

void pof::ReportsDialog::CreateEmptyEodPage()
{
	mEmptyEod = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmptyEod, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No transaction available"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);


	m7->SetSizer(bSizer9);
	m7->Layout();
	bSizer9->Fit(m7);
	bSizer8->Add(m7, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);


	m5->SetSizer(bSizer8);
	m5->Layout();
	bSizer8->Fit(m5);
	bSizer6->Add(m5, 1, wxEXPAND | wxALL, 5);


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
	auto datastore = wxGetApp().mProductManager.GetEndOfDay(mSelectDay);
	if (!datastore.has_value()) {
		wxMessageBox("Cannot get consumption pattern from database, call administrator", "Reports", wxICON_ERROR | wxOK);
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
	wks.setName(fmt::format("EOD for {:%d/%m/%Y}", mSelectDay));
	const size_t colSize = mListReport->GetColumnCount();
	const size_t rowSize = datastore.value().size() + 2; //plus title and total row
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};

	writeHeader("DATE/TIME");
	writeHeader("PRODUCT NAME");
	writeHeader("QUANTITY");
	writeHeader("AMOUNT");


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
	}
	//totals
	iter++; //skip two columns
	iter++; //skip two columns

	iter->value().set(mTotalQuantity->GetLabel().ToStdString());
	iter++;

	iter->value().set(mTotalAmount->GetLabel().ToStdString());
	iter++;

	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Reports", wxICON_INFORMATION | wxOK);

}

void pof::ReportsDialog::UpdateTotals(const pof::base::data& data)
{
	if (data.empty()) return;
	pof::base::currency totalAmount;
	pof::base::currency totalAmountCash;
	pof::base::currency totalAmountTransfer;
	pof::base::currency totalAmountPos;
	std::uint64_t totalQuantity = 0;


	for (const auto& d : data) {
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


