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
	EVT_TOOL(pof::PatientView::ID_SALE_PATIENT_MED, pof::PatientView::OnSellCurrentMedication)
	EVT_TOOL(pof::PatientView::ID_PATIENT_SALE_HIST, pof::PatientView::OnPatientSaleHist)
	//date events
	EVT_DATE_CHANGED(pof::PatientView::ID_START_DATE_PICKER, pof::PatientView::OnDateChanged)
	EVT_DATE_CHANGED(pof::PatientView::ID_STOP_DATE_PICKER, pof::PatientView::OnDateChanged)
	//menus
	EVT_MENU(pof::PatientView::ID_REMOVE_PATIENTS, pof::PatientView::OnRemovePatient)
	EVT_MENU(pof::PatientView::ID_STOP_PRODUCT, pof::PatientView::OnStopProduct)
	EVT_MENU(pof::PatientView::ID_REMOVE_PRODUCT, pof::PatientView::OnRemoveMedication)
	EVT_MENU(pof::PatientView::ID_ADD_OUTCOME, pof::PatientView::OnAddText)
	EVT_MENU(pof::PatientView::ID_ADD_REASON, pof::PatientView::OnAddText)
	//search
	EVT_SEARCH(pof::PatientView::ID_SEARCH, pof::PatientView::OnSearchPatient)
	EVT_SEARCH_CANCEL(pof::PatientView::ID_SEARCH, pof::PatientView::OnSearchCleared)
	EVT_TEXT(pof::PatientView::ID_SEARCH, pof::PatientView::OnSearchPatient)

	EVT_CHECKBOX(pof::PatientView::ID_REMIND_CHECK, pof::PatientView::OnReminded)

	//dataview
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientsContextMenu)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnMedicationsContextMenu)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientActivated)
	EVT_DATAVIEW_SELECTION_CHANGED(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnMedicationsSelected)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientHeaderClicked)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnMedHeaderClicked)
	EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnEditingStarted)

	EVT_SPLITTER_SASH_POS_CHANGED(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnPositionChanged)
	EVT_SPLITTER_SASH_POS_CHANGING(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnPositionChanging)
	EVT_SPLITTER_SASH_POS_RESIZE(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnPositionResize)
	EVT_SPLITTER_DCLICK(pof::PatientView::ID_PATIENT_PANEL, pof::PatientView::OnDClick)
	EVT_PG_CHANGED(pof::PatientView::ID_MED_DETAILS, pof::PatientView::OnPatientDetailsChange)
	EVT_UPDATE_UI(pof::PatientView::ID_PATIENT_MED_DETAILS, pof::PatientView::OnPatientDetailsUpdateUI)
	//clear timer
	EVT_TIMER(pof::PatientView::ID_CLEAR_TIMER, pof::PatientView::OnClearTimer)
END_EVENT_TABLE()

