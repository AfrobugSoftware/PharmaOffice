///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"

///////////////////////////////////////////////////////////////////////////
using namespace pof;

StockCheck::StockCheck( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 878,689 ), wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	
	mMainPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( mMainPane, wxAuiPaneInfo() .Left() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	mStockData = new wxDataViewCtrl( mMainPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	mProductName = mStockData->AppendTextColumn( wxT("Product Name"), 0 );
	mCurrenctStock = mStockData->AppendTextColumn( wxT("Currenct Stock"), 0 );
	mCheckedStock = mStockData->AppendTextColumn( wxT("Checked Stock"), 0 );
	mShortage = mStockData->AppendTextColumn( wxT("Shortage"), 0 );
	mStatus = mStockData->AppendBitmapColumn( wxT("Status"), 0 );
	bSizer1->Add( mStockData, 1, wxALL|wxEXPAND, 0 );
	
	
	mMainPane->SetSizer( bSizer1 );
	mMainPane->Layout();
	bSizer1->Fit( mMainPane );
	mSummary = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mSummary->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	m_mgr.AddPane( mSummary, wxAuiPaneInfo() .Name( wxT("TotalPane") ).Bottom() .Caption( wxT("Summary") ).PinButton( true ).Dock().Resizable().FloatingSize( wxSize( 50,67 ) ).TopDockable( false ).LeftDockable( false ).RightDockable( false ).Floatable( false ).Row( 1 ).MinSize( wxSize( 50,150 ) ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 0, 2, 2, 2 );
	
	mTotalStockCheckedLabel = new wxStaticText( mSummary, wxID_ANY, wxT("Total Stock Checked:"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalStockCheckedLabel->Wrap( -1 );
	gSizer1->Add( mTotalStockCheckedLabel, 0, wxALL, 2 );
	
	mTotalStockCheckedValue = new wxStaticText( mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalStockCheckedValue->Wrap( -1 );
	gSizer1->Add( mTotalStockCheckedValue, 0, wxALL, 5 );
	
	mTotalShortageLabel = new wxStaticText( mSummary, wxID_ANY, wxT("Total Shortage"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalShortageLabel->Wrap( -1 );
	gSizer1->Add( mTotalShortageLabel, 0, wxALL, 5 );
	
	mTotalShortageValue = new wxStaticText( mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalShortageValue->Wrap( -1 );
	gSizer1->Add( mTotalShortageValue, 0, wxALL, 5 );
	
	mShortageAmountLabel = new wxStaticText( mSummary, wxID_ANY, wxT("Shortage Amount:"), wxDefaultPosition, wxDefaultSize, 0 );
	mShortageAmountLabel->Wrap( -1 );
	gSizer1->Add( mShortageAmountLabel, 0, wxALL, 5 );
	
	mShortageAmountValue = new wxStaticText( mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mShortageAmountValue->Wrap( -1 );
	gSizer1->Add( mShortageAmountValue, 0, wxALL, 5 );
	
	
	bSizer2->Add( gSizer1, 1, wxEXPAND, 0 );
	
	
	mSummary->SetSizer( bSizer2 );
	mSummary->Layout();
	bSizer2->Fit( mSummary );
	
	m_mgr.Update();
	this->Centre( wxBOTH );
}

StockCheck::~StockCheck()
{
	m_mgr.UnInit();
	
}
