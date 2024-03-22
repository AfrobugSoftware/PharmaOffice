#include "ChartDialog.h"
#include "PofPch.h"
BEGIN_EVENT_TABLE(pof::ChartDialog, wxDialog)
	EVT_DATE_CHANGED(pof::ChartDialog::ID_DATE_PICKER, pof::ChartDialog::OnDateChanged)
	//EVT_DATE_CHANGED(pof::ChartDialog::ID_TO_DATE, pof::ChartDialog::OnDateRange)
	//EVT_DATE_CHANGED(pof::ChartDialog::ID_FROM_DATE, pof::ChartDialog::OnDateRange)
	EVT_TOOL(pof::ChartDialog::ID_EXPORT_IMAGE, pof::ChartDialog::OnExportChartImage)
	EVT_TOOL(pof::ChartDialog::ID_PAN, pof::ChartDialog::OnSetMode)
	EVT_TOOL(pof::ChartDialog::ID_ZOOM, pof::ChartDialog::OnSetMode)
	EVT_BUTTON(pof::ChartDialog::ID_APPLY_RANGE, pof::ChartDialog::OnApplyRange)
END_EVENT_TABLE()

pof::ChartDialog::ChartDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style) {

	mManager.SetManagedWindow(this);
	mManager.SetFlags(463L);
	SetBackgroundColour(*wxWHITE); //move to theme

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
	mTools->AddTool(ID_ZOOM, "Zoom", wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR, wxSize(16, 16)), "Set zoom");
	mTools->AddSpacer(5);
	mTools->AddTool(ID_PAN, "Pan", wxArtProvider::GetBitmap("File"), "Set pan");
	mTools->AddSpacer(5);


	switch (mCurrentChartIdx)
	{
	case WEEKLY_SALES:
		mTools->AddStretchSpacer();
		mDatePicker = new wxDatePickerCtrl(mTools, ID_DATE_PICKER, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
		mDatePicker->SetRange(wxDateTime{}, wxDateTime::Now());

		mSelectDay = pof::base::data::clock_t::from_time_t(mDatePicker->GetValue().GetTicks());
		mTools->AddControl(mDatePicker);
		mTools->AddSpacer(5);
		mTools->AddTool(ID_EXPORT_IMAGE, "Export chart", wxArtProvider::GetBitmap(wxART_PRINT, wxART_TOOLBAR, wxSize(16, 16)), "Export chart as image");
		break;
	case COMPARE_SALES:
		mTools->AddControl(new wxStaticText(mTools, wxID_ANY, wxT("From: ")));
		mTools->AddSpacer(5);
		fromDate = new wxDatePickerCtrl(mTools, ID_FROM_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
		mTools->AddControl(fromDate);
		
		mTools->AddSpacer(10);

		mTools->AddControl(new wxStaticText(mTools, wxID_ANY, wxT("To: ")));
		mTools->AddSpacer(5);
		toDate = new wxDatePickerCtrl(mTools, ID_TO_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
		mTools->AddControl(toDate);

		mTools->AddSpacer(5);
		mTools->AddControl(new wxButton(mTools, ID_APPLY_RANGE, "Apply"));
		
		mTools->AddStretchSpacer();
		mTools->AddSeparator();
		mTools->AddTool(ID_EXPORT_IMAGE, "Export chart", wxArtProvider::GetBitmap(wxART_PRINT, wxART_TOOLBAR, wxSize(16, 16)), "Export chart as image");
		break;
	default:
		break;
	}

	mTools->Realize();
	mManager.AddPane(mTools, wxAuiPaneInfo().Name("Toolbar").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
	mManager.Update();
}


void pof::ChartDialog::CreateChartPanel() {
	mChartPanel = new wxChartPanel(mBook, ID_CHART); // this should be inside hist book
#ifdef wxUSE_GRAPHICS_CONTEXT
	mChartPanel->SetAntialias(true);
	mChartPanel->SetWindowStyle(wxNO_BORDER);
#endif
	ZoomMode* mode = new ChartZoom();
	mode->SetAllowHorizontalZoom(true);
	mode->SetAllowVertialZoom(true);
	mChartPanel->SetMode(mode);

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
	mChartPanel->SetChart(nullptr);
	mCurrentChart = nullptr;
	LoadWeeklySalesChart();
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

bool pof::ChartDialog::LoadChart(int chart) {
	if (chart == wxNOT_FOUND) return false;
	bool ret = false;
	mCurrentChartIdx = chart;
	CreateToolbar();
	switch (chart)
	{
	case WEEKLY_SALES:
		SetTitle("Charts - Weekly sales");
		ret = LoadWeeklySalesChart();
		break;
	case COMPARE_SALES:
		SetTitle("Charts - Sales compare");
		ret = LoadCompareSales();
		break;
	default:
		break;
	}
	return ret;
}

void pof::ChartDialog::OnApplyRange(wxCommandEvent& evt)
{
	mChartPanel->SetChart(nullptr);
	mCurrentChart = nullptr;
	LoadCompareSales(); //reload the chart
}


void pof::ChartDialog::OnSetMode(wxCommandEvent& evt)
{
	wxWindowID id = evt.GetId();
	switch (id)
	{
	case ID_PAN:
		mChartPanel->SetMode(new ChartZoom{});
		break;
	case ID_ZOOM:
		mChartPanel->SetMode(new ChartPan{});
		break;
	default:
		break;
	}
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
	SetTitle("Chart - Weekly sales");
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
				auto day = boost::variant2::get<std::uint64_t>(data.value()[i].first[0]);
				int dayCount = static_cast<unsigned char>((day + 4) % 7);
				amounts[dayCount] = static_cast<double>(boost::variant2::get<pof::base::currency>(data.value()[i].first[2]));
				average += (boost::variant2::get<pof::base::currency>(data.value()[i].first[2]));
			}
		}
		average /= static_cast<double>(dates.size());
		CategorySimpleDataset* dataset = new CategorySimpleDataset(dates.data(), dates.size());
		dataset->AddSerie(wxT("Amount"), amounts.data(), amounts.size());

		BarType* barType = new NormalBarType(30);
		dataset->SetRenderer(new BarRenderer(barType));

		LineMarker* lineMarker = new LineMarker(wxPen(wxColour(*wxRED), 1, wxPENSTYLE_SHORT_DASH));
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

		const auto header = new Header(fmt::format("Weekly sales for {:%d/%m/%Y}", mSelectDay));
		mCurrentChart = new Chart(plot, header);
	}
	mChartPanel->SetChart(mCurrentChart);
	mBook->SetSelection(WEEKLY_SALES);
}

bool pof::ChartDialog::LoadCompareSales() {
	wxBusyCursor cursor;
	if (mCompareSalesArg.empty()) return false;
	pof::base::data::datetime_t to = std::chrono::time_point_cast<pof::base::data::datetime_t::duration>(pof::base::data::clock_t::from_time_t(toDate->GetValue().GetTicks()));
	pof::base::data::datetime_t from = std::chrono::time_point_cast<pof::base::data::datetime_t::duration>(pof::base::data::clock_t::from_time_t(fromDate->GetValue().GetTicks()));

	std::int64_t days = floor<date::days>(to - from).count();
	if (days > 30 || days <= 0) {
		//wxMessageBox("Invalid date range, pick a date range that is less then 30days and is a positive range", "Compare sales", wxICON_ERROR | wxOK);
		ShowEmpty("Invalid date range");
		return false;
	}

	data = wxGetApp().mSaleManager.GetSalesFor(mCompareSalesArg, std::make_pair(from, to));
	if (!data.has_value()) {
		wxMessageBox("Cannot create chart, error in reading database", "Charts", wxICON_ERROR | wxOK);
		return false;
	}
	if (data->empty()) {
		ShowEmpty("No compare chart");
		return true;
	}

	//prep data
	boost::container::flat_map<pof::base::data::duuid_t, wxRealPointArray> datapoints;
	std::set<wxString> names;
	for (auto& p : data.value()) {
		const auto& puid = boost::variant2::get<pof::base::data::duuid_t>(p.first[0]);
		wxRealPointArray dd;
		dd.reserve(days);
		pof::base::data::datetime_t temp = from;
		dd.push_back(wxRealPoint(pof::base::data::clock_t::to_time_t(from), 0));
		while(temp <= to) {
			temp += std::chrono::days(1);
			dd.push_back(wxRealPoint(pof::base::data::clock_t::to_time_t(temp), 0));
		}

		auto [iter, in] = datapoints.insert(std::make_pair(puid, std::move(dd)));

		if (!in) {
			//not new insert, find it
			auto i = datapoints.find(puid);
			if (i != datapoints.end()) {
				auto& dt = boost::variant2::get<pof::base::data::datetime_t>(p.first[2]);
				int idx = date::floor<date::days>((dt + std::chrono::days(1)) - from).count();
				if (idx >= i->second.size()) {
					continue;
				}
				i->second[idx].y = static_cast<double>(boost::variant2::get<pof::base::currency>(p.first[3]));
				//i->second[idx].y = pof::base::data::clock_t::to_time_t(dt);
				auto& name = boost::variant2::get<std::string>(p.first[1]);
				names.insert(name);
				spdlog::info("{} {:%d/%m/%Y} - {:cu}",name, dt, boost::variant2::get<pof::base::currency>(p.first[3]));

			}

		}
		else {
			auto& dt = boost::variant2::get<pof::base::data::datetime_t>(p.first[2]);
			int idx = date::floor<date::days>((dt + std::chrono::days(1)) - from).count();
			if (idx >= iter->second.size()) {
				continue;
			}
			iter->second[idx].y = static_cast<double>(boost::variant2::get<pof::base::currency>(p.first[3]));
			auto& name = boost::variant2::get<std::string>(p.first[1]);
			names.insert(name);
;			spdlog::info("{} {:%d/%m/%Y} - {:cu}", name, dt, boost::variant2::get<pof::base::currency>(p.first[3]));

			//iter->second[idx].y = pof::base::data::clock_t::to_time_t(dt);
		}
	}
	if (datapoints.empty()) {
		wxMessageBox("Fatal error in loading compare chart", "Chart", wxICON_ERROR | wxOK);
		return false;
	}
	XYDynamicDataset* dxy = new XYDynamicDataset();
	auto render = new XYLineRenderer(true);
	dxy->SetRenderer(render);

	int x = 0;
	for (auto s = datapoints.begin(); s != datapoints.end(); s++) {
		//dump
		for (int i = 0; i < s->second.size(); i++) {
			spdlog::info("{:f} {:f}", s->second[i].x, s->second[i].y);
		}
		dxy->AddSerie(new XYDynamicSerie(s->second));
		dxy->SetSerieName(x, *(std::next(names.begin(), x)));
		x++;
	}

	XYPlot* plot = new XYPlot();
	

	JulianDateAxis* bottomAxis = new JulianDateAxis(AXIS_BOTTOM);
	bottomAxis->SetMargins(5, 5);
	bottomAxis->SetLabelTextColour(*wxBLACK);
	bottomAxis->SetLabelPen(wxPen(*wxBLACK));
	bottomAxis->SetDateFormat("%d-%m-%Y");
	bottomAxis->SetFixedBounds(pof::base::data::clock_t::to_time_t(from), 
		pof::base::data::clock_t::to_time_t(to));
	plot->AddAxis(bottomAxis);

	NumberAxis* leftAxis = new NumberAxis(AXIS_LEFT);
	leftAxis->SetMargins(5, 0);
	leftAxis->SetLabelTextColour(*wxBLACK);
	leftAxis->SetLabelPen(wxPen(*wxBLACK));
	leftAxis->SetMajorGridlinePen(wxPen(*wxBLACK));
	plot->AddAxis(leftAxis);


	plot->AddDataset(dxy);

	plot->SetBackground(new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));

	plot->LinkDataHorizontalAxis(0, 0);
	plot->LinkDataVerticalAxis(0, 0);


	Legend* legend = new Legend(wxCENTER, wxRIGHT, new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));
	plot->SetLegend(legend);

	
	const auto header = new Header(fmt::format("Compare sales from {:%d/%m/%Y} to {:%d/%m/%Y}", from, to));
	mCurrentChart = new Chart(plot, header);
	mChartPanel->SetChart(mCurrentChart);
	mBook->SetSelection(WEEKLY_SALES);
	return true;
}

void pof::ChartPan::Init(wxChartPanel* chartPanel)
{
	chartPanel->Refresh();
}

void pof::ChartZoom::Init(wxChartPanel* chartPanel)
{
	chartPanel->Refresh();
}
