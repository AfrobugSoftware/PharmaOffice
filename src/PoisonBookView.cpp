#include "PoisonBookView.h"
#include "PofPch.h"
BEGIN_EVENT_TABLE(pof::PoisonBookView, wxPanel)
	EVT_LIST_ITEM_ACTIVATED(pof::PoisonBookView::ID_BOOKLIST, pof::PoisonBookView::OnBookActivated)
	
	EVT_TOOL(pof::PoisonBookView::ID_ADD_PRODUCT, pof::PoisonBookView::OnAddProduct)
	EVT_TOOL(wxID_BACKWARD, pof::PoisonBookView::OnBack)
	EVT_BUTTON(pof::PoisonBookView::ID_ADD_PRODUCT, pof::PoisonBookView::OnAddProduct)

	EVT_DATE_CHANGED(pof::PoisonBookView::ID_DATE_PICKER, pof::PoisonBookView::OnDateChanged)

	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::PoisonBookView::ID_BOOKDATA, pof::PoisonBookView::OnContextMenu)

	EVT_MENU(pof::PoisonBookView::ID_VERIFY, pof::PoisonBookView::OnVerify)

	EVT_SEARCH(pof::PoisonBookView::ID_SEARCH, pof::PoisonBookView::OnSearch)
	EVT_TEXT(pof::PoisonBookView::ID_SEARCH, pof::PoisonBookView::OnSearch)
	EVT_SEARCH_CANCEL(pof::PoisonBookView::ID_SEARCH, pof::PoisonBookView::OnSearchCleared)
END_EVENT_TABLE()


pof::PoisonBookView::PoisonBookView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxPanel(parent, id, pos, size, style), mClearTimer{ this, ID_CLEAR_TIMER }
{
	mManager.SetManagedWindow(this);
	mManager.SetFlags(AUIMGRSTYLE);
	SetSizeHints(wxDefaultSize, wxDefaultSize);
	SetBackgroundColour(*wxWHITE);
	SetDoubleBuffered(true);

	mBook = new wxSimplebook(this, ID_BOOK, wxDefaultPosition, wxDefaultSize, 0);
	CreateToolBars();
	CreateBookToolBar();
	CreateEmptyBookPane();
	CreateViews();
	CreateDataView();
	CreateSpecialCols();


	mManager.AddPane(mBook, wxAuiPaneInfo().Name("Book").CenterPane().CaptionVisible(false).Show());
	wxGetApp().mPoisonBookManager.GetBook()->ConnectSlot([&](pof::base::data::const_iterator iter) {
		LoadBooks();
	}, pof::DataModel::Signals::STORE_LOAD);
	LoadBooks(); //do not think I would leave this here
	if(!mBookList->IsEmpty())
		mBook->SetSelection(THUMBNAIL_SELECT);


	SetupAuiTheme();
	mManager.Update();
}

pof::PoisonBookView::~PoisonBookView()
{
}

