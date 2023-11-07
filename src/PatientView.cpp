#include "PofPch.h"
#include "PatientView.h"
BEGIN_EVENT_TABLE(pof::PatientView, wxPanel)
	EVT_TOOL(pof::PatientView::ID_ADD_PATIENTS, pof::PatientView::OnAddPatient)
	EVT_TOOL(pof::PatientView::ID_SELECT, pof::PatientView::OnSelectCol)
	EVT_TOOL(pof::PatientView::ID_SELECT_MED, pof::PatientView::OnSelectMedCol)
	EVT_TOOL(wxID_BACKWARD, pof::PatientView::OnBack)
	EVT_TOOL(pof::PatientView::ID_ADD_PRODUCT, pof::PatientView::OnAddMedication)
	EVT_TOOL(pof::PatientView::ID_ADD_PACK, pof::PatientView::OnAddPacks)
	EVT_TOOL(pof::PatientView::ID_PATIENT_MED_DETAILS, pof::PatientView::OnHidePatientMedicalDetails)

	//menus
	EVT_MENU(pof::PatientView::ID_REMOVE_PATIENTS, pof::PatientView::OnRemovePatient)
	EVT_MENU(pof::PatientView::ID_STOP_PRODUCT, pof::PatientView::OnStopProduct)
	EVT_MENU(pof::PatientView::ID_REMOVE_PRODUCT, pof::PatientView::OnRemoveMedication)
	//search
	EVT_SEARCH(pof::PatientView::ID_SEARCH, pof::PatientView::OnSearchPatient)
	EVT_SEARCH_CANCEL(pof::PatientView::ID_SEARCH, pof::PatientView::OnSearchCleared)
	EVT_TEXT(pof::PatientView::ID_SEARCH, pof::PatientView::OnSearchPatient)

	//dataview
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientsContextMenu)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnMedicationsContextMenu)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientActivated)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientHeaderClicked)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnMedHeaderClicked)

	EVT_SPLITTER_SASH_POS_CHANGED(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnPositionChanged)
	EVT_SPLITTER_SASH_POS_CHANGING(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnPositionChanging)
	EVT_SPLITTER_SASH_POS_RESIZE(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnPositionResize)
	EVT_SPLITTER_DCLICK(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnDClick)
	EVT_PG_CHANGED(pof::PatientView::ID_MED_DETAILS, pof::PatientView::OnPatientDetailsChange)
END_EVENT_TABLE()

pof::PatientView::PatientView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPanel(parent, id, pos, size, style){
	mManager.SetManagedWindow(this);
	mManager.SetFlags(AUIMGRSTYLE);
	SetSizeHints(wxDefaultSize, wxDefaultSize);
	SetBackgroundColour(*wxWHITE);


	CreateToolBars();
	SetupAuiTheme();
	CreateViews();
	CreateSpecialCols();
	CreatePatientSummaryPopUp();
	CreatePatientDetailsPane();

	mManager.Update();
}

pof::PatientView::~PatientView()
{
}

