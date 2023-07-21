#include "PackView.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::PackView, wxDialog)
EVT_TOOL(pof::PackView::ID_TOOL_CREATE_PACK, pof::PackView::OnCreatePack)
EVT_TOOL(pof::PackView::ID_TOOL_ADD_PACK, pof::PackView::OnAddPack)
EVT_TOOL(pof::PackView::ID_TOOL_REMOVE_PACK, pof::PackView::OnRemovePack)
END_EVENT_TABLE()


pof::PackView::PackView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	mTopTools = new wxAuiToolBar( this, ID_TOOL, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT );
	mTopTools->SetToolBitmapSize( wxSize( 16,16 ) );
	mTopTools->SetMinSize( wxSize( -1, 40 ) );
	
	mTopTools->Realize(); 
	
	bSizer1->Add( mTopTools, 0, wxALL|wxEXPAND, 5 );
	
	mBook = new wxSimplebook( this, ID_BOOK, wxDefaultPosition, wxDefaultSize, 0 );
	mPackView = new wxPanel( mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	mPackSelect = new wxListCtrl( mPackView, ID_PACK_SELECT, wxDefaultPosition, wxDefaultSize, wxLC_AUTOARRANGE|wxLC_ICON|wxLC_NO_HEADER );
	bSizer2->Add(mPackSelect, 1, wxALL|wxEXPAND, 2 );
	
	
	mPackView->SetSizer( bSizer2 );
	mPackView->Layout();
	bSizer2->Fit( mPackView );
	mBook->AddPage( mPackView, wxT("a page"), true );
	mPackData = new wxPanel( mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_dataViewCtrl3 = new wxDataViewCtrl( mPackData, ID_PACK_DATA, wxDefaultPosition, wxDefaultSize, 0 );
	mProductName = m_dataViewCtrl3->AppendTextColumn( wxT("Name"), 0 );
	mProductQuantity = m_dataViewCtrl3->AppendTextColumn( wxT("Quantity"), 0 );
	mPrice = m_dataViewCtrl3->AppendTextColumn( wxT("Price"), 0 );
	mExtPrice = m_dataViewCtrl3->AppendTextColumn( wxT("Exact Price"), 1 );
	bSizer3->Add( m_dataViewCtrl3, 1, wxALL|wxEXPAND, 2 );
	
	m_panel4 = new wxPanel( mPackData, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel4->SetMinSize( wxSize( -1,50 ) );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_panel4, wxID_ANY, wxT("Total Quantity: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL, 5 );
	
	mTotalQuantity = new wxStaticText( m_panel4, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalQuantity->Wrap( -1 );
	bSizer4->Add( mTotalQuantity, 0, wxALL|wxLEFT, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	mTotalAmountText = new wxStaticText( m_panel4, wxID_ANY, wxT("Total Amount:  "), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalAmountText->Wrap( -1 );
	bSizer4->Add( mTotalAmountText, 0, wxALL, 5 );
	
	mTotalAmount = new wxStaticText( m_panel4, wxID_ANY, wxT("N 0.0"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalAmount->Wrap( -1 );
	bSizer4->Add( mTotalAmount, 0, wxALL, 5 );
	
	
	m_panel4->SetSizer( bSizer4 );
	m_panel4->Layout();
	bSizer4->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 0, wxEXPAND | wxALL, 5 );
	
	
	mPackData->SetSizer( bSizer3 );
	mPackData->Layout();
	bSizer3->Fit( mPackData );
	mBook->AddPage( mPackData, wxT("a page"), false );
	
	bSizer1->Add( mBook, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

pof::PackView::~PackView()
{
}

bool pof::PackView::TransferDataFromWindow()
{
	return false;
}

void pof::PackView::ShowPack()
{
}

void pof::PackView::OnPackActivate(wxListEvent& evt)
{
}

void pof::PackView::OnPackSelected(wxListEvent& evt)
{
}

void pof::PackView::OnAddPack(wxCommandEvent& evt)
{
}

void pof::PackView::OnCreatePack(wxCommandEvent& evt)
{
}

void pof::PackView::OnRemovePack(wxCommandEvent& evt)
{
}