void pof::PoisonBookView::CreateToolBars()
{
	mToolbar = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mSearchbar = new wxSearchCtrl(mToolbar, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(400, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	mSearchbar->ShowSearchButton(true);
#endif
	mSearchbar->ShowCancelButton(true);
	mSearchbar->SetDescriptiveText("Search for books");

	mToolbar->AddControl(mSearchbar, "Search bar");

	mToolbar->AddStretchSpacer();
	mToolbar->AddTool(ID_ADD_PRODUCT, "Create New Book", wxArtProvider::GetBitmap("action_add"));

	mToolbar->Realize();
	mManager.AddPane(mToolbar, wxAuiPaneInfo().Name("Tools").Top().MinSize(-1, 30).ToolbarPane().PaneBorder(false).DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::PoisonBookView::CreateBookToolBar()
{
	mBookbar =  new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mBookbar->AddTool(wxID_BACKWARD, "Back", wxArtProvider::GetBitmap("arrow_back"), "Back to poison books");
	mBookbar->AddSeparator();
		
	mBookbar->AddStretchSpacer();
	mDateCtrl = new wxDatePickerCtrl(mBookbar, ID_DATE_PICKER, wxDateTime::Now(), wxDefaultPosition, wxSize(100, -1), wxDP_DROPDOWN);
	mBookbar->AddControl(mDateCtrl, "Date");
	mBookbar->AddSpacer(2);
	mBookbar->AddTool(ID_RESET, "Reset", wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR, wxSize(16, 16)), "Reset date search");
	mBookbar->Realize();
	mManager.AddPane(mBookbar, wxAuiPaneInfo().Name("BookTools").Top().MinSize(-1, 30).ToolbarPane().PaneBorder(false).DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());

}

void pof::PoisonBookView::CreateViews()
{
	//mThumbCtrl = new ThumbnailCtrl(mBook, ID_THUMBNAIL, wxDefaultPosition, wxDefaultSize);
	wxPanel* panel = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	wxPanel* textPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxSizer* tsz = new wxBoxSizer(wxHORIZONTAL);

	auto text = new wxStaticText(textPanel, wxID_ANY, "Please select a controlled book to view");
	text->SetFont(wxFontInfo().AntiAliased());
	text->SetBackgroundColour(*wxWHITE);

	tsz->Add(text, 0, wxALL | wxEXPAND, 5);
	tsz->AddStretchSpacer();

	textPanel->SetSizer(tsz);
	tsz->SetSizeHints(textPanel);
	textPanel->Layout();

	mBookList = new wxListCtrl(panel, ID_BOOKLIST, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxLC_ALIGN_TOP | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxFULL_REPAINT_ON_RESIZE | wxNO_BORDER);
	wxImageList* imagelist = new wxImageList(40, 40);
	imagelist->Add(wxArtProvider::GetBitmap("product"));
	mBookList->AssignImageList(imagelist, wxIMAGE_LIST_NORMAL);

	sz->AddSpacer(2);
	sz->Add(textPanel, 0, wxALL | wxEXPAND, 2);
	sz->Add(new wxStaticLine(panel, wxID_ANY), 0, wxALL | wxEXPAND, 2);
	sz->AddSpacer(10);
	sz->Add(mBookList, 1, wxALL | wxEXPAND, 2);


	panel->SetSizer(sz);
	sz->SetSizeHints(panel);
	panel->Layout();

	mBook->AddPage(panel, "booklist", false);

}

void pof::PoisonBookView::CreateDataView()
{
	wxPanel* panel = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	mTextPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxSizer* tsz = new wxBoxSizer(wxHORIZONTAL);

	mProductName = new wxStaticText(mTextPanel, wxID_ANY, wxEmptyString);
	mProductName->SetFont(wxFontInfo().AntiAliased());
	mProductName->SetBackgroundColour(*wxWHITE);

	mInfoBar = new wxInfoBar(panel);

	mBookData = new wxDataViewCtrl(panel, ID_BOOKDATA, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	mBookData->AssociateModel(wxGetApp().mPoisonBookManager.GetBook().get());

	mBookData->AppendTextColumn(wxT("Date"), pof::PoisonBookManager::DATE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mBookData->AppendBitmapColumn(wxT("Verified"), pof::PoisonBookManager::ISVERIFED, wxDATAVIEW_CELL_INERT, 60, wxALIGN_CENTER);
	mBookData->AppendTextColumn(wxT("Patient name"), pof::PoisonBookManager::PNAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_CENTER);
	mBookData->AppendTextColumn(wxT("Patient Address"), pof::PoisonBookManager::PADDY, wxDATAVIEW_CELL_INERT, 250, wxALIGN_CENTRE);
	mBookData->AppendTextColumn(wxT("Pharmacist"), pof::PoisonBookManager::PHARMNAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_CENTRE);
	mBookData->AppendTextColumn(wxT("Quantity"), pof::PoisonBookManager::QUAN, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mBookData->AppendTextColumn(wxT("Balance"), pof::PoisonBookManager::RUNBALANCE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);

	tsz->Add(mProductName, 0, wxALL | wxEXPAND, 5);
	tsz->AddStretchSpacer();


	mTextPanel->SetSizer(tsz);
	tsz->SetSizeHints(mTextPanel);
	mTextPanel->Layout();

	sz->Add(mInfoBar, 0, wxALL | wxEXPAND, 2);
	sz->Add(mTextPanel, 0, wxALL | wxEXPAND, 2);
	sz->Add(new wxStaticLine(panel, wxID_ANY), 0, wxALL | wxEXPAND, 2);
	sz->Add(mBookData, 1, wxALL | wxEXPAND, 2);

	panel->SetSizer(sz);
	sz->SetSizeHints(panel);
	panel->Layout();

	mBook->AddPage(panel, "bookdata", false);
}

void pof::PoisonBookView::CreateSpecialCols()
{
	pof::DataModel::SpeicalColHandler_t col;
	col.first = [&](size_t r, size_t c) -> wxVariant {
		auto& datastore = wxGetApp().mPoisonBookManager.GetBook()->GetDatastore();
		std::uint64_t i =  boost::variant2::get<std::uint64_t>(datastore[r].first[c]);
		switch (i)
		{
		case 0:
			return wxVariant(wxArtProvider::GetBitmap("action_remove"));
		case 1:
			return wxVariant(wxArtProvider::GetBitmap("login"));
		default:
			break;
		}
	};
	
	wxGetApp().mPoisonBookManager.GetBook()->SetSpecialColumnHandler(pof::PoisonBookManager::ISVERIFED, std::move(col));
}


void pof::PoisonBookView::CreateEmptyBookPane()
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

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No book created for controlled medications"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	wxButton* btn = new wxButton(m7, ID_ADD_PRODUCT);
	btn->SetBitmap(wxArtProvider::GetBitmap("action_add"));
	btn->SetLabel("Create new book");
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

	mBook->AddPage(mEmpty, "Empty", false);
}

void pof::PoisonBookView::LoadBooks()
{
	auto books = wxGetApp().mPoisonBookManager.GetBooks();
	if (!books.has_value()) {
		wxMessageBox("Cannot load controlled books, call D-GLOPA admin", "Poison book", wxICON_ERROR | wxOK);
		spdlog::error("Failed to load controlled medication books");
		return;
	}
	if (books->empty()) return;

	size_t i = 0;
	mBookList->ClearAll();
	for (auto& tup : books.value())
	{
		std::string string = fmt::format("{}{}\n{}", std::get<1>(tup),
			std::get<2>(tup), std::get<3>(tup));
		wxListItem item;
		item.SetId(i);
		item.SetText(string);
		item.SetData(new pof::base::data::duuid_t(std::get<0>(tup)));
		item.SetImage(0);
		item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT | wxLIST_MASK_DATA);
		
		mBookList->InsertItem(std::move(item));
		i++;
	}
}

void pof::PoisonBookView::LoadBookValues()
{

}

void pof::PoisonBookView::OnBookActivated(wxListEvent& evt)
{
	const wxListItem& item = evt.GetItem();
	auto puid = reinterpret_cast<boost::uuids::uuid*>(item.GetData());
	auto text = item.GetText().ToStdString();
	if (!puid) return;

	mBookData->Freeze();
	LoadProductName(text);
	wxGetApp().mPoisonBookManager.LoadBook(*puid);
	mBookData->Thaw();

	SwitchToolBar();
	mBook->SetSelection(BOOK_VIEW);
	mSelItem = item;
}

void pof::PoisonBookView::OnBookSelected(ThumbnailEvent& evt)
{
}

void pof::PoisonBookView::OnAddProduct(wxCommandEvent& evt)
{
	pof::SearchProduct dialog(this);
	if (dialog.ShowModal() == wxID_OK) {
		if (dialog.HasMultipleSelections()) {
			//cannot open a book for multiple selections
			wxMessageBox("Multple product selected, cannot open book for more than one product at a time", "Poison book", wxICON_INFORMATION | wxOK);
			return;
		}
		auto& prow = dialog.GetSelectedProduct();
		auto& puid = boost::variant2::get<pof::base::data::duuid_t>(prow.first[pof::ProductManager::PRODUCT_UUID]);
		auto& name = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_NAME]);
		auto& cls = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_CLASS]);

		if (cls != "CONTROLLED") {
			wxMessageBox(fmt::format("{} is not a controlled medicaiton", name), "Products", wxICON_INFORMATION | wxOK);
			return;
		}

		if (wxGetApp().mPoisonBookManager.IsBookCreated(puid)) {
			wxMessageBox(fmt::format("Controlled book register already created for {}", name), "Products", wxICON_INFORMATION | wxOK);
			return;
		}
		pof::base::data::row_t row;
		//create the first entry into the book table	
		auto& v = row.first;
		auto& p = prow.first;

		v.push_back(p[pof::ProductManager::PRODUCT_UUID]);
		v.push_back("ENTRY"s);
		v.push_back(wxGetApp().MainPharmacy->GetAddressAsString());
		v.push_back(fmt::format("{} {}", wxGetApp().MainAccount->lastname, wxGetApp().MainAccount->name));
		v.push_back(static_cast<std::uint64_t>(1));
		v.push_back(static_cast<std::uint64_t>(0));
		//create with the current stock
		std::uint64_t stock = boost::variant2::get<std::uint64_t>(p[pof::ProductManager::PRODUCT_STOCK_COUNT]);
		v.push_back(stock);
		v.push_back(stock);
		v.push_back(pof::base::data::clock_t::now());

		if (!wxGetApp().mPoisonBookManager.CreateNewBook(std::move(row))) {
			wxMessageBox(fmt::format("Failed to create book for {}", name), "Products", wxICON_INFORMATION | wxOK);
		}

		//add the one
		auto& formulation = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_FORMULATION]);
		auto string = fmt::format("{}\n{}", name, formulation);

		mBookList->Freeze();
		size_t i = mBookList->GetItemCount();
		wxListItem item;
		item.SetId(i);
		item.SetText(string);
		item.SetData(new pof::base::data::duuid_t(puid));
		item.SetImage(0);
		item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT | wxLIST_MASK_DATA);

		mBook->SetSelection(THUMBNAIL_SELECT);

		mBookList->InsertItem(std::move(item));
		mBookList->EnsureVisible(i);
		mBookList->Thaw();
	}
}