void pof::PatientView::CreateToolBars()
{
	mTopTools = new wxAuiToolBar(this, ID_TOP_TOOLS, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTopTools->SetToolBitmapSize(wxSize(16, 16));

	mPatientTools = new wxAuiToolBar(this, ID_PATIENT_TOOLS, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mPatientTools->SetToolBitmapSize(wxSize(16, 16));

	mTopTools->AddTool(ID_SELECT, "Select", wxArtProvider::GetBitmap("action_check"), "Show Selections", wxITEM_CHECK);
	mTopTools->AddSpacer(10);
	mSearchbar = new wxSearchCtrl(mTopTools, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(400, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	mSearchbar->ShowSearchButton(true);
#endif
	mSearchbar->ShowCancelButton(true);
	mSearchbar->SetDescriptiveText("Search for patients");
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_LAST_NAME, "Search by last name");
	menu->Check(ID_SEARCH_BY_NAME, true);
	mSearchbar->SetMenu(menu);
	this->Bind(wxEVT_MENU, [&](wxCommandEvent& evt) {
		wxWindowID id = evt.GetId();
	switch (id)
	{
	case ID_SEARCH_BY_NAME:
		mSearchColumn = pof::PatientManager::PATIENT_NAME;
		break;
	case ID_SEARCH_BY_LAST_NAME:
		mSearchColumn = pof::PatientManager::PATIENT_LAST_NAME;
		break;
	default:
		break;
	}
	}, ID_SEARCH_BY_NAME, ID_SEARCH_BY_LAST_NAME);

	mTopTools->AddControl(mSearchbar, "Search bar");
	mTopTools->AddStretchSpacer();
	mTopTools->AddTool(ID_ADD_PATIENTS, "Add Patients", wxArtProvider::GetBitmap("action_add"), "Add a patient to the pharmacy");
	mTopTools->AddSpacer(5);
	mTopTools->AddTool(ID_SHOW_PATIENT_DETAILS, "Show details", wxArtProvider::GetBitmap("application"), "Show patient details", wxITEM_CHECK);

	mPatientTools->AddTool(wxID_BACKWARD, "Back", wxArtProvider::GetBitmap("arrow_back"), "Back to patients");
	mPatientTools->AddSeparator();
	mPatientTools->AddSpacer(5);

	mPatientTools->AddStretchSpacer();
	pd = mPatientTools->AddTool(ID_PATIENT_MED_DETAILS, "Patient Details", wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_TOOLBAR, wxSize(16, 16)), "Patient medical details", wxITEM_CHECK);
	std::bitset<32> bitset(pd->GetState());
	bitset.set(5);
	pd->SetState(bitset.to_ulong());


	mPatientTools->AddSpacer(5);
	mPatientTools->AddTool(ID_ADD_PACK, "Rx Packs", wxArtProvider::GetBitmap(wxART_FOLDER, wxART_TOOLBAR, wxSize(16, 16)), "Add Pharmacy packs to patient");
	mPatientTools->AddSpacer(5);
	mPatientTools->AddTool(ID_ADD_PRODUCT, "Add Medication", wxArtProvider::GetBitmap("action_add"), "Add Medication to patient");
	
	
	mManager.AddPane(mTopTools, wxAuiPaneInfo().Name("TopTools").Top().MinSize(-1, 30).PaneBorder(false).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
	mManager.AddPane(mPatientTools, wxAuiPaneInfo().Name("PatientTools").Top().MinSize(-1, 30).ToolbarPane().PaneBorder(false).DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());
}

void pof::PatientView::CreateViews()
{
	mBook = new wxSimplebook(this, ID_BOOK);
	mPatientSelect = new wxDataViewCtrl(mBook, ID_PATIENT_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mPatientSelect->AssociateModel(wxGetApp().mPatientManager.GetPatientData().get());
	wxGetApp().mPatientManager.LoadPatients();

	mPatientSelect->AppendTextColumn("Name", pof::PatientManager::PATIENT_NAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);;
	mPatientSelect->AppendTextColumn("Last name", pof::PatientManager::PATIENT_LAST_NAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);;
	mPatientSelect->AppendTextColumn("Date of birth", pof::PatientManager::PATIENT_AGE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);;
	mPatientSelect->AppendTextColumn("Status", 1000, wxDATAVIEW_CELL_INERT, 50, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);;

	wxPanel* panel = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	mSPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	mPatientNameText = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mPatientNameText->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mPatientNameText->Wrap(-1);
	bSizer4->Add(mPatientNameText, 0, wxALL, 5);

	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

	mDobText = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mDobText->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mDobText->Wrap(-1);
	bSizer4->Add(mDobText, 0, wxALL, 5);

	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

	mGenderText = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mGenderText->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mGenderText->Wrap(-1);
	bSizer4->Add(mGenderText, 0, wxALL, 5);

	mSPanel->SetSizer(bSizer4);
	mSPanel->Layout();
	bSizer4->Fit(mSPanel);
	sz->Add(mSPanel, 0, wxEXPAND | wxALL, 0);


	mPatientPanel =  new wxSplitterWindow(panel, ID_PATIENT_PANEL, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE |
		wxCLIP_CHILDREN | wxNO_BORDER/* | wxSP_NO_XP_THEME */);
	//wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	
	mCurrentMedicationView = new wxDataViewCtrl(mPatientPanel, ID_PATIENT_MEDS_VIEW, wxDefaultPosition, wxSize(-1, -1), wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mCurrentMedicationView->AssociateModel(wxGetApp().mPatientManager.GetPatientMedData().get());
	mCurrentMedicationView->AppendTextColumn("Medication", pof::PatientManager::MED_NAME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Reason", pof::PatientManager::MED_PURPOSE, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Outcome", pof::PatientManager::MED_OUTCOME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Direction For Use", 1000, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Start Stock", pof::PatientManager::MED_STOCK, wxDATAVIEW_CELL_INERT, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendDateColumn("Start Date", pof::PatientManager::MED_START_DATE, wxDATAVIEW_CELL_ACTIVATABLE, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendDateColumn("Stop Date", pof::PatientManager::MED_STOP_DATE, wxDATAVIEW_CELL_ACTIVATABLE, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	


	mMedHistoryView = new wxDataViewCtrl(mPatientPanel, ID_PATIENT_HISTORY_VIEW, wxDefaultPosition, wxSize(-1, -1), wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mMedHistoryView->AssociateModel(wxGetApp().mPatientManager.GetPatientHistotyData().get());
	mMedHistoryView->AppendTextColumn("Medication", pof::PatientManager::MED_NAME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Reason", pof::PatientManager::MED_PURPOSE, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Outcome", pof::PatientManager::MED_OUTCOME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Direction For Use", 1000, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Start Stock", pof::PatientManager::MED_STOCK, wxDATAVIEW_CELL_INERT, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Start Date", pof::PatientManager::MED_START_DATE, wxDATAVIEW_CELL_INERT, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Stop Date", pof::PatientManager::MED_STOP_DATE, wxDATAVIEW_CELL_INERT, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);


	mCurrentMedicationView->Show();
	mMedHistoryView->Show();
	mPatientPanel->SplitHorizontally(mCurrentMedicationView, mMedHistoryView);

	sz->Add(mPatientPanel, wxSizerFlags().Proportion(1).Expand().Border(wxALL, 0));

	panel->SetSizer(sz);
	sz->SetSizeHints(panel);
	panel->Layout();
	mPatientPanel->SetExtraStyle(wxWS_EX_PROCESS_IDLE);
	mPatientPanel->Connect(wxEVT_IDLE, wxIdleEventHandler(PatientView::OnSpliterOnIdle), NULL, this);

	mBook->AddPage(mPatientSelect, "Patient Select", true);
	mBook->AddPage(panel, "Patient medication panel", false);
	
	mManager.AddPane(mBook, wxAuiPaneInfo().Name("Book").CenterPane().Show());
}

void pof::PatientView::CreatePatientSummaryPopUp()
{
	wxPanel* mPopup = new wxPanel;
	wxBitmap pbm = GetPatientBitMap();
}

void pof::PatientView::CreateSpecialCols()
{
	pof::DataModel::SpeicalColHandler_t SelectColHandler;
	pof::DataModel::SpeicalColHandler_t SelectMedColHandler;
	pof::DataModel::SpeicalColHandler_t statusHandler;
	pof::DataModel::SpeicalColHandler_t startDateHandler;
	pof::DataModel::SpeicalColHandler_t stopDateHandler;



	SelectColHandler.first = [&](size_t row, size_t col) -> wxVariant {
		auto found = mPatientSelections.find(pof::DataModel::GetItemFromIdx(row));
		return wxVariant((found != mPatientSelections.end()));
	};
	SelectColHandler.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		if (v.GetBool()) {
			auto [iter, inserted] = mPatientSelections.insert(pof::DataModel::GetItemFromIdx(row));
			return inserted;
		}
		else {
			mPatientSelections.erase(pof::DataModel::GetItemFromIdx(row));
			return true;
		}
		
	};

	SelectMedColHandler.first = [&](size_t row, size_t col) -> wxVariant {
		auto found = mMedicationSelections.find(pof::DataModel::GetItemFromIdx(row));
		return wxVariant((found != mMedicationSelections.end()));
	};
	SelectMedColHandler.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		if (v.GetBool()) {
			auto [iter, inserted] = mMedicationSelections.insert(pof::DataModel::GetItemFromIdx(row));
			return inserted;
		}
		else {
			mMedicationSelections.erase(pof::DataModel::GetItemFromIdx(row));
			return true;
		}
	};

	statusHandler.first = [&](size_t row, size_t col) -> wxVariant {
		auto& datastore = wxGetApp().mPatientManager.GetPatientData()->GetDatastore();
		auto& pid = boost::variant2::get<pof::base::data::duuid_t>(datastore[row].first[pof::PatientManager::PATIENT_UUID]);
		if (wxGetApp().mPatientManager.IsPatientActive(pid)) {
			return "Active";
		}
		else return "Inactive";
	};

	startDateHandler.first = [&](size_t row, size_t col) ->wxVariant {
		auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
		auto& date =  boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_START_DATE]);
		wxDateTime d{pof::base::data::clock_t::to_time_t(date)};
		return d;
	};

	startDateHandler.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
		auto& date = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_START_DATE]);
		date = pof::base::data::clock_t::from_time_t(v.GetDateTime().GetTicks());
		return true;
	};

	stopDateHandler.first = [&](size_t row, size_t col) ->wxVariant {
		auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
		auto& date = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_STOP_DATE]);
		wxDateTime d{ pof::base::data::clock_t::to_time_t(date) };
		return d;
	};

	stopDateHandler.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
		auto& date = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_STOP_DATE]);
		date = pof::base::data::clock_t::from_time_t(v.GetDateTime().GetTicks());
		return true;
	};


	wxGetApp().mPatientManager.GetPatientData()->SetSpecialColumnHandler(SELECTION_COL, std::move(SelectColHandler));;
	wxGetApp().mPatientManager.GetPatientData()->SetSpecialColumnHandler(1000, std::move(statusHandler));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(SELECTION_COL, std::move(SelectMedColHandler));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(pof::PatientManager::MED_START_DATE, std::move(startDateHandler));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(pof::PatientManager::MED_STOP_DATE, std::move(stopDateHandler));;

}