pof::PatientView::PatientView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxPanel(parent, id, pos, size, style), mClearTimer{this, ID_CLEAR_TIMER} {
	mManager.SetManagedWindow(this);
	mManager.SetFlags(AUIMGRSTYLE);
	SetSizeHints(wxDefaultSize, wxDefaultSize);
	SetBackgroundColour(*wxWHITE);
	SetDoubleBuffered(true);

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
	//mTopTools->AddSpacer(5);
	//mTopTools->AddTool(ID_SHOW_PATIENT_DETAILS, "Show details", wxArtProvider::GetBitmap("application"), "Show patient details", wxITEM_CHECK);
	mTopTools->Realize();

	mPatientTools->AddTool(wxID_BACKWARD, "Back", wxArtProvider::GetBitmap("arrow_back"), "Back to patients");
	mPatientTools->AddSeparator();
	mPatientTools->AddSpacer(5);
	mPatientTools->AddTool(ID_SALE_PATIENT_MED,"Add Sale", wxArtProvider::GetBitmap("sci"), "Add current medication to sale");
	mPatientTools->AddSpacer(5);

	mIsReminded = new wxCheckBox(mPatientTools, ID_REMIND_CHECK, "Alert on inactive", wxDefaultPosition, wxDefaultSize, 0);
	mIsReminded->SetBackgroundColour(*wxWHITE);
	mPatientTools->AddControl(mIsReminded);

	mPatientTools->AddStretchSpacer();
	mPatientTools->AddTool(ID_PATIENT_SALE_HIST, "Sale History", wxArtProvider::GetBitmap("application"), "Patient sale history");
	mPatientTools->AddSpacer(2);
	pd = mPatientTools->AddTool(ID_PATIENT_MED_DETAILS, "Patient Details", wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_TOOLBAR, wxSize(16, 16)), "Patient medical details", wxITEM_CHECK);
	std::bitset<32> bitset(pd->GetState());
	bitset.set(5);
	pd->SetState(bitset.to_ulong());


	mPatientTools->AddSpacer(5);
	mPatientTools->AddTool(ID_ADD_PACK, "Rx Packs", wxArtProvider::GetBitmap(wxART_FOLDER, wxART_TOOLBAR, wxSize(16, 16)), "Add Pharmacy packs to patient");
	mPatientTools->AddSpacer(5);
	mPatientTools->AddTool(ID_ADD_PRODUCT, "Add Medication", wxArtProvider::GetBitmap("action_add"), "Add Medication to patient");
	mPatientTools->Realize();
	
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

	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

	mPhoneText = new wxStaticText(mSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mPhoneText->SetFont(wxFont(wxFontInfo().AntiAliased()));
	mPhoneText->Wrap(-1);
	bSizer4->Add(mPhoneText, 0, wxALL, 5);




	mSPanel->SetSizer(bSizer4);
	mSPanel->Layout();
	bSizer4->Fit(mSPanel);
	sz->Add(mSPanel, 0, wxEXPAND | wxALL, 0);

	mPatientInfoBar = new wxInfoBar(panel, wxID_ANY);

	mPatientPanel = new wxSplitterWindow(panel, ID_PATIENT_PANEL, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE |
		wxCLIP_CHILDREN | wxNO_BORDER/* | wxSP_NO_XP_THEME */);
	//wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	wxPanel* currentMedPanel = new wxPanel(mPatientPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxSizer* medSz = new wxBoxSizer(wxVERTICAL);

	mMedTools = new wxAuiToolBar(currentMedPanel, ID_PATIENT_TOOLS, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mMedTools->SetMinSize(wxSize(-1, 30));
	mMedTools->SetBackgroundColour(*wxWHITE); //add to theme

	mMedTools->AddTool(ID_SELECT_MED, wxT("Select"), wxArtProvider::GetBitmap("action_check"), "Select medications", wxITEM_CHECK);
	mMedTools->AddSeparator();
	mMedTools->AddStretchSpacer();
	auto today = pof::base::data::clock_t::now();

	mStartDatePicker = new wxDatePickerCtrl(mMedTools, ID_START_DATE_PICKER, wxDateTime::Now(), wxDefaultPosition, wxSize(100, -1), wxDP_DROPDOWN);
	mStopDatePicker = new wxDatePickerCtrl(mMedTools, ID_STOP_DATE_PICKER, wxDateTime::Now(), wxDefaultPosition, wxSize(100, -1), wxDP_DROPDOWN);

//	mStartDatePicker->SetRange(wxDateTime::Now(), wxDateTime{}); //set the range for the start date
//	mStopDatePicker->SetRange(wxDateTime::Now(), wxDateTime{}); //set the range for the start date
	
	mMedTools->AddControl(new wxStaticText(mMedTools, wxID_ANY, "Start:"), "Start date");
	mMedTools->AddSpacer(10);
	mMedTools->AddControl(mStartDatePicker);

	mMedTools->AddSpacer(10);
	mMedTools->AddControl(new wxStaticText(mMedTools, wxID_ANY, "Stop:"), "Stop date");
	mMedTools->AddSpacer(10);
	mMedTools->AddControl(mStopDatePicker);
	mMedTools->Realize();

	mCurrentMedicationView = new wxDataViewCtrl(currentMedPanel, ID_PATIENT_MEDS_VIEW, wxDefaultPosition, wxSize(-1, -1), wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mCurrentMedicationView->AssociateModel(wxGetApp().mPatientManager.GetPatientMedData().get());
	mCurrentMedicationView->AppendTextColumn("Medication", pof::PatientManager::MED_NAME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Reason", pof::PatientManager::MED_PURPOSE, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Outcome", pof::PatientManager::MED_OUTCOME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mStockMedCol = mCurrentMedicationView->AppendTextColumn("Start Stock", pof::PatientManager::MED_STOCK, wxDATAVIEW_CELL_EDITABLE, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendDateColumn("Start Date", pof::PatientManager::MED_START_DATE, wxDATAVIEW_CELL_EDITABLE, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendDateColumn("Stop Date", pof::PatientManager::MED_STOP_DATE,  wxDATAVIEW_CELL_EDITABLE, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

	medSz->Add(mMedTools, 0, wxEXPAND | wxALL, 0);
	medSz->Add(mCurrentMedicationView, 1, wxEXPAND | wxALL, 0);

	currentMedPanel->SetSizer(medSz);
	medSz->SetSizeHints(currentMedPanel);
	currentMedPanel->Layout();

	mMedHistoryView = new wxDataViewCtrl(mPatientPanel, ID_PATIENT_HISTORY_VIEW, wxDefaultPosition, wxSize(-1, -1), wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mMedHistoryView->AssociateModel(wxGetApp().mPatientManager.GetPatientHistotyData().get());
	mMedHistoryView->AppendTextColumn("Medication", pof::PatientManager::MED_NAME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Reason", pof::PatientManager::MED_PURPOSE, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Outcome", pof::PatientManager::MED_OUTCOME, wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Stock", pof::PatientManager::MED_STOCK, wxDATAVIEW_CELL_INERT, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendDateColumn("Started Date", pof::PatientManager::MED_START_DATE, wxDATAVIEW_CELL_INERT, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	mMedHistoryView->AppendDateColumn("Stopped Date", pof::PatientManager::MED_STOP_DATE, wxDATAVIEW_CELL_INERT, 70, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);


	mCurrentMedicationView->Show();
	mMedHistoryView->Show();
	mPatientPanel->SplitHorizontally(currentMedPanel, mMedHistoryView);

	sz->Add(mPatientInfoBar, wxSizerFlags().Expand().Border(wxALL, 0));
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
	pof::DataModel::SpeicalColHandler_t startDateHandler2;
	pof::DataModel::SpeicalColHandler_t stopDateHandler2;
	pof::DataModel::SpeicalColHandler_t stockHandler;



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
		auto& stopDate = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_STOP_DATE]);
		auto setdate = pof::base::data::clock_t::from_time_t(v.GetDateTime().GetTicks());
		//start date cannot be less than today
		auto floorDate = date::floor<date::days>(stopDate);
		auto floorSetDate = date::floor<date::days>(setdate);

		spdlog::info("stopdate = {:d} setdate = {:d}", floorDate.time_since_epoch().count(), floorSetDate.time_since_epoch().count());

		if ((date::floor<date::days>(setdate) > date::floor<date::days>(stopDate)) || 
			(date::floor<date::days>(setdate) < date::floor<date::days>(pof::base::data::clock_t::now()))) {
			mPatientInfoBar->ShowMessage("Start date cannot be less than current date or greater than stop date", wxICON_ERROR);
			StartTimer();
			return false;
		}

		date = setdate;

		datastore[row].second.second.set(pof::PatientManager::MED_START_DATE);
		wxGetApp().mPatientManager.GetPatientMedData()->Signal(pof::DataModel::Signals::UPDATE, row);
		datastore[row].second.second.reset();

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
		auto setdate = pof::base::data::clock_t::from_time_t(v.GetDateTime().GetTicks());
		auto& date = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_STOP_DATE]);
		auto& startdate = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_START_DATE]);
		if ((date::floor<date::days>(setdate) < date::floor<date::days>(startdate)) || 
			(date::floor<date::days>(setdate) < date::floor<date::days>(pof::base::data::clock_t::now()))) {
			//stop date cannot be less than start date
			mPatientInfoBar->ShowMessage("Stop date cannot be less than start date or current date", wxICON_ERROR);
			StartTimer();
			return false;
		}

		date = setdate;
		datastore[row].second.second.set(pof::PatientManager::MED_STOP_DATE);
		wxGetApp().mPatientManager.GetPatientMedData()->Signal(pof::DataModel::Signals::UPDATE, row);
		datastore[row].second.second.reset();

		return true;
	};

	stockHandler.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
		try {
			datastore[row].first[pof::PatientManager::MED_STOCK] = boost::lexical_cast<std::uint64_t>(v.GetString().ToStdString());
			datastore[row].second.second.set(pof::PatientManager::MED_STOCK);
			wxGetApp().mPatientManager.GetPatientMedData()->Signal(pof::DataModel::Signals::UPDATE, row);
			datastore[row].second.second.reset();
		}
		catch (std::exception& exp) {
			spdlog::error(exp.what());
			return false;
		}
		return true;
	};

	startDateHandler2.first = [&](size_t row, size_t col) ->wxVariant {
		auto& datastore = wxGetApp().mPatientManager.GetPatientHistotyData()->GetDatastore();
		auto& date = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_START_DATE]);
		wxDateTime d{ pof::base::data::clock_t::to_time_t(date) };
		return d;
	};

	stopDateHandler2.first = [&](size_t row, size_t col) ->wxVariant {
		auto& datastore = wxGetApp().mPatientManager.GetPatientHistotyData()->GetDatastore();
		auto& date = boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[pof::PatientManager::MED_STOP_DATE]);
		wxDateTime d{ pof::base::data::clock_t::to_time_t(date) };
		return d;
	};
	
	wxGetApp().mPatientManager.GetPatientData()->SetSpecialColumnHandler(SELECTION_COL, std::move(SelectColHandler));;
	wxGetApp().mPatientManager.GetPatientData()->SetSpecialColumnHandler(1000, std::move(statusHandler));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(SELECTION_COL, std::move(SelectMedColHandler));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(pof::PatientManager::MED_START_DATE, std::move(startDateHandler));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(pof::PatientManager::MED_STOP_DATE, std::move(stopDateHandler));;
	wxGetApp().mPatientManager.GetPatientHistotyData()->SetSpecialColumnHandler(pof::PatientManager::MED_START_DATE, std::move(startDateHandler2));;
	wxGetApp().mPatientManager.GetPatientHistotyData()->SetSpecialColumnHandler(pof::PatientManager::MED_STOP_DATE, std::move(stopDateHandler2));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(pof::PatientManager::MED_STOCK, std::move(stockHandler));;

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
	mPatientDetails->SetDoubleBuffered(true);
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
	auto pp6 = grid->Append(new wxIntProperty("Body tempreture ('o)", "6"));
	auto pp7 = grid->Append(new wxLongStringProperty("Clinical Indication", "7"));
	auto pp8 = grid->Append(new wxDateProperty("Date entered", "8"));
	auto pp9 = grid->Append(new wxDateProperty("Date modified", "9"));

	pp8->Enable(false);
	pp9->Enable(false);

	grid->SetPropertyHelpString(pp0, R"(Body max index for the patient, Range:
BMI is less than 18.5, it falls within the underweight range.
BMI is 18.5 to < 25, it falls within the healthy weight range.
BMI is 25.0 to < 30, it falls within the overweight range.
BMI is 30.0 or higher, it falls within the obesity range.)");
	grid->SetPropertyHelpString(pp1, "Weight of the patient.");
	grid->SetPropertyHelpString(pp2, "Heart rate of the patient, Range: Between 60 and 100 bpm for normal heart rate.");
	grid->SetPropertyHelpString(pp3, "Systolic blood pressure of the patient, Range:\nNormal : < 120mmHg\nElevated: 120 - 129mmHg\nHigh: > 140mmHg\nHypertensive crisis: > 180mmHg.");
	grid->SetPropertyHelpString(pp4, "Diastolic blood pressure of the patient, Range:\nNormal : < 80mmHg\nElevated: 80mmHg\nHigh: > 90mmHg\nHypertensive crisis: > 120mmHg.");
	grid->SetPropertyHelpString(pp5, "Respiratory rate of the patient, Range: 12 to 16 breaths per minute for normal.");
	grid->SetPropertyHelpString(pp6, R"(Body tempreture of the patient, Range:
Babies and children: 97.9°F (36.6°C) to 99°F (37.2°C)
Adults: 97°F (36.1°C) to 99°F (37.2°C)
Adults over age 65: 96.4°F (35.8°C) to 98.3°F (36.8°C).)");
	grid->SetPropertyHelpString(pp7, "Clinical indication for the patient.");
	grid->SetPropertyHelpString(pp8, "Date created");
	grid->SetPropertyHelpString(pp9, "Date modified");


	mManager.AddPane(mPatientDetails, wxAuiPaneInfo().Name("PatientDetails").Caption("Details").Row(1).Right().CaptionVisible(false).MinSize(350, -1).Hide());
}

