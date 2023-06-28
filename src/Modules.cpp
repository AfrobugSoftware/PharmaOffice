#include "Modules.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::Modules, wxPanel)
	EVT_TREE_ITEM_ACTIVATED(pof::Modules::ID_TREE, pof::Modules::OnActivated)
	EVT_TREE_SEL_CHANGED(pof::Modules::ID_TREE, pof::Modules::OnSelected)
	EVT_TREE_BEGIN_DRAG(pof::Modules::ID_TREE, pof::Modules::OnBeginDrag)
	EVT_TREE_END_DRAG(pof::Modules::ID_TREE, pof::Modules::OnEndDrag)
	EVT_TREE_ITEM_MENU(pof::Modules::ID_TREE, pof::Modules::OnContextMenu)
	EVT_TREE_BEGIN_LABEL_EDIT(pof::Modules::ID_TREE, pof::Modules::OnBeginEditLabel)
	EVT_TREE_END_LABEL_EDIT(pof::Modules::ID_TREE, pof::Modules::OnEndEditLabel)

	//context menus
	EVT_MENU(pof::Modules::CONTEXT_MENU_EDIT, pof::Modules::OnContextEdit)
	EVT_MENU(pof::Modules::CONTEXT_MENU_REMOVE, pof::Modules::OnContextRemove)
END_EVENT_TABLE()


void pof::Modules::OnActivated(wxTreeEvent& evt)
{
	const auto item = evt.GetItem();
	auto winIter = mModuleViews.find(item);
	if (winIter == mModuleViews.end()) {
		//what to do here, main not pressed check children of main
		auto found = std::ranges::find(mChildId, item);
		if (found != mChildId.end()) {
			auto string = mModuleTree->GetItemText(item).ToStdString();
			mChildSignal(std::move(string));
		}
		return;
	}
	mSig(winIter, Evt::ACTIVATED);
}

void pof::Modules::OnSelected(wxTreeEvent& evt)
{
	const auto item = evt.GetItem();
	auto winIter = mModuleViews.find(item);
	if (winIter == mModuleViews.end()) {
		//what to do here 
		return;
	}
	mSig(winIter, Evt::SEL_CHANGED);
}

void pof::Modules::OnBeginDrag(wxTreeEvent& evt)
{
	spdlog::info("Starting drag");
	evt.Allow();
	auto item = evt.GetItem();
	auto iter = mModuleViews.find(item);
	if (iter == mModuleViews.end()) return;
	pof::TreeItemDataObject itemData(std::make_tuple(iter->first, iter->second,
			GetText(iter), GetImage(iter)));
	wxDropSource source(itemData);
	wxDragResult result = source.DoDragDrop(wxDrag_CopyOnly);

	switch (result)
	{
	case wxDragError:
		break;
	case wxDragNone:

		break;
	case wxDragCopy:
		break;
	case wxDragMove:
		break;
	case wxDragLink:
		break;
	case wxDragCancel:
		//canceled 
		break;
	default:
		break;
	}
	evt.Veto();
}

void pof::Modules::OnEndDrag(wxTreeEvent& evt)
{
	evt.Veto();
	spdlog::info("Ending drag");
}

void pof::Modules::OnContextMenu(wxTreeEvent& evt)
{
	auto itemID = evt.GetItem();
	auto iter = std::ranges::find(mChildId, itemID);
	if (iter == std::end(mChildId)) return; //only for child ids

	wxMenu* menu = new wxMenu;
	auto edit = menu->Append(CONTEXT_MENU_EDIT, "Edit");
	edit->SetBitmap(pof::ArtProvider::GetBitmap("pen"));
	auto remove = menu->Append(CONTEXT_MENU_REMOVE, "Remove");
	remove->SetBitmap(pof::ArtProvider::GetBitmap("action_delete"));

	PopupMenu(menu);
}

void pof::Modules::OnBeginEditLabel(wxTreeEvent& evt)
{
	spdlog::info("Starting edit label");
	auto item = evt.GetItem();
	if (!item.IsOk()) {
		spdlog::info("Cannot edit");
		evt.Veto();
		return;
	}

	auto iter = std::ranges::find(mChildId, item);
	if (iter == std::end(mChildId)) {
		spdlog::info("Edit label is not for parent items");
		evt.Veto();
		return;
	}

	evt.Skip();
}