void pof::PatientView::CreatePatientDetailsPane()
{
	mPatientDetails = new wxPropertyGridManager(this, ID_MED_DETAILS,
		wxDefaultPosition, wxSize(350, -1), wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER |
		wxPG_TOOLBAR |
		wxPG_DESCRIPTION |
		wxPG_SPLITTER_AUTO_CENTER |
		wxNO_BORDER |
		wxPGMAN_DEFAULT_STYLE);
	mPatientDetails->SetBackgroundColour(*wxWHITE);
	mPatientDetails->SetExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_MODE_BUTTONS);
	auto grid = mPatientDetails->AddPage("Medication", wxNullBitmap);
	grid->GetGrid()->SetBackgroundColour(*wxWHITE);
	grid->GetGrid()->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->GetGrid()->SetCaptionTextColour(*wxBLACK);
	grid->GetGrid()->SetMarginColour(wxTheColourDatabase->Find("Aqua"));


	//details
	auto ct0 = grid->Append(new wxPropertyCategory("Patient Details"));
	auto pp0 = grid->Append(new wxIntProperty("Body mass index (BMI)", "0"));
	auto pp1 = grid->Append(new wxIntProperty("Weight (KG)", "1"));
	auto pp2 = grid->Append(new wxIntProperty("Heart rate (HR)", "2"));
	auto pp3 = grid->Append(new wxIntProperty("Blood pressure systolic (BP/SYS)", "3"));
	auto pp4 = grid->Append(new wxIntProperty("Blood pressure diastolic (BP/DIA)", "4"));
	auto pp5 = grid->Append(new wxIntProperty("Resipratory rate (RR)", "5"));
	auto pp6 = grid->Append(new wxIntProperty("Body tempreture (o')", "6"));
	auto pp7 = grid->Append(new wxLongStringProperty("Clinical Indication", "7"));
	auto pp8 = grid->Append(new wxStringProperty("Date entered", "8"));
	auto pp9 = grid->Append(new wxStringProperty("Date modified", "9"));

	pp8->Enable(false);
	pp9->Enable(false);

	mManager.AddPane(mPatientDetails, wxAuiPaneInfo().Name("PatientDetails").Row(1).Right().CaptionVisible(false).MinSize(350, -1).Hide());
}