void pof::PatientView::StartTimer()
{
	//restart timer if it is running
	constexpr const auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(5));
	mClearTimer.StartOnce(timeout.count());
}

void pof::PatientView::SetupAuiTheme()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::PatientView::OnAuiThemeChange, this));
}

void pof::PatientView::LoadPatientDetails()
{
	if (!mCurrentPatient.has_value()) return;
	auto page = mPatientDetails->GetPage("Medication");
	if (!page) return;
	auto& v = mCurrentPatient.value().get().first;
	page->GetGrid()->GetProperty("0")->SetValue(wxVariant(static_cast<int>(boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_BMI]))));
	page->GetGrid()->GetProperty("1")->SetValue(wxVariant(static_cast<int>(boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_WEIGHT]))));
	page->GetGrid()->GetProperty("2")->SetValue(wxVariant(static_cast<int>(boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_HR]))));
	page->GetGrid()->GetProperty("3")->SetValue(wxVariant(static_cast<int>(boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_BP_SYS]))));
	page->GetGrid()->GetProperty("4")->SetValue(wxVariant(static_cast<int>(boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_BP_DYS]))));
	page->GetGrid()->GetProperty("5")->SetValue(wxVariant(static_cast<int>(boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_RR]))));
	page->GetGrid()->GetProperty("6")->SetValue(wxVariant(static_cast<int>(boost::variant2::get<std::uint64_t>(v[pof::PatientManager::PATIENT_TEMPT]))));
	page->GetGrid()->GetProperty("7")->SetValue(wxVariant(boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_CLINICAL_INDICATION])));
	page->GetGrid()->GetProperty("8")->SetValue(wxVariant(wxDateTime(
				pof::base::data::clock_t::to_time_t(boost::variant2::get<pof::base::data::datetime_t>(v[pof::PatientManager::PATIENT_ENTERED_DATE])))));
	page->GetGrid()->GetProperty("9")->SetValue(wxVariant(wxDateTime(
				pof::base::data::clock_t::to_time_t(boost::variant2::get<pof::base::data::datetime_t>(v[pof::PatientManager::PATIENT_MODIFIED_DATE])))));
	auto& puid = boost::variant2::get<pof::base::data::duuid_t>(mCurrentPatient.value().get().first[pof::PatientManager::PATIENT_UUID]);
	auto addInfo = wxGetApp().mPatientManager.GetAddInfo(puid);
	if (!addInfo.has_value()) {
		//assume that the addinfo for this patient was not created
		mCurPatientAddInfo.mPatientUid = puid;
		nl::json obj;
		try {
			obj["uuid"] = boost::lexical_cast<std::string>(puid);
			obj["isReminded"] = false;
			mCurPatientAddInfo.mData = obj;
			wxGetApp().mPatientManager.SetAddInfo(mCurPatientAddInfo);
		}
		catch (std::exception& exp) {
			spdlog::critical(exp.what());
		}
	}
	else {
		mCurPatientAddInfo = addInfo.value();
	}
	LoadPatientAddInfo();
}

