#include "OrderView.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::OrderListView, wxDialog)
EVT_TOOL(pof::OrderListView::ID_CLEAR_ORDERLIST, pof::OrderListView::OnClearOrderList)
EVT_TOOL(pof::OrderListView::ID_PRINT_ORDER, pof::OrderListView::OnPrintOrder)
EVT_TOOL(pof::OrderListView::ID_ADD_PRODUCT, pof::OrderListView::OnAddProduct)
EVT_TOOL(pof::OrderListView::ID_CLEAR_ORDERED, pof::OrderListView::OnClearOrdered)

EVT_BUTTON(pof::OrderListView::ID_ADD_PRODUCT, pof::OrderListView::OnAddProduct)

EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::OrderListView::ID_ORDER_VIEW, pof::OrderListView::OnContexMenu)
EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::OrderListView::ID_ORDER_VIEW, pof::OrderListView::OnHeaderClick)
EVT_UPDATE_UI(pof::OrderListView::ID_ORDER_VIEW, pof::OrderListView::OnUpdateUI)
EVT_MENU(pof::OrderListView::ID_REMOVE_ORDER, pof::OrderListView::OnRemoveOrder)
EVT_MENU(pof::OrderListView::ID_REORDER, pof::OrderListView::OnReorder)
EVT_MENU(pof::OrderListView::ID_SELECT, pof::OrderListView::OnSelect)
END_EVENT_TABLE()


