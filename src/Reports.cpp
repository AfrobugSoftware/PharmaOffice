#include "Reports.h"


pof::ReportsDialog::ReportsDialog(wxWindow* parent, const pof::ReportsDialog::ReportParams& params, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	m_panel5 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel5->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	mTools = new wxAuiToolBar(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);
	mTools->SetMaxSize(wxSize(-1, 40));

	auto text = new wxStaticText(mTools, wxID_ANY, "FROM: ");
	text->SetBackgroundColour(*wxWHITE);
	mDateFrom = new wxDatePickerCtrl(mTools, ID_DATE_FROM);
	
	auto text2 = new wxStaticText(mTools, wxID_ANY, "TO: ");
	text2->SetBackgroundColour(*wxWHITE);
	mDateTo = new wxDatePickerCtrl(mTools, ID_DATE_TO);

	mTools->AddControl(text);
	mTools->AddSpacer(5);
	mTools->AddControl(mDateFrom);
	
	mTools->AddSpacer(5);

	mTools->AddControl(text2);
	mTools->AddSpacer(5);
	mTools->AddControl(mDateTo);

    mTools->AddStretchSpacer();
	mPrint = mTools->AddTool(ID_PRINT, wxT("Print"), wxArtProvider::GetBitmap(wxART_PRINT), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);


	mTools->Realize();

	bSizer7->Add(mTools, 0, wxALL | wxEXPAND, 2);

	mGrid = new wxGrid(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	// Grid
	mGrid->CreateGrid(params.rowCount, params.colCount);
	mGrid->EnableEditing(params.enableEditing);
	mGrid->EnableGridLines(params.enableGridLines);
	mGrid->EnableDragGridSize(params.enableDragGridSize);
	mGrid->SetMargins(0, 0);

	// Columns
	mGrid->EnableDragColMove(true);
	mGrid->EnableDragColSize(true);
	mGrid->SetColLabelSize(params.colLabelSize);
	mGrid->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);

	// Rows
	mGrid->EnableDragRowSize(true);
	mGrid->SetRowLabelSize(params.rowLabelSize);
	mGrid->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);

	// Label Appearance
	mGrid->SetLabelBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

	// Cell Defaults
	mGrid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);
	bSizer7->Add(mGrid, 1, wxALL | wxEXPAND, 0);


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