void pof::PoisonBookView::OnSearch(wxCommandEvent& evt)
{
}

void pof::PoisonBookView::OnSearchCleared(wxCommandEvent& evt)
{
}

void pof::PoisonBookView::OnAuiThemeChange()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::PoisonBookView::OnBack(wxCommandEvent& evt)
{
	wxBusyCursor cursor;

	mBook->SetSelection(THUMBNAIL_SELECT);
	SwitchToolBar();
}

void pof::PoisonBookView::OnDateChanged(wxDateEvent& evt)
{

}

void pof::PoisonBookView::OnVerify(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)){
		wxMessageBox("User account cannot verify a poison book entry", "Poison book", wxICON_WARNING | wxOK);
		return;
	}
	auto item = mBookData->GetSelection();
	if (!item.IsOk()) return;
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& row = wxGetApp().mPoisonBookManager.GetBook()->GetDatastore()[idx];

	if (boost::variant2::get<std::uint64_t>(row.first[pof::PoisonBookManager::ISVERIFED]) == 1){
		wxMessageBox("Entry has already been verified", "Posion book", wxICON_INFORMATION | wxOK);
		return;
	}

	auto& v = row.first;
	auto& up = row.second.second;

	v[pof::PoisonBookManager::ISVERIFED] = static_cast<std::uint64_t>(1);
	v[pof::PoisonBookManager::PHARMNAME] = fmt::format("{} {}", wxGetApp().MainAccount->lastname, wxGetApp().MainAccount->name);

	up.set(pof::PoisonBookManager::ISVERIFED);
	up.set(pof::PoisonBookManager::PHARMNAME);

	wxGetApp().mPoisonBookManager.GetBook()->UpdateItem(item);
	up.reset();
}

