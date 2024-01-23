#include "StockCheck.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::StockCheck, wxDialog)
EVT_UPDATE_UI(pof::StockCheck::ID_TOOL, pof::StockCheck::OnToolUpdateUI)
EVT_TOOL(pof::StockCheck::ID_ADD_PRODUCT, pof::StockCheck::OnAddProduct)
EVT_TOOL(pof::StockCheck::ID_SHOW_SUMMARY, pof::StockCheck::OnShowSummary)
EVT_TOOL(wxID_BACKWARD, pof::StockCheck::OnBack)
EVT_TOOL(wxID_RESET, pof::StockCheck::OnReset)
EVT_TOOL(pof::StockCheck::ID_STOCK_MARK_AS_COMPLETE, pof::StockCheck::OnMarkAsComplete)
EVT_DATE_CHANGED(pof::StockCheck::ID_DATE, pof::StockCheck::OnDate)
EVT_CHOICE(pof::StockCheck::ID_CATEGORY_SELECT, pof::StockCheck::OnCategorySelected)
EVT_LIST_ITEM_ACTIVATED(pof::StockCheck::ID_STOCK_SELECT, pof::StockCheck::OnStockActivated)
EVT_LIST_ITEM_SELECTED(pof::StockCheck::ID_STOCK_SELECT, pof::StockCheck::OnStockSelected)
EVT_INIT_DIALOG(pof::StockCheck::OnInitDialog)
EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::StockCheck::ID_STOCK_DATA, pof::StockCheck::OnEditingStarted)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::StockCheck::ID_STOCK_DATA, pof::StockCheck::OnContextMenu)
EVT_LIST_ITEM_RIGHT_CLICK(pof::StockCheck::ID_STOCK_SELECT, pof::StockCheck::OnStockSelectRightClick)
EVT_MENU(pof::StockCheck::ID_STOCK_CONSUMPTION_PATTERN, pof::StockCheck::OnStockConsumptionPattern)
EVT_MENU(pof::StockCheck::ID_REMOVE_STOCK, pof::StockCheck::OnRemoveStock)
EVT_MENU(pof::StockCheck::ID_STOCK_MARK_PROD_AS_COMPLETE, pof::StockCheck::OnMarkAsComplete)
EVT_MENU(pof::StockCheck::ID_STOCK_UNMARK_AS_COMPLETE, pof::StockCheck::OnUnmarkAsComplete)
EVT_TOOL(pof::StockCheck::ID_SELECT, pof::StockCheck::OnSelect)
EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::StockCheck::ID_STOCK_DATA, pof::StockCheck::OnHeaderClicked)
END_EVENT_TABLE()



