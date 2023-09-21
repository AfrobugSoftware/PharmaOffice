#include "Reports.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::ReportsDialog, wxDialog)
EVT_TOOL(pof::ReportsDialog::ID_PRINT, pof::ReportsDialog::OnPrint)
EVT_TOOL(pof::ReportsDialog::ID_EXCEL, pof::ReportsDialog::OnDownloadExcel)
END_EVENT_TABLE()

pof::ReportsDialog::ReportsDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	m_panel5 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel5->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	mTools = new wxAuiToolBar(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTools->SetMaxSize(wxSize(-1, 40));	
	
    mTools->AddStretchSpacer();
	mTools->AddTool(ID_EXCEL, wxT("Download as EXCEL"), wxArtProvider::GetBitmap("dowload"));
	mPrint = mTools->AddTool(ID_PRINT, wxT("Print"), wxArtProvider::GetBitmap(wxART_PRINT), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);


	mTools->Realize();

	bSizer7->Add(mTools, 0, wxALL | wxEXPAND, 2);

	mListReport = new wxListCtrl(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxNO_BORDER | wxLC_HRULES | wxLC_VRULES);
	wxImageList* imageList = new wxImageList(16, 16);
	imageList->Add(wxArtProvider::GetBitmap("action_check"));
	imageList->Add(wxArtProvider::GetBitmap("action_delete"));
	mListReport->AssignImageList(imageList, wxIMAGE_LIST_SMALL);
	
	bSizer7->Add(mListReport, 1, wxALL | wxEXPAND, 0);
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

bool pof::ReportsDialog::LoadReport(ReportType repType)
{
	bool ret = false;
	mCurReportType = repType;
	switch (repType)
	{
	case pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN:
		ret = LoadConsumptionPattern();
		break;
	case pof::ReportsDialog::ReportType::EOD:
		ret = LoadEndOFDay();
		break;
	default:
		break;
	}
	return ret;
}

bool pof::ReportsDialog::LoadConsumptionPattern()
{
	auto data = wxGetApp().mProductManager.GetConsumptionPattern();
	if (!data.has_value()) return false;

	if (data->empty()) {
		wxMessageBox("NO Transaction has occured in your store this month", "CONSUMPTION PATTERN", wxICON_INFORMATION | wxOK);
		return false;
	}
	auto& report = *mListReport;
	wxBusyCursor cursor;
	//create the columns

	report.AppendColumn("PRODUCT NAME", wxLIST_FORMAT_LEFT, 200);
	report.AppendColumn("STOCK", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("IN", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("AMOUNT IN", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("OUT", wxLIST_FORMAT_LEFT, 150);
	report.AppendColumn("AMOUNT OUT", wxLIST_FORMAT_LEFT, 150);

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
		item.SetText(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(v[4])));
		item.SetMask(wxLIST_MASK_TEXT );
		report.SetItem(item);


		item.SetColumn(4);
		item.SetId(i);
		item.SetText(fmt::format("{:d}", boost::variant2::get<std::uint64_t>(v[5])));
		item.SetMask(wxLIST_MASK_TEXT);
		report.SetItem(item);


		item.SetColumn(5);
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
	auto data = wxGetApp().mProductManager.GetEndOfDay();
	if (!data.has_value()) return false;

	if (data->empty()) {
		wxMessageBox("No end of day, No sale has happened yet.", "END OF DAY", wxICON_INFORMATION | wxOK);
		return false;
	}
	auto& report = *mListReport;
	{
		//load data into grid
		wxBusyCursor cursor;
		pof::base::currency totalAmount;
		std::uint64_t totalQuan = 0;

		//make the columns
		report.AppendColumn("DATE", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("PRODUCT NAME", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("QUANTITY", wxLIST_FORMAT_LEFT, 200);
		report.AppendColumn("AMOUNT", wxLIST_FORMAT_LEFT, 200);

		size_t i = 0;
		for (auto iter = data->begin(); iter != data->end(); iter++) {
			auto& v = iter->first;
			wxListItem item;

			item.SetColumn(0);
			item.SetId(i);
			auto t = pof::base::data::clock_t::to_time_t(boost::variant2::get<pof::base::data::datetime_t>(v[1]));
			auto s = fmt::format("{:%d-%m-%y}", fmt::localtime(t));
			item.SetText(std::move(s));
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
	return true;
}

void pof::ReportsDialog::OnPrint(wxCommandEvent& evt)
{
}

void pof::ReportsDialog::OnDownloadExcel(wxCommandEvent& evt)
{
}