void pof::PatientView::SetupAuiTheme()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::PatientView::OnAuiThemeChange, this));
}

void pof::PatientView::ShowPatientDetails()
{
}

wxBitmap pof::PatientView::GetPatientBitMap()
{
	auto item = mPatientSelect->GetSelection();
	if (!item.IsOk()) return wxNullBitmap;

	wxMemoryDC dc;



	return wxBitmap{};
}

void pof::PatientView::SwitchToolBar()
{
	auto& topPane = mManager.GetPane("TopTools");
	auto& pPane = mManager.GetPane("PatientTools");
	if (!topPane.IsOk() || !pPane.IsOk()) return;
	if (pPane.IsShown()) {
		topPane.Show();
		pPane.Hide();
	}
	else {
		pPane.Show();
		topPane.Hide();
	}
	mManager.Update();
}

void pof::PatientView::OnPatientActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	auto& pane = mManager.GetPane("PatientDetails");
	if (pane.IsOk()) {
		std::bitset<32> bitset(pd->GetState());
		if (bitset.test(5)) {
			pane.Show();
			mManager.Update();
		}
	}
	mCurrentPatient.emplace(wxGetApp().mPatientManager.GetPatientData()->GetDatastore()[pof::DataModel::GetIdxFromItem(item)]);
	SwitchToolBar();
	auto& v = mCurrentPatient.value().get().first;
	mSPanel->Freeze();
	mPatientNameText->SetLabelText(fmt::format("Patient Name:  {} {}", boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_LAST_NAME]),
		boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_NAME])));
	mDobText->SetLabelText(fmt::format("Date of birth:  {:%d/%m/%Y}", boost::variant2::get<pof::base::data::datetime_t>(v[pof::PatientManager::PATIENT_AGE])));
	mGenderText->SetLabelText(fmt::format("Gender:  {}", boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_GENDER])));
	mSPanel->Thaw();
	mSPanel->Layout();
	mSPanel->Refresh();

	mBook->SetSelection(PATIENT_VIEW);
}