pof::StockCheck::StockCheck( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	//this->SetSizeHints( wxSize( 1178,689 ), wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	



	mBook = new wxSimplebook(this, ID_BOOK);
	m_mgr.AddPane( mBook, wxAuiPaneInfo().Name("MainPane").CaptionVisible(false).CloseButton(false).PaneBorder(true).Dock().Resizable().FloatingSize(wxDefaultSize).CentrePane());

	mMainPane = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	CreateToolBar();
	
	mStockData = new wxDataViewCtrl( mMainPane,ID_STOCK_DATA, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mStockData->AssociateModel(wxGetApp().mProductManager.GetStockCheckData().get());

	mProductName = mStockData->AppendTextColumn( wxT("Product Name"), STOCK_PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 300, wxALIGN_CENTER);
	mCurrenctStock = mStockData->AppendTextColumn( wxT("Closing Stock"), STOCK_CURRENT_STOCK, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mCheckedStock = mStockData->AppendTextColumn(wxT("Checked Stock"), STOCK_CHECKED_STOCK, wxDATAVIEW_CELL_EDITABLE, 100, wxALIGN_CENTER);
	mExpiredStock = mStockData->AppendTextColumn(wxT("Expired Stock"), STOCK_EXPIRED, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	
	mShortage = mStockData->AppendTextColumn( wxT("Shortage"), STOCK_SHORTAGE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mStatus = mStockData->AppendBitmapColumn( wxT("Status"), STOCK_STATUS, wxDATAVIEW_CELL_INERT, 50, wxALIGN_CENTER);
	mDateAdded = mStockData->AppendTextColumn(wxT("Date Added"), STOCK_DATE_ADDED, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT);
	
	bSizer1->Add( mStockData, 1, wxALL|wxEXPAND, 0 );
	
	
	mMainPane->SetSizer( bSizer1 );
	mMainPane->Layout();
	bSizer1->Fit( mMainPane );

	CreateEmptyStockCheck();
	mBook->AddPage(mMainPane, "Stock List", false);
	CreateStockSelect();

	mSummary = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER );
	mSummary->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	m_mgr.AddPane( mSummary, wxAuiPaneInfo() .Name( wxT("SummaryPane") ).Bottom() .Caption( wxT("Summary") ).PinButton( true ).Dock().Resizable().FloatingSize(wxDefaultSize).TopDockable( false ).LeftDockable( false ).RightDockable( false ).Floatable( false ).Row( 1 ).MinSize( wxSize( 50,150 ) ).Hide() );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	
	//bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 0, 2, 2, 2 );
	
	mTotalStockCheckedLabel = new wxStaticText(mSummary, wxID_ANY, wxT("Total Stock Checked:"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalStockCheckedLabel->SetFont(wxFontInfo(9).AntiAliased());
	mTotalStockCheckedLabel->Wrap(-1);
	gSizer1->Add(mTotalStockCheckedLabel, 0, wxALIGN_CENTER | wxALL, 2);

	mTotalStockCheckedValue = new wxStaticText(mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalStockCheckedValue->SetFont(wxFontInfo(9).AntiAliased());
	mTotalStockCheckedValue->Wrap(-1);
	gSizer1->Add(mTotalStockCheckedValue, 0, wxALIGN_CENTER | wxALL, 0);

	mTotalShortageLabel = new wxStaticText(mSummary, wxID_ANY, wxT("Total Shortage"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalShortageLabel->SetFont(wxFontInfo(9).AntiAliased());
	mTotalShortageLabel->Wrap(-1);
	gSizer1->Add(mTotalShortageLabel, 0, wxALL, 5);

	mTotalShortageValue = new wxStaticText(mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalShortageValue->SetFont(wxFontInfo(9).AntiAliased());
	mTotalShortageValue->Wrap(-1);
	gSizer1->Add(mTotalShortageValue, 0, wxALIGN_CENTER | wxALL, 5);

	mShortageAmountLabel = new wxStaticText(mSummary, wxID_ANY, wxT("Shortage Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	mShortageAmountLabel->SetFont(wxFontInfo(9).AntiAliased());
	mShortageAmountLabel->Wrap(-1);
	gSizer1->Add(mShortageAmountLabel, 0, wxALL, 5);

	mShortageAmountValue = new wxStaticText(mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mShortageAmountValue->SetFont(wxFontInfo(9).AntiAliased());
	mShortageAmountValue->Wrap(-1);
	gSizer1->Add(mShortageAmountValue, 0, wxALIGN_CENTER | wxALL, 5);


	bSizer2->Add(gSizer1, 0, wxALIGN_TOP, 0);
	
	
	mSummary->SetSizer( bSizer2 );
	mSummary->Layout();
	bSizer2->Fit( mSummary );
	

	OnAuiThemeChange();
	AddSpecialCols();
	
	LoadStockSelect();
	m_mgr.Update();

	this->Layout();
	this->Centre( wxBOTH );

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);

	this->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& evt) {
		wxGetApp().mProductManager.GetStockCheckData()->Clear();
		evt.Skip();
	});
}

void pof::StockCheck::CreateToolBar()
{
	mTools = new wxAuiToolBar(this, ID_TOOL, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mBackButton = mTools->AddTool(wxID_BACKWARD, "Back", wxArtProvider::GetBitmap("arrow_back"));
	mTools->AddSeparator();
	mTools->AddSpacer(2);
	mTools->AddTool(ID_SELECT, "Select", wxNullBitmap, "Show select", wxITEM_CHECK);
	mTools->AddSpacer(5);

	auto& category = wxGetApp().mProductManager.GetCategories();
	wxArrayString choices;

	for (auto& value : category){
		choices.push_back(boost::variant2::get<pof::base::data::text_t>(value.first[pof::ProductManager::CATEGORY_NAME]));
	}
	mCategorySelect = new wxChoice(mTools, ID_CATEGORY_SELECT, wxDefaultPosition, wxSize(200, -1), choices);
	mCategorySelect->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mCategorySelect);
	wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

	dc.SetBrush(*wxWHITE);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRoundedRectangle(rect, 2.0f);
	dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
	auto sel = mCategorySelect->GetStringSelection();
	if (!sel.IsEmpty()) {
		dc.DrawLabel(sel, rect, wxALIGN_CENTER);
	}
	});
	mTools->AddControl(mCategorySelect, "Select Category");
	mTools->AddSpacer(5);
	auto item = mTools->AddTool(wxID_RESET, "Reset", wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR, wxSize(16,16)));
	mTools->AddTool(ID_SHOW_SUMMARY, "Show Summary", wxNullBitmap, "Show the summary of the stock check");
	mTools->AddStretchSpacer();
	mAddButton = mTools->AddTool(ID_ADD_PRODUCT, "Add Product", wxArtProvider::GetBitmap("action_add"));
	mTools->AddSpacer(2);
	mTools->AddTool(ID_STOCK_MARK_AS_COMPLETE, "Mark as Complete", wxArtProvider::GetBitmap("action_check"));
	m_mgr.AddPane(mTools, wxAuiPaneInfo().Name("Tools").ToolbarPane().Top().MinSize(-1, 30).Resizable().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false).Hide());
}

void pof::StockCheck::CreateEmptyStockCheck()
{
	mEmptyPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mEmptyPanel->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmptyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No stock check on store"), wxDefaultPosition, wxDefaultSize, 0);
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


	mEmptyPanel->SetSizer(bSizer6);
	mEmptyPanel->Layout();
	mBook->AddPage(mEmptyPanel, "Stock Empty", false);
}

void pof::StockCheck::CreateStockSelect()
{
	wxPanel* selectPanel = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	selectPanel->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);
	wxStaticText* t1 = new wxStaticText(selectPanel, wxID_ANY, wxT("Please select a stock check month"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer1->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	bSizer1->AddSpacer(5);
		
	mStockSelect = new wxListCtrl(selectPanel, ID_STOCK_SELECT, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxFULL_REPAINT_ON_RESIZE | wxLC_EDIT_LABELS | wxNO_BORDER);

	bSizer1->Add(mStockSelect, 1, wxALL | wxEXPAND, 5);


	selectPanel->SetSizer(bSizer1);
	selectPanel->Layout();
	bSizer1->Fit(selectPanel);
	mBook->AddPage(selectPanel, "Stock Select", false);


	wxImageList* imagelist = new wxImageList(48, 48);
	imagelist->Add(wxArtProvider::GetBitmap("book"));
	imagelist->Add(wxArtProvider::GetBitmap("plus"));
	mStockSelect->AssignImageList(imagelist, wxIMAGE_LIST_NORMAL);
}

void pof::StockCheck::LoadStockSelect()
{
	auto stockStatus = wxGetApp().mProductManager.GetStockMonthStatus();
	if (!stockStatus.has_value())
	{
		mBook->SetSelection(PAGE_STOCK_EMPTY);
		mBook->Refresh();
		return;
	}

	size_t i = 0;
	size_t sz = wxGetApp().mProductManager.GetProductData()->GetDatastore().size();
	for (auto& tup : stockStatus.value()){
		wxListItem item;
		item.SetId(i);

		int monthCount = date::floor<date::months>(std::get<0>(tup)).time_since_epoch().count() % 12;

		item.SetText(fmt::format("{}, {:%Y}", pof::MainFrame::monthNames[monthCount], std::get<0>(tup)));
		item.SetImage(0);
		item.SetData(new pof::base::data::datetime_t(std::get<0>(tup)));
		item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_DATA | wxLIST_MASK_IMAGE);
		mStockSelect->InsertItem(item);

		i++;
	}

	wxListItem item;
	item.SetText("Start S/C");
	item.SetId(i);
	item.SetImage(1);
	item.SetData(new pof::base::data::datetime_t(pof::base::data::clock_t::now()));
	item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_DATA | wxLIST_MASK_IMAGE);
	mStockSelect->InsertItem(item);


	mBook->SetSelection(PAGE_STOCK_SELECT);
	mBook->Refresh();
	return;
}

void pof::StockCheck::SwitchToolBar(int page)
{
}

void pof::StockCheck::AddSpecialCols()
{
	auto& datastore = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore();
	pof::DataModel::SpeicalColHandler_t checkedStock;
	pof::DataModel::SpeicalColHandler_t status;
	pof::DataModel::SpeicalColHandler_t shortage;
	pof::DataModel::SpeicalColHandler_t dateAdded;
	pof::DataModel::SpeicalColHandler_t expiredStock;
	pof::DataModel::SpeicalColHandler_t select;

	checkedStock.second = [&](size_t row, size_t col, const wxVariant& value) -> bool
	{
		
		auto& datum = datastore[row];
		auto& v = datum.first;
		std::uint64_t preQuan = boost::variant2::get<std::uint64_t>(v[STOCK_CHECKED_STOCK]);
		auto quan = static_cast<std::uint64_t>(atoll(value.GetString().ToStdString().c_str()));
		if (quan > boost::variant2::get<std::uint64_t>(v[STOCK_CURRENT_STOCK])){
			wxMessageBox("Excess stock, please enter excess as inventory and update check to current stock.", "STOCK CHECK", wxICON_INFORMATION | wxOK);
			return false;
		}
		else if (quan == preQuan) {
			return false; //value did not change so ignore
		}
		const auto& pid = boost::variant2::get<pof::base::data::duuid_t>(v[STOCK_PRODUCT_UUID]);


		v[STOCK_CHECKED_STOCK] = quan;
		wxGetApp().mProductManager.UpdateStockCheck(pid, quan);
		UpdateSummary();
		return true;
	};

	status.first = [&](size_t row, size_t col) -> wxVariant {
		std::uint64_t& s = boost::variant2::get<std::uint64_t>(datastore[row].first[col]);
		switch (s)
		{
		case DONE:
			return wxVariant(wxArtProvider::GetBitmap("action_check"));
		case PENDING:
			return wxVariant(wxArtProvider::GetBitmap("action_remove"));
		default:
			break;
		}
		return wxVariant(wxNullBitmap);
	};

	shortage.first = [&](size_t r, size_t col) -> wxVariant {
		auto& row = datastore[r];
		auto text = fmt::format("{:d}", (boost::variant2::get<std::uint64_t>(row.first[STOCK_CURRENT_STOCK]) -
			boost::variant2::get<std::uint64_t>(row.first[STOCK_CHECKED_STOCK])));
		return text;
	};

	dateAdded.first = [&](size_t r, size_t col) -> wxVariant
	{
		auto& row = datastore[r];
		pof::base::data::datetime_t& datt = boost::variant2::get<pof::base::data::datetime_t>(row.first[STOCK_DATE_ADDED]);
		return fmt::format("{:%d/%m/%Y}", datt);
	};

	expiredStock.first = [&](size_t r, size_t col) -> wxVariant
	{
		if (!mExpiredStockValues.has_value()) return wxVariant{ fmt::to_string(static_cast<std::uint64_t>(0)) };
		if (mExpiredStockValues.value().empty()) return  wxVariant{ fmt::to_string(static_cast<std::uint64_t>(0)) };
		auto& row = datastore[r];
		auto iter = std::ranges::find_if(mExpiredStockValues.value(), [&](auto& val) -> bool {
				return (boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]) == val.first);
		});
		
		if (iter == mExpiredStockValues.value().end()) return  wxVariant{ fmt::to_string(static_cast<std::uint64_t>(0)) };
		else return wxVariant{ fmt::to_string(static_cast<std::uint64_t>(iter->second)) };

	};

	select.first = [&](size_t row, size_t col) -> wxVariant {
		auto found = mSelections.find(pof::DataModel::GetItemFromIdx(row));
		return wxVariant{ found != mSelections.end() };
	};

	select.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		if (v.GetBool()) {
			auto [iter, inserted] = mSelections.insert(pof::DataModel::GetItemFromIdx(row));
			return inserted;
		}
		else {
			mSelections.erase(pof::DataModel::GetItemFromIdx(row));
			return true;
		}
	};


	auto& dm = wxGetApp().mProductManager.GetStockCheckData();
	dm->SetSpecialColumnHandler(STOCK_STATUS, std::move(status));
	dm->SetSpecialColumnHandler(STOCK_SHORTAGE, std::move(shortage));
	dm->SetSpecialColumnHandler(STOCK_CHECKED_STOCK, std::move(checkedStock));
	dm->SetSpecialColumnHandler(STOCK_DATE_ADDED, std::move(dateAdded));
	dm->SetSpecialColumnHandler(STOCK_EXPIRED, std::move(expiredStock));
	dm->SetSpecialColumnHandler(STOCK_SELECT, std::move(select));
	
}

bool pof::StockCheck::TransferDataFromWindow()
{
	return true;
}

pof::StockCheck::~StockCheck()
{
	m_mgr.UnInit();
	wxGetApp().mProductManager.GetStockCheckData()->RemoveSpecialColumnHandler(STOCK_CHECKED_STOCK);
	wxGetApp().mProductManager.GetStockCheckData()->RemoveSpecialColumnHandler(STOCK_SHORTAGE);
	wxGetApp().mProductManager.GetStockCheckData()->RemoveSpecialColumnHandler(STOCK_STATUS);
}


void pof::StockCheck::UpdateSummary()
{
	try {
		auto& datastore = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore();
		auto& prodStore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
		std::uint64_t totalShortage = 0;
		totalShortage = std::accumulate(datastore.begin(), datastore.end(), totalShortage, [&](const std::uint64_t& v,
			const auto& row) -> std::uint64_t {
				std::uint64_t shortage = (boost::variant2::get<std::uint64_t>(row.first[STOCK_CURRENT_STOCK]) -
				boost::variant2::get<std::uint64_t>(row.first[STOCK_CHECKED_STOCK]));
		return  v + shortage;
			});

		mSummary->Freeze();
		auto prodchecked = wxGetApp().mProductManager.GetProductCheckedCount(*mSelectedMonth);
		mTotalStockCheckedValue->SetLabel(fmt::format("{:d} of {:d}", prodchecked.value_or((std::uint64_t)0), prodStore.size()));
		mTotalShortageValue->SetLabel(fmt::format("{:d}", totalShortage));
		auto shortage = wxGetApp().mProductManager.GetShortageCost(*mSelectedMonth);
		mShortageAmountValue->SetLabelText(fmt::format("{:c}", shortage.value_or(pof::base::currency{})));

		mSummary->Layout();
		mSummary->Thaw();
		mSummary->Update();
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
	}
}

void pof::StockCheck::OnAddProduct(wxCommandEvent& evt)
{
	pof::base::data::datetime_t today = pof::base::data::clock_t::now();
	auto curMonth = date::floor<date::months>(today);
	auto tMonth = date::floor<date::months>(*mSelectedMonth);
	if (curMonth != tMonth){
		//cannot add product to a different month
		wxMessageBox("Cannont stock check a product from another month, other than the current one, change to the current month and try again", "STOCK CHECK", wxICON_INFORMATION | wxOK);
		return;
	}

	pof::SearchProduct dialog(this);
	auto& datastore = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore();
	if (dialog.ShowModal() == wxID_OK){
		wxBusyCursor cursor;
		if (dialog.HasMultipleSelections()){
			auto vec = dialog.GetSelectedProducts();
			for (auto& prod : vec) {

				pof::base::data::duuid_t& pid = boost::variant2::get<pof::base::data::duuid_t>(prod.get().first[pof::ProductManager::PRODUCT_UUID]);
				//insert into the datamodel
				pof::base::data::row_t row;
				auto& v = row.first;
				auto& p = prod.get().first;
				if (std::ranges::any_of(datastore, [&](pof::base::data::row_t& item) -> bool {
					return boost::variant2::get<pof::base::data::duuid_t>(item.first[STOCK_PRODUCT_UUID]) == pid; }))
				{
					continue;
				}
 				v.resize(6);


				v[STOCK_PRODUCT_UUID] = pid;
				v[STOCK_PRODUCT_NAME] = p[pof::ProductManager::PRODUCT_NAME];
				v[STOCK_CURRENT_STOCK] = p[pof::ProductManager::PRODUCT_STOCK_COUNT];
				v[STOCK_CHECKED_STOCK] = static_cast<std::uint64_t>(0);
				v[STOCK_STATUS] = static_cast<std::uint64_t>(PENDING);
				v[STOCK_DATE_ADDED] = pof::base::data::clock_t::now();

				wxGetApp().mProductManager.GetStockCheckData()->EmplaceData(std::move(row));
				wxGetApp().mProductManager.InsertProductInStockCheck(pid);
				//wxGetApp().mProductManager.CaptureStock(pid);

			}
		}
		else {
			auto& row = dialog.GetSelectedProduct();
			const pof::base::data::duuid_t& pid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
			
			pof::base::data::row_t rowx;
			auto& v = rowx.first;
			auto& p = row.first;

			if (std::ranges::any_of(datastore, [&](pof::base::data::row_t& item) -> bool {
				return boost::variant2::get<pof::base::data::duuid_t>(item.first[STOCK_PRODUCT_UUID]) == pid; }))
			{
				return;
			}

			v.resize(6);


			v[STOCK_PRODUCT_UUID] = pid;
			v[STOCK_PRODUCT_NAME] = p[pof::ProductManager::PRODUCT_NAME];
			v[STOCK_CURRENT_STOCK] = p[pof::ProductManager::PRODUCT_STOCK_COUNT];
			v[STOCK_CHECKED_STOCK] = static_cast<std::uint64_t>(0);
			v[STOCK_STATUS] = static_cast<std::uint64_t>(PENDING);
			v[STOCK_DATE_ADDED] = pof::base::data::clock_t::now();

			mStockData->Freeze();
			wxGetApp().mProductManager.GetStockCheckData()->EmplaceData(std::move(rowx));
			wxGetApp().mProductManager.InsertProductInStockCheck(pid);
			//wxGetApp().mProductManager.CaptureStock(pid);
			mStockData->Thaw();
			mStockData->Refresh();
		}
		UpdateSummary();
		int sel = mBook->GetSelection();
		if (sel == PAGE_STOCK_EMPTY) {
			mBook->SetSelection(PAGE_STOCK_LIST);
			mBook->Refresh();
		}
	}
	else {
		
	}
	UpdateSummary();
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, "Add product(s) for stock check");
}

