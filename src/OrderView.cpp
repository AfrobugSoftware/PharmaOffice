#include "OrderView.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::OrderListView, wxDialog)
END_EVENT_TABLE()


pof::OrderListView::OrderListView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	mTopTools = new wxAuiToolBar( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_GRIPPER|wxAUI_TB_HORZ_LAYOUT|wxAUI_TB_HORZ_TEXT|wxAUI_TB_OVERFLOW|wxSTATIC_BORDER ); 
	mTopTools->SetMinSize( wxSize( -1,40 ) );
	
	m_tool11 = mTopTools->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	mTopTools->Realize(); 
	
	bSizer2->Add( mTopTools, 0, wxEXPAND, 5 );
	
	mBook = new wxSimplebook( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	
	mOrderView = new wxDataViewCtrl( mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	mOrderView->AssociateModel(wxGetApp().mProductManager.GetOrderList().get());
	
	mProductCol = mOrderView->AppendTextColumn( wxT("Product"), pof::ProductManager::ORDER_PRODUCT_NAME, wxDATAVIEW_CELL_INERT,  200);
	mQuanCol = mOrderView->AppendTextColumn( wxT("Quantity"), pof::ProductManager::ORDER_QUANTITY, wxDATAVIEW_CELL_EDITABLE, 100 );
	m_dataViewColumn3 = mOrderView->AppendTextColumn(wxT("Cost"), pof::ProductManager::ORDER_COST, wxDATAVIEW_CELL_INERT, 100);
	m_dataViewColumn4 = mOrderView->AppendBitmapColumn( wxT("Ordered"), pof::ProductManager::ORDER_STATE, wxDATAVIEW_CELL_INERT, 100);
	

	CreateEmptyPanel();
	mBook->AddPage(mOrderView, wxT("ORDER VIEW"), true);
	mBook->AddPage(mEmpty, wxT("EMPTY"), false);
	mBook->Layout();
	bSizer2->Add( mBook, 1, wxEXPAND, 0 );
	
	m_panel3 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( m_panel4, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_panel4, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALL, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_panel4, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_panel4, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer4->Add( m_staticText4, 0, wxALL, 5 );
	
	
	m_panel4->SetSizer( bSizer4 );
	m_panel4->Layout();
	bSizer4->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 1, wxEXPAND | wxALL, 0 );
	
	
	m_panel3->SetSizer( bSizer3 );
	m_panel3->Layout();
	bSizer3->Fit( m_panel3 );
	bSizer2->Add( m_panel3, 0, wxEXPAND | wxALL, 0 );
	
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

pof::OrderListView::~OrderListView()
{
	
}

void pof::OrderListView::CreateEmptyPanel()
{
	mEmpty = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m4 = new wxPanel(mEmpty, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmpty, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("Pack is empty"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);


	m7->SetSizer(bSizer9);
	m7->Layout();
	bSizer9->Fit(m7);
	bSizer8->Add(m7, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);


	m5->SetSizer(bSizer8);
	m5->Layout();
	bSizer8->Fit(m5);
	bSizer7->Add(m5, 1, wxEXPAND | wxALL, 5);


	m4->SetSizer(bSizer7);
	m4->Layout();
	bSizer7->Fit(m4);
	bSizer6->Add(m4, 1, wxEXPAND | wxALL, 0);


	mEmpty->SetSizer(bSizer6);
	mEmpty->Layout();
}

void pof::OrderListView::CreateSpeicalCol()
{
	pof::DataModel::SpeicalColHandler_t stateCol;
	pof::DataModel::SpeicalColHandler_t quanCol;
	auto& datastore = wxGetApp().mProductManager.GetOrderList()->GetDatastore();
	stateCol.first = [&](size_t row, size_t col) -> wxVariant
	{
		auto& datum = datastore[row].first[pof::ProductManager::ORDER_STATE];
		std::uint64_t state = boost::variant2::get<std::uint64_t>(datum);
		switch (state)
		{
		case pof::ProductManager::PENDING:
			return wxVariant(wxArtProvider::GetBitmap("action_remove"));
		case pof::ProductManager::ORDERED:
			return wxVariant(wxArtProvider::GetBitmap("action_check"));
		default:
			return wxVariant(wxArtProvider::GetBitmap("action_delete"));
			break;
		}
	};

	quanCol.second = [&](size_t row, size_t col, const wxVariant& value) -> bool {


	};


	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_STATE, std::move(stateCol));
	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_QUANTITY, std::move(quanCol));

}