void pof::Modules::OnEndEditLabel(wxTreeEvent& evt)
{
	spdlog::info("Ending edit label");
	if (evt.IsEditCancelled()) {
		spdlog::info("Edit is cancelled");
		evt.Veto();
		return;
	}
	auto item = evt.GetItem();
	auto oldName = mModuleTree->GetItemText(item).ToStdString();
	auto name = evt.GetLabel().ToStdString();
	if (name.empty() || oldName == name) {
		evt.Veto();
	}
	spdlog::info("Changing {} to {}", oldName, name);
	mChildEditedSignal(oldName, name); //signal a name change
	evt.Skip();
}

void pof::Modules::OnContextEdit(wxCommandEvent& evt)
{
	auto item = mModuleTree->GetSelection();
	if (!item.IsOk()) return;

	mModuleTree->EditLabel(item);
}

void pof::Modules::OnContextRemove(wxCommandEvent& evt)
{
	auto item = mModuleTree->GetSelection();
	if (!item.IsOk()) return;
	auto name = mModuleTree->GetItemText(item).ToStdString();
	if (wxMessageBox(fmt::format("Are you sure you want to remove {}", name), "REMOVE CATEGORY", wxICON_WARNING | wxYES_NO) == wxYES) {
		//send a remove signal so the rest of the application knows you have removed
		mChildRemoveSignal(name);
		auto iter = std::ranges::find(mChildId, item);
		if (iter != std::end(mChildId)) mChildId.erase(iter);
		mModuleTree->Delete(item);
	}
}

void pof::Modules::SetupFont()
{
	mFonts[FONT_MAIN] = std::move(wxFont(
			wxFontInfo(10).Family(wxFONTFAMILY_SWISS).AntiAliased()
		.FaceName("Bookman").Bold()));
	mFonts[FONT_CHILD] = std::move(wxFont(wxFontInfo(9).AntiAliased()
		.Family(wxFONTFAMILY_SWISS).FaceName("Bookman")));
	mFonts[FONT_ACCOUNT] = std::move(wxFont(wxFontInfo(8).AntiAliased()
		.Family(wxFONTFAMILY_SWISS).FaceName("Monospaced")));
}
void pof::Modules::AppendChildTreeId(wxTreeItemId parent, const std::string& name, int img)
{
	if (name.empty()) return;
	auto id = mModuleTree->AppendItem(parent, name, img);
	mModuleTree->SetItemFont(id, mFonts[FONT_CHILD]);
	mChildId.emplace_back(std::move(id));
}
void pof::Modules::RemoveChildTreeId(const std::string& name)
{
	if (name.empty()) return;
	auto found = std::ranges::find_if(mChildId, [&](const wxTreeItemId& item) -> bool {
		const auto str = mModuleTree->GetItemText(item).ToStdString();
		return str == name;
	});
	if (found != mChildId.end()) {
		mModuleTree->Delete(*found);
		mChildId.erase(found);
	}
}