void pof::PatientView::OnAddPatient(wxCommandEvent& evt)
{
	pof::AddPatient ap(this, wxID_ANY, "Add Patients");
	if (ap.ShowModal() == wxID_OK) {
		auto& exp = ap.GetPatientData();
		wxGetApp().mPatientManager.GetPatientData()->StoreData(std::move(const_cast<pof::base::data::row_t&&>(exp)));
	}
}

void pof::PatientView::OnRemovePatient(wxCommandEvent& evt)
{
}

void pof::PatientView::OnAddMedication(wxCommandEvent& evt)
{
	pof::SearchProduct prodSearch(this, wxID_ANY);
	auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
	if (prodSearch.ShowModal() != wxID_OK) return;
	wxBusyCursor cursor;
	mCurrentMedicationView->Freeze();
	if (prodSearch.HasMultipleSelections()) {
		auto vec = prodSearch.GetSelectedProducts();
		auto& puuid = boost::variant2::get<pof::base::data::duuid_t>
					(mCurrentPatient.value().get().first[pof::PatientManager::PATIENT_UUID]);
		for (auto& prod : vec) {
			pof::base::data::duuid_t& pid = boost::variant2::get<pof::base::data::duuid_t>(prod.get().first[pof::ProductManager::PRODUCT_UUID]);
			pof::base::data::row_t row;
			auto& v = row.first;
			auto& p = prod.get().first;
			if (std::ranges::any_of(datastore, [&](pof::base::data::row_t& item) -> bool {
				return boost::variant2::get<pof::base::data::duuid_t>(item.first[pof::PatientManager::MED_PRODUCT_UUID]) == pid; }))
			{
				continue;
			}
			v.resize(pof::PatientManager::MED_MAX);
			auto today = pof::base::data::clock_t::now();
			auto& vs = prod.get().first;

			v[pof::PatientManager::MED_PRODUCT_UUID] = pid;
			v[pof::PatientManager::MED_PATIENT_UUID] = puuid;
			v[pof::PatientManager::MED_NAME] = boost::variant2::get<pof::base::data::text_t>(vs[pof::ProductManager::PRODUCT_NAME]);
			v[pof::PatientManager::MED_PURPOSE] = ""s;
			v[pof::PatientManager::MED_OUTCOME] = ""s;
			v[pof::PatientManager::MED_STOCK] = static_cast<std::uint64_t>(0);
			v[pof::PatientManager::MED_DIR_FOR_USE_QUANTITY] = static_cast<std::uint64_t>(0);
			v[pof::PatientManager::MED_DIR_FOR_USE_STRENGTH] = boost::variant2::get<pof::base::data::text_t>(vs[pof::ProductManager::PRODUCT_STRENGTH_TYPE]);
			v[pof::PatientManager::MED_DURATION] = static_cast<std::uint64_t>(0);
			v[pof::PatientManager::MED_START_DATE] = today;
			v[pof::PatientManager::MED_STOP_DATE] = today;


			//store
			wxGetApp().mPatientManager.GetPatientMedData()->StoreData(std::move(row));
		}
	}
	mCurrentMedicationView->Thaw();
	mCurrentMedicationView->Refresh();
}

