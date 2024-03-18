#include "ChartDialog.h"
#include "PofPch.h"
BEGIN_EVENT_TABLE(pof::ChartDialog, wxDialog)
	EVT_DATE_CHANGED(pof::ChartDialog::ID_DATE_PICKER, pof::ChartDialog::OnDateChanged)
	EVT_TOOL(pof::ChartDialog::ID_EXPORT_IMAGE, pof::ChartDialog::OnExportChartImage)
END_EVENT_TABLE()

pof::ChartDialog::ChartDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style) {

	mManager.SetManagedWindow(this);
	mManager.SetFlags(463L);
	SetBackgroundColour(*wxWHITE); //move to theme

	CreateToolbar();
	mBook = new wxSimplebook(this, ID_BOOK);
	CreateEmptyPanel();
	CreateChartPanel();

	SetupAuiTheme();
	mManager.AddPane(mBook, wxAuiPaneInfo().Name("Book").CaptionVisible(false).CenterPane());
	mManager.Update();
	this->Centre(wxBOTH);



	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

pof::ChartDialog::~ChartDialog()
{
	//dtor
	mManager.UnInit();
}

void pof::ChartDialog::CreateToolbar() {
	mTools = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTools->SetToolBitmapSize(wxSize(16, 16));
	mTools->AddStretchSpacer();
	mDatePicker = new wxDatePickerCtrl(mTools, ID_DATE_PICKER, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	mDatePicker->SetRange(wxDateTime{}, wxDateTime::Now());

	mSelectDay = pof::base::data::clock_t::from_time_t(mDatePicker->GetValue().GetTicks());
	mTools->AddControl(mDatePicker);
	mTools->AddSpacer(5);
	mTools->AddTool(ID_EXPORT_IMAGE, "Export chart", wxArtProvider::GetBitmap(wxART_PRINT, wxART_TOOLBAR, wxSize(16, 16)), "Export chart as image");
	mTools->Realize();
	mManager.AddPane(mTools, wxAuiPaneInfo().Name("Toolbar").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}
void pof::ChartDialog::CreateChartPanel() {
	mChartPanel = new wxChartPanel(mBook, ID_CHART); // this should be inside hist book
#ifdef wxUSE_GRAPHICS_CONTEXT
	mChartPanel->SetAntialias(true);
	mChartPanel->SetWindowStyle(wxNO_BORDER);
#endif
	mBook->AddPage(mChartPanel, "Chart", false);
}

wxPanel* pof::ChartDialog::CreateEmptyPanel(const std::string& text)
{
	mEmpty = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmpty, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	mEmptyStr = new wxStaticText(m7, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
	mEmptyStr->Wrap(-1);
	bSizer9->Add(mEmptyStr, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

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


	mEmpty->SetSizer(bSizer6);
	mEmpty->Layout();
	mBook->AddPage(mEmpty, "Empty", false);

	return mEmpty;
}

void pof::ChartDialog::OnExportChartImage(wxCommandEvent& evt)
{
	Chart* chart = mChartPanel->GetChart();
	if (chart != NULL) {
		wxFileDialog dlg(this, wxT("Choose file..."), wxEmptyString, wxEmptyString,
			wxString(wxT("PNG files (*.png)|*.png")), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		if (dlg.ShowModal() != wxID_OK)
			return;

		wxBitmap bitmap = mChartPanel->CopyBackbuffer();
		bitmap.ConvertToImage().SaveFile(dlg.GetPath(), wxBITMAP_TYPE_PNG);
	}
	else {
		spdlog::error("No chart is chosen!");
	}
}

void pof::ChartDialog::OnDateChanged(wxDateEvent& evt) {
	mSelectDay = pof::base::data::clock_t::from_time_t(evt.GetDate().GetTicks());

}

void pof::ChartDialog::CheckEmpty(int viewIdx)
{
	switch (viewIdx)
	{

	default:
		break;
	}
}

void pof::ChartDialog::ShowEmpty(const std::string& text)
{
	mEmpty->Freeze();
	mEmptyStr->SetLabelText(text);
	mEmpty->Layout();
	mEmpty->Thaw();

	mBook->SetSelection(EMPTY);
}

void pof::ChartDialog::OnAuiThemeChange()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::ChartDialog::SetupAuiTheme()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::ChartDialog::OnAuiThemeChange, this));
}

bool pof::ChartDialog::LoadWeeklySalesChart()
{
	data = wxGetApp().mSaleManager.GetWeeklySales(mSelectDay);
	if (!data.has_value()) {
		wxMessageBox("Cannot create chart, error in reading database", "Charts", wxICON_ERROR | wxOK);
		return false;
	}
	if (data->empty()) {
		ShowEmpty("No weekly sales");
		return true;
	}
	if (mCurrentChart == nullptr) {
		BarPlot* plot = new BarPlot();
		std::vector<wxString> dates = { "SUN", "MON", "TUE", "WED", "THURS", "FRI", "SAT"};
		std::vector<double> amounts;
		pof::base::currency average;


		//prep data
		amounts.resize(dates.size());
		std::ranges::fill(amounts, 0.0);
		for (size_t i = 0; i < dates.size(); i++) {
			if (i < data->size()) {
				amounts[i] = static_cast<double>(boost::variant2::get<pof::base::currency>(data.value()[i].first[2]));
				average += (boost::variant2::get<pof::base::currency>(data.value()[i].first[2]));
			}
		}
		average /= static_cast<double>(dates.size());
		CategorySimpleDataset* dataset = new CategorySimpleDataset(dates.data(), dates.size());
		dataset->AddSerie(wxT("Amount"), amounts.data(), amounts.size());

		BarType* barType = new NormalBarType(30);
		dataset->SetRenderer(new BarRenderer(barType));

		LineMarker* lineMarker = new LineMarker(wxPen(wxColour(*wxBLACK), 1, wxPENSTYLE_SHORT_DASH));
		lineMarker->SetHorizontalLine(static_cast<double>(average));

		dataset->AddMarker(lineMarker);

		NumberAxis* leftAxis = new NumberAxis(AXIS_LEFT);
		leftAxis->SetMargins(5, 0);
		leftAxis->SetLabelTextColour(*wxBLACK);
		leftAxis->SetLabelPen(wxPen(*wxBLACK));
		leftAxis->SetMajorGridlinePen(wxPen(*wxBLACK));
		plot->AddAxis(leftAxis);

		CategoryAxis* bottomAxis = new CategoryAxis(AXIS_BOTTOM);
		bottomAxis->SetMargins(5, 5);
		bottomAxis->SetLabelTextColour(*wxBLACK);
		bottomAxis->SetLabelPen(wxPen(*wxBLACK));
		plot->AddAxis(bottomAxis);

		plot->AddDataset(dataset);

		plot->SetBackground(new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));

		plot->LinkDataHorizontalAxis(0, 0);

		plot->LinkDataVerticalAxis(0, 0);
		Legend* legend = new Legend(wxCENTER, wxRIGHT, new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));
		plot->SetLegend(legend);

		const auto header = new Header(fmt::format("Weekly sales for {:%d/%m:%Y}", mSelectDay));
		mCurrentChart = new Chart(plot, header);
	}
	mChartPanel->SetChart(mCurrentChart);
	mBook->SetSelection(WEEKLY_SALES);
}
