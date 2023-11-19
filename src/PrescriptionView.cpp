#include "PrescriptionView.h"

BEGIN_EVENT_TABLE(pof::PrescriptionView, wxPanel)
EVT_TOOL(pof::PrescriptionView::ID_ADD_PRESCRIPTION, pof::PrescriptionView::OnAddPrescription)
EVT_TOOL(pof::PrescriptionView::ID_DISPENSE, pof::PrescriptionView::OnDispense)
EVT_TOOL(pof::PrescriptionView::ID_PREVIEW, pof::PrescriptionView::OnLabelPreview)
EVT_TOOL(pof::PrescriptionView::ID_BACK, pof::PrescriptionView::OnBack)
EVT_TOOL(pof::PrescriptionView::ID_REFRESH, pof::PrescriptionView::OnRefresh)
EVT_TOOL(pof::PrescriptionView::ID_SEARCH_FOR_PRESCRIPTION, pof::PrescriptionView::OnSearchforPrescription)
EVT_TOOL(pof::PrescriptionView::ID_GET_PRESCRIPTION_SOURCES, pof::PrescriptionView::OnGetPrescriptionSources)
EVT_CHOICE(pof::PrescriptionView::ID_PRESCRIPTION_SOURCE, pof::PrescriptionView::OnPrescriptionChoiceChange)

EVT_DATAVIEW_ITEM_ACTIVATED(PrescriptionView::ID_DATA_VIEW, PrescriptionView::OnPrescriptionActivated)
END_EVENT_TABLE()


pof::PrescriptionView::PrescriptionView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
	: wxPanel(parent, id, position, size, style) {
	mPanelManager = std::make_unique<wxAuiManager>(this);
	CreateToolBar();
	CreateDispensaryView();
	CreateDispensaryToolBar();
	LoadPrescriptions();
	SetDefaultAuiArt();
	InitDataView();
	SetSpecialColumns();
	mPanelManager->Update();
}

void pof::PrescriptionView::CreateToolBar()
{
	auto bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	bar->SetToolBitmapSize(wxSize(16, 16));
	//search bar
	auto search = new wxSearchCtrl(bar, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
	search->SetDescriptiveText("Search prescription by patient name");
	search->ShowCancelButton(true);
	bar->AddControl(search);
	bar->AddSeparator();
	mPrescriptionSourceChoiceBox = new wxChoice(bar, ID_PRESCRIPTION_SOURCE, wxDefaultPosition, wxSize(200, -1), wxArrayString{});
	bar->AddTool(ID_GET_PRESCRIPTION_SOURCES, wxT("Prescription source"), wxNullBitmap);
	bar->AddControl(mPrescriptionSourceChoiceBox, "Prescription sources");
	mPrescriptionSourceChoiceBox->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mPrescriptionSourceChoiceBox);
	wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

	dc.SetBrush(*wxWHITE);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRoundedRectangle(rect, 2.0f);
	dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
	auto sel = mPrescriptionSourceChoiceBox->GetStringSelection();
	if (!sel.IsEmpty()) {
		dc.DrawLabel(sel, rect, wxALIGN_CENTER);
	}
		});
	mPrescriptionSourceChoiceBox->SetHelpText("Select Prescription Source");
	mPrescriptionSourceChoiceBox->SetLabelText("Select Prescription Source");
	

	bar->AddStretchSpacer();
	mPrescriptionDate = new wxDatePickerCtrl(bar, ID_PRESCRIPTION_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	bar->AddControl(mPrescriptionDate, wxT("Prescription Date"));
	bar->AddSpacer(10);
	bar->AddTool(ID_ADD_PRESCRIPTION, wxT("Add Fake Prescription"), wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR, wxSize(16,16)));
	bar->AddTool(ID_SUBSCRIBE, wxT("Connect"), wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR, wxSize(16,16)));
	bar->AddTool(ID_REFRESH, wxT("Refresh"), wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR, wxSize(16,16)));
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::PrescriptionView::LoadPrescriptions()
{

}

void pof::PrescriptionView::CreateDispensaryView()
{
	mDispensaryView = new pof::DispensaryView(this, ID_DISPENSARY);
	mPanelManager->AddPane(mDispensaryView, wxAuiPaneInfo().Name("DispensaryView").Caption("Dispensary").CenterPane().Hide());
	mPanelManager->Update();
}