void pof::PatientView::OnAddPacks(wxCommandEvent& evt)
{
	pof::PackView pk(this, true, wxID_ANY, wxT("Add Pack to patient"));
	if (pk.ShowModal() != wxID_OK) return;

}

void pof::PatientView::OnRemoveMedication(wxCommandEvent& evt)
{
}

void pof::PatientView::OnSelectCol(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		ShowPatientsSelectCol();
	}
	else {
		HidePatientsSelectCol();
	}
}

void pof::PatientView::OnSelectMedCol(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		ShowMedSelectCol();
	}
	else {
		HideMedSelectCol();
	}
}

void pof::PatientView::OnPatientsContextMenu(wxDataViewEvent& evt)
{
	if (!evt.GetItem().IsOk()) return;
	wxMenu* menu = new wxMenu;
	auto rm = menu->Append(ID_REMOVE_PATIENTS, "Remove patient", nullptr);

	mPatientSelect->PopupMenu(menu);
}

void pof::PatientView::OnMedicationsContextMenu(wxDataViewEvent& evt)
{
	if (!evt.GetItem().IsOk()) return;

	wxMenu* menu = new wxMenu;
	auto st = menu->Append(ID_STOP_PRODUCT, "Stop medication", nullptr);
	auto rm = menu->Append(ID_REMOVE_PRODUCT, "Remove medication", nullptr);

	mCurrentMedicationView->PopupMenu(menu);
}

void pof::PatientView::OnMedicationsSelected(wxDataViewEvent& evt)
{
}

void pof::PatientView::OnMedicationHistorySelected(wxDataViewEvent& evt)
{
}