void pof::StockCheck::OnDate(wxDateEvent& evt)
{
	auto& date = evt.GetDate();
	pof::base::data::datetime_t curTime = pof::base::data::clock_t::from_time_t(date.GetTicks());
	auto month = std::chrono::duration_cast<date::months>(curTime.time_since_epoch());


	wxGetApp().mProductManager.LoadStockCheckDate(pof::base::data::datetime_t(month));
	UpdateSummary();
}

void pof::StockCheck::OnDialogInit(wxInitDialogEvent& evt)
{
	//load the dialog, with today's time
	OnAuiThemeChange();
	AddSpecialCols();
	mStockCheckMonth->SetValue(wxDateTime::Now());
	wxGetApp().mProductManager.LoadStockCheckDate(pof::base::data::clock_t::now());
}

void pof::StockCheck::OnEditingStarted(wxDataViewEvent& evt)
{
	pof::base::data::datetime_t today = pof::base::data::clock_t::now();
	auto curMonth = date::floor<date::months>(today);
	auto tMonth = date::floor<date::months>(*mSelectedMonth);
	if (curMonth != tMonth) {
		//cannot add product to a different month
		wxMessageBox("Cannont stock check a product from another month, other than the current one, change to the current month and try again", "STOCK CHECK", wxICON_INFORMATION | wxOK);
		return;
	}

	//move this to setup, no need to call every time
	auto item = evt.GetItem();
	auto Col = evt.GetDataViewColumn();
	auto Ctrl = Col->GetRenderer()->GetEditorCtrl();
	wxIntegerValidator<size_t> val{ NULL };
	val.SetMin(0);
	val.SetMax(10000);
	Ctrl->SetValidator(val);
}