void pof::PatientView::LoadPatientAddInfo()
{
	if (mCurPatientAddInfo.mPatientUid == boost::uuids::nil_uuid()) return;

	//check reminded flag
	auto iter = mCurPatientAddInfo.mData.find("isReminded");
	if (iter != mCurPatientAddInfo.mData.end()) {
		mIsReminded->SetValue(static_cast<bool>(*iter));
	}


}

void pof::PatientView::CheckStoppedMedication()
{
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
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Stock check", wxICON_INFORMATION | wxOK);
		return;
	}

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
	mCurrentMedicationView->Freeze();
	auto& puid = boost::variant2::get<pof::base::data::duuid_t>(v[pof::PatientManager::PATIENT_UUID]);
	wxGetApp().mPatientManager.LoadPatientMedication(puid);
	wxGetApp().mPatientManager.LoadPatientHistory(puid);
	mCurrentMedicationView->Thaw();
	mCurrentMedicationView->Refresh();
	mCurrentMedicationView->SetFocus();

	mSPanel->Freeze();
	mPatientNameText->SetLabelText(fmt::format("Patient Name:  {} {}", boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_LAST_NAME]),
		boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_NAME])));
	mDobText->SetLabelText(fmt::format("Date of birth:  {:%d/%m/%Y}", boost::variant2::get<pof::base::data::datetime_t>(v[pof::PatientManager::PATIENT_AGE])));
	mGenderText->SetLabelText(fmt::format("Gender:  {}", boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_GENDER])));
	mPhoneText->SetLabelText(fmt::format("Phone no: {}", boost::variant2::get<pof::base::data::text_t>(v[pof::PatientManager::PATIENT_PHONE_NUMBER])));
	mSPanel->Thaw();
	mSPanel->Layout();
	mSPanel->Refresh();
	
	LoadPatientDetails();
	mBook->SetSelection(PATIENT_VIEW);
}

