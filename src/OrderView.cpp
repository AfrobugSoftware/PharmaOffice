#include "OrderView.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::OrderListView, wxDialog)
EVT_TOOL(pof::OrderListView::ID_PRINT_ORDER, pof::OrderListView::OnPrintOrder)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::OrderListView::ID_ORDER_VIEW, pof::OrderListView::OnContexMenu)
EVT_MENU(pof::OrderListView::ID_REMOVE_ORDER, pof::OrderListView::OnRemoveOrder)
END_EVENT_TABLE()


pof::OrderListView::OrderListView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	mTopTools = new wxAuiToolBar( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_GRIPPER|wxAUI_TB_HORZ_LAYOUT|wxAUI_TB_HORZ_TEXT|wxAUI_TB_OVERFLOW| wxNO_BORDER ); 
	mTopTools->SetMinSize( wxSize( -1,40 ) );
	mTopTools->AddStretchSpacer();
	m_tool11 = mTopTools->AddTool( wxID_ANY, wxT("Print Order"), wxArtProvider::GetBitmap("download"), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	
	mTopTools->Realize(); 
	
	bSizer2->Add( mTopTools, 0, wxEXPAND, 5 );
	
	m_panel3 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL |wxNO_BORDER );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer4->AddStretchSpacer();
	
	m_staticText1 = new wxStaticText( m_panel4, wxID_ANY, wxT("Total Orders:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->SetFont(wxFont(wxFontInfo(10).Bold()));
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_panel4, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->SetFont(wxFont(wxFontInfo(10)));
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALL, 5 );
	
	
	
	m_staticText3 = new wxStaticText( m_panel4, wxID_ANY, wxT("Order Cost: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->SetFont(wxFont(wxFontInfo(10).Bold()));
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText(m_panel4, wxID_ANY, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->SetFont(wxFont(wxFontInfo(10)));
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
	
	mBook = new wxSimplebook(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mOrderView = new wxDataViewCtrl(mBook, ID_ORDER_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	mOrderView->AssociateModel(wxGetApp().mProductManager.GetOrderList().get());

	mProductCol = mOrderView->AppendTextColumn(wxT("Product"), pof::ProductManager::ORDER_PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 500);
	mQuanCol = mOrderView->AppendTextColumn(wxT("Quantity"), pof::ProductManager::ORDER_QUANTITY, wxDATAVIEW_CELL_EDITABLE, 100);
	m_dataViewColumn3 = mOrderView->AppendTextColumn(wxT("Cost"), pof::ProductManager::ORDER_COST, wxDATAVIEW_CELL_INERT, 100);
	m_dataViewColumn4 = mOrderView->AppendBitmapColumn(wxT("Ordered"), pof::ProductManager::ORDER_STATE, wxDATAVIEW_CELL_INERT, 100);


	CreateEmptyPanel();
	bool empty = wxGetApp().mProductManager.GetOrderList()->GetDatastore().empty();
	mBook->AddPage(mOrderView, wxT("ORDER VIEW"), !empty);
	mBook->AddPage(mEmpty, wxT("EMPTY"), empty);
	mBook->Layout();
	bSizer2->Add(mBook, 1, wxEXPAND, 0);


	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );

	CreateSpeicalCol();
	UpdateTexts();
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

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("Order list is empty"), wxDefaultPosition, wxDefaultSize, 0);
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
	bSizer6->Add(m5, 1, wxEXPAND | wxALL, 5);


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
	pof::DataModel::SpeicalColHandler_t costCol;

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
		wxBusyCursor cusor;
		std::uint64_t quan = static_cast<std::uint64_t>(value.GetInteger());
		auto& rowid = datastore[row].first[pof::ProductManager::ORDER_PRODUCT_UUID];
		auto& rowQuan = datastore[row].first[pof::ProductManager::ORDER_QUANTITY];
		auto& cost = datastore[row].first[pof::ProductManager::ORDER_COST];
		rowQuan = quan;
		wxGetApp().mProductManager.UpdateOrderList(boost::variant2::get<pof::base::data::duuid_t>(rowid),quan);
		UpdateTexts();
		return true;
	};

	costCol.first = [&](size_t row, size_t col) -> wxVariant {
	   auto& cost = datastore[row].first[pof::ProductManager::ORDER_COST];
	   auto& rowQuan = datastore[row].first[pof::ProductManager::ORDER_QUANTITY];
	   auto show = boost::variant2::get<pof::base::data::currency_t>(cost) * 
			static_cast<double>(boost::variant2::get<std::uint64_t>(rowQuan));
	   return wxVariant(fmt::format("{:cu}", show));

	};

	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_STATE, std::move(stateCol));
	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_QUANTITY, std::move(quanCol));
	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_COST, std::move(costCol));
}

void pof::OrderListView::OnPrintOrder(wxCommandEvent& evt)
{
}

void pof::OrderListView::OnContexMenu(wxDataViewEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto remv = menu->Append(ID_REMOVE_ORDER, "Remove Order");
	remv->SetBackgroundColour(*wxWHITE);
	remv->SetBitmap(wxArtProvider::GetBitmap("action_delete"));
	PopupMenu(menu);
}

void pof::OrderListView::OnRemoveOrder(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to remove product from order list ?", "ORDER LIST", wxICON_WARNING | wxYES_NO, this) == wxNO) return;
	auto item = mOrderView->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& orderList = wxGetApp().mProductManager.GetOrderList();
	auto& row = orderList->GetDatastore()[idx];
	auto& uid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::ORDER_PRODUCT_UUID]);
	wxGetApp().mProductManager.RemvFromOrderList(uid);
	orderList->RemoveData(item);
	UpdateTexts();
}

void pof::OrderListView::UpdateTexts()
{
	size_t count = 0;
	pof::base::currency cur;
	auto& datastore = wxGetApp().mProductManager.GetOrderList()->GetDatastore();
	cur = std::accumulate(datastore.begin(), datastore.end(), cur, [&](const pof::base::currency&, const pof::base::data::row_t& row) {
		auto& orderAmount = boost::variant2::get<pof::base::currency>(row.first[pof::ProductManager::ORDER_COST]);
		auto& rowQuan = boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::ORDER_QUANTITY]);

		return(cur = cur + (orderAmount * static_cast<double>(rowQuan)));
	});

	m_panel4->Freeze();
	m_staticText2->SetLabel(fmt::format("{:d}", datastore.size()));
	m_staticText4->SetLabel(fmt::format("{:cu}", cur));

	m_panel4->Layout();
	m_panel4->Thaw();
	m_panel4->Refresh();
}