void pof::StockCheck::OnCategorySelected(wxCommandEvent& evt)
{
	int sel = evt.GetSelection();
	if (sel == wxNOT_FOUND) return;
	auto& data = wxGetApp().mProductManager.GetCategories();
	std::string value = mCategorySelect->GetStringSelection().ToStdString();
	auto iter = std::ranges::find_if(data, [&](const pof::base::data::row_t& row) -> bool {
		return (boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::CATEGORY_NAME]) == value);
	});
	if (iter == data.end()) return;
	std::uint64_t id = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::CATEGORY_ID]);
	mStockData->Freeze();
	wxGetApp().mProductManager.LoadStockDataByCategory(pof::base::data::clock_t::now(), id);
	mStockData->Thaw();
	mStockData->Refresh();
}

void pof::StockCheck::OnShowSummary(wxCommandEvent& evt)
{
	wxAuiPaneInfo& pane = m_mgr.GetPane("SummaryPane");
	if (pane.IsOk() && !pane.IsShown()){
		pane.Show();
		m_mgr.Update();
	}
}

void pof::StockCheck::OnAuiThemeChange()
{
	auto auiArtProvider = m_mgr.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::StockCheck::OnStockActivated(wxListEvent& evt)
{
	const wxListItem& item = evt.GetItem();
	mCategorySelect->Freeze();
	mCategorySelect->SetSelection(wxNOT_FOUND);
	mCategorySelect->Thaw();
	mCategorySelect->Refresh();

	if (item.GetId() == mStockSelect->GetItemCount() - 1)
	{
		//adding stock check
		auto today = pof::base::data::clock_t::now();
		int monthCount = date::floor<date::months>(today).time_since_epoch().count() % 12;
		if (wxGetApp().mProductManager.CheckIfMonthStarted(today)) {
			wxMessageBox(fmt::format("Stock check has already started for {},{:%Y}",
				pof::MainFrame::monthNames[monthCount], today), "Stock check", wxICON_INFORMATION | wxOK);
			return;
		}
		mSelectedMonth = (pof::base::data::datetime_t*)(item.GetData());
		wxGetApp().mProductManager.GetStockCheckData()->Clear();
	}
	else {
		pof::base::data::datetime_t* tt = (pof::base::data::datetime_t*)(item.GetData());
		if (tt == nullptr) return;
		mSelectedMonth = tt;
		wxGetApp().mProductManager.LoadStockCheckDate(*tt);

	}
	wxAuiPaneInfo& tool = m_mgr.GetPane("Tools");
	wxAuiPaneInfo& summary = m_mgr.GetPane("SummaryPane");

	if (tool.IsOk() && summary.IsOk()) {
		tool.Show();
		summary.Show();

		m_mgr.Update();
	}

	UpdateSummary();
	mBook->SetSelection(PAGE_STOCK_LIST);
	mBook->Refresh();
	mBackButton->SetActive(true);
	mStockSelect->ClearAll(); 
}

void pof::StockCheck::OnStockSelected(wxListEvent& evt)
{
	const wxListItem& item = evt.GetItem();
	if (item.GetId() == mStockSelect->GetItemCount() - 1){
		return;
	}
	else{
		pof::base::data::datetime_t* tt = (pof::base::data::datetime_t*)(item.GetData());
		if (tt == nullptr) return;
		mSelectedMonth = tt;
		mExpiredStockValues = wxGetApp().mProductManager.GetExpiredProductsStock(*tt);
	}
}

void pof::StockCheck::OnBack(wxCommandEvent& evt)
{
	int sel = mBook->GetSelection();
	if (sel == PAGE_STOCK_LIST){
		mBackButton->SetActive(false);
		wxAuiPaneInfo& tool = m_mgr.GetPane("Tools");
		wxAuiPaneInfo& summary = m_mgr.GetPane("SummaryPane");

		if (tool.IsOk() && summary.IsOk()) {
			tool.Hide();
			summary.Hide();

			m_mgr.Update();
		}
		LoadStockSelect();
	}
}

void pof::StockCheck::OnRemoveStock(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	if (mSelectedMonth == nullptr) return;
	auto month = std::chrono::duration_cast<date::months>(pof::base::data::clock_t::now().time_since_epoch());
	if (*mSelectedMonth <
		(pof::base::data::datetime_t(month) - date::months(1))) {
		//the select month is a month that is less that the previous month
		wxMessageBox("Cannot remove this stock check, month is too far back", "Stock Check", wxICON_WARNING | wxOK);
		return;
	}
	if (mSelections.empty()) {
		wxDataViewItem item = mStockData->GetSelection();
		if (!item.IsOk()) return;
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& datastore = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore();
		const auto& row = datastore[idx];
		if (wxGetApp().mProductManager.CheckIfDone(
			boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]), boost::variant2::get<pof::base::data::datetime_t>(row.first[STOCK_DATE_ADDED]))) {
			wxMessageBox("Cannot remove entry, already marked as Completed", "Stock check", wxICON_INFORMATION | wxOK);
			return;
		}

		if (wxGetApp().mProductManager.RemoveStockEntry(
			boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]), boost::variant2::get<pof::base::data::datetime_t>(row.first[STOCK_DATE_ADDED]))) {
			mStockData->Freeze();
			wxGetApp().mProductManager.GetStockCheckData()->RemoveData(item);
			mStockData->Thaw();
			mStockData->Refresh();
		}
		else {
			wxMessageBox("Failed to remove stock check", "Stock check", wxICON_ERROR | wxOK);
		}
	}
	else {
		mStockData->Freeze();
		auto& datastore = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore();
		wxDataViewItemArray arr;
		for (auto& item : mSelections) {
			if (!item.IsOk()) continue;

			size_t idx = pof::DataModel::GetIdxFromItem(item);
			const auto& row = datastore[idx];
			if (wxGetApp().mProductManager.CheckIfDone(
				boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]), boost::variant2::get<pof::base::data::datetime_t>(row.first[STOCK_DATE_ADDED]))) {
				continue;
			}

			if (wxGetApp().mProductManager.RemoveStockEntry(
				boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]), boost::variant2::get<pof::base::data::datetime_t>(row.first[STOCK_DATE_ADDED]))) {
				arr.push_back(item);
			}
			else {
				wxMessageBox("Failed to remove stock check", "Stock check", wxICON_ERROR | wxOK);
			}
		}
		wxGetApp().mProductManager.GetStockCheckData()->RemoveData(arr);
		mStockData->Thaw();
		mStockData->Refresh();
	}
	UpdateSummary();
}