void pof::PatientView::OnAddPatient(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Patients", wxICON_INFORMATION | wxOK);
		return;
	}
	pof::AddPatient ap(this, wxID_ANY, "Add Patients");
	if (ap.ShowModal() == wxID_OK) {
		auto& exp = ap.GetPatientData();
		wxGetApp().mPatientManager.GetPatientData()->StoreData(std::move(const_cast<pof::base::data::row_t&&>(exp)));
	}
}

void pof::PatientView::OnRemovePatient(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Patient", wxICON_INFORMATION | wxOK);
		return;
	}
	if ((wxMessageBox("Are you sure you want to remove patient", "Patients", wxICON_INFORMATION | wxYES_NO) == wxNO)) return;
	mPatientSelect->Freeze();
	if (mPatientSelections.empty()) {
		auto item = mPatientSelect->GetSelection();
		if (!item.IsOk()) return;
		wxGetApp().mPatientManager.GetPatientData()->RemoveData(item);
	}
	else {
		wxDataViewItemArray items;
		for (const wxDataViewItem& item : mPatientSelections) {
			items.push_back(item);
		}
		wxGetApp().mPatientManager.GetPatientData()->RemoveData(items);
	}
	mPatientSelect->Thaw();
	mPatientSelect->Refresh();
}

void pof::PatientView::OnAddMedication(wxCommandEvent& evt)
{
	pof::SearchProduct prodSearch(this, wxID_ANY);
	auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
	if (prodSearch.ShowModal() != wxID_OK) return;
	mPatientInfoBar->Dismiss();
	wxBusyCursor cursor;
	mCurrentMedicationView->Freeze();
	if (prodSearch.HasMultipleSelections()) {
		auto vec = prodSearch.GetSelectedProducts();
		auto& puuid = boost::variant2::get<pof::base::data::duuid_t>
					(mCurrentPatient.value().get().first[pof::PatientManager::PATIENT_UUID]);
		auto today = pof::base::data::clock_t::now();
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
			v[pof::PatientManager::MED_STOP_DATE] = today + date::days(1);


			//store
			wxGetApp().mPatientManager.GetPatientMedData()->StoreData(std::move(row));
		}
	}
	else {
		auto& prod = prodSearch.GetSelectedProduct();
		auto& puuid = boost::variant2::get<pof::base::data::duuid_t>
			(mCurrentPatient.value().get().first[pof::PatientManager::PATIENT_UUID]);
		const pof::base::data::duuid_t& pid = boost::variant2::get<pof::base::data::duuid_t>(prod.first[pof::ProductManager::PRODUCT_UUID]);
		pof::base::data::row_t row;

		auto& v = row.first;
		auto& p = prod.first;
		if (std::ranges::any_of(datastore, [&](pof::base::data::row_t& item) -> bool {
			return boost::variant2::get<pof::base::data::duuid_t>(item.first[pof::PatientManager::MED_PRODUCT_UUID]) == pid; }))
		{
			mPatientInfoBar->ShowMessage("Medication already added", wxICON_INFORMATION);
			StartTimer();
			return;
		}
		v.resize(pof::PatientManager::MED_MAX);
		auto today = pof::base::data::clock_t::now();
		auto& vs = prod.first;

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
		v[pof::PatientManager::MED_STOP_DATE] = today + date::days(1);

		wxGetApp().mPatientManager.GetPatientMedData()->StoreData(std::move(row));
	}
	mCurrentMedicationView->Thaw();
	mCurrentMedicationView->Refresh();
}

