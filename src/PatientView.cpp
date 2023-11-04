#include "PofPch.h"
#include "PatientView.h"
BEGIN_EVENT_TABLE(pof::PatientView, wxPanel)
	EVT_TOOL(pof::PatientView::ID_ADD_PATIENTS, pof::PatientView::OnAddPatient)

	//dataview
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientsContextMenu)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::PatientView::ID_PATIENT_MEDS_VIEW, pof::PatientView::OnMedicationsContextMenu)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::PatientView::ID_PATIENT_VIEW, pof::PatientView::OnPatientSelected)
END_EVENT_TABLE()

pof::PatientView::PatientView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPanel(parent, id, pos, size, style){
	mManager.SetManagedWindow(this);
	SetSizeHints(wxDefaultSize, wxDefaultSize);
	SetBackgroundColour(*wxWHITE);

	CreateToolBars();
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
	mTopTools = new wxAuiToolBar(this, ID_PATIENT_TOOLS, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);

	mTopTools->AddTool(ID_SELECT, "Select", wxArtProvider::GetBitmap("action_check"), "Show Selections");
	mTopTools->AddStretchSpacer();
	mTopTools->AddTool(ID_ADD_PATIENTS, "Add Patients", wxArtProvider::GetBitmap("action_add"), "Add a patient to the pharmacy");

	mManager.AddPane(mTopTools, wxAuiPaneInfo().Name("TopTools").Top().MinSize(-1, 30).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
	mManager.AddPane(mTopTools, wxAuiPaneInfo().Name("PaitentTools").Top().MinSize(-1, 30).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());
}

void pof::PatientView::CreateViews()
{
	mBook = new wxSimplebook(this, ID_BOOK);
	mPatientSelect = new wxDataViewCtrl(mBook, ID_PATIENT_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	
	mPatientPanel =  new wxScrolledWindow(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	mCurrentMedicationPane = new wxCollapsiblePane(mPatientPanel, wxID_ANY, "Current Medications");
	wxSizer* csz = new wxBoxSizer(wxVERTICAL);
	
	mCurrentMedicationView = new wxDataViewCtrl(mCurrentMedicationPane->GetPane(), ID_PATIENT_MEDS_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	
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
	mMedHistoryView = new wxDataViewCtrl(mBook, ID_PATIENT_HISTORY_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mMedHistoryView->AppendTextColumn("Medication", pof::PatientManager::MED_NAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Reason", pof::PatientManager::MED_PURPOSE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Outcome", pof::PatientManager::MED_OUTCOME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Direction For Use", 1000, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Start Stock", pof::PatientManager::MED_STOCK, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Start Date", pof::PatientManager::MED_START_DATE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	mMedHistoryView->AppendTextColumn("Stop Date", pof::PatientManager::MED_STOP_DATE, wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

	msz->Add(mMedHistoryView, wxSizerFlags().Expand().Border(wxALL, 0));
	mMedsHistoryPane->GetPane()->SetSizer(csz);
	msz->SetSizeHints(mMedsHistoryPane->GetPane());
	mMedsHistoryPane->Collapse();

	sz->Add(mCurrentMedicationPane, wxSizerFlags().Expand().Border(wxALL, 2).Align(wxALIGN_CENTER_HORIZONTAL));
	sz->Add(mMedsHistoryPane, wxSizerFlags().Expand().Border(wxALL, 2).Align(wxALIGN_CENTER_HORIZONTAL));


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
}

void pof::PatientView::CreateSpecialCols()
{
}

void pof::PatientView::ShowPatientDetails()
{
}

wxBitmap pof::PatientView::GetPatientBitMap()
{

	return wxBitmap();
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
}

void pof::PatientView::OnRemovePatient(wxCommandEvent& evt)
{
}

void pof::PatientView::OnSelectCol(wxCommandEvent& evt)
{
}

void pof::PatientView::OnPatientsContextMenu(wxDataViewEvent& evt)
{
}

void pof::PatientView::OnMedicationsContextMenu(wxDataViewEvent& evt)
{
}
