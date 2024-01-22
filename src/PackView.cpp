#include "PackView.h"
#include "PofPch.h"
//#include "Application.h"

BEGIN_EVENT_TABLE(pof::PackView, wxDialog)
EVT_TOOL(pof::PackView::ID_TOOL_CREATE_PACK, pof::PackView::OnCreatePack)
EVT_TOOL(pof::PackView::ID_TOOL_ADD_PACK, pof::PackView::OnAddPack)
EVT_TOOL(wxID_BACKWARD, pof::PackView::OnBack)
EVT_TOOL(pof::PackView::ID_SALE_PACK, pof::PackView::OnSalePack)
EVT_TOOL(pof::PackView::ID_TOOL_ADD_PRODUCT_PACK, pof::PackView::OnAddProductPack)
EVT_TOOL(pof::PackView::ID_TOOL_REMOVE_PRODUCT_PACK, pof::PackView::OnRemoveProductPack)
EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::PackView::ID_PACK_DATA, pof::PackView::OnColEdited)
EVT_LIST_ITEM_SELECTED(pof::PackView::ID_PACK_SELECT, pof::PackView::OnPackSelected)
EVT_LIST_ITEM_ACTIVATED(pof::PackView::ID_PACK_SELECT, pof::PackView::OnPackActivate)
//EVT_LIST_BEGIN_LABEL_EDIT(pof::PackView::ID_PACK_SELECT, pof::PackView::OnEditPackName)
EVT_LIST_END_LABEL_EDIT(pof::PackView::ID_PACK_SELECT, pof::PackView::OnEditPackName)
EVT_LIST_ITEM_RIGHT_CLICK(pof::PackView::ID_PACK_SELECT, pof::PackView::OnRightClick)

EVT_BUTTON(pof::PackView::ID_TOOL_ADD_PRODUCT_PACK, pof::PackView::OnAddProductPack)
EVT_BUTTON(pof::PackView::ID_TOOL_ADD_PACK, pof::PackView::OnAddPack)

EVT_MENU(pof::PackView::ID_RENAME_PACK, pof::PackView::OnRenamePack)
EVT_MENU(pof::PackView::ID_TOOL_REMOVE_PACK, pof::PackView::OnRemovePack)
EVT_MENU(pof::PackView::ID_OPEN_PACK, pof::PackView::OnOpenPack)
END_EVENT_TABLE()


