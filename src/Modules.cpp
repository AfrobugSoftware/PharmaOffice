#include "Modules.h"
BEGIN_EVENT_TABLE(pof::Modules, wxPanel)
	EVT_TREE_ITEM_ACTIVATED(pof::Modules::ID_TREE, pof::Modules::OnActivated)
	EVT_TREE_SEL_CHANGED(pof::Modules::ID_TREE, pof::Modules::OnSelected)
END_EVENT_TABLE()


void pof::Modules::OnActivated(wxTreeEvent& evt)
{
	const auto item = evt.GetItem();
	auto winIter = mModuleViews.find(item);
	if (winIter == mModuleViews.end()) {
		//what to do here 
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

void pof::Modules::SetupFont()
{
	mFonts[FONT_MAIN].SetPointSize(12);
	mFonts[FONT_MAIN].SetFamily(wxFONTFAMILY_SWISS);
	mFonts[FONT_MAIN].SetFaceName("Bookman");
	mFonts[FONT_MAIN].SetWeight(wxFONTWEIGHT_BOLD);

	mFonts[FONT_CHILD].SetPointSize(9);
	mFonts[FONT_CHILD].SetFamily(wxFONTFAMILY_SWISS);
}
pof::Modules::Modules(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	m_bitmap1 = new wxStaticBitmap(m_panel1, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_bitmap1, 0, wxALIGN_CENTER | wxALL, 5);

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText(m_panel1, wxID_ANY, wxT("PHARMAOFFICE"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	bSizer2->Add(m_staticText1, 0, wxALIGN_CENTER | wxALL, 5);


	m_panel1->SetSizer(bSizer2);
	m_panel1->Layout();
	bSizer2->Fit(m_panel1);
	bSizer1->Add(m_panel1, 0, wxEXPAND | wxALL, 0);
	m_panel1->SetBackgroundColour(wxTheColourDatabase->Find("Aqua"));

	m_panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	mModuleTree = new wxTreeCtrl(m_panel2, ID_TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS
		| wxTR_FULL_ROW_HIGHLIGHT | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxTR_SINGLE | wxNO_BORDER);
	bSizer3->Add( mModuleTree, 1, wxALL|wxEXPAND, 0 );
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

	
	wxImageList* imgList = new wxImageList();
	imgList->Add(wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	
	
	mModuleTree->AssignImageList(imgList);
	wxTreeItemId root = mModuleTree->AddRoot("Root", -1);


	mPharmacy      = mModuleTree->AppendItem(root, "Pharamacy", 0);
	mTransactions  = mModuleTree->AppendItem(root, "Transactions", 0);
	mApplication   = mModuleTree->AppendItem(root, "Settings", 0);


	mPrescriptions = mModuleTree->AppendItem(mPharmacy, "Prescriptions", 0);
	mPaitents      = mModuleTree->AppendItem(mPharmacy, "Patients", 0);
	mPoisionBook   = mModuleTree->AppendItem(mPharmacy, "Poision book", 0);
	mProducts      = mModuleTree->AppendItem(mPharmacy, "Products");
	
	mSales         = mModuleTree->AppendItem(mTransactions, "Sales", 0);
	mOrders        = mModuleTree->AppendItem(mTransactions, "Orders", 0);
	mRequisitions = mModuleTree->AppendItem(mTransactions, "Requisitions", 0);
	mReports       = mModuleTree->AppendItem(mTransactions, "Reports", 0);

	
	mModuleTree->Expand(mPharmacy);
	mModuleTree->Expand(mTransactions);
}

void pof::Modules::Style()
{
	SetupFont();
	mModuleTree->SetItemFont(mPharmacy, mFonts[FONT_MAIN]);
	mModuleTree->SetItemFont(mTransactions, mFonts[FONT_MAIN]);
	mModuleTree->SetItemFont(mApplication, mFonts[FONT_MAIN]);
	
	mModuleTree->SetItemFont(mPrescriptions, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mPaitents, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mPoisionBook, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mProducts, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mSales, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mOrders, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mRequisitions, mFonts[FONT_CHILD]);
	mModuleTree->SetItemFont(mReports, mFonts[FONT_CHILD]);

}

std::string pof::Modules::GetText(const_iterator item) const
{
	return mModuleTree->GetItemText(item->first).ToStdString();
}

int pof::Modules::GetImage(const_iterator item) const
{
	return mModuleTree->GetItemImage(item->first);
}

boost::signals2::connection pof::Modules::SetSlot(signal_t::slot_type&& slot)
{
	return mSig.connect(std::forward<signal_t::slot_type>(slot));
}
