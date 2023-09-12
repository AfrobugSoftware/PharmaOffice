#include "StockCheck.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::StockCheck, wxDialog)
EVT_TOOL(pof::StockCheck::ID_ADD_PRODUCT, pof::StockCheck::OnAddProduct)
EVT_DATE_CHANGED(pof::StockCheck::ID_DATE, pof::StockCheck::OnDate)
//EVT_INIT_DIALOG(pof::StockCheck::OnInitDialog)
EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::StockCheck::ID_STOCK_DATA, pof::StockCheck::OnEditingStarted)
END_EVENT_TABLE()



pof::StockCheck::StockCheck( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 878,689 ), wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	
	mMainPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER );
	m_mgr.AddPane( mMainPane, wxAuiPaneInfo().Name("MainPane").Left().CaptionVisible(false).CloseButton(false).PaneBorder(false).Dock().Resizable().FloatingSize(wxDefaultSize).CentrePane());
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	CreateToolBar();
	mStockData = new wxDataViewCtrl( mMainPane,ID_STOCK_DATA, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES);
	mStockData->AssociateModel(wxGetApp().mProductManager.GetStockCheckData().get());

	mProductName = mStockData->AppendTextColumn( wxT("Product Name"), STOCK_PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 300, wxALIGN_CENTER);
	mCurrenctStock = mStockData->AppendTextColumn( wxT("Currenct Stock"), STOCK_CURRENT_STOCK, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mCheckedStock = mStockData->AppendTextColumn(wxT("Checked Stock"), STOCK_CHECKED_STOCK, wxDATAVIEW_CELL_EDITABLE, 100, wxALIGN_CENTER);
	mShortage = mStockData->AppendTextColumn( wxT("Shortage"), STOCK_SHORTAGE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mStatus = mStockData->AppendBitmapColumn( wxT("Status"), STOCK_STATUS, wxDATAVIEW_CELL_INERT, 300, wxALIGN_CENTER);
	bSizer1->Add( mStockData, 1, wxALL|wxEXPAND, 0 );
	
	
	mMainPane->SetSizer( bSizer1 );
	mMainPane->Layout();
	bSizer1->Fit( mMainPane );
	mSummary = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER );
	mSummary->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	m_mgr.AddPane( mSummary, wxAuiPaneInfo() .Name( wxT("TotalPane") ).Bottom() .Caption( wxT("Summary") ).PinButton( true ).Dock().Resizable().FloatingSize( wxSize( 50,67 ) ).TopDockable( false ).LeftDockable( false ).RightDockable( false ).Floatable( false ).Row( 1 ).MinSize( wxSize( 50,150 ) ) );
	
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
	
	m_mgr.Update();
	this->Centre( wxBOTH );

	OnAuiThemeChange();
	AddSpecialCols();
	mStockCheckMonth->SetValue(wxDateTime::Now());
	wxGetApp().mProductManager.LoadStockCheckDate(pof::base::data::clock_t::now());
	UpdateSummary();
}

void pof::StockCheck::CreateToolBar()
{
	mTools = new wxAuiToolBar(this, ID_TOOL, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTools->AddStretchSpacer();
	mTools->AddSeparator();
	mTools->AddSpacer(5);
	
	mStockCheckMonth = new wxDatePickerCtrl(mTools, ID_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	mTools->AddControl(mStockCheckMonth, "Stock Check Month");
	mTools->AddTool(ID_ADD_PRODUCT, "Add Product", wxArtProvider::GetBitmap("action_add"));

	m_mgr.AddPane(mTools, wxAuiPaneInfo().Name("Tools").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Resizable().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::StockCheck::AddSpecialCols()
{
	auto& datastore = wxGetApp().mProductManager.GetStockCheckData()->GetDatastore();
	pof::DataModel::SpeicalColHandler_t checkedStock;
	pof::DataModel::SpeicalColHandler_t status;
	pof::DataModel::SpeicalColHandler_t shortage;

	checkedStock.second = [&](size_t row, size_t col, const wxVariant& value) -> bool
	{
		auto& datum = datastore[row];
		auto& v = datum.first;
		auto quan = static_cast<std::uint64_t>(atoll(value.GetString().ToStdString().c_str()));
		if (quan > boost::variant2::get<std::uint64_t>(v[STOCK_CURRENT_STOCK])){
			wxMessageBox("Excess stock, please enter excess as inventory and update check to current stock.", "STOCK CHECK", wxICON_INFORMATION | wxOK);
			return false;
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

	auto& dm = wxGetApp().mProductManager.GetStockCheckData();
	dm->SetSpecialColumnHandler(STOCK_STATUS, std::move(status));
	dm->SetSpecialColumnHandler(STOCK_SHORTAGE, std::move(shortage));
	dm->SetSpecialColumnHandler(STOCK_CHECKED_STOCK, std::move(checkedStock));
	
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
 				v.resize(5);


				v[STOCK_PRODUCT_UUID] = pid;
				v[STOCK_PRODUCT_NAME] = p[pof::ProductManager::PRODUCT_NAME];
				v[STOCK_CURRENT_STOCK] = p[pof::ProductManager::PRODUCT_STOCK_COUNT];
				v[STOCK_CHECKED_STOCK] = static_cast<std::uint64_t>(0);
				v[STOCK_STATUS] = static_cast<std::uint64_t>(PENDING);

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

			v.resize(5);


			v[STOCK_PRODUCT_UUID] = pid;
			v[STOCK_PRODUCT_NAME] = p[pof::ProductManager::PRODUCT_NAME];
			v[STOCK_CURRENT_STOCK] = p[pof::ProductManager::PRODUCT_STOCK_COUNT];
			v[STOCK_CHECKED_STOCK] = static_cast<std::uint64_t>(0);
			v[STOCK_STATUS] = static_cast<std::uint64_t>(PENDING);

			wxGetApp().mProductManager.GetStockCheckData()->EmplaceData(std::move(rowx));
			wxGetApp().mProductManager.InsertProductInStockCheck(pid);
		}
		UpdateSummary();
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
	wxGetApp().mProductManager.LoadStockCheckDate(curTime);
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

void pof::StockCheck::OnAuiThemeChange()
{
	auto auiArtProvider = m_mgr.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}
