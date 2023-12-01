#include "PoisonBookView.h"
#include "PofPch.h"
BEGIN_EVENT_TABLE(pof::PoisonBookView, wxPanel)
	EVT_LIST_ITEM_ACTIVATED(pof::PoisonBookView::ID_BOOKLIST, pof::PoisonBookView::OnBookActivated)
	
	EVT_TOOL(pof::PoisonBookView::ID_ADD_PRODUCT, pof::PoisonBookView::OnAddProduct)


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
	CreateEmptyBookPane();
	CreateViews();



	mManager.AddPane(mBook, wxAuiPaneInfo().Name("Book").CenterPane().CaptionVisible(false).Show());
	LoadBooks(); //do not think I would leave this here
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
	mToolbar->AddTool(ID_ADD_PRODUCT, "Open Book", wxArtProvider::GetBitmap("action_add"));

	mToolbar->Realize();
	mManager.AddPane(mToolbar, wxAuiPaneInfo().Name("Tools").Top().MinSize(-1, 30).ToolbarPane().PaneBorder(false).DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
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

	mBookList = new wxListCtrl(panel, ID_BOOKLIST, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxFULL_REPAINT_ON_RESIZE | wxNO_BORDER);
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

void pof::PoisonBookView::CreateSpecialCols()
{
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

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_WARNING, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No book created for controlled medications"), wxDefaultPosition, wxDefaultSize, 0);
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


	mEmpty->SetSizer(bSizer6);
	mEmpty->Layout();

	mBook->AddPage(mEmpty, "Empty", true);
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
	for (auto& tup : books.value())
	{
		std::string string = fmt::format("{}{}\n{}", std::get<0>(tup),
			std::get<1>(tup), std::get<2>(tup));
		wxListItem item;
		item.SetId(i);
		item.SetText(string);
		item.SetImage(0);
		item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT | wxLIST_MASK_DATA);
		
		mBookList->InsertItem(std::move(item));
		i++;
	}
	mBook->SetSelection(THUMBNAIL_SELECT);
}

void pof::PoisonBookView::LoadBookValues()
{
}

void pof::PoisonBookView::OnBookActivated(wxListEvent& evt)
{
	wxMessageBox("Message");
}

void pof::PoisonBookView::OnBookSelected(ThumbnailEvent& evt)
{
}

void pof::PoisonBookView::OnAddProduct(wxCommandEvent& evt)
{
	pof::SearchProduct dialog(this);
	if (dialog.ShowModal() == wxID_OK) {
		wxMessageBox("Added message");
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

void pof::PoisonBookView::SetupAuiTheme()
{
	auto auiArtProvider = mManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::PoisonBookView::OnAuiThemeChange, this));
}