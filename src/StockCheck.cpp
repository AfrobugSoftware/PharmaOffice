#include "StockCheck.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::StockCheck, wxDialog)
EVT_TOOL(pof::StockCheck::ID_ADD_PRODUCT, pof::StockCheck::OnAddProduct)
EVT_TOOL(pof::StockCheck::ID_SHOW_SUMMARY, pof::StockCheck::OnShowSummary)
EVT_TOOL(wxID_BACKWARD, pof::StockCheck::OnBack)
EVT_TOOL(wxID_RESET, pof::StockCheck::OnReset)
EVT_DATE_CHANGED(pof::StockCheck::ID_DATE, pof::StockCheck::OnDate)
EVT_CHOICE(pof::StockCheck::ID_CATEGORY_SELECT, pof::StockCheck::OnCategorySelected)
EVT_LIST_ITEM_ACTIVATED(pof::StockCheck::ID_STOCK_SELECT, pof::StockCheck::OnStockActivated)
EVT_INIT_DIALOG(pof::StockCheck::OnInitDialog)
EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::StockCheck::ID_STOCK_DATA, pof::StockCheck::OnEditingStarted)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::StockCheck::ID_STOCK_DATA, pof::StockCheck::OnContextMenu)
EVT_LIST_ITEM_RIGHT_CLICK(pof::StockCheck::ID_STOCK_SELECT, pof::StockCheck::OnStockSelectRightClick)
EVT_MENU(pof::StockCheck::ID_STOCK_CONSUMPTION_PATTERN, pof::StockCheck::OnStockConsumptionPattern)
EVT_MENU(pof::StockCheck::ID_REMOVE_STOCK, pof::StockCheck::OnRemoveStock)
END_EVENT_TABLE()