void pof::PrescriptionView::CreateDispensaryToolBar()
{
	auto bar = new wxAuiToolBar(this, ID_DISPENSARY_TOOL_BAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	bar->SetToolBitmapSize(wxSize(16, 16));
	bar->AddTool(ID_BACK, wxT("Back"), wxArtProvider::GetBitmap("arrow_back"));
	bar->AddSpacer(2);
	bar->AddSeparator();
	bar->AddStretchSpacer();
	bar->AddTool(ID_ADD_DRUG_TO_PRESCRIPTION, "Add Drug To Prescription", wxArtProvider::GetBitmap("action_add"));
	bar->AddTool(ID_SHOW_PATIENT_FILE, "Show Patient File", wxArtProvider::GetBitmap("folder"));
	bar->AddTool(ID_MAKE_INTERVENTION, "Add Drug Intervention", wxArtProvider::GetBitmap("pen"));
	bar->AddTool(ID_REPORT_INTERACTION, "Report Drug Interaction", wxArtProvider::GetBitmap("comments"));
	bar->AddTool(ID_PREVIEW, "Preiew Label", wxArtProvider::GetBitmap("file"));
	bar->AddTool(ID_DISPENSE, "Dispense", wxArtProvider::GetBitmap("download"));
	bar->Realize();
	mPanelManager->AddPane(bar, wxAuiPaneInfo().Name(wxT("DispensaryToolBar")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());

}

void pof::PrescriptionView::CreatePrescriptionSourceChoice()
{

}

void pof::PrescriptionView::InitDataView()
{
	//create the columns For the data
	mDataView = new wxDataViewCtrl(this, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxNO_BORDER);
	mModel = std::make_unique<pof::DataModel>();
	mDataView->AssociateModel(mModel.get());
	mModel->Adapt<
		std::uint64_t,
		pof::base::data::duuid_t,
		pof::base::data::datetime_t,
		pof::base::data::text_t, //medication
		pof::base::data::text_t, //patient_name
		pof::base::data::text_t, //patient address
		pof::base::data::text_t, //patient age
		pof::base::data::text_t, // patient weight

		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		std::uint32_t>();

	mDataView->AppendTextColumn("Date issued", PRESCRIPTION_DATE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Patient name", PRESCRIPTION_PATIENT_NAME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Patient Address", PRESCRIPTION_PATIENT_ADDRESS, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Prescription status", 1000, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);

	mPanelManager->AddPane(mDataView, wxAuiPaneInfo().Name("DataView").Caption("Prescriptions").CenterPane().Show());
}

void pof::PrescriptionView::SetSpecialColumns()
{
	constexpr static const std::array<std::string_view, static_cast<std::underlying_type_t<PrescriptionState>>(PrescriptionState::MAX)> StateToText
	{ "PENDING", "COMPLETED", "REJECTED", "INVALID", "PARTIAL COMPLETED" };
	//set upspecial columns
	mModel->SetSpecialColumnHandler(1000, [&](size_t row, size_t col)-> wxVariant {
		return wxVariant(std::string(StateToText[0])); //where would the state come from
	});


}

void pof::PrescriptionView::SetDefaultAuiArt()
{
	auto auiArtProvider = mPanelManager->GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::Register(std::bind_front(&pof::PrescriptionView::OnAuiThemeChangeSlot, this));
}

pof::PrescriptionView::~PrescriptionView()
{
	mPanelManager->UnInit();
	mModel.release();
}

void pof::PrescriptionView::GenerateFakePrescription()
{
	pof::base::data::duuid_t id = boost::uuids::random_generator_mt19937{}();

	js::json med1 = {
		{"medication_name", "Paracetamol"},
		{"dosage_form", "Tablet"},
		{"strength", "500"},
		{"strength_type", "mg"},
		{"dir_for_use", "Take 2 tablets when required"},
		{"quantity", 30},
		{"status", "pending"}
	};

	js::json med2 = {
		{"medication_name", "Vitamin C"},
		{"dosage_form", "Tablet"},
		{"strength", "500"},
		{"strength_type", "mg"},
		{"dir_for_use", "Take 2 tablets when required"},
		{"quantity", 30},
		{"status", "pending"}
	};

	js::json med3 = {
		{"medication_name", "Aspirin"},
		{"dosage_form", "Tablet"},
		{"strength", "75"},
		{"strength_type", "mg"},
		{"dir_for_use", "Take 2 tablets when required"},
		{"quantity", 30},
		{"status", "pending"}
	};
	//changee from object to array
	js::json object = js::json::array({ med1, med2, med3 });
	std::string medications = js::to_string(object);
	//continue here tomorrow
	pof::base::data::row_t row{};
	row.first.resize(PRESCRIPTION_MAX);
	auto& v = row.first;
	v[PRESCRIPTION_ID] = 1234556;
	v[PRESCRIPTION_UUID] = id;
	v[PRESCRIPTION_DATE] = pof::base::data::clock_t::now();
	v[PRESCRIPTION_MEDICATIONS] = medications;
	v[PRESCRIPTION_PATIENT_NAME] = "Zino Ferife";
	v[PRESCRIPTION_PATIENT_ADDRESS] = "433 DBS ROAD, ASABA, DELTA STATE";
	v[PRESCRIPTION_PATIENT_AGE] = "26";
	v[PRESCRIPTION_PATIENT_WEIGHT] = "63KG";
	v[PRESCRIPTION_PRESCRIBER_NAME] = "Enife";
	v[PRESCRIPTION_PRESCRIBER_ADDRESS] = "Federal Medical Center, Asaba, Delta state";
	v[PRESCRIPTION_STATE] = static_cast<std::uint32_t>(PrescriptionState::PENDING);

	mModel->EmplaceData(std::move(row));

}

void pof::PrescriptionView::OnAddPrescription(wxCommandEvent& evt)
{
	//test
	GenerateFakePrescription();
}

void pof::PrescriptionView::OnDispense(wxCommandEvent& evt)
{
	//mDispensaryView->Dispense();
}

void pof::PrescriptionView::OnShowPaitientFile(wxCommandEvent& evt)
{
}

void pof::PrescriptionView::OnMakeIntervention(wxCommandEvent& evt)
{
}

void pof::PrescriptionView::OnReportInteraction(wxCommandEvent& evt)
{
}

void pof::PrescriptionView::OnAddDrugToPrescription(wxCommandEvent& evt)
{
}

void pof::PrescriptionView::OnLabelPreview(wxCommandEvent& evt)
{
	//mDispensaryView->PreviewLabel();
}

void pof::PrescriptionView::OnRefresh(wxCommandEvent& evt)
{
	//reloads new precriptions from the server,

}

void pof::PrescriptionView::OnPrescriptionChoiceChange(wxCommandEvent& evt)
{
}

void pof::PrescriptionView::OnSearchforPrescription(wxCommandEvent& evt)
{
}

void pof::PrescriptionView::OnGetPrescriptionSources(wxCommandEvent& evt)
{

}



void pof::PrescriptionView::OnPrescriptionActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (item.IsOk()) {
		int index = pof::DataModel::GetIdxFromItem(item);
		auto& disPane = mPanelManager->GetPane("DispensaryView");
		if (disPane.IsOk()) {
			auto& prescrip = mModel->GetDatastore()[index];
			mPanelManager->GetPane("DataView").Hide();
			mPanelManager->GetPane("Tool").Hide();
			mDispensaryView->Load(prescrip);
			disPane.Show();
			mPanelManager->GetPane("DispensaryToolBar").Show();
			mPanelManager->Update();
		}
	}
}

void pof::PrescriptionView::OnBack(wxCommandEvent& evt)
{
	if (!mPanelManager->GetPane("DataView").IsShown()) {
		mDispensaryView->ResetViewData();
		mPanelManager->GetPane("DispensaryView").Hide();
		mPanelManager->GetPane("DispensaryToolBar").Hide();
		mPanelManager->GetPane("DataView").Show();
		mPanelManager->GetPane("Tool").Show();
		mPanelManager->Update();
	}

}

void pof::PrescriptionView::OnAuiThemeChangeSlot()
{
	auto auiArtProvider = mPanelManager->GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::PrescriptionView::OnPrscriptionSource()
{
	

}

void pof::PrescriptionView::OnError(const std::string& what)
{

}

void pof::PrescriptionView::TestPrescription()
{
}
