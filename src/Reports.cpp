#include "Reports.h"
BEGIN_EVENT_TABLE(pof::ReportsDialog, wxDialog)
EVT_TOOL(pof::ReportsDialog::ID_PRINT, pof::ReportsDialog::OnPrint)
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

void pof::ReportsDialog::OnPrint(wxCommandEvent& evt)
{
}

void pof::ReportsDialog::OnDownloadExcel(wxCommandEvent& evt)
{
}


