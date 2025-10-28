#include "PofPch.h"
#include "ExpiredView.h"
BEGIN_EVENT_TABLE(pof::ExpiredView, wxDialog)
	EVT_DATE_CHANGED(pof::ExpiredView::ID_DATE, pof::ExpiredView::OnDateChanged)
	EVT_BUTTON(pof::ExpiredView::ID_RESET, pof::ExpiredView::OnReset)
	EVT_TOOL(pof::ExpiredView::ID_MOVE_EXPIRED, pof::ExpiredView::OnMarkAsExpired)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::ExpiredView::ID_VIEW, pof::ExpiredView::OnContextMenu)
END_EVENT_TABLE()


pof::ExpiredView::ExpiredView(wxWindow* parent, wxWindowID id, std::shared_ptr<pof::base::data> data, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxDialog(parent, id, title, pos, size, style) {
	this->SetSize(FromDIP(size));
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	CreateViewManagerDefaultArt();
	CreateToolBar();
	mBook = new wxSimplebook(this, wxID_ANY);
	mdata = data;
	CreateEmptyPanel();
	CreateView();


	m_mgr.AddPane(mBook, wxAuiPaneInfo().Center().CaptionVisible(false).CloseButton(false).PaneBorder(false).Movable(false).Dock().Resizable().FloatingSize(wxDefaultSize).BottomDockable(false).TopDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).CentrePane());
	m_mgr.Update();
	this->Centre(wxBOTH);


	LoadExpiredProducts();
	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

pof::ExpiredView::~ExpiredView()
{
	m_mgr.UnInit();
}

void pof::ExpiredView::CreateToolBar()
{
	ToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW);
	ToolBar->SetToolBitmapSize(wxSize(16, 16));
	ToolBar->AddTool(ID_MOVE_EXPIRED, wxT("Remove expired stock"), wxArtProvider::GetBitmap("add_task", wxART_OTHER, FromDIP(wxSize(16, 16))), "Add to expired list");
	ToolBar->AddSeparator();
	mInfoItem = ToolBar->AddTool(wxID_ANY, wxEmptyString, wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	ToolBar->AddStretchSpacer();
	ToolBar->AddControl(new wxButton(ToolBar, ID_RESET, "Reset"));
	ToolBar->AddSpacer(FromDIP(5));
	mDatePicker = new wxDatePickerCtrl(ToolBar, ID_DATE, wxDateTime::Now(), wxDefaultPosition, FromDIP(wxSize(200, -1)), wxDP_DROPDOWN);
	mDatePicker->SetRange(wxDateTime{}, wxDateTime{});
	auto dd = mDatePicker->GetValue();
	std::chrono::month month = static_cast<std::chrono::month>(dd.GetMonth() + 1);
	std::chrono::year year = static_cast<std::chrono::year>(dd.GetYear());
	std::chrono::day day = static_cast<std::chrono::day>(dd.GetDay());
	const std::chrono::year_month_day ymd{ month / day / year };
	const auto mSelectDay = static_cast<std::chrono::sys_days>(ymd);
	ToolBar->AddControl(mDatePicker);

	ToolBar->Realize();
	m_mgr.AddPane(ToolBar, wxAuiPaneInfo().Name(wxT("SearchToolBar")).Top().MinSize(FromDIP(wxSize(-1, 30))).DockFixed().CloseButton(false).PaneBorder(false).Dock().Resizable().BottomDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).Layer(10).ToolbarPane());

}

void pof::ExpiredView::CreateView()
{
	m_panel1 = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel1->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);
	if (!mdata) {
		mModel = new DataModel(wxGetApp().mProductManager.GetProductData()->ShareDatastore());
	}
	else {
		mModel = new DataModel(mdata);
	}

	SearchData = new wxDataViewCtrl(m_panel1, ID_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES | wxDV_ROW_LINES);
	SearchData->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, FromDIP(300), static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Formulation"), pof::ProductManager::PRODUCT_FORMULATION, wxDATAVIEW_CELL_INERT, FromDIP(100), static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, FromDIP(100), static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Stock"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, FromDIP(100), static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, FromDIP(120), static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	bSizer1->Add(SearchData, 1, wxALL | wxEXPAND, 0);

	SearchData->AssociateModel(mModel);
	mModel->DecRef();
	mModel->Reload();

	m_panel1->SetSizer(bSizer1);
	m_panel1->Layout();
	bSizer1->Fit(m_panel1);

	mBook->AddPage(m_panel1, "View", true);
}

void pof::ExpiredView::CreateViewManagerDefaultArt()
{
	wxColour colour = wxTheColourDatabase->Find("Aqua");
	wxAuiDockArt* art = m_mgr.GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);
}

void pof::ExpiredView::CreateEmptyPanel(const std::string& text)
{
	mEmpty = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mEmpty->SetBackgroundColour(*wxWHITE);
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

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
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
}

