#include "SaleView.h"
#include "Application.h"
BEGIN_EVENT_TABLE(pof::SaleView, wxPanel)
	EVT_BUTTON(pof::SaleView::ID_CHECKOUT, pof::SaleView::OnCheckout)
	EVT_BUTTON(pof::SaleView::ID_CLEAR, pof::SaleView::OnClear)
	EVT_BUTTON(pof::SaleView::ID_SAVE, pof::SaleView::OnSave)
	EVT_DATAVIEW_ITEM_DROP_POSSIBLE(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDropPossible)
	EVT_DATAVIEW_ITEM_DROP(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDrop)
END_EVENT_TABLE()


pof::SaleView::SaleView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	mMainPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	mTopTools = new wxPanel( mMainPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	mProductNameText = new wxStaticText( mTopTools, wxID_ANY, wxT("Product Name: "), wxDefaultPosition, wxDefaultSize, 0 );
	mProductNameText->Wrap( -1 );
	bSizer7->Add( mProductNameText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mProductNameValue = new wxTextCtrl( mTopTools,ID_PRODUCT_SEARCH_NAME, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	bSizer7->Add( mProductNameValue, 0, wxALL, 5 );
	
	mScanProduct = new wxStaticText( mTopTools, wxID_ANY, wxT("Scan Product: "), wxDefaultPosition, wxDefaultSize, 0 );
	mScanProduct->Wrap( -1 );
	bSizer7->Add( mScanProduct, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mScanProductValue = new wxTextCtrl( mTopTools, ID_PRODUCT_SCAN, wxEmptyString, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	bSizer7->Add( mScanProductValue, 0, wxALL, 5 );
	
	
	mTopTools->SetSizer( bSizer7 );
	mTopTools->Layout();
	bSizer7->Fit( mTopTools );
	bSizer5->Add( mTopTools, 0, wxEXPAND | wxALL, 0 );
	
	mDataPane = new wxPanel( mMainPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_dataViewCtrl1 = new wxDataViewCtrl( mDataPane, ID_SALE_DATA_VIEW, wxDefaultPosition, wxDefaultSize, 0 );
	m_dataViewCtrl1->AssociateModel(wxGetApp().mSaleManager.GetSaleData().get());


	mSerialNumber = m_dataViewCtrl1->AppendTextColumn( wxT("SERIAL NUMBER"), pof::SaleManager::PRODUCT_SERIAL_NUM );
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn( wxT("PRODUCT NAME"), pof::SaleManager::PRODUCT_NAME );
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn( wxT("PRODUCT CATEGORY"), pof::SaleManager::PRODUCT_CATEGORY );
	mQuantityColumn = m_dataViewCtrl1->AppendTextColumn( wxT("QUANTITY"), pof::SaleManager::PRODUCT_QUANTITY);
	mPriceCol = m_dataViewCtrl1->AppendTextColumn( wxT("PRICE"), 0 );
	bSizer6->Add( m_dataViewCtrl1, 1, wxALL|wxEXPAND, 0 );
	
	
	mDataPane->SetSizer( bSizer6 );
	mDataPane->Layout();
	bSizer6->Fit( mDataPane );
	bSizer5->Add( mDataPane, 1, wxEXPAND | wxALL, 0 );
	
	
	mMainPane->SetSizer( bSizer5 );
	mMainPane->Layout();
	bSizer5->Fit( mMainPane );
	bSizer1->Add( mMainPane, 1, wxEXPAND | wxALL, 0 );
	
	mSaleOutputPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	mSaleDisplayPane = new wxPanel( mSaleOutputPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	mTextOutPut = new wxPanel( mSaleDisplayPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 4, 2, 5, 5 );
	
	mQuantity = new wxStaticText( mTextOutPut, wxID_ANY, wxT("Quantity"), wxDefaultPosition, wxDefaultSize, 0 );
	mQuantity->Wrap( -1 );
	gSizer1->Add( mQuantity, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mQuantityValue = new wxStaticText( mTextOutPut, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mQuantityValue->Wrap( -1 );
	gSizer1->Add( mQuantityValue, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mExtQuantity = new wxStaticText( mTextOutPut, wxID_ANY, wxT("Ext. Quantity"), wxDefaultPosition, wxDefaultSize, 0 );
	mExtQuantity->Wrap( -1 );
	gSizer1->Add( mExtQuantity, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mExtQuantityItem = new wxStaticText( mTextOutPut, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mExtQuantityItem->Wrap( -1 );
	gSizer1->Add( mExtQuantityItem, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mDiscountAmount = new wxStaticText( mTextOutPut, wxID_ANY, wxT("Discount"), wxDefaultPosition, wxDefaultSize, 0 );
	mDiscountAmount->Wrap( -1 );
	gSizer1->Add( mDiscountAmount, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mDiscountValue = new wxStaticText( mTextOutPut, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mDiscountValue->Wrap( -1 );
	gSizer1->Add( mDiscountValue, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mTotalQuantity = new wxStaticText( mTextOutPut, wxID_ANY, wxT("Total Quantity"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalQuantity->Wrap( -1 );
	gSizer1->Add( mTotalQuantity, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	mTotalQuantityValue = new wxStaticText( mTextOutPut, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	mTotalQuantityValue->Wrap( -1 );
	gSizer1->Add( mTotalQuantityValue, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	mTextOutPut->SetSizer( gSizer1 );
	mTextOutPut->Layout();
	gSizer1->Fit( mTextOutPut );
	bSizer3->Add( mTextOutPut, 1, wxEXPAND | wxALL, 5 );
	
	
	mSaleDisplayPane->SetSizer( bSizer3 );
	mSaleDisplayPane->Layout();
	bSizer3->Fit( mSaleDisplayPane );
	bSizer2->Add( mSaleDisplayPane, 1, wxEXPAND | wxALL, 5 );
	
	mSalePaymentButtonsPane = new wxPanel( mSaleOutputPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	mClear = new wxButton( mSalePaymentButtonsPane, ID_CLEAR, wxT("CLEAR"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( mClear, 0, wxALL, 5 );
	
	mSave = new wxButton( mSalePaymentButtonsPane, ID_SAVE, wxT("SAVE"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( mSave, 0, wxALL, 5 );
	
	mCheckout = new wxButton( mSalePaymentButtonsPane, ID_CHECKOUT, wxT("CHECK OUT"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( mCheckout, 0, wxALL, 5 );
	
	
	mSalePaymentButtonsPane->SetSizer( bSizer4 );
	mSalePaymentButtonsPane->Layout();
	bSizer4->Fit( mSalePaymentButtonsPane );
	bSizer2->Add( mSalePaymentButtonsPane, 0, wxEXPAND | wxALL, 5 );
	
	
	mSaleOutputPane->SetSizer( bSizer2 );
	mSaleOutputPane->Layout();
	bSizer2->Fit( mSaleOutputPane );
	bSizer1->Add( mSaleOutputPane, 0, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();

	SetupDropTarget();
}

pof::SaleView::~SaleView()
{
}

void pof::SaleView::SetupDropTarget()
{
	//allow drops from the dataobject
	auto DataObject = new pof::DataObject("PRODUCTDATA", mDropRow, wxGetApp().mProductManager.GetProductData()->GetDatastore().get_metadata());
	auto DropTarget = new pof::DropTarget(DataObject, std::bind_front(&pof::SaleView::DropData, this));
	SetDropTarget(DropTarget);
}

void pof::SaleView::OnClear(wxCommandEvent& evt)
{
	wxMessageBox("CLEAR", "CLEARING");
}

void pof::SaleView::OnCheckout(wxCommandEvent& evt)
{
	wxMessageBox("CHECKOUT", "CHECKOUT");
}

void pof::SaleView::OnSave(wxCommandEvent& evt)
{
	wxMessageBox("SAVE", "SAVE");
}

void pof::SaleView::OnDropPossible(wxDataViewEvent& evt)
{
	spdlog::info("Drop possible");
}

void pof::SaleView::OnDrop(wxDataViewEvent& evt)
{
	spdlog::info("Dropped");
}

void pof::SaleView::DropData(const pof::DataObject& dat)
{
	auto& meta = dat.GetMeta();
	auto& row = dat.GetSetData();
	if (row.has_value()) {
		auto val = row.value();
		spdlog::info("Droping {}", boost::variant2::get<std::string>(val.first[pof::ProductManager::PRODUCT_NAME]));
	}
	else {
		spdlog::error("Drop data invalid or does not exist");
	}
}