pof::Modules::Modules(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	SetDoubleBuffered(true);
	SetupFont();
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	bSizer2->Add(0, 20, 0, wxEXPAND, 5);

	m_bitmap1 = new wxStaticBitmap(m_panel1, wxID_ANY, wxArtProvider::GetBitmap("pharmacist"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_bitmap1, 0, wxALIGN_CENTER | wxALL, 5);

	//add the account type
	wxStaticText* m_staticText3;
	std::string AccountType = wxGetApp().MainAccount.GetAccountTypeString();
	m_staticText3 = new wxStaticText(m_panel1, wxID_ANY, AccountType, wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->SetFont(mFonts[FONT_ACCOUNT]);
	m_staticText3->Wrap(-1);
	bSizer2->Add(m_staticText3, 0, wxALIGN_CENTER | wxALL, 2);

	wxStaticText* m_staticText1;
	std::string AccountName = wxGetApp().MainAccount.GetName();
	std::transform(AccountName.begin(), AccountName.end(),
		AccountName.begin(), [&](unsigned char c) -> unsigned char { return std::toupper(c); });
	m_staticText1 = new wxStaticText(m_panel1, wxID_ANY, AccountName , wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->SetFont(mFonts[FONT_ACCOUNT]);
	m_staticText1->Wrap(-1);
	bSizer2->Add(m_staticText1, 0, wxALIGN_CENTER | wxALL, 2);

	wxStaticText* m_staticText2;
	std::string PharmacyName = wxGetApp().MainPharamcy.GetName();
	std::transform(PharmacyName.begin(), PharmacyName.end(),
			PharmacyName.begin(), [&](unsigned char c) -> unsigned char { return std::toupper(c); });

	m_staticText2 = new wxStaticText(m_panel1, wxID_ANY, PharmacyName, wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->SetFont(mFonts[FONT_ACCOUNT]);
	m_staticText2->Wrap(-1);
	//m_staticText2->SetFont(wxFont(wxFontInfo(12)));
	bSizer2->Add(m_staticText2, 0, wxALIGN_CENTER | wxALL, 2);

	bSizer2->Add(0, 5, 0, wxEXPAND, 5);


	m_panel1->SetSizer(bSizer2);
	m_panel1->Layout();
	bSizer2->Fit(m_panel1);
	bSizer1->Add(m_panel1, 0, wxEXPAND | wxALL, 0);
	m_panel1->SetBackgroundColour(wxTheColourDatabase->Find("Aqua"));

	m_panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	mModuleTree = new wxTreeCtrl(m_panel2, ID_TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS
		| wxTR_EDIT_LABELS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxTR_SINGLE | wxNO_BORDER);
	bSizer3->Add( mModuleTree, 1, wxALL|wxEXPAND, 5 );
	mModuleTree->SetDoubleBuffered(true);
	CreateTree();
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	bSizer1->Add( m_panel2, 1, wxEXPAND | wxALL, 0 );
	
	Style(); //should it be here ?
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

pof::Modules::~Modules()
{
}

void pof::Modules::CreateTree()
{
	mModuleTree->SetIndent(20);
	wxTreeItemId root = mModuleTree->AddRoot("Root", -1);


	mPharmacy      = mModuleTree->AppendItem(root, "Pharamacy", 0);
	mTransactions  = mModuleTree->AppendItem(root, "Transactions", 0);
	mWarehouse     = mModuleTree->AppendItem(root, "Warehouse", 0);
	mReports       = mModuleTree->AppendItem(root, "Reports", 0);


	mProducts      = mModuleTree->AppendItem(mPharmacy, "Products", 1);
	mPaitents      = mModuleTree->AppendItem(mPharmacy, "Patients", 3);
	mPrescriptions = mModuleTree->AppendItem(mPharmacy, "Prescriptions", 1);
	mPoisionBook   = mModuleTree->AppendItem(mPharmacy, "Poision book", 1);
	
	mSales         = mModuleTree->AppendItem(mTransactions, "Sales", 1);
	mOrders        = mModuleTree->AppendItem(mTransactions, "Orders", 1);
	mRequisitions  = mModuleTree->AppendItem(mTransactions, "Requisitions", 1);

	mAuditTrails        = mModuleTree->AppendItem(mReports, "Audit Trails", 1);
	mConsumptionPattern = mModuleTree->AppendItem(mReports, "Consumption Patterns", 1);
	
	mModuleTree->Expand(mPharmacy);
	mModuleTree->Expand(mTransactions);
	mModuleTree->Expand(mReports);
}

void pof::Modules::Style()
{
	mModuleTree->SetItemFont(mPharmacy, mFonts[FONT_MAIN]);
	mModuleTree->SetItemFont(mTransactions, mFonts[FONT_MAIN]);
	mModuleTree->SetItemFont(mWarehouse, mFonts[FONT_MAIN]);
	mModuleTree->SetItemFont(mReports, mFonts[FONT_MAIN]);
	
	mModuleTree->SetItemFont(mPrescriptions, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mPaitents, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mPoisionBook, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mProducts, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mSales, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mOrders, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mRequisitions, mFonts[FONT_CHILD]);

}

std::string pof::Modules::GetText(const_iterator item) const
{
	return mModuleTree->GetItemText(item->first).ToStdString();
}

int pof::Modules::GetImage(const_iterator item) const
{
	return mModuleTree->GetItemImage(item->first);
}

pof::Modules::const_iterator::value_type pof::Modules::GetModuleItem(wxTreeItemId item) const
{
	auto iter = mModuleViews.find(item);
	if (iter == mModuleViews.end()) {
		return std::make_pair<wxTreeItemId, wxWindow*>(wxTreeItemId{}, nullptr);
	}
	return *iter;
}

boost::signals2::connection pof::Modules::SetSlot(signal_t::slot_type&& slot)
{
	return mSig.connect(std::forward<signal_t::slot_type>(slot));
}

boost::signals2::connection pof::Modules::SetChildTreeSlot(childtree_signal_t::slot_type&& slot)
{
	return mChildSignal.connect(std::forward<childtree_signal_t::slot_type>(slot));
}

boost::signals2::connection pof::Modules::SetChildTreeRemoveSlot(childtree_signal_t::slot_type&& slot)
{
	return mChildRemoveSignal.connect(std::forward<childtree_signal_t::slot_type>(slot));
}

boost::signals2::connection pof::Modules::SetChildTreeEditSlot(childEditTree_signal_t::slot_type&& slot)
{
	return mChildEditedSignal.connect(std::forward<childEditTree_signal_t::slot_type>(slot));
}