void pof::PoisonBookView::OnReset(wxCommandEvent& evt)
{
}

void pof::PoisonBookView::OnContextMenu(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;

	wxMenu* menu = new wxMenu;
	auto r = menu->Append(ID_VERIFY, "Verify entry");

	mBookData->PopupMenu(menu);
}

void pof::PoisonBookView::OnProductRemoved(pof::base::data::const_iterator iter)
{
	if (mBookList->IsEmpty()) return;
	auto& puid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[pof::ProductManager::PRODUCT_UUID]);
	auto& cls = boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_CLASS]);
	if (cls != "CONTROLLED") return;

	int sel = mBook->GetSelection();
	if (sel == BOOK_VIEW){
		auto p = reinterpret_cast<pof::base::data::duuid_t*>(mSelItem.GetData());
		if (p) {
			if (*p == puid) {
				//if we are removing the product of the selected book go back to thumbnail
				mBook->SetSelection(THUMBNAIL_SELECT);
			}
		}
	}
	
	//reload books, because this wxListCtrl api is so stupid, there is no mechanism to iterate through all the items
	LoadBooks();
	if (mBookList->IsEmpty()) {
		mBook->SetSelection(EMPTY);
	}
}

void pof::PoisonBookView::OnBookAdded(const pof::base::data::row_t& prow)
{
	auto& puid = boost::variant2::get<pof::base::data::duuid_t>(prow.first[pof::ProductManager::PRODUCT_UUID]);
	auto& name = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_NAME]);
	auto& cls = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_CLASS]);

	if (wxGetApp().mPoisonBookManager.IsBookCreated(puid)) {
		return;
	}
	pof::base::data::row_t row;
	//create the first entry into the book table	
	auto& v = row.first;
	auto& p = prow.first;

	v.push_back(p[pof::ProductManager::PRODUCT_UUID]);
	v.push_back("ENTRY"s);
	v.push_back(wxGetApp().MainPharmacy->GetAddressAsString());
	v.push_back(fmt::format("{} {}", wxGetApp().MainAccount->lastname, wxGetApp().MainAccount->name));
	v.push_back(static_cast<std::uint64_t>(1));
	v.push_back(static_cast<std::uint64_t>(0));
	//create with the current stock
	std::uint64_t stock = boost::variant2::get<std::uint64_t>(p[pof::ProductManager::PRODUCT_STOCK_COUNT]);
	v.push_back(stock);
	v.push_back(stock);
	v.push_back(pof::base::data::clock_t::now());

	if (!wxGetApp().mPoisonBookManager.CreateNewBook(std::move(row))) {
		wxMessageBox(fmt::format("Failed to create book for {}", name), "Products", wxICON_INFORMATION | wxOK);
	}

	//add the one
	auto& formulation = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_FORMULATION]);
	auto string = fmt::format("{}\n{}", name, formulation);

	mBookList->Freeze();
	size_t i = mBookList->GetItemCount();
	wxListItem item;
	item.SetId(i);
	item.SetText(string);
	item.SetData(new pof::base::data::duuid_t(puid));
	item.SetImage(0);
	item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT | wxLIST_MASK_DATA);

	mBook->SetSelection(THUMBNAIL_SELECT);
		
	mBookList->InsertItem(std::move(item));
	mBookList->EnsureVisible(i);
	mBookList->Thaw();
}

void pof::PoisonBookView::SetupAuiTheme()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::PoisonBookView::OnAuiThemeChange, this));
}

void pof::PoisonBookView::SwitchToolBar()
{
	auto& topPane = mManager.GetPane("Tools");
	auto& pPane = mManager.GetPane("BookTools");
	if (!topPane.IsOk() || !pPane.IsOk()) return;
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

void pof::PoisonBookView::LoadProductName(const std::string& name)
{
	auto panel = mBook->GetPage(BOOK_VIEW);
	panel->Freeze();
	mProductName->SetLabelText(name);
	panel->Layout();
	panel->Thaw();
}
