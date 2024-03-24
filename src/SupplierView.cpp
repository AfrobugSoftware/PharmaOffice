#include "PofPch.h"
#include "SupplierView.h"
BEGIN_EVENT_TABLE(pof::SupplierView, wxPanel)
	EVT_TOOL(pof::SupplierView::ID_BACK, pof::SupplierView::OnBack)
	EVT_TOOL(pof::SupplierView::ID_ADD_SUPPLIER, pof::SupplierView::OnCreateSupplier)
	EVT_TOOL(pof::SupplierView::ID_REMV_SUPPLIER, pof::SupplierView::OnRemoveSupplier)

	EVT_MENU(pof::SupplierView::ID_REMOVE_INVOICE, pof::SupplierView::OnRemoveInvoice)
	EVT_MENU(pof::SupplierView::ID_COPY_INVOICE_NAME, pof::SupplierView::OnCopyInvoice)
	EVT_MENU(pof::SupplierView::ID_REMOVE_PRODUCT_IN_INVOICE, pof::SupplierView::OnRemoveProductInInvoice)
	EVT_MENU(pof::SupplierView::ID_COPY_SUPPLIER_NAME, pof::SupplierView::OnCopySupplierName)

	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::SupplierView::ID_SUPPLIER_VIEW, pof::SupplierView::OnContextMenu)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::SupplierView::ID_INVOICE_VIEW, pof::SupplierView::OnInvoiceContextMenu)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::SupplierView::ID_INVOICE_PRODUCT_VIEW, pof::SupplierView::OnContextMenu)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::SupplierView::ID_SUPPLIER_VIEW, pof::SupplierView::OnSupplierActivated)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::SupplierView::ID_INVOICE_VIEW, pof::SupplierView::OnInvoiceActivated)

	EVT_SEARCH(pof::SupplierView::ID_SEARCH, pof::SupplierView::OnSearch)
	EVT_SEARCH_CANCEL(pof::SupplierView::ID_SEARCH, pof::SupplierView::OnSearchCleared)
	EVT_TEXT(pof::SupplierView::ID_SEARCH, pof::SupplierView::OnSearch)

END_EVENT_TABLE()


pof::SupplierView::SupplierView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPanel(parent, id, pos, size, style) {
	mManager.SetManagedWindow(this);
	mManager.SetFlags(AUIMGRSTYLE);
	SetSizeHints(wxDefaultSize, wxDefaultSize);
	SetBackgroundColour(*wxWHITE);
	SetDoubleBuffered(true);

	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::SupplierView::OnAuiThemeChange, this));

	mBook = new wxSimplebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	CreateToolbar();
	CreateViews();
	CreateEmptyPanel();

	CheckEmpty(SUPPLIER_VIEW);
	page = SUPPLIER_VIEW;
	mManager.AddPane(mBook, wxAuiPaneInfo().Name("Book").CenterPane());
	mManager.Update();
}

pof::SupplierView::~SupplierView()
{
	//this might cause issues
	//if (mInvoiceProductModel) delete mInvoiceProductModel;
}

void pof::SupplierView::CreateToolbar()
{
	mTools = new wxAuiToolBar(this, ID_TOOLS, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTools->SetToolBitmapSize(FromDIP(wxSize(16, 16)));

	mTools->AddSpacer(5);
	mSupplierSearch = new wxSearchCtrl(mTools, ID_SEARCH, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(400, -1)), wxWANTS_CHARS);
	mTools->AddControl(mSupplierSearch);

	mTools->AddStretchSpacer();
	mTools->AddSeparator();
	mTools->AddTool(ID_ADD_SUPPLIER, "Create supplier", wxArtProvider::GetBitmap("action_add"), "Create supplier");
	mTools->AddSpacer(FromDIP(5));
	mTools->Realize();
	mManager.AddPane(mTools, wxAuiPaneInfo().Name("Tools").Top().MinSize(FromDIP(wxSize(-1, 30))).PaneBorder(false).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));

	mInvoiceTools = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mInvoiceTools->SetToolBitmapSize(FromDIP(wxSize(16, 16)));
	mInvoiceTools->AddTool(ID_BACK, "Back", wxArtProvider::GetBitmap("arrow_back"), "Back to patients");

	mSupplierName =  new wxStaticText(mInvoiceTools, wxID_ANY, "TEST", wxDefaultPosition, wxDefaultSize, 0);
	mSupplierName->SetFont(wxFontInfo().AntiAliased().Bold());
	mSupplierName->SetBackgroundColour(*wxWHITE);

	mInvoiceTools->AddSeparator();
	mInvoiceTools->AddSpacer(FromDIP(5));
	mSupplierNameItem = mInvoiceTools->AddControl(mSupplierName);
	mInvoiceTools->Realize();

	mManager.AddPane(mInvoiceTools, wxAuiPaneInfo().Name("InvoiceTools").Top().MinSize(FromDIP(wxSize(-1, 30))).PaneBorder(false).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());
}