void pof::StockCheck::OnContextMenu(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	wxMenu* menu = new wxMenu;
	auto& row = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore()[pof::DataModel::GetIdxFromItem(item)];
	std::uint64_t s = boost::variant2::get<std::uint64_t>(row.first[STOCK_STATUS]);
	if (s == DONE) {
		auto mp = menu->Append(ID_STOCK_UNMARK_AS_COMPLETE, "Unmark product as complete", nullptr);
	}
	else if (s == PENDING) {
		auto mp = menu->Append(ID_STOCK_MARK_PROD_AS_COMPLETE, "Mark product as complete", nullptr);
	}
	auto tt = menu->Append(ID_REMOVE_STOCK, "Remove stock check", nullptr);

	mStockData->PopupMenu(menu);
}

void pof::StockCheck::OnReset(wxCommandEvent& evt)
{
	mCategorySelect->Freeze();
	mCategorySelect->SetSelection(wxNOT_FOUND);
	mCategorySelect->Thaw();
	mCategorySelect->Refresh();

	mStockData->Freeze();
	wxGetApp().mProductManager.GetStockCheckData()->Clear();
	wxGetApp().mProductManager.LoadStockCheckDate(*mSelectedMonth);
	mStockData->Thaw();
	mStockData->Refresh();
}