void pof::ExpiredView::ShowEmpty(const std::string& text)
{
	mEmpty->Freeze();
	mEmptyStr->SetLabelText(text);
	mEmpty->Layout();
	mEmpty->Thaw();

	mBook->SetSelection(0);
}

void pof::ExpiredView::LoadExpiredProducts()
{
	wxBusyInfo wait("Loading expired products\nPlease wait...");
	const auto items = wxGetApp().mProductManager.DoExpiredProducts();
	if (!items.has_value() || items->empty()) {
		UpdateText("No transaction");
		ShowEmpty("No expired products in store");
		return;
	}
	SearchData->Freeze();
	UpdateText("Expired products in store");
	mModel->Reload(items.value());
	SearchData->Thaw();

	if (mBook->GetSelection() != 1)
		mBook->SetSelection(1);
}

void pof::ExpiredView::OnDateChanged(wxDateEvent& evt)
{
	const auto dd = evt.GetDate();
	std::chrono::month month = static_cast<std::chrono::month>(dd.GetMonth() + 1);
	std::chrono::year year = static_cast<std::chrono::year>(dd.GetYear());
	std::chrono::day day = static_cast<std::chrono::day>(dd.GetDay());

	const std::chrono::year_month_day ymd{ month / day / year };
	const auto mSelectDay = static_cast<std::chrono::sys_days>(ymd);

	const auto items = wxGetApp().mProductManager.DoExpiredProducts(mSelectDay);
	if (!items.has_value() || items->empty()) {
		UpdateText("No transaction");
		ShowEmpty(fmt::format("No expired products in store at {:%m/%Y}",
			std::chrono::time_point_cast<
			std::chrono::system_clock::duration>(mSelectDay)));
		return;
	}

	SearchData->Freeze();
	UpdateText(fmt::format("Expired products in store at {:%m/%Y}",
		std::chrono::time_point_cast<
		std::chrono::system_clock::duration>(mSelectDay)));
	mModel->Reload(items.value());
	SearchData->Thaw();

	if (mBook->GetSelection() != 1)
		mBook->SetSelection(1);

}

void pof::ExpiredView::OnReset(wxCommandEvent& evt)
{
	LoadExpiredProducts();
}

void pof::ExpiredView::OnMarkAsExpired(wxCommandEvent& evt)
{
	const auto item = SearchData->GetSelection();
	if (!item.IsOk()) return;
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	pof::base::data::text_t& name = boost::variant2::get<pof::base::data::text_t>(datastore[idx].first[pof::ProductManager::PRODUCT_NAME]);
	pof::base::data::duuid_t& pid = boost::variant2::get<pof::base::data::duuid_t>(datastore[idx].first[pof::ProductManager::PRODUCT_UUID]);
	std::uint64_t& stockCount     = boost::variant2::get<std::uint64_t>(datastore[idx].first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
	//cannot move empty stock
	if (stockCount == 0) {
		wxMessageBox(fmt::format("{} has no stock to move", name), "Expired stock", wxICON_INFORMATION | wxOK);
		return;
	}
	auto reqS = wxGetTextFromUser("Please enter the amount of stock you want to move?", "Move stock").ToStdString();
	if (reqS.empty()) return;
	std::uint64_t remvStock = 0;
	std::uint64_t req = 0;
	wxBusyInfo wait("Moving stock\nPlease wait...");
	try {
		req = boost::lexical_cast<std::uint64_t>(reqS);
		if (req > stockCount) {
			wxMessageBox("Cannot remove more than the stock count", "Move stock", wxICON_ERROR | wxOK);
			return;
		}
		remvStock = stockCount - req;
	}
	catch (const std::exception& exp) {
		wxMessageBox("Invalud stock amount", "Move stock", wxICON_ERROR | wxOK);
		return;
	}
	wxGetApp().mProductManager.MoveStockToExpire(pid, req);
	wxGetApp().mProductManager.UpdatePD(std::make_tuple(pid, remvStock), { "uuid", "stock_count" });
	
	SearchData->Freeze();
	datastore[idx].first[pof::ProductManager::PRODUCT_STOCK_COUNT] = remvStock;
	SearchData->Thaw();

}

void pof::ExpiredView::OnContextMenu(wxDataViewEvent& evt)
{
	const auto item = evt.GetItem();
	if (!item.IsOk()) return;


	wxMenu* menu = new wxMenu;
	menu->Append(ID_MOVE_EXPIRED, "Remove expired stock", nullptr);

	SearchData->PopupMenu(menu);
}

void pof::ExpiredView::UpdateText(const std::string& str)
{
	mInfoItem->SetLabel(str);
	ToolBar->Realize();
	ToolBar->Refresh();
	ToolBar->Update();
}