void pof::PatientView::OnAuiThemeChange()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::PatientView::OnSearchPatient(wxCommandEvent& evt)
{
	auto searchString = evt.GetString().ToStdString();
	auto& datastore = wxGetApp().mPatientManager.GetPatientData();
	mPatientSelect->Freeze();
	if (searchString.empty()) {
		datastore->Reload();
	}
	else {
		datastore->StringSearchAndReload(mSearchColumn, std::move(searchString));
	}
	mPatientSelect->Thaw();
	mPatientSelect->Refresh();
}

void pof::PatientView::OnSearchCleared(wxCommandEvent& evt)
{
	auto& datastore = wxGetApp().mPatientManager.GetPatientData();
	mPatientSelect->Freeze();
	datastore->Reload();
	mPatientSelect->Thaw();
	mPatientSelect->Refresh();
}

void pof::PatientView::OnStopProduct(wxCommandEvent& evt)
{
}

void pof::PatientView::OnBack(wxCommandEvent& evt)
{
	auto& pane = mManager.GetPane("PatientDetails");
	if (pane.IsOk()) {
		pane.Hide();
		mManager.Update();
	}
	SwitchToolBar();
	mBook->SetSelection(PATIENT_SELECT);
}

void pof::PatientView::OnHidePatientMedicalDetails(wxCommandEvent& evt)
{
	auto& pane = mManager.GetPane("PatientDetails");
	if (!pane.IsOk()) return;
	pane.Show(evt.IsChecked());
	mManager.Update();
}

void pof::PatientView::ShowPatientsSelectCol()
{
	mSelectCol = mPatientSelect->PrependToggleColumn(wxT("Select"), SELECTION_COL,
		wxDATAVIEW_CELL_ACTIVATABLE, 50);
}

void pof::PatientView::ShowMedSelectCol()
{
	mSelectMedCol = mCurrentMedicationView->PrependToggleColumn(wxT("Select"), SELECTION_COL,
		wxDATAVIEW_CELL_ACTIVATABLE, 50);
}

void pof::PatientView::HidePatientsSelectCol()
{
	if (mSelectCol != nullptr) {
		mPatientSelections.clear();

		mPatientSelect->Freeze();
		mPatientSelect->DeleteColumn(mSelectCol);
		mPatientSelect->Thaw();
		mPatientSelect->Update();
		mSelectCol = nullptr;
	}
}

void pof::PatientView::HideMedSelectCol()
{
	if (mSelectMedCol != nullptr) {
		mMedicationSelections.clear();

		mCurrentMedicationView->Freeze();
		mCurrentMedicationView->DeleteColumn(mSelectMedCol);
		mCurrentMedicationView->Thaw();
		mCurrentMedicationView->Update();
		mSelectMedCol = nullptr;
	}
}

void pof::PatientView::OnPatientHeaderClicked(wxDataViewEvent& evt)
{
	if (mSelectCol == evt.GetDataViewColumn()) {
		static bool sel = true;
		mPatientSelect->Freeze();
		auto& items = wxGetApp().mPatientManager.GetPatientData()->GetDataViewItems();
		if (sel) {
			if (!mPatientSelections.empty()) {
				mPatientSelections.clear();
				sel = false;
			}
			else {
				std::ranges::copy(items, std::inserter(mPatientSelections, mPatientSelections.end()));
			}
		}
		else {
			for (auto& item : items) {
				mPatientSelections.erase(item);
			}
		}
		sel = !sel;
		mPatientSelect->Thaw();
		mPatientSelect->Refresh();
		evt.Veto();
	}
	else {
		evt.Skip();
	}
}

void pof::PatientView::OnMedHeaderClicked(wxDataViewEvent& evt)
{
	if (mSelectMedCol == evt.GetDataViewColumn()) {
		static bool sel = true;
		mCurrentMedicationView->Freeze();
		auto& items = wxGetApp().mPatientManager.GetPatientMedData()->GetDataViewItems();
		if (sel) {
			if (!mMedicationSelections.empty()) {
				mMedicationSelections.clear();
				sel = false;
			}
			else {
				std::ranges::copy(items, std::inserter(mMedicationSelections, mMedicationSelections.end()));
			}
		}
		else {
			for (auto& item : items) {
				mMedicationSelections.erase(item);
			}
		}
		sel = !sel;
		mCurrentMedicationView->Thaw();
		mCurrentMedicationView->Refresh();
		evt.Veto();
	}
	else {
		evt.Skip();
	}
}