void pof::StockCheck::OnStockSelectRightClick(wxListEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto pp = menu->Append(ID_STOCK_CONSUMPTION_PATTERN, "Consumption pattern", nullptr);
	mStockSelect->PopupMenu(menu);
}

void pof::StockCheck::OnStockConsumptionPattern(wxCommandEvent& evt)
{
	pof::ReportsDialog dialog(this, wxID_ANY, "Consumption pattern");
	if (dialog.LoadReport(pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN, *mSelectedMonth)) dialog.ShowModal();
}

void pof::StockCheck::OnToolUpdateUI(wxUpdateUIEvent& evt)
{
	int sel = mBook->GetSelection();
	bool active = (sel != PAGE_STOCK_SELECT);
	mBackButton->SetActive(active);
	mAddButton->SetActive(active);
}

void pof::StockCheck::OnMarkAsComplete(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Stock check", wxICON_INFORMATION | wxOK);
		return;
	}
	auto& stockCheck = wxGetApp().mProductManager.GetStockCheckData();
	auto& datastore = stockCheck->GetDatastore();
	
	if (evt.GetId() == ID_STOCK_MARK_PROD_AS_COMPLETE)
	{
		if (wxMessageBox("Are you sure you want to mark stock check as complete", "Stock check", wxICON_INFORMATION | wxYES_NO) == wxNO) return;
		wxBusyCursor cur;
		if (mSelections.empty()) {
			auto item = mStockData->GetSelection();
			if (!item.IsOk()) return; //not a valid selection
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& row = datastore[idx];
			mStockData->Freeze();
			row.first[STOCK_STATUS] = static_cast<std::uint64_t>(DONE);
			mStockData->Thaw();
			mStockData->Refresh();

			if (!wxGetApp().mProductManager.MarkStockCheckAsDone(boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]), *mSelectedMonth))
			{
				wxMessageBox("Cannot mark all as stock check", "Stock check", wxICON_ERROR | wxOK);
			}
		}
		else {
			mStockData->Freeze(); 
			for (auto& item : mSelections) {
				if (!item.IsOk()) continue; //not a valid selection
				size_t idx = pof::DataModel::GetIdxFromItem(item);
				auto& row = datastore[idx];
				row.first[STOCK_STATUS] = static_cast<std::uint64_t>(DONE);
				if (!wxGetApp().mProductManager.MarkStockCheckAsDone(boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]), *mSelectedMonth))
				{
					wxMessageBox("Cannot mark all as stock check", "Stock check", wxICON_ERROR | wxOK);
				}
			}
			mStockData->Thaw();
			mStockData->Refresh();
		}
	}
	else {
		if (wxMessageBox("Are you sure you want to mark all stock check products as complete", "Stock check", wxICON_INFORMATION | wxYES_NO) == wxNO) return;
		mStockData->Freeze();
		for (auto& row : datastore) {
			row.first[STOCK_STATUS] = static_cast<std::uint64_t>(DONE);
		}
		mStockData->Thaw();
		mStockData->Refresh();
		if (!wxGetApp().mProductManager.MarkAllAsDone(*mSelectedMonth)) {
			wxMessageBox("Cannot mark all as stock check", "Stock check", wxICON_ERROR | wxOK);
		}
	}
	UpdateSummary();
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Marked stock check as complete");
}