pof::PackView::PackView( wxWindow* parent, bool showSale, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : 
	wxDialog( parent, id, wxT("Pharmacy Packs"), pos, size, style),
	mShowSale(showSale)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	mTopTools = new wxAuiToolBar( this, ID_TOOL, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW);
	mTopTools->SetToolBitmapSize( wxSize( 16,16 ) );
	mTopTools->SetMinSize( wxSize( -1, 40 ) );
	
	mTopTools->AddTool(ID_TOOL_ADD_PACK, "Add Pack", wxArtProvider::GetBitmap("action_add"), "Add a new pack");
	mTopTools->AddStretchSpacer();
	if (mShowSale) {
		mTopTools->AddTool(ID_SALE_PACK, "Sell Pack", wxArtProvider::GetBitmap("application"), "Sell pack");
	}

	mTopTools->Realize(); 
	
	bSizer1->Add( mTopTools, 0, wxALL|wxEXPAND, 5 );
	
	mBook = new wxSimplebook( this, ID_BOOK, wxDefaultPosition, wxDefaultSize, 0 );
	mPackView = new wxPanel( mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	mPackSelect = new wxListCtrl( mPackView, ID_PACK_SELECT, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxFULL_REPAINT_ON_RESIZE | wxLC_EDIT_LABELS | wxNO_BORDER);
	bSizer2->Add(mPackSelect, 1, wxALL|wxEXPAND, 2 );
	wxImageList* imagelist = new wxImageList(60, 60);
	imagelist->Add(wxArtProvider::GetBitmap("cart"));
	mPackSelect->AssignImageList(imagelist, wxIMAGE_LIST_NORMAL);
	
	mPackView->SetSizer( bSizer2 );
	mPackView->Layout();
	bSizer2->Fit( mPackView );
	mBook->AddPage( mPackView, wxT("a page"), true );
	mPackData = new wxPanel( mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_dataViewCtrl3 = new wxDataViewCtrl( mPackData, ID_PACK_DATA, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	mPackModel = new pof::DataModel();

	m_dataViewCtrl3->AssociateModel(mPackModel);
	mPackModel->DecRef();
	CreateSpeicalCols();


	mPackModel->Adapt<
		pof::base::data::duuid_t,
		pof::base::data::text_t,
		std::uint64_t, //quantity
		std::uint64_t, //package size
		pof::base::data::currency_t, //current price
		pof::base::data::currency_t
	>();

	mProductName = m_dataViewCtrl3->AppendTextColumn( wxT("Name"), 1, wxDATAVIEW_CELL_INERT, 500);
	mProductQuantity = m_dataViewCtrl3->AppendTextColumn( wxT("Quantity"), 2, wxDATAVIEW_CELL_EDITABLE, 100);
	mPackageSize = m_dataViewCtrl3->AppendTextColumn( wxT("Package Size"),  3);
	mPrice = m_dataViewCtrl3->AppendTextColumn( wxT("Price"), 4);
	mExtPrice = m_dataViewCtrl3->AppendTextColumn( wxT("Exact Price"), 5);
	bSizer3->Add( m_dataViewCtrl3, 1, wxALL|wxEXPAND, 2 );
	
	m_panel4 = new wxPanel( mPackData, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	
	mTotalQuantity = new wxStaticText( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mTotalQuantity->SetFont(wxFontInfo(11).AntiAliased());
	mTotalQuantity->Wrap(-1);
	bSizer4->Add( mTotalQuantity, 0, wxALL, 5 );
	
	
	bSizer4->AddSpacer(5);

	bSizer4->Add(new wxStaticLine(m_panel4, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(5);

		
	mTotalAmount = new wxStaticText( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mTotalAmount->SetFont(wxFontInfo(11).AntiAliased());
	mTotalAmount->Wrap( -1 );
	bSizer4->Add( mTotalAmount, 0, wxALL, 5 );
	
	
	m_panel4->SetSizer( bSizer4 );
	m_panel4->Layout();
	bSizer4->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 0, wxEXPAND | wxALL, 0 );
	
	
	mPackData->SetSizer( bSizer3 );
	mPackData->Layout();
	bSizer3->Fit( mPackData );

	CreateEmptyPackPanel();
	CreateEmptyPanel();
	mBook->AddPage(mPackView, wxT("pack view"), false); //1
	mBook->AddPage( mPackData, wxT("pack data"), false ); //2
	mBook->AddPage(mEmptyPack, wxT("empty pack"), false); //3
	
	bSizer1->Add( mBook, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );

	LoadPackDescSelect();

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

pof::PackView::~PackView()
{
}

bool pof::PackView::TransferDataFromWindow()
{
	return true;
}

std::vector<pof::ProductManager::packType> pof::PackView::GetPackProducts() const
{
	if (mCurPackId == nullptr) return {};
	auto opt = wxGetApp().mProductManager.GetProductPack(*mCurPackId);
	if (!opt.has_value()) {
		spdlog::error("error in getting pack product");
		return {};
	}
	else {
		return opt.value();
	}
}

void pof::PackView::CreateEmptyPackPanel()
{
	mEmptyPack = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmptyPack, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
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

	
	wxButton* btn = new wxButton(m7, ID_TOOL_ADD_PRODUCT_PACK);
	btn->SetBitmap(wxArtProvider::GetBitmap("action_add"));
	btn->SetLabel("Add product to pack");
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


	mEmptyPack->SetSizer(bSizer6);
	mEmptyPack->Layout();
}

void pof::PackView::CreateEmptyPanel()
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

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No packs created in pharmacy"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	wxButton* btn = new wxButton(m7, ID_TOOL_ADD_PACK);
	btn->SetBitmap(wxArtProvider::GetBitmap("action_add"));
	btn->SetLabel("Create new pack");
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

	mBook->AddPage(mEmpty, "Empty", true);
}

void pof::PackView::CreatePackTools()
{
	mTopTools->Freeze();
	mTopTools->Clear();

	mTopTools->AddTool(wxID_BACKWARD, wxT("Back"), wxArtProvider::GetBitmap("arrow_back"), "Back");
	//mTopTools->AddSpacer(20);

	if (mPackText) {
		mPackText->Destroy();
		mPackText = nullptr;
	}
	mPackText = new wxStaticText(mTopTools, wxID_ANY, fmt::format("{}", mPackName));
	mPackText->SetFont(wxFontInfo().AntiAliased().Bold());
	mPackText->SetBackgroundColour(*wxWHITE);
	mTopTools->AddSeparator();
	mTopTools->AddSpacer(2);
	mTextItem = mTopTools->AddControl(mPackText);
	mTextItem->SetAlignment(wxALIGN_BOTTOM);

	mTopTools->AddStretchSpacer();
	mTopTools->AddTool(ID_TOOL_ADD_PRODUCT_PACK, wxT("Add Product"), wxArtProvider::GetBitmap("action_add"), "Add product to pack");
	mTopTools->AddTool(ID_TOOL_REMOVE_PRODUCT_PACK, wxT("Remove Product"), wxArtProvider::GetBitmap("action_remove"), "Remove product from pack");
	mTopTools->Realize();
	mTopTools->Thaw();
	mTopTools->Refresh();
}

void pof::PackView::CreateTopTools()
{
	mTopTools->Freeze();
	mTopTools->Clear();

	mTopTools->AddTool(ID_TOOL_ADD_PACK, "Add Pack", wxArtProvider::GetBitmap("action_add"), "Add a new pack");
	mTopTools->AddStretchSpacer();
	if (mShowSale) {
		mTopTools->AddTool(ID_SALE_PACK, "Sell Pack", wxArtProvider::GetBitmap("application"), "Sell pack");
	}	

	if (mPackText) {
		mPackText->Destroy();
		mPackText = nullptr;
	}

	mTopTools->Realize();
	mTopTools->Thaw();
	mTopTools->Refresh();
}

void pof::PackView::CreateSpeicalCols()
{
	pof::DataModel::SpeicalColHandler_t quan;
	quan.second = [&](size_t row, size_t col, const wxVariant& val) -> bool {
		wxBusyCursor cursoir;
		auto& datastore = mPackModel->GetDatastore();
		auto& r = datastore[row];
		auto& prodId = boost::variant2::get<pof::base::data::duuid_t>(r.first[0]);
		auto& quantity = boost::variant2::get<std::uint64_t>(r.first[2]);
		auto& price = boost::variant2::get<pof::base::data::currency_t>(r.first[4]);
		auto& extprice = boost::variant2::get<pof::base::data::currency_t>(r.first[5]);

		quantity = val.GetInteger();
		extprice = price * static_cast<double>(quantity);

		pof::ProductManager::packType pk;
		std::get<0>(pk) = *mCurPackId;
		std::get<1>(pk) = prodId;
		std::get<3>(pk) = quantity;
		std::get<6>(pk) = extprice;

		wxGetApp().mProductManager.UpdateProductPack(*mCurPackId, std::move(pk));

		UpdateTotals();
		return true;
	};

	mPackModel->SetSpecialColumnHandler(2,std::move(quan));
}

void pof::PackView::ShowPack()
{
}

void pof::PackView::OnPackActivate(wxListEvent& evt)
{
	const wxListItem& item = evt.GetItem();
	const std::string value = item.GetText().ToStdString();
	boost::uuids::uuid* id = reinterpret_cast<boost::uuids::uuid*>(item.GetData());
	mPackName = std::move(item.GetText().ToStdString());

	if (id == nullptr) return;

	auto packRelation = wxGetApp().mProductManager.GetProductPack(*id);
	if (!packRelation.has_value()) {
		wxMessageBox("Error trying to get pack", "PACK", wxICON_ERROR | wxOK);
		return;
	}

	CreatePackTools();
	mPackModel->Clear();

	if (packRelation.value().empty()) {
		mBook->SetSelection(PACK_PRODUCT_EMPTY); //show empty pack
		return; 
	}
	for (auto& pk : *packRelation) {
		pof::base::data::row_t row;
		auto& v = row.first;
		v.resize(6);
		v[0] = std::get<1>(pk);
		v[1] = std::get<2>(pk);
		v[2] = std::get<3>(pk);
		v[3] = std::get<4>(pk);
		v[4] = std::get<5>(pk);
		v[5] = std::get<6>(pk);
		

		mPackModel->EmplaceData(std::move(row));
	}
	UpdateTotals();
	if (!mPackModel->GetDatastore().empty())
	{
		mCurPackId = id;
		mBook->SetSelection(PACK_DATA); //show the pack view
	}
	else {
		mBook->SetSelection(PACK_PRODUCT_EMPTY); //show the pack view
	}
}

void pof::PackView::OnEditPackName(wxListEvent& evt)
{
	if (evt.IsEditCancelled()) return;

	auto& item = evt.GetItem();
	auto packID = reinterpret_cast<boost::uuids::uuid*>(item.GetData());
	if (!packID) {
		evt.Veto();
		return;
	}

	auto name = evt.GetLabel().ToStdString();
	if (name.empty()){
		evt.Veto();
		return;
	}
	
	//check for duplicates
	for (int i = 0; i < mPackSelect->GetItemCount(); i++){
		if (name == mPackSelect->GetItemText(i)) {
			wxMessageBox("Name already exisits", "PACK", wxICON_INFORMATION | wxOK);
			evt.Veto();
			return; 
		}
	}

	//save to database
	bool status = wxGetApp().mProductManager.UpdatePackDesc({ *packID, name, pof::base::data::text_t{} });
	if (!status) {
		evt.Veto();
	}else evt.Skip();
}

void pof::PackView::OnPackSelected(wxListEvent& evt)
{
	auto& item = evt.GetItem();
	mCurPackId = reinterpret_cast<boost::uuids::uuid*>(item.GetData());
	mPackName = std::move(item.GetText().ToStdString());
}

void pof::PackView::OnAddPack(wxCommandEvent& evt)
{
	wxTextEntryDialog dialog(this, "Please enter the name of the pack", "Pack Entry");
	if (dialog.ShowModal() == wxID_OK) {
		std::string value = dialog.GetValue().ToStdString();
		if (value.empty()) return;


		pof::ProductManager::packDescType pdesc;
		std::get<0>(pdesc) = wxGetApp().mProductManager.UuidGen();
		std::get<1>(pdesc) = value;


		mPackSelect->Freeze();
		size_t i = mPackSelect->GetItemCount();
		wxListItem item;
		item.SetId(i);
		item.SetData(new boost::uuids::uuid(std::get<0>(pdesc)));
		item.SetText(value);
		item.SetImage(0);
		item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT | wxLIST_MASK_DATA);

		mPackSelect->InsertItem(std::move(item));

		mPackSelect->Thaw();

		if (!wxGetApp().mProductManager.CreatePack(std::move(pdesc))) {
			wxMessageBox("Error creating pack", "PACK", wxICON_ERROR | wxOK);
		}
		mBook->SetSelection(PACK_VIEW);
	}


	if (mPackSelect->IsEmpty()){
		mBook->SetSelection(PACK_EMPTY);
	}
}

void pof::PackView::OnCreatePack(wxCommandEvent& evt)
{
}

void pof::PackView::OnRemovePack(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to remove pack?", "PACK", wxICON_QUESTION | wxYES_NO) == wxNO) return;
	wxGetApp().mProductManager.RemovePack(*mCurPackId);

	mPackSelect->ClearAll();
	LoadPackDescSelect();

	if (mPackSelect->IsEmpty()) mBook->SetSelection(PACK_EMPTY);
}

void pof::PackView::OnAddProductPack(wxCommandEvent& evt)
{
	pof::SearchProduct dialog(this);
	if (dialog.ShowModal() == wxID_OK) {
		if (dialog.HasMultipleSelections()) {
			auto vec = dialog.GetSelectedProducts();
			for (auto& prod : vec){
				auto& row = prod.get();
				auto& rowId = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
				if (wxGetApp().mProductManager.ExistsInPack(*mCurPackId, rowId)) {
					wxMessageBox(fmt::format("\'{}\' already exisit in pack",
						boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_NAME])), "PACK", wxICON_WARNING | wxOK);
					continue;
				}

				pof::ProductManager::packType pk;
				std::get<0>(pk) = *mCurPackId;
				std::get<1>(pk) = rowId;
				std::get<2>(pk) = boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_NAME]);
				std::get<3>(pk) = static_cast<std::uint64_t>(1);
				std::get<5>(pk) = boost::variant2::get<pof::base::data::currency_t>(row.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
				std::get<6>(pk) = boost::variant2::get<pof::base::data::currency_t>(row.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);

				wxGetApp().mProductManager.AddProductPack(std::move(pk));

				pof::base::data::row_t r;
				auto& v = r.first;
				v.resize(6);

				v[0] = std::get<1>(pk);
				v[1] = std::get<2>(pk);
				v[2] = std::get<3>(pk);
				v[3] = std::get<4>(pk);
				v[4] = std::get<5>(pk);
				v[5] = std::get<6>(pk);


				mPackModel->EmplaceData(std::move(r));
			}
		}
		else {
			auto& row = dialog.GetSelectedProduct();

			auto& rowId = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
			if (wxGetApp().mProductManager.ExistsInPack(*mCurPackId, rowId)) {
				wxMessageBox(fmt::format("\'{}\' already exisit in pack",
					boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_NAME])), "PACK", wxICON_WARNING | wxOK);
				return;
			}

			pof::ProductManager::packType pk;
			std::get<0>(pk) = *mCurPackId;
			std::get<1>(pk) = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
			std::get<2>(pk) = boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_NAME]);
			std::get<3>(pk) = static_cast<std::uint64_t>(1);
			std::get<5>(pk) = boost::variant2::get<pof::base::data::currency_t>(row.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
			std::get<6>(pk) = boost::variant2::get<pof::base::data::currency_t>(row.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);

			wxGetApp().mProductManager.AddProductPack(std::move(pk));

			pof::base::data::row_t r;
			auto& v = r.first;
			v.resize(6);
			v[0] = std::get<1>(pk);
			v[1] = std::get<2>(pk);
			v[2] = std::get<3>(pk);
			v[3] = std::get<4>(pk);
			v[4] = std::get<5>(pk);
			v[5] = std::get<6>(pk);


			mPackModel->EmplaceData(std::move(r));
		}
		UpdateTotals();
		mBook->SetSelection(PACK_DATA);
	}
	if (mPackModel->GetDatastore().empty()){
		mBook->SetSelection(PACK_PRODUCT_EMPTY);
	}
}

