#include "PofPch.h"
#include "PatientView.h"
BEGIN_EVENT_TABLE(pof::PatientView, wxPanel)
	EVT_TOOL(pof::PatientView::ID_ADD_PATIENTS, pof::PatientView::OnAddPatient)
	EVT_TOOL(pof::PatientView::ID_SELECT, pof::PatientView::OnSelectCol)
	EVT_TOOL(pof::PatientView::ID_SELECT_MED, pof::PatientView::OnSelectMedCol)


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
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientSelected)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientHeaderClicked)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnMedHeaderClicked)
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


	mManager.AddPane(mTopTools, wxAuiPaneInfo().Name("TopTools").Top().MinSize(-1, 30).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
	mManager.AddPane(mPatientTools, wxAuiPaneInfo().Name("PaitentTools").Top().MinSize(-1, 30).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());
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

	mPatientPanel =  new wxScrolledWindow(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	mCurrentMedicationPane = new wxCollapsiblePane(mPatientPanel, wxID_ANY, "Current Medications");
	wxSizer* csz = new wxBoxSizer(wxVERTICAL);
	
	mCurrentMedicationView = new wxDataViewCtrl(mCurrentMedicationPane->GetPane(), ID_PATIENT_MEDS_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mCurrentMedicationView->AssociateModel(wxGetApp().mPatientManager.GetPatientMedData().get());
	mCurrentMedicationView->AppendTextColumn("Medication", pof::PatientManager::MED_NAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Reason", pof::PatientManager::MED_PURPOSE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Outcome", pof::PatientManager::MED_OUTCOME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Direction For Use", 1000, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Start Stock", pof::PatientManager::MED_STOCK, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Start Date", pof::PatientManager::MED_START_DATE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mCurrentMedicationView->AppendTextColumn("Stop Date", pof::PatientManager::MED_STOP_DATE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	
	csz->Add(mCurrentMedicationView, wxSizerFlags().Expand().Border(wxALL, 0));
	
	mCurrentMedicationPane->GetPane()->SetSizer(csz);
	csz->SetSizeHints(mCurrentMedicationPane->GetPane());
	mCurrentMedicationPane->Expand();



	mMedsHistoryPane = new wxCollapsiblePane(mPatientPanel, wxID_ANY, "Patient medication history");
	wxSizer* msz = new wxBoxSizer(wxVERTICAL);
	mMedHistoryView = new wxDataViewCtrl(mMedsHistoryPane->GetPane(), ID_PATIENT_HISTORY_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mMedHistoryView->AssociateModel(wxGetApp().mPatientManager.GetPatientHistotyData().get());
	mMedHistoryView->AppendTextColumn("Medication", pof::PatientManager::MED_NAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Reason", pof::PatientManager::MED_PURPOSE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Outcome", pof::PatientManager::MED_OUTCOME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Direction For Use", 1000, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Start Stock", pof::PatientManager::MED_STOCK, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Start Date", pof::PatientManager::MED_START_DATE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Stop Date", pof::PatientManager::MED_STOP_DATE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

	msz->Add(mMedHistoryView, wxSizerFlags().Expand().Border(wxALL, 0));
	mMedsHistoryPane->GetPane()->SetSizer(msz);
	msz->SetSizeHints(mMedsHistoryPane->GetPane());
	mMedsHistoryPane->Collapse();

	sz->Add(mCurrentMedicationPane, wxSizerFlags().Expand().Border(wxALL, 2));
	sz->Add(mMedsHistoryPane, wxSizerFlags().Expand().Border(wxALL, 2));


	mPatientPanel->SetSizer(sz);
	mPatientPanel->Layout();
	sz->Fit(mPatientPanel);

	mBook->AddPage(mPatientSelect, "Patient Select", true);
	mBook->AddPage(mPatientPanel, "Patient medication panel", false);
	
	mManager.AddPane(mBook, wxAuiPaneInfo().Name("Book").CenterPane().Show());
}

void pof::PatientView::CreatePatientSummaryPopUp()
{
	wxPanel* mPopup = new wxPanel;
	wxBitmap pbm = GetPatientBitMap();
}

void pof::PatientView::CreatePatientDetailsPane()
{
	///
}

void pof::PatientView::CreateSpecialCols()
{
	pof::DataModel::SpeicalColHandler_t SelectColHandler;
	pof::DataModel::SpeicalColHandler_t SelectMedColHandler;

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

	wxGetApp().mPatientManager.GetPatientData()->SetSpecialColumnHandler(SELECTION_COL, std::move(SelectColHandler));;
	wxGetApp().mPatientManager.GetPatientMedData()->SetSpecialColumnHandler(SELECTION_COL, std::move(SelectMedColHandler));;

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

void pof::PatientView::OnPatientSelected(wxDataViewEvent& evt)
{
}

void pof::PatientView::OnAddPatient(wxCommandEvent& evt)
{
	pof::AddPatient ap(this, wxID_ANY, "Add Patients");
	if (ap.ShowModal() == wxID_OK) {
		auto& exp = ap.GetPatientData();
		wxGetApp().mPatientManager.GetPatientData()->EmplaceData(std::move(const_cast<pof::base::data::row_t&&>(exp)));
	}
}

void pof::PatientView::OnRemovePatient(wxCommandEvent& evt)
{
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

}

void pof::PatientView::OnSearchCleared(wxCommandEvent& evt)
{
}

void pof::PatientView::OnStopProduct(wxCommandEvent& evt)
{
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