void pof::StockCheck::OnUnmarkAsComplete(wxCommandEvent& evt)
{
	auto item = mStockData->GetSelection();
	if (!item.IsOk()) return;
	if (date::floor<date::months>(pof::base::data::clock_t::now()) !=
		date::floor<date::months>(*mSelectedMonth)) {
		wxMessageBox("Cannot unmark a month that is not the current month", "Stock check", wxICON_INFORMATION | wxOK);
		return;
	}

	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Stock check", wxICON_INFORMATION | wxOK);
		return;
	}
	mStockData->Freeze();
	auto& stockCheck = wxGetApp().mProductManager.GetStockCheckData();
	auto& datastore = stockCheck->GetDatastore();
	auto& puid = boost::variant2::get<pof::base::data::duuid_t>(datastore[pof::DataModel::GetIdxFromItem(item)].first[STOCK_PRODUCT_UUID]);
	if (wxGetApp().mProductManager.UnmarkStockCheckAsDone(puid, *mSelectedMonth)) {
		boost::variant2::get<std::uint64_t>(datastore[pof::DataModel::GetIdxFromItem(item)].first[STOCK_STATUS]) 
			= static_cast<std::uint64_t>(PENDING);
	}
	else {
		wxMessageBox("Failed to unmark product as complete", "Stock check", wxICON_INFORMATION | wxOK);
	}
	mStockData->Thaw();
	mStockData->Refresh();
	UpdateSummary();
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Unmarked stock check as complete");

}

void pof::StockCheck::OnHeaderClicked(wxDataViewEvent& evt)
{
	if (mSelectCol == evt.GetDataViewColumn()) {
		static bool sel = true;
		mStockData->Freeze();
		auto& items = wxGetApp().mProductManager.GetStockCheckData()->GetDataViewItems();
		if (sel) {
			if (!mSelections.empty()) {
				mSelections.clear();
				sel = false;
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
		mStockData->Thaw();
		mStockData->Refresh();
		evt.Veto();
	}
	else {
		evt.Skip();
	}
}

void pof::StockCheck::OnSelect(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		ShowSelect();
	}
	else {
		HideSelect();
	}
}


void pof::StockCheck::ShowSelect()
{
	mSelectCol = mStockData->PrependToggleColumn(wxT("Select"), STOCK_SELECT,
		wxDATAVIEW_CELL_ACTIVATABLE, 50);
}

void pof::StockCheck::HideSelect()
{
	if (mSelectCol != nullptr) {
		mSelections.clear();

		mStockData->Freeze();
		mStockData->DeleteColumn(mSelectCol);
		mStockData->Thaw();
		mStockData->Update();
		mSelectCol = nullptr;
	}
}