pof::OrderListView::OrderListView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSize(FromDIP(wxSize(991, 513)));
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	mTopTools = new wxAuiToolBar( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_GRIPPER|wxAUI_TB_HORZ_LAYOUT|wxAUI_TB_HORZ_TEXT|wxAUI_TB_OVERFLOW| wxNO_BORDER ); 
	mTopTools->SetMinSize( FromDIP(wxSize( -1,40 )) );
	mTopTools->AddTool(ID_SELECT, "Select", wxArtProvider::GetBitmap("action_check"), "Show Selections", wxITEM_CHECK);

	mTopTools->AddStretchSpacer();
	mTopTools->AddSeparator();
	mTopTools->AddTool(ID_ADD_PRODUCT, "Add Product", wxArtProvider::GetBitmap("action_add"), "Add product to order list");
	mTopTools->AddTool(ID_CLEAR_ORDERED, "Remove ordered", wxArtProvider::GetBitmap(wxART_REMOVABLE, wxART_TOOLBAR, FromDIP(wxSize(16,16))), "Remove ordered items", wxITEM_NORMAL);
	mTopTools->AddTool(ID_CLEAR_ORDERLIST, wxT("Clear all"), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR, FromDIP(wxSize(16, 16))), "Clear order list");
	m_tool11 = mTopTools->AddTool( ID_PRINT_ORDER, wxT("Print Order"), wxArtProvider::GetBitmap("download"), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	
	mTopTools->Realize(); 
	
	bSizer2->Add( mTopTools, 0, wxEXPAND, FromDIP(5) );
	
	m_panel3 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL |wxNO_BORDER );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer4->AddStretchSpacer();
	
	m_staticText1 = new wxStaticText( m_panel4, wxID_ANY, wxT("Total Orders:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL, FromDIP(5) );
	
	m_staticText2 = new wxStaticText( m_panel4, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALL, FromDIP(5) );
	
	bSizer4->AddSpacer(FromDIP(5));

	bSizer4->Add(new wxStaticLine(m_panel4, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));

	
	m_staticText3 = new wxStaticText( m_panel4, wxID_ANY, wxT("Order Cost: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, FromDIP(5));
	
	m_staticText4 = new wxStaticText(m_panel4, wxID_ANY, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText4->Wrap( -1 );
	bSizer4->Add( m_staticText4, 0, wxALL, FromDIP(5) );
	
	
	m_panel4->SetSizer( bSizer4 );
	m_panel4->Layout();
	bSizer4->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 1, wxEXPAND | wxALL, 0 );
	
	
	m_panel3->SetSizer( bSizer3 );
	m_panel3->Layout();
	bSizer3->Fit( m_panel3 );
	//bSizer2->Add( m_panel3, 0, wxEXPAND | wxALL, 0 );
	
	mBook = new wxSimplebook(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mOrderView = new wxDataViewCtrl(mBook, ID_ORDER_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	mOrderView->AssociateModel(wxGetApp().mProductManager.GetOrderList().get());

	mProductCol = mOrderView->AppendTextColumn(wxT("Product"), pof::ProductManager::ORDER_PRODUCT_NAME, wxDATAVIEW_CELL_INERT, FromDIP(500));
	mQuanCol = mOrderView->AppendTextColumn(wxT("Quantity"), pof::ProductManager::ORDER_QUANTITY, wxDATAVIEW_CELL_EDITABLE, FromDIP(100));
	m_dataViewColumn3 = mOrderView->AppendTextColumn(wxT("Cost"), pof::ProductManager::ORDER_COST, wxDATAVIEW_CELL_INERT, FromDIP(100));
	m_dataViewColumn4 = mOrderView->AppendBitmapColumn(wxT("Ordered"), pof::ProductManager::ORDER_STATE, wxDATAVIEW_CELL_INERT, FromDIP(100));


	CreateEmptyPanel();
	bool empty = wxGetApp().mProductManager.GetOrderList()->GetDatastore().empty();
	mBook->AddPage(mOrderView, wxT("ORDER VIEW"), !empty);
	mBook->AddPage(mEmpty, wxT("EMPTY"), empty);
	mBook->Layout();
	
	bSizer2->Add(mBook, 1, wxEXPAND, 0);
	bSizer2->Add(m_panel3, 0, wxEXPAND | wxALL, FromDIP(2));

	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );

	CreateSpeicalCol();
	UpdateTexts();

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);

	wxGetApp().mPrintManager->printSig.connect(std::bind_front(&pof::OrderListView::OnPrintComplete, this));
}

pof::OrderListView::~OrderListView()
{
	
}

void pof::OrderListView::CreateEmptyPanel()
{
	mEmpty = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmpty, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap("basket", wxART_OTHER, FromDIP(wxSize(64,64))), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No product in order list"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	wxButton* btn = new wxButton(m7, ID_ADD_PRODUCT);
	btn->SetBitmap(wxArtProvider::GetBitmap("action_add", wxART_OTHER, FromDIP(wxSize(16,16))));
	btn->SetLabel("Add product");
	btn->SetBackgroundColour(*wxWHITE);
	bSizer9->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


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


	mEmpty->SetSizer(bSizer6);
	mEmpty->Layout();

}

void pof::OrderListView::CreateSpeicalCol()
{
	pof::DataModel::SpeicalColHandler_t stateCol;
	pof::DataModel::SpeicalColHandler_t quanCol;
	pof::DataModel::SpeicalColHandler_t costCol;
	pof::DataModel::SpeicalColHandler_t SelectColHandler;


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

	SelectColHandler.first = [&](size_t row, size_t col) -> wxVariant {
		auto found = mSelections.find(pof::DataModel::GetItemFromIdx(row));
		return wxVariant((found != mSelections.end()));
	};
	SelectColHandler.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		if (v.GetBool()) {
			auto [iter, inserted] = mSelections.insert(pof::DataModel::GetItemFromIdx(row));
			return inserted;
		}
		else {
			mSelections.erase(pof::DataModel::GetItemFromIdx(row));
			return true;
		}
	};

	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_STATE, std::move(stateCol));
	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_QUANTITY, std::move(quanCol));
	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(pof::ProductManager::ORDER_COST, std::move(costCol));
	wxGetApp().mProductManager.GetOrderList()->SetSpecialColumnHandler(selcol, std::move(SelectColHandler));
}

void pof::OrderListView::OnPrintOrder(wxCommandEvent& evt)
{
	if (wxGetApp().mProductManager.GetOrderList()->GetDatastore().empty()){
		wxMessageBox("Order list is empty", "Order list", wxICON_WARNING | wxOK);
		return;
	}
	wxIcon cop;
	cop.CopyFromBitmap(wxArtProvider::GetBitmap("checkout"));
	wxBusyInfo info
	(
		wxBusyInfoFlags()
		.Parent(this)
		.Icon(cop)
		.Title("Printing order")
		.Text("Please wait...")
		.Foreground(*wxBLACK)
		.Background(*wxWHITE)
		.Transparency(4 * wxALPHA_OPAQUE / 5)
	);

	wxGetApp().mPrintManager->PrintOrderList(this);
}