void pof::PackView::OnRemoveProductPack(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl3->GetSelection();
	if (!item.IsOk()) return;
	
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& datastore = mPackModel->GetDatastore();
	auto& row = datastore[idx];

	pof::base::data::duuid_t& prodUuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[0]);
	wxGetApp().mProductManager.RemoveProductPack(*mCurPackId, prodUuid);

	mPackModel->RemoveData(item);
	if (datastore.empty()) {
		mBook->SetSelection(PACK_PRODUCT_EMPTY);
	}
	else {
		UpdateTotals();
	}
}

void pof::PackView::OnBack(wxCommandEvent& evt)
{
	CreateTopTools();
	if (mPackSelect->IsEmpty())
	{
		mBook->SetSelection(PACK_PRODUCT_EMPTY);
	}else{
		mBook->SetSelection(PACK_VIEW);
	}
}

void pof::PackView::OnSalePack(wxCommandEvent& evt)
{
	if (mCurPackId == nullptr) EndModal(wxID_CANCEL);
	else {
		EndModal(wxID_OK);
	}
}

void pof::PackView::OnColEdited(wxDataViewEvent& evt)
{
	auto col = evt.GetDataViewColumn();
	auto Ctrl = col->GetRenderer()->GetEditorCtrl();
	wxIntegerValidator<size_t> val{ NULL };
	val.SetMin(0);
	val.SetMax(10000);
	Ctrl->SetValidator(val);
}