void pof::SupplierView::CreateViews()
{
	//supplier view
	mView = new wxDataViewCtrl(mBook, ID_SUPPLIER_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mView->AssociateModel(wxGetApp().mProductManager.GetSupplier().get());

	mView->AppendTextColumn("Supplier Name", pof::ProductManager::SUPPLIER_NAME, wxDATAVIEW_CELL_INERT, FromDIP(450), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mView->AppendTextColumn("Date created", pof::ProductManager::SUPPLIER_DATE_CREATED, wxDATAVIEW_CELL_INERT, FromDIP(250), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mView->AppendTextColumn("Date modified", pof::ProductManager::SUPPLIER_DATE_MODIFIED, wxDATAVIEW_CELL_INERT, FromDIP(250), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);

	mBook->AddPage(mView, "View", false);

	mInvoiceView = new wxDataViewCtrl(mBook, ID_INVOICE_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mInvoiceView->AssociateModel(wxGetApp().mProductManager.GetInvoices().get());
	mInvoiceView->AppendTextColumn("Invoices", pof::ProductManager::INVOICE_ID, wxDATAVIEW_CELL_INERT, FromDIP(250), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);


	mBook->AddPage(mInvoiceView, "Invoice", false);

	wxPanel* panel = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	mInvoiceProductView = new wxDataViewCtrl(panel, ID_INVOICE_PRODUCT_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mInvoiceProductModel = new pof::DataModel();
	mInvoiceProductModel->Adapt<
		pof::base::data::text_t,
		std::uint64_t,
		pof::base::currency,
		pof::base::data::datetime_t
	>();
	mInvoiceProductView->AssociateModel(mInvoiceProductModel);
	mInvoiceProductModel->DecRef();

	mInvoiceProductView->AppendTextColumn("Product", 0, wxDATAVIEW_CELL_INERT, FromDIP(250), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mInvoiceProductView->AppendTextColumn("Stock entry", 1, wxDATAVIEW_CELL_INERT, FromDIP(250), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mInvoiceProductView->AppendTextColumn("Cost", 2, wxDATAVIEW_CELL_INERT, FromDIP(250), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mInvoiceProductView->AppendTextColumn("Entry date", 3, wxDATAVIEW_CELL_INERT, FromDIP(250), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);

	mCSPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	bSizer4->AddStretchSpacer();

	mTotalStock = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalStock->SetFont(wxFont(wxFontInfo(12).Bold().AntiAliased()));
	mTotalStock->Wrap(-1);
	bSizer4->Add(mTotalStock, 0, wxALL, FromDIP(5));

	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(mCSPanel, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));

	mTotalAmount = new wxStaticText(mCSPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmount->SetFont(wxFont(wxFontInfo(12).Bold().AntiAliased()));
	mTotalAmount->Wrap(-1);
	bSizer4->Add(mTotalAmount, 0, wxALL, FromDIP(5));

	mCSPanel->SetSizer(bSizer4);
	mCSPanel->Layout();
	bSizer4->Fit(mCSPanel);

	sz->Add(mInvoiceProductView, 1, wxEXPAND | wxALL, FromDIP(0));
	sz->Add(mCSPanel, 0, wxEXPAND | wxALL, FromDIP(0));
	sz->AddSpacer(5);

	panel->SetSizer(sz);
	sz->SetSizeHints(panel);
	panel->Layout();
	mBook->AddPage(panel, "Products", false);
}

void pof::SupplierView::LoadSuppliers()
{
	wxGetApp().mProductManager.LoadSuplliers();
	CheckEmpty(SUPPLIER_VIEW);
}

void pof::SupplierView::LoadInvoices(std::uint64_t supid)
{
	wxGetApp().mProductManager.LoadInvoices(supid);
}

void pof::SupplierView::LoadInvoiceProducts(std::uint64_t sid, const std::string& in)
{
	auto rel = wxGetApp().mProductManager.GetProductsInInvoice(sid, in);
	if (!rel.has_value()) {
		wxMessageBox("Error in getting product for invoice, contact admin", "Critical error", wxICON_ERROR | wxOK);
		return;
	}
	mInvoiceProductModel->Clear();
	auto& datastore = mInvoiceProductModel->GetDatastore();
	for (auto&& tup : rel.value())
	{
		auto v = pof::base::make_row_from_tuple(std::move(tup));
		datastore.emplace(std::move(v));
	}
	mInvoiceProductModel->Reload();
	UpdateTotals();
}

wxPanel* pof::SupplierView::CreateEmptyPanel(const std::string& text)
{
	mEmpty = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmpty, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap("supplement-bottle"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	mEmptyStr = new wxStaticText(m7, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
	mEmptyStr->Wrap(-1);
	bSizer9->Add(mEmptyStr, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

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

	mBook->AddPage(mEmpty, "Empty", false);

	return mEmpty;
}

void pof::SupplierView::CheckEmpty(int viewIdx)
{
	switch (viewIdx)
	{
	case SUPPLIER_VIEW:
		if (wxGetApp().mProductManager.GetSupplier()->GetDatastore().empty()) {
			ShowEmpty("No supplier in store");
		}
		else {
			mBook->SetSelection(SUPPLIER_VIEW);
		}
		break;
	case INVOICE_VIEW:
		if (wxGetApp().mProductManager.GetInvoices()->GetDatastore().empty()){
			ShowEmpty("There are no invoices associated with supplier");
		}
		else {
			mBook->SetSelection(INVOICE_VIEW);
		}
		break;
	case INVOICE_PRODUCT_VIEW:
		if (mInvoiceProductModel->GetDatastore().empty()){
			ShowEmpty("There are no product in invoice");
		}
		else {
			mBook->SetSelection(INVOICE_PRODUCT_VIEW);
		}
		break;
	default:
		break;
	}

	SwitchTool(viewIdx);
}

void pof::SupplierView::ShowEmpty(const std::string& text)
{
	mEmpty->Freeze();
	mEmptyStr->SetLabelText(text);
	mEmpty->Layout();
	mEmpty->Thaw();

	mBook->SetSelection(EMPTY);
}

void pof::SupplierView::SwitchTool(int viewIdx)
{
	mManager.GetPane("Tools").Hide();
	mManager.GetPane("InvoiceTools").Hide();

	switch (viewIdx)
	{
	case SUPPLIER_VIEW:
		mManager.GetPane("Tools").Show();
		break;
	case INVOICE_PRODUCT_VIEW:
	case INVOICE_VIEW:
		mManager.GetPane("InvoiceTools").Show();
		break;
	default:
		break;
	}
	mManager.Update();
}

void pof::SupplierView::UpdateTotals()
{
	auto& datastore = mInvoiceProductModel->GetDatastore();
	std::uint64_t totalStock = 0;
	pof::base::currency totalAmount;

	for (auto& i : datastore){
		totalStock += boost::variant2::get<std::uint64_t>(i.first[1]);
		totalAmount += boost::variant2::get<pof::base::currency>(i.first[2]);
	}

	mCSPanel->Freeze();
	mTotalStock->SetLabelText(fmt::format("Total Quantity:   {:d}", totalStock));
	mTotalAmount->SetLabelText(fmt::format("Total Amount:   {:cu}", totalAmount));
	mCSPanel->Layout();
	mCSPanel->Thaw();
}

void pof::SupplierView::OnAuiThemeChange()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::SupplierView::ShowTitle(const std::string& text)
{
	mInvoiceTools->Freeze();
	mSupplierName->SetLabelText(text);
	mSupplierNameItem->SetMinSize(mSupplierName->GetSize());
	mInvoiceTools->Realize();
	mInvoiceTools->Thaw();

	mManager.Update();
}

void pof::SupplierView::OnChangeFont(const wxFont& font)
{
	mView->SetFont(font);
	mInvoiceView->SetFont(font);
	mInvoiceProductView->SetFont(font);
	mEmptyStr->SetFont(font);
}

void pof::SupplierView::OnSupplierActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& row = wxGetApp().mProductManager.GetSupplier()->GetDatastore()[idx];
	
	mCurSupplier = boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::SUPPLIER_ID]);
	LoadInvoices(mCurSupplier);
	SwitchTool(INVOICE_VIEW);
	CheckEmpty(INVOICE_VIEW);

	ShowTitle(fmt::format("Supplier - {}", boost::variant2::get<std::string>(row.first[pof::ProductManager::SUPPLIER_NAME])));
	page = INVOICE_VIEW;
}

void pof::SupplierView::OnInvoiceActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	auto supitem = mView->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& row = wxGetApp().mProductManager.GetInvoices()->GetDatastore()[idx];

	size_t sidx = pof::DataModel::GetIdxFromItem(supitem);
	auto& srow = wxGetApp().mProductManager.GetSupplier()->GetDatastore()[sidx];
	std::string& invoice_id = boost::variant2::get<std::string>(row.first[pof::ProductManager::INVOICE_ID]);
	
	ShowTitle(fmt::format("Supplier - {} - {}", boost::variant2::get<std::string>(srow.first[pof::ProductManager::SUPPLIER_NAME]), invoice_id));
	LoadInvoiceProducts(mCurSupplier, invoice_id);
	mCurInvoice = invoice_id;
	CheckEmpty(INVOICE_PRODUCT_VIEW);
	page = INVOICE_PRODUCT_VIEW;
}

void pof::SupplierView::OnBack(wxCommandEvent& evt)
{
	switch (page)
	{
	case SUPPLIER_VIEW:
		break;
	case INVOICE_VIEW:
		SwitchTool(SUPPLIER_VIEW);
		CheckEmpty(SUPPLIER_VIEW);
		page = SUPPLIER_VIEW;
		break;
	case INVOICE_PRODUCT_VIEW:
	{
		SwitchTool(INVOICE_VIEW);
		CheckEmpty(INVOICE_VIEW);

		auto item = mView->GetSelection();
		if (item.IsOk()) {
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& row = wxGetApp().mProductManager.GetSupplier()->GetDatastore()[idx];
			
			ShowTitle(fmt::format("Supplier - {}", boost::variant2::get<std::string>(row.first[pof::ProductManager::SUPPLIER_NAME])));
		}

		page = INVOICE_VIEW;
		mCurInvoice.clear();
	}
		break;
	default:
		break;
	}
}

void pof::SupplierView::OnCreateSupplier(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Supplier", wxICON_INFORMATION | wxOK);
		return;
	}

	auto str = wxGetTextFromUser("Please enter the name of the supplier", "Supplier").ToStdString();
	if (str.empty()) return;

	pof::base::data::row_t row;
	auto& v = row.first;
	v.resize(pof::ProductManager::SUPPLIER_MAX);
	auto dt = pof::base::data::clock_t::now();

	v[pof::ProductManager::SUPPLIER_ID] = pof::GenRandomId();
	v[pof::ProductManager::SUPPLIER_NAME] = str;
	v[pof::ProductManager::SUPPLIER_DATE_CREATED] = dt;
	v[pof::ProductManager::SUPPLIER_DATE_MODIFIED] = dt;
	v[pof::ProductManager::SUPPLIER_INFO] = ""s;

	wxGetApp().mProductManager.GetSupplier()->StoreData(std::move(row));
	wxMessageBox("Successfully added supplier!", "Supplier", wxICON_INFORMATION | wxOK);
	CheckEmpty(SUPPLIER_VIEW);
}

void pof::SupplierView::OnRemoveSupplier(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Supplier", wxICON_INFORMATION | wxOK);
		return;
	}
	if (wxMessageBox("Are you sure you want to remove supplier?\nThis wouold remove all invoices associtated with the supplier", "Supplier", wxICON_WARNING | wxYES_NO) == wxNO) return;

	auto item = mView->GetSelection();
	if (!item.IsOk()) return;
	
	wxBusyCursor cursor;
	wxGetApp().mProductManager.GetSupplier()->RemoveData(item);
	wxMessageBox("Removed supplier from store!", "Supplier", wxICON_INFORMATION | wxOK);
	CheckEmpty(SUPPLIER_VIEW);
}

void pof::SupplierView::OnRemoveInvoice(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Invoice", wxICON_INFORMATION | wxOK);
		return;
	}

	if (wxMessageBox("Are you sure you want to remove invoice?", "Invoice", wxICON_WARNING | wxYES_NO) == wxNO) return;
	auto item = mInvoiceView->GetSelection();
	if (!item.IsOk()) return;

	wxGetApp().mProductManager.GetInvoices()->RemoveData(item);
	CheckEmpty(INVOICE_VIEW);
}

void pof::SupplierView::OnRemoveProductInInvoice(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Invoice", wxICON_INFORMATION | wxOK);
		return;
	}

	if (wxMessageBox("Are you sure you want to remove product from invoice?", "Invoice", wxICON_WARNING | wxYES_NO) == wxNO) return;
	auto item = mInvoiceProductView->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& row = mInvoiceProductModel->GetDatastore()[idx];
	const auto& uid = boost::variant2::get<pof::base::data::duuid_t>(row.first[4]);
	wxGetApp().mProductManager.RemoveProductFromInvoice(uid, mCurSupplier, mCurInvoice);

	LoadInvoiceProducts(mCurSupplier, mCurInvoice);
	CheckEmpty(INVOICE_PRODUCT_VIEW);
}

void pof::SupplierView::OnContextMenu(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	wxWindowID id = evt.GetId();
	if (!item.IsOk()) return;
	
	wxMenu* menu = new wxMenu;
	switch (id)
	{
	case ID_SUPPLIER_VIEW:
		menu->Append(ID_COPY_SUPPLIER_NAME, "Copy supplier name", nullptr);
		menu->AppendSeparator();
		menu->Append(ID_REMV_SUPPLIER, "Remove supplier", nullptr);
		break;
	case ID_INVOICE_VIEW:
		break;
	case ID_INVOICE_PRODUCT_VIEW:
		menu->Append(ID_REMOVE_PRODUCT_IN_INVOICE, "Remove from invoice", nullptr);
		break;
	default:
		break;
	}
	mView->PopupMenu(menu);
}

void pof::SupplierView::OnInvoiceContextMenu(wxDataViewEvent& evt)
{
	auto item = mInvoiceView->GetSelection();
	if (!item.IsOk()) return;
	wxMenu* menu = new wxMenu;
	menu->Append(ID_COPY_INVOICE_NAME, "Copy invoice name", nullptr);
	menu->AppendSeparator();
	menu->Append(ID_REMOVE_INVOICE, "Remove invoice", nullptr);

	mInvoiceView->PopupMenu(menu);
}

void pof::SupplierView::OnSearch(wxCommandEvent& evt)
{
	auto str = evt.GetString().ToStdString();
	auto& datastore = wxGetApp().mProductManager.GetSupplier();
	if (datastore->GetDatastore().empty()) return;
	bool empty = false;
	mView->Freeze();
	if (str.empty()) datastore->Reload();
	else {
		empty = datastore->StringSearchAndReload(pof::ProductManager::SUPPLIER_NAME, str);
	}
	
	if (empty) {
		ShowEmpty(fmt::format("No supplier name {} in store", str));
	}
	else {
		mBook->SetSelection(SUPPLIER_VIEW);
	}
	mView->Thaw();

}

void pof::SupplierView::OnSearchCleared(wxCommandEvent& evt)
{
	mView->Freeze();
	wxGetApp().mProductManager.GetSupplier()->Reload();
	mView->Thaw();

	CheckEmpty(SUPPLIER_VIEW);
}

void pof::SupplierView::OnCopyInvoice(wxCommandEvent& evt)
{
	auto item = mInvoiceView->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& row = wxGetApp().mProductManager.GetInvoices()->GetDatastore()[idx];

	wxClipboardLocker clipLocker;
	if (!clipLocker) {
		spdlog::error("Can't open the clipboard");
		return;
	}

	auto str = boost::variant2::get<std::string>(row.first[pof::ProductManager::INVOICE_ID]);
	wxTextDataObject* tObj = new wxTextDataObject(std::move(str));
	wxTheClipboard->Clear();
	wxTheClipboard->AddData(tObj);
}

void pof::SupplierView::OnCopySupplierName(wxCommandEvent& evt)
{
	auto item = mView->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	wxClipboardLocker clipLocker;
	if (!clipLocker) {
		spdlog::error("Can't open the clipboard");
		return;
	}

	auto& r = wxGetApp().mProductManager.GetSupplier()->GetDatastore()[idx];
	auto str = boost::variant2::get<std::string>(r.first[pof::ProductManager::SUPPLIER_NAME]);
	wxTextDataObject* tObj = new wxTextDataObject(std::move(str));
	wxTheClipboard->Clear();
	wxTheClipboard->AddData(tObj);
}