void pof::OrderListView::OnContexMenu(wxDataViewEvent& evt)
{
	auto item = mOrderView->GetSelection();
	if (!item.IsOk() && mSelections.empty()) return;

 	wxMenu* menu = new wxMenu;
	if (mSelections.empty()) {
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& row = wxGetApp().mProductManager.GetOrderList()->GetDatastore()[idx];
		auto& state = boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::ORDER_STATE]);
		if (state == static_cast<std::uint64_t>(pof::ProductManager::ORDERED)) {
			menu->Append(ID_REORDER, "Reorder product", nullptr);
		}
		auto remv = menu->Append(ID_REMOVE_ORDER, "Remove order");
		remv->SetBackgroundColour(*wxWHITE);
	}
	else {
		menu->Append(ID_REORDER, fmt::format("Reorder {:d} products", mSelections.size()), nullptr);
		auto remv = menu->Append(ID_REMOVE_ORDER, fmt::format("Remove {:d} orders", mSelections.size()));
	}
	//remv->SetBitmap(wxArtProvider::GetBitmap("action_delete"));
	PopupMenu(menu);
}

void pof::OrderListView::OnRemoveOrder(wxCommandEvent& evt)
{
	auto& orderList = wxGetApp().mProductManager.GetOrderList();
	if (mSelections.empty()) {
		if (wxMessageBox("Are you sure you want to remove product from order list ?", "ORDER LIST", wxICON_WARNING | wxYES_NO, this) == wxNO) return;
		auto item = mOrderView->GetSelection();
		if (!item.IsOk()) return;

		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& row = orderList->GetDatastore()[idx];
		auto& uid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::ORDER_PRODUCT_UUID]);
		wxGetApp().mProductManager.RemvFromOrderList(uid);
		orderList->RemoveData(item);
	}
	else {
		if (wxMessageBox(fmt::format("Are you sure you want to remove {:d} products from order list ?", mSelections.size()), "ORDER LIST", wxICON_WARNING | wxYES_NO, this) == wxNO) return;
		wxBusyCursor cursor;
		wxDataViewItemArray items;
		for (auto& item : mSelections) {
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& row = orderList->GetDatastore()[idx];

			auto& uid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::ORDER_PRODUCT_UUID]);
			wxGetApp().mProductManager.RemvFromOrderList(uid);
			items.push_back(item);
		}
		orderList->RemoveData(items);
		mSelections.clear();
	}

	UpdateTexts();
	if (orderList->GetDatastore().empty()) {
		mBook->SetSelection(1);
	}
}

void pof::OrderListView::OnAddProduct(wxCommandEvent& evt)
{
	pof::SearchProduct dialog(this);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto& datastore = wxGetApp().mProductManager.GetOrderList()->GetDatastore();
	wxBusyCursor cursor;
	mOrderView->Freeze();
	if (dialog.HasMultipleSelections())
	{
		auto vec = dialog.GetSelectedProducts();
		wxGetApp().mProductManager.AddMulipleToOrderList(vec);
		//for (auto& p : vec){
		//	auto& row = p.get();
		//	pof::base::data::duuid_t uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
		//	//check if already exisits
		//	if (std::any_of(datastore.begin(), datastore.end(), [&](const pof::base::data::row_t& row) -> bool {
		//		return uuid == boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::ORDER_PRODUCT_UUID]);
		//	})){
		//		//skip
		//		continue;
		//	}
		//	wxGetApp().mProductManager.AddToOrderList(uuid, 1);
		//}
	}
	else {
		auto& row =  dialog.GetSelectedProduct();
		pof::base::data::duuid_t uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
		wxGetApp().mProductManager.AddToOrderList(uuid, 1);
	}

	wxGetApp().mProductManager.LoadOrderList();
	UpdateTexts();
	mOrderView->Thaw();

	mBook->SetSelection(0);
}

void pof::OrderListView::OnClearOrderList(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to clear order list?", "Order list", wxICON_WARNING | wxYES_NO) == wxNO) return;
	wxGetApp().mProductManager.GetOrderList()->Clear();
	wxGetApp().mProductManager.ClearOrderList();

	//set to empty
	UpdateTexts();
	mBook->SetSelection(1);
}

void pof::OrderListView::OnPrintComplete(bool status, size_t printstate)
{
	if (!status) {
		spdlog::error("Order list print failed");
		return;
	}

	switch (printstate)
	{
	case pof::PrintManager::ORDERlIST:
	{
		//mark list as ordered
		auto& datastore = wxGetApp().mProductManager.GetOrderList()->GetDatastore();
		//mark everything as ordered
		wxBusyCursor cursor;
		mOrderView->Freeze();
		for (auto& o : datastore) {
			if (boost::variant2::get<std::uint64_t>(o.first[pof::ProductManager::ORDER_STATE]) ==
				pof::ProductManager::ORDERED) continue;

			o.first[pof::ProductManager::ORDER_STATE] = static_cast<std::uint64_t>(pof::ProductManager::ORDERED);
			wxGetApp().mProductManager.UpdateOrderState(boost::variant2::get<pof::base::data::duuid_t>(o.first[pof::ProductManager::ORDER_PRODUCT_UUID]),
				static_cast<std::uint64_t>(pof::ProductManager::ORDERED));
		}
		mOrderView->Thaw();
	}
		break;
	default:
		break;
	}
}