void pof::PackView::OnRightClick(wxListEvent& evt)
{
	mSelItem = evt.GetItem();
	wxMenu* menu = new wxMenu;
	auto op = menu->Append(ID_OPEN_PACK, "Open pack", nullptr);
	auto rn = menu->Append(ID_RENAME_PACK, "Rename", nullptr);
	auto rv = menu->Append(ID_TOOL_REMOVE_PACK, "Remove", nullptr);

	PopupMenu(menu);
}

void pof::PackView::OnRenamePack(wxCommandEvent& evt)
{
	int id = mSelItem.GetId();
	if (id == wxNOT_FOUND) return;
	mPackSelect->EditLabel(id);
}

void pof::PackView::OnOpenPack(wxCommandEvent& evt)
{
	boost::uuids::uuid* id = mCurPackId;
	if (id == nullptr) return;

	auto packRelation = wxGetApp().mProductManager.GetProductPack(*id);
	if (!packRelation.has_value()) {
		wxMessageBox("Error trying to get pack", "PACK", wxICON_ERROR | wxOK);
		return;
	}

	CreatePackTools();
	mPackModel->Clear();

	if (packRelation.value().empty()) {
		mBook->SetSelection(PACK_PRODUCT_EMPTY); //show empty pack
		return;
	}
	for (auto& pk : *packRelation) {
		pof::base::data::row_t row;
		auto& v = row.first;
		v.resize(6);
		v[0] = std::get<1>(pk);
		v[1] = std::get<2>(pk);
		v[2] = std::get<3>(pk);
		v[3] = std::get<4>(pk);
		v[4] = std::get<5>(pk);
		v[5] = std::get<6>(pk);


		mPackModel->EmplaceData(std::move(row));
	}
	UpdateTotals();
	if (!mPackModel->GetDatastore().empty())
	{
		mBook->SetSelection(PACK_DATA); //show the pack view
	}
	else {
		mBook->SetSelection(PACK_PRODUCT_EMPTY); //show the pack view
	}
}