void pof::PatientView::OnAddPacks(wxCommandEvent& evt)
{
	pof::PackView pk(this, true, wxID_ANY, wxT("Add Pack to patient"));
	if (pk.ShowModal() != wxID_OK) return;
	auto values = pk.GetPackProducts();
	if (values.empty()) return;
	auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
	auto& puuid = boost::variant2::get<pof::base::data::duuid_t>
		(mCurrentPatient.value().get().first[pof::PatientManager::PATIENT_UUID]);
	wxBusyCursor cursor;
	auto today = pof::base::data::clock_t::now();
	for (auto& p : values) {
		if (std::ranges::any_of(datastore, [&](pof::base::data::row_t& item) -> bool {
			return boost::variant2::get<pof::base::data::duuid_t>(item.first[pof::PatientManager::MED_PRODUCT_UUID]) == std::get<1>(p); }))
		{
			continue;
		}
		pof::base::data::row_t row;
		auto& v = row.first;
		v.resize(pof::PatientManager::MED_MAX);

		v[pof::PatientManager::MED_PRODUCT_UUID] = std::get<1>(p);
		v[pof::PatientManager::MED_PATIENT_UUID] = puuid;
		v[pof::PatientManager::MED_NAME] = std::get<2>(p);
		v[pof::PatientManager::MED_PURPOSE] = ""s;
		v[pof::PatientManager::MED_OUTCOME] = ""s;
		v[pof::PatientManager::MED_STOCK] = std::get<3>(p);
		v[pof::PatientManager::MED_DIR_FOR_USE_QUANTITY] = static_cast<std::uint64_t>(0);
		v[pof::PatientManager::MED_DIR_FOR_USE_STRENGTH] = ""s;
		v[pof::PatientManager::MED_DURATION] = static_cast<std::uint64_t>(0);
		v[pof::PatientManager::MED_START_DATE] = today;
		v[pof::PatientManager::MED_STOP_DATE] = today + date::days(1);

		wxGetApp().mPatientManager.GetPatientMedData()->StoreData(std::move(row));
	}
	mPatientInfoBar->ShowMessage(fmt::format("Added {:d} products from pack to patient", values.size()));
	StartTimer();

}

void pof::PatientView::OnRemoveMedication(wxCommandEvent& evt)
{
	auto item = mCurrentMedicationView->GetSelection();
	if (!item.IsOk()) return;
	if (wxMessageBox("Are you sure you want to remove medication from the patient?", "Patient", wxICON_QUESTION | wxYES_NO) == wxNO) return;
	mCurrentMedicationView->Freeze();
	if (mMedicationSelections.empty()) {
			wxGetApp().mPatientManager.GetPatientMedData()->RemoveData(item);
			mPatientInfoBar->ShowMessage(fmt::format("Successfully removed product from medication"));
			StartTimer();
	}
	else {
		wxDataViewItemArray items;
		for (auto& item : mMedicationSelections) {
			items.push_back(item);
		}
		wxGetApp().mPatientManager.GetPatientMedData()->RemoveData(items);
		mPatientInfoBar->ShowMessage(fmt::format("Successfully removed {:d} products from medication", items.size()));
	}
	mCurrentMedicationView->Thaw();
	mCurrentMedicationView->Refresh();
	mCurrentMedicationView->SetFocus();
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Removed product from medication");
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
	auto ou = menu->Append(ID_ADD_OUTCOME, "Add outcome", nullptr);
	auto rs = menu->Append(ID_ADD_REASON, "Add reason", nullptr);

	mCurrentMedicationView->PopupMenu(menu);
}