pof::StockCheck::StockCheck( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 1178,689 ), wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	
	mBook = new wxSimplebook(this, ID_BOOK);
	m_mgr.AddPane( mBook, wxAuiPaneInfo().Name("MainPane").CaptionVisible(false).CloseButton(false).PaneBorder(false).Dock().Resizable().FloatingSize(wxDefaultSize).CentrePane());

	mMainPane = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	CreateToolBar();
	mStockData = new wxDataViewCtrl( mMainPane,ID_STOCK_DATA, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	mStockData->AssociateModel(wxGetApp().mProductManager.GetStockCheckData().get());

	mProductName = mStockData->AppendTextColumn( wxT("Product Name"), STOCK_PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 300, wxALIGN_CENTER);
	mCurrenctStock = mStockData->AppendTextColumn( wxT("Currenct Stock"), STOCK_CURRENT_STOCK, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mCheckedStock = mStockData->AppendTextColumn(wxT("Checked Stock"), STOCK_CHECKED_STOCK, wxDATAVIEW_CELL_EDITABLE, 100, wxALIGN_CENTER);
	mShortage = mStockData->AppendTextColumn( wxT("Shortage"), STOCK_SHORTAGE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mStatus = mStockData->AppendBitmapColumn( wxT("Status"), STOCK_STATUS, wxDATAVIEW_CELL_INERT, 50, wxALIGN_CENTER);
	mDateAdded = mStockData->AppendTextColumn(wxT("Date Added"), STOCK_DATE_ADDED, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT);
	
	bSizer1->Add( mStockData, 1, wxALL|wxEXPAND, 0 );
	
	
	mMainPane->SetSizer( bSizer1 );
	mMainPane->Layout();
	bSizer1->Fit( mMainPane );

	CreateEmptyStockCheck();
	CreateStockSelect();

	mBook->AddPage(mEmptyPanel, "Stock Empty", false);
	mBook->AddPage(mMainPane, "Stock List", false);
	mBook->AddPage(mStockSelect, "Stock Select", false);


	mSummary = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER );
	mSummary->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	m_mgr.AddPane( mSummary, wxAuiPaneInfo() .Name( wxT("SummaryPane") ).Bottom() .Caption( wxT("Summary") ).PinButton( true ).Dock().Resizable().FloatingSize(wxDefaultSize).TopDockable( false ).LeftDockable( false ).RightDockable( false ).Floatable( false ).Row( 1 ).MinSize( wxSize( 50,150 ) ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 0, 2, 2, 2 );
	
	mTotalStockCheckedLabel = new wxStaticText(mSummary, wxID_ANY, wxT("Total Stock Checked:"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalStockCheckedLabel->Wrap(-1);
	gSizer1->Add(mTotalStockCheckedLabel, 0, wxALIGN_CENTER | wxALL, 2);

	mTotalStockCheckedValue = new wxStaticText(mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalStockCheckedValue->Wrap(-1);
	gSizer1->Add(mTotalStockCheckedValue, 0, wxALIGN_CENTER | wxALL, 0);

	mTotalShortageLabel = new wxStaticText(mSummary, wxID_ANY, wxT("Total Shortage"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalShortageLabel->Wrap(-1);
	gSizer1->Add(mTotalShortageLabel, 0, wxALL, 5);

	mTotalShortageValue = new wxStaticText(mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalShortageValue->Wrap(-1);
	gSizer1->Add(mTotalShortageValue, 0, wxALIGN_CENTER | wxALL, 5);

	mShortageAmountLabel = new wxStaticText(mSummary, wxID_ANY, wxT("Shortage Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	mShortageAmountLabel->Wrap(-1);
	gSizer1->Add(mShortageAmountLabel, 0, wxALL, 5);

	mShortageAmountValue = new wxStaticText(mSummary, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mShortageAmountValue->Wrap(-1);
	gSizer1->Add(mShortageAmountValue, 0, wxALIGN_CENTER | wxALL, 5);


	bSizer2->Add(gSizer1, 0, wxALIGN_TOP, 0);
	
	
	mSummary->SetSizer( bSizer2 );
	mSummary->Layout();
	bSizer2->Fit( mSummary );
	
	this->Centre( wxBOTH );

	OnAuiThemeChange();
	AddSpecialCols();
	mStockCheckMonth->SetValue(wxDateTime::Now());

	/*auto curTime = pof::base::data::clock_t::now();
	auto month = std::chrono::duration_cast<date::months>(curTime.time_since_epoch());
	wxGetApp().mProductManager.LoadStockCheckDate(pof::base::data::datetime_t(month));*/

	LoadStockSelect();
	UpdateSummary();
	m_mgr.Update();
}

void pof::StockCheck::CreateToolBar()
{
	mTools = new wxAuiToolBar(this, ID_TOOL, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mBackButton = mTools->AddTool(wxID_BACKWARD, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	mTools->AddStretchSpacer();
	mTools->AddSeparator();
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
	auto item = mTools->AddTool(wxID_RESET, "Reset", wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR));
	mTools->AddTool(ID_SHOW_SUMMARY, "Show Summary", wxNullBitmap, "Show the summary of the stock check");
	mTools->AddSpacer(5);
	mStockCheckMonth = new wxDatePickerCtrl(mTools, ID_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	auto firstMonth = wxGetApp().mProductManager.GetFirstStockMonth();
	if (!firstMonth.has_value()){
		firstMonth.emplace();
	}
	mStockCheckMonth->SetRange(wxDateTime(std::chrono::system_clock::to_time_t(firstMonth.value())), wxDateTime::Now());

	mTools->AddControl(mStockCheckMonth, "Stock Check Month");
	mTools->AddSpacer(5);
	mTools->AddTool(ID_ADD_PRODUCT, "Add Product", wxArtProvider::GetBitmap("action_add"));

	m_mgr.AddPane(mTools, wxAuiPaneInfo().Name("Tools").ToolbarPane().Top().MinSize(-1, 30).Resizable().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::StockCheck::CreateStockSelectToolbar()
{
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
}

void pof::StockCheck::CreateStockSelect()
{
	mStockSelect = new wxListCtrl(mBook, ID_STOCK_SELECT, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER | wxLC_HRULES | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxFULL_REPAINT_ON_RESIZE | wxLC_EDIT_LABELS | wxNO_BORDER);

	wxImageList* imagelist = new wxImageList(16, 16);
	imagelist->Add(wxArtProvider::GetBitmap("folder_files"));
	mStockSelect->AssignImageList(imagelist, wxIMAGE_LIST_SMALL);

	mStockSelect->AppendColumn("Stock date", wxLIST_FORMAT_CENTER, 500);
	mStockSelect->AppendColumn("Stock date", wxLIST_FORMAT_LEFT, 100);
	mStockSelect->AppendColumn("Stock date", wxLIST_FORMAT_LEFT, 100);

}

void pof::StockCheck::LoadStockSelect()
{
	auto stockStatus = wxGetApp().mProductManager.GetStockMonthStatus();
	if (!stockStatus.has_value() || stockStatus->empty())
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
		item.SetText(fmt::format("Stock checked on {:%m/%y}", std::get<0>(tup)));
		item.SetImage(0);
		item.SetData(new pof::base::data::datetime_t(std::get<0>(tup)));
		item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_DATA | wxLIST_MASK_IMAGE);
		item.SetColumn(0);
		mStockSelect->InsertItem(item);

		item.SetId(i);
		item.SetText(fmt::format("{:d} out of {:d}", std::get<1>(tup), sz));		
		item.SetMask(wxLIST_MASK_TEXT);
		item.SetColumn(1);
		mStockSelect->SetItem(item);

		item.SetId(i);
		if (std::get<2>(tup)) {
			item.SetText("COMPLETE");
		}
		else {
			item.SetText("INCOMPLETE");
		}
		item.SetMask(wxLIST_MASK_TEXT);
		item.SetColumn(2);
		mStockSelect->SetItem(item);
		i++;
	}
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
		v[STOCK_STATUS] = DONE;
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
		return fmt::format("{:%d/%m/%y}", datt);
	};

	auto& dm = wxGetApp().mProductManager.GetStockCheckData();
	dm->SetSpecialColumnHandler(STOCK_STATUS, std::move(status));
	dm->SetSpecialColumnHandler(STOCK_SHORTAGE, std::move(shortage));
	dm->SetSpecialColumnHandler(STOCK_CHECKED_STOCK, std::move(checkedStock));
	dm->SetSpecialColumnHandler(STOCK_DATE_ADDED, std::move(dateAdded));
	
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

	mTotalStockCheckedValue->SetLabel(fmt::format("{:d} of {:d}", datastore.size(), prodStore.size()));
	mTotalShortageValue->SetLabel(fmt::format("{:d}", totalShortage));


	mSummary->Layout();
	mSummary->Thaw();
	mSummary->Update();
}

void pof::StockCheck::OnAddProduct(wxCommandEvent& evt)
{
	auto date = mStockCheckMonth->GetValue();
	pof::base::data::datetime_t curTime = pof::base::data::clock_t::from_time_t(date.GetTicks());
	pof::base::data::datetime_t today = pof::base::data::clock_t::now();

	auto curMonth = std::chrono::duration_cast<date::months>(curTime.time_since_epoch());
	auto tMonth = std::chrono::duration_cast<date::months>(today.time_since_epoch());
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
			wxGetApp().mProductManager.CaptureStock(pid);
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
}

//void pof::StockCheck::OnMonthChange(wxDateEvent& evt)
//{
//	auto& date = evt.GetDate();
//	pof::base::data::datetime_t curTime = pof::base::data::clock_t::from_time_t(date.GetTicks());
//	wxGetApp().mProductManager.LoadStockCheckDate(curTime);	
//}

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
	pof::base::data::datetime_t* tt = (pof::base::data::datetime_t*)(item.GetData());
	if (tt == nullptr) return;
	mSelectedMonth = tt;
	wxGetApp().mProductManager.LoadStockCheckDate(*tt);
	UpdateSummary();
	mBook->SetSelection(PAGE_STOCK_LIST);
	mBook->Refresh();
	mBackButton->SetActive(true);
}

void pof::StockCheck::OnBack(wxCommandEvent& evt)
{
	int sel = mBook->GetSelection();
	if (sel == PAGE_STOCK_LIST){
		mBackButton->SetActive(false);
		if (mStockSelect->GetItemCount() != 0)
		{
			mBook->SetSelection(PAGE_STOCK_SELECT);
			mBook->Refresh();
		}
		else {
			LoadStockSelect();
		}
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
	wxDataViewItem item = mStockData->GetSelection();
	if (!item.IsOk()) return;
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& datastore = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore();
	const auto& row = datastore[idx];

	if (wxGetApp().mProductManager.RemoveStockEntry(
		boost::variant2::get<pof::base::data::duuid_t>(row.first[STOCK_PRODUCT_UUID]), boost::variant2::get<pof::base::data::datetime_t>(row.first[STOCK_DATE_ADDED]))){
		mStockData->Freeze();
		wxGetApp().mProductManager.GetStockCheckData()->RemoveData(item);
		mStockData->Thaw();
		mStockData->Refresh();
	}
	else {
		wxMessageBox("Failed to remove stock check", "Stock check", wxICON_ERROR | wxOK);
	}
	UpdateSummary();
}

void pof::StockCheck::OnContextMenu(wxDataViewEvent& evt)
{
	if (!evt.GetItem().IsOk()) return;
	wxMenu* menu = new wxMenu;
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
	if (dialog.LoadReport(pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN)) dialog.ShowModal();
}