void pof::PackView::UpdateTotals()
{
	auto& datastore = mPackModel->GetDatastore();
	pof::base::currency totalAmount;
	std::uint64_t extQuantity = 0;

	for (auto& d : datastore)
	{
		totalAmount += boost::variant2::get<pof::base::currency>(d.first[5]);
		extQuantity += boost::variant2::get<std::uint64_t>(d.first[2]);
	}

	m_panel4->Freeze();
	mTotalQuantity->SetLabel(fmt::format("Total Quantity:  {:d}", extQuantity));
	mTotalAmount->SetLabel(fmt::format("Total Amount:  {:cu}", totalAmount));
	m_panel4->Thaw();

	m_panel4->Layout();
	m_panel4->Refresh();
}

void pof::PackView::LoadPackDescSelect()
{
	auto rel = wxGetApp().mProductManager.GetPackDesc();
	if (!rel.has_value()) {
		wxMessageBox("Error in loading Packs", "PACK", wxICON_ERROR | wxOK);
		return;
	}
	auto& packs = rel.value();
	if (packs.empty()) return;

	mPackSelect->Freeze();
	for (auto& pk : packs) {
		size_t i = mPackSelect->GetItemCount();
		wxListItem item;
		item.SetId(i);
		item.SetData(new boost::uuids::uuid(std::get<0>(pk)));
		item.SetText(std::get<1>(pk));
		item.SetImage(0);
		item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT | wxLIST_MASK_DATA);

		mPackSelect->InsertItem(std::move(item));

	}
	mPackSelect->Thaw();
	mPackSelect->Refresh();
	mPackSelect->Update();

	mBook->SetSelection(PACK_VIEW);
}

void pof::PackView::LoadPackModel()
{
}