void pof::PatientView::OnMedicationsSelected(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& row = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore()[idx];
	auto& startDate = boost::variant2::get<pof::base::data::datetime_t>(row.first[pof::PatientManager::MED_START_DATE]);
	auto& stopDate = boost::variant2::get<pof::base::data::datetime_t>(row.first[pof::PatientManager::MED_STOP_DATE]);


	mStartDatePicker->SetValue(wxDateTime{ pof::base::data::clock_t::to_time_t(startDate) });
	mStopDatePicker->SetValue(wxDateTime{ pof::base::data::clock_t::to_time_t(stopDate) });

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
	mPatientInfoBar->Dismiss();
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

void pof::PatientView::OnDateChanged(wxDateEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Stock check", wxICON_INFORMATION | wxOK);
		return;
	}
	auto item = mCurrentMedicationView->GetSelection();
	if (!item.IsOk()) {
		mPatientInfoBar->ShowMessage("No Medication selected for date change", wxICON_INFORMATION | wxOK);
		return;
	} 
	const wxDateTime& dt = evt.GetDate();
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& row = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore()[idx];
	auto& startDate = boost::variant2::get<pof::base::data::datetime_t>(row.first[pof::PatientManager::MED_START_DATE]);
	auto& stopDate = boost::variant2::get<pof::base::data::datetime_t>(row.first[pof::PatientManager::MED_STOP_DATE]);

	mCurrentMedicationView->Freeze();
	auto setdate = pof::base::data::clock_t::from_time_t(dt.GetTicks());
	switch (evt.GetId())
	{
	case ID_START_DATE_PICKER:
		//if less than today
		if ((date::floor<date::days>(setdate) > date::floor<date::days>(stopDate)) || 
			(date::floor<date::days>(setdate) < date::floor<date::days>(pof::base::data::clock_t::now()))) {
			mPatientInfoBar->ShowMessage("Start date cannot be less than current date or greater than stop date", wxICON_ERROR);
			evt.Skip();
			break;
		}
		startDate = setdate;
		break;
	case ID_STOP_DATE_PICKER:
		//if less than start date or less tha today
		if ((date::floor<date::days>(setdate) < date::floor<date::days>(startDate)) ||
			(date::floor<date::days>(setdate) < date::floor<date::days>(pof::base::data::clock_t::now()))) {
			mPatientInfoBar->ShowMessage("Stop date cannot be less than start date or current date", wxICON_ERROR);
			evt.Skip();
			break;
		}
		stopDate = pof::base::data::clock_t::from_time_t(dt.GetTicks());
		break;
	default:
		break;
	}
	mCurrentMedicationView->Thaw();
	mCurrentMedicationView->Refresh();
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
			//modified is always updated
			break;
		default:
			break;
		};
		//always set modified if we change the details, also update the view
		vp[pof::PatientManager::PATIENT_MODIFIED_DATE] = pof::base::data::clock_t::now();
		vps.set(pof::PatientManager::PATIENT_MODIFIED_DATE);
		mPatientDetails->GetPage("Medication")->GetGrid()->GetProperty("9")->SetValue(wxVariant(wxDateTime(
			pof::base::data::clock_t::to_time_t(boost::variant2::get<pof::base::data::datetime_t>(vp[pof::PatientManager::PATIENT_MODIFIED_DATE])))));
		
		auto item = mPatientSelect->GetSelection();
		if (item.IsOk()) {
			auto& dp = wxGetApp().mPatientManager.GetPatientData();
			dp->Signal(pof::DataModel::Signals::UPDATE, pof::DataModel::GetIdxFromItem(item));
			mPatientInfoBar->ShowMessage("Patient updated successfully", wxICON_INFORMATION);
			StartTimer();
			vps.reset();
		}
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}
}

