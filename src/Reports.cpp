#include "Reports.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::ReportsDialog, wxDialog)
EVT_TOOL(pof::ReportsDialog::ID_PRINT, pof::ReportsDialog::OnPrint)
EVT_TOOL(pof::ReportsDialog::ID_EXCEL, pof::ReportsDialog::OnDownloadExcel)
EVT_DATE_CHANGED(pof::ReportsDialog::ID_EOD_DATE, pof::ReportsDialog::OnDateChange)
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

	mListReport = new wxListCtrl(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxNO_BORDER | wxLC_HRULES | wxLC_VRULES);
	wxImageList* imageList = new wxImageList(16, 16);
	imageList->Add(wxArtProvider::GetBitmap("action_check"));
	imageList->Add(wxArtProvider::GetBitmap("action_delete"));
	mListReport->AssignImageList(imageList, wxIMAGE_LIST_NORMAL);
	CreateEmptyEodPage();


	mBook->AddPage(mListReport, "Report", true);
	mBook->AddPage(mEmptyEod, "Empty", false);

	bSizer7->Add(mBook, 1, wxALL | wxEXPAND, 0);
	m_panel5->SetSizer(bSizer7);
	m_panel5->Layout();
	bSizer7->Fit(m_panel5);
	bSizer6->Add(m_panel5, 1, wxEXPAND | wxALL, 0);


	this->SetSizer(bSizer6);
	this->Layout();

	this->Centre(wxBOTH);
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
		break;
	case pof::ReportsDialog::ReportType::EOD:
		ret = LoadEndOFDay();
		break;
	default:
		break;
	}
	return ret;
}

bool pof::ReportsDialog::LoadConsumptionPattern(pof::base::data::datetime_t month)
{
	auto data = wxGetApp().mProductManager.GetConsumptionPattern(month);
	auto exStock = wxGetApp().mProductManager.GetExpiredProductsStock(month);
	if (!data.has_value() || !exStock.has_value()) return false;

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

	report.AppendColumn("PRODUCT NAME", wxLIST_FORMAT_LEFT, 430);
	report.AppendColumn("CLOSING STOCK", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("EXPIRED STOCK", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("INVENTORY IN", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("AMOUNT IN", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("INVENTORY OUT", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("AMOUNT OUT", wxLIST_FORMAT_LEFT, 150);

	wxItemAttr attr;
	attr.SetBackgroundColour(*wxBLACK);
	attr.SetFont(wxFontInfo().Bold().AntiAliased());
	report.SetHeaderAttr(attr);

	size_t i = 0;
	auto expired = [&](const pof::base::data::row_t& row) -> std::uint64_t{
		if (exStock.value().empty()) return 0;
		auto iter = std::ranges::find_if(exStock.value(), [&](auto& val) -> bool {
			return (boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]) ==
				val.first);
		});
		if (iter == exStock.value().end()) return 0;
		else return iter->second;
	};

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
		item.SetText(fmt::format("{:d}", expired(*iter)));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		item.SetColumn(3);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", boost::variant2::get<std::uint64_t>(v[3])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);


		item.SetColumn(4);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(v[4])));
		item.SetMask(wxLIST_MASK_TEXT );
		report.SetItem(item);


		item.SetColumn(5);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", boost::variant2::get<std::uint64_t>(v[5])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);


		item.SetColumn(6);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(v[6])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);

		i++;
	}
	return true;
}

bool pof::ReportsDialog::LoadEndOFDay()
{
	auto data = wxGetApp().mProductManager.GetEndOfDay(mSelectDay);
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
		std::string tt = fmt::format("End of day for {:%d/%m/%Y}", mSelectDay);
		text->SetFont(wxFontInfo().AntiAliased().Bold().Italic());
		text->SetLabelText(tt);
		textItem->SetMinSize(text->GetSize());
		text->Update();
		mTools->Thaw();
		mTools->Realize();
		mTools->Refresh();
		m_panel5->Layout();


		//make the columns
		report.AppendColumn("TIME", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("PRODUCT NAME", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("QUANTITY", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("AMOUNT", wxLIST_FORMAT_LEFT, 200);

		size_t i = 0;
		for (auto iter = data->begin(); iter != data->end(); iter++) {
			auto& v = iter->first;
			wxListItem item;

			item.SetColumn(0);
			item.SetId(i);
			item.SetText(fmt::format("{:%H:%M:%S}", boost::variant2::get<pof::base::data::datetime_t>(v[1])));
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

			i++;
		}
	}
	if (mBook->GetSelection() == REPORT_EMPTY_EOD){
		mBook->SetSelection(REPORT_VIEW);
	}
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
	mEodDate = new wxDatePickerCtrl(mTools, ID_EOD_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	mEodDate->SetRange(wxDateTime{}, wxDateTime::Now());
	mSelectDay = pof::base::data::clock_t::from_time_t(mEodDate->GetValue().GetTicks());
	mTools->AddControl(mEodDate);
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
	const size_t rowSize = datastore.value().size();
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
	writeHeader("CURRENT STOCK");
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

		iter->value().set(fmt::format("{:cu}",boost::variant2::get<pof::base::data::currency_t>(row[4])));
		iter++;

		iter->value().set(boost::variant2::get<std::uint64_t>(row[5]));
		iter++;
		
		iter->value().set(fmt::format("{:cu}",boost::variant2::get<pof::base::data::currency_t>(row[6])));
		iter++;
	}

	doc.save();
	doc.close();

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
	const size_t rowSize = datastore.value().size();
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

	doc.save();
	doc.close();
}