void pof::OrderListView::OnReorder(wxCommandEvent& evt)
{
	auto item = mOrderView->GetSelection();
	if (!item.IsOk() && mSelections.empty()) return;

	wxBusyCursor cur;
	mOrderView->Freeze();
	if (mSelections.empty()) {
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& row = wxGetApp().mProductManager.GetOrderList()->GetDatastore()[idx];
		auto& state = boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::ORDER_STATE]);
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::ORDER_PRODUCT_UUID]);
		state = pof::ProductManager::PENDING;
		wxGetApp().mProductManager.UpdateOrderState(uuid, pof::ProductManager::PENDING);
	}
	else {
		for (auto& item : mSelections) {
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& row = wxGetApp().mProductManager.GetOrderList()->GetDatastore()[idx];
			auto& state = boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::ORDER_STATE]);
			auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::ORDER_PRODUCT_UUID]);
			state = pof::ProductManager::PENDING;
			wxGetApp().mProductManager.UpdateOrderState(uuid, pof::ProductManager::PENDING);
		}
		mSelections.clear();
	}
	mOrderView->Thaw();
}

void pof::OrderListView::OnSelect(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		ShowSelect();
	}
	else {
		HideSelect();
	}
}

void pof::OrderListView::OnHeaderClick(wxDataViewEvent& evt)
{
	if (mSelectCol == evt.GetDataViewColumn()) {
		static bool sel = true;
		mOrderView->Freeze();
		auto& items = wxGetApp().mProductManager.GetOrderList()->GetDataViewItems();
		if (sel) {
			if (!mSelections.empty()) {
				mSelections.clear();
				sel = false;
				mOrderView->SetSelections({});
			}
			else {
				std::ranges::copy(items, std::inserter(mSelections, mSelections.end()));
			}
		}
		else {
			for (auto& item : items) {
				mSelections.erase(item);
			}
		}
		sel = !sel;
		mOrderView->Thaw();
		evt.Veto();
	}
	else {
		evt.Skip();
	}
}

void pof::OrderListView::OnUpdateUI(wxUpdateUIEvent& evt)
{
	auto id = evt.GetId();
	switch (id)
	{
	case ID_ORDER_VIEW:
	{
		static size_t count = 0;
		if (!mSelections.empty() && count != mSelections.size()) {
			wxDataViewItemArray arr;
			arr.reserve(mSelections.size());
			for (auto& sel : mSelections) {
				arr.push_back(sel);
			}
			mOrderView->SetSelections(arr);
		}
	}
	default:
		break;
	}
}

void pof::OrderListView::OnClearOrdered(wxCommandEvent& evt)
{
	auto& datastore = wxGetApp().mProductManager.GetOrderList()->GetDatastore();
	if (datastore.empty()) {
		wxMessageBox("Order list is empty", "Order list", wxICON_INFORMATION | wxOK);
		return;
	}
	wxBusyCursor cor;
	wxDataViewItemArray items;
	size_t idx = 0;
	for (auto& row : datastore) {
		auto item = pof::DataModel::GetItemFromIdx(idx);
		auto& state = boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::ORDER_STATE]);
		if (state == pof::ProductManager::ORDERED) {
			auto& uid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::ORDER_PRODUCT_UUID]);
			wxGetApp().mProductManager.RemvFromOrderList(uid);
			items.push_back(item);
		}
		idx++;
	}
	if (!items.empty()) {
		wxGetApp().mProductManager.GetOrderList()->RemoveData(items);
	}

	UpdateTexts();
	if (datastore.empty()) {
		mBook->SetSelection(1);
	}
}

void pof::OrderListView::ShowSelect()
{
	mSelectCol = mOrderView->PrependToggleColumn(wxT("Select"), selcol,
		wxDATAVIEW_CELL_ACTIVATABLE, FromDIP(50));
}

void pof::OrderListView::HideSelect()
{
	if (mSelectCol != nullptr) {
		mSelections.clear();

		mOrderView->Freeze();
		mOrderView->DeleteColumn(mSelectCol);
		mOrderView->Thaw();
		mSelectCol = nullptr;
	}
}

void pof::OrderListView::UpdateTexts()
{
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