void pof::PatientView::OnPatientDetailsChange(wxPropertyGridEvent& evt)
{
	wxPGProperty* props = evt.GetProperty();
	if (!props || props->IsCategory()) return;
	try {
		size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName().ToStdString());
		wxVariant v = evt.GetPropertyValue();
		auto& vp = mCurrentPatient.value().get().first;
		auto& vps = mCurrentPatient.value().get().second.second;
		switch (n)
		{
		case 0:
			vp[pof::PatientManager::PATIENT_BMI] = static_cast<std::uint64_t>(v.GetInteger());
			vps.set(pof::PatientManager::PATIENT_BMI);
			break;
		case 1:
			vp[pof::PatientManager::PATIENT_WEIGHT] = static_cast<std::uint64_t>(v.GetInteger());
			vps.set(pof::PatientManager::PATIENT_WEIGHT);
			break;
		case 2:
			vp[pof::PatientManager::PATIENT_HR] = static_cast<std::uint64_t>(v.GetInteger());
			vps.set(pof::PatientManager::PATIENT_HR);
			break;
		case 3:
			vp[pof::PatientManager::PATIENT_BP_SYS] = static_cast<std::uint64_t>(v.GetInteger());
			vps.set(pof::PatientManager::PATIENT_BP_SYS);
			break;
		case 4:
			vp[pof::PatientManager::PATIENT_BP_DYS] = static_cast<std::uint64_t>(v.GetInteger());
			vps.set(pof::PatientManager::PATIENT_BP_DYS);
			break;
		case 5:
			vp[pof::PatientManager::PATIENT_RR] = static_cast<std::uint64_t>(v.GetInteger());
			vps.set(pof::PatientManager::PATIENT_RR);
			break;
		case 6:
			vp[pof::PatientManager::PATIENT_TEMPT] = static_cast<std::uint64_t>(v.GetInteger());
			vps.set(pof::PatientManager::PATIENT_TEMPT);
			break;
		case 7:
			vp[pof::PatientManager::PATIENT_CLINICAL_INDICATION] = v.GetString().ToStdString();
			vps.set(pof::PatientManager::PATIENT_CLINICAL_INDICATION);
			break;
		case 8:
			//do not change date entered
			break;
		case 9:
			vp[pof::PatientManager::PATIENT_MODIFIED_DATE] = pof::base::data::clock_t::now();
			vps.set(pof::PatientManager::PATIENT_MODIFIED_DATE);
			break;
		default:
			break;
		};
		auto item = mPatientSelect->GetSelection();
		if (item.IsOk()) {
			auto& dp = wxGetApp().mPatientManager.GetPatientData();
			dp->Signal(pof::DataModel::Signals::UPDATE, pof::DataModel::GetIdxFromItem(item));
		}
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}
}

void pof::PatientView::OnPositionChanged(wxSplitterEvent& event)
{
	event.Skip();
}

void pof::PatientView::OnPositionChanging(wxSplitterEvent& event)
{
	event.Skip();
}

void pof::PatientView::OnPositionResize(wxSplitterEvent& event)
{
	event.Skip();
}

void pof::PatientView::OnDClick(wxSplitterEvent& event)
{
	event.Veto(); //prevent double click
}

void pof::PatientView::OnUnsplitEvent(wxSplitterEvent& event)
{
}

void pof::PatientView::OnSpliterOnIdle(wxIdleEvent& evt)
{
	mPatientPanel->SetSashPosition(350);
	mPatientPanel->Disconnect(wxEVT_IDLE, wxIdleEventHandler(PatientView::OnSpliterOnIdle), NULL, this);
}