void pof::PatientView::OnSellCurrentMedication(wxCommandEvent& evt)
{
	auto& saleData = wxGetApp().mSaleManager.GetSaleData();
	if (!saleData->GetDatastore().empty()) {
		wxMessageBox("Sale is not empty, checkout or clear sale before adding medications to sale","Patients", wxICON_INFORMATION | wxOK);
		return;
	}
	
	wxBusyCursor cursor;
	if (mMedicationSelections.empty()) {
		if (wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore().empty()) {
			mPatientInfoBar->ShowMessage("Patient active medication list is empty", wxICON_INFORMATION);

		}
		else if (!fSaleSignal(wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore()).value_or(false)) {
			mPatientInfoBar->ShowMessage("Could not add medications to sale successfully, please check stock and expiry of the products in store", wxICON_ERROR);
		}
		else {
			mPatientInfoBar->ShowMessage("Medication added to sale successfully", wxICON_INFORMATION);
		}
	}
	else {
		auto& medData = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
		pof::base::data newData;
		newData.set_metadata(medData.get_metadata());
		for (auto& item : mMedicationSelections) {
			auto& row = medData[pof::DataModel::GetIdxFromItem(item)];
			newData.insert(row.first);
		}
		 if (!fSaleSignal(newData).value_or(false)) {
			mPatientInfoBar->ShowMessage("Could not add medications to sale successfully, please check stock and expiry of the products in store", wxICON_ERROR);
		}
		else {
			mPatientInfoBar->ShowMessage(fmt::format("medications added to sale successfully"), wxICON_INFORMATION);
		}
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

void pof::PatientView::OnClearTimer(wxTimerEvent& evt)
{
	if (mPatientInfoBar->IsShown())
		mPatientInfoBar->Dismiss();
	RemovePropertyModification();
}

void pof::PatientView::OnEditingStarted(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	auto col = evt.GetDataViewColumn();
	if (col == mStockMedCol) {
		auto ctrl = col->GetRenderer()->GetEditorCtrl();
		wxIntegerValidator<size_t> val{ NULL };
		val.SetMin(0);
		val.SetMax(10000);
		ctrl->SetValidator(val);
	}

	//check dates for invalid values ->

}

void pof::PatientView::RemovePropertyModification()
{
	auto grid = mPatientDetails->GetPage("Medication")->GetGrid();
	if (!grid->IsAnyModified()) return;

	auto gridIter = grid->GetIterator();
	while (!gridIter.AtEnd()) {
		if (gridIter.GetProperty()->HasFlag(wxPG_PROP_MODIFIED)) {
			gridIter.GetProperty()->SetFlagRecursively(wxPG_PROP_MODIFIED, false);
		}
		gridIter++;
	}
}

void pof::PatientView::OnPatientDetailsUpdateUI(wxUpdateUIEvent& evt)
{
	//mPatientDetails->Refresh();
}

void pof::PatientView::OnAddText(wxCommandEvent& evt)
{
	//add reason or out come
	auto item = mCurrentMedicationView->GetSelection();
	if (!item.IsOk()) return;
	wxWindowID d = evt.GetId();
	mCurrentMedicationView->Freeze();
	auto& datastore = wxGetApp().mPatientManager.GetPatientMedData()->GetDatastore();
	switch (d)
	{
	case ID_ADD_OUTCOME:
	{
		auto& row = datastore[pof::DataModel::GetIdxFromItem(item)];
		auto& v = boost::variant2::get<pof::base::data::text_t>(row.first[pof::PatientManager::MED_OUTCOME]);
		auto text = wxGetTextFromUser("Add a possible outcome for this medication", "Add Outcome", v);
		if (text.empty()) goto exit;
		
		row.first[pof::PatientManager::MED_OUTCOME] = text.ToStdString();
		row.second.second.set(pof::PatientManager::MED_OUTCOME);
		wxGetApp().mPatientManager.GetPatientMedData()->ItemChanged(item);
		wxGetApp().mPatientManager.GetPatientMedData()->Signal(pof::DataModel::Signals::UPDATE, pof::DataModel::GetIdxFromItem(item));
	}
		break;
	case ID_ADD_REASON:
	{
		auto& row = datastore[pof::DataModel::GetIdxFromItem(item)];
		auto& v = boost::variant2::get<pof::base::data::text_t>(row.first[pof::PatientManager::MED_PURPOSE]);
		auto text = wxGetTextFromUser("Add a possible reason for this medication", "Add Reason", v);
		if (text.empty()) goto exit;


		row.second.second.set(pof::PatientManager::MED_PURPOSE);
		row.first[pof::PatientManager::MED_PURPOSE] = text.ToStdString();

		wxGetApp().mPatientManager.GetPatientMedData()->ItemChanged(item);
		wxGetApp().mPatientManager.GetPatientMedData()->Signal(pof::DataModel::Signals::UPDATE, pof::DataModel::GetIdxFromItem(item));
	}
	break;
	default:
		break;
	}
exit:
	mCurrentMedicationView->Thaw();
	mCurrentMedicationView->Refresh();
}

void pof::PatientView::OnReminded(wxCommandEvent& evt)
{
	if (mCurPatientAddInfo.mPatientUid != boost::uuids::nil_uuid()) {
		auto iter = mCurPatientAddInfo.mData.find("isReminded");
		if (iter != mCurPatientAddInfo.mData.end()) {
			*iter = evt.IsChecked();
		}
		wxGetApp().mPatientManager.UpdateAddInfo(mCurPatientAddInfo);
	}
	else {
		wxMessageBox("Error in loading patient information, please contact D-GLOPA admin", "Patients", wxICON_ERROR | wxOK);
		mIsReminded->SetValue(false);
	}
	
}

void pof::PatientView::OnPatientSaleHist(wxCommandEvent& evt)
{
	auto rel = wxGetApp().mPatientManager.GetSaleForPatient(mCurPatientAddInfo.mPatientUid);
	if (!rel.has_value() || rel->empty()) {
		wxMessageBox("No sale avaliable for patient", "Patients", wxICON_INFORMATION | wxOK);
		return;
	}
}

void pof::PatientView::OnPatientSaleCompleted(const pof::base::data::duuid_t& saleId, size_t type)
{
	if (type != 1) return; //not a patient type
	auto iter = mCurPatientAddInfo.mData.find("saleIds");
	if (iter == mCurPatientAddInfo.mData.end()) {
		mCurPatientAddInfo.mData["saleIds"] = nl::json::array();
		iter = mCurPatientAddInfo.mData.find("saleIds");
	}
	try {
		iter->push_back(boost::lexical_cast<std::string>(saleId));
		wxGetApp().mPatientManager.UpdateAddInfo(mCurPatientAddInfo);
	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
	}
}
