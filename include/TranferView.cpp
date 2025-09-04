#include "TranferView.h"

pof::TransferView::TransferView(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxDialog(parent, id, title, pos, size, style){
	this->SetSize(FromDIP(wxSize(991, 513)));
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	mTopTools = new wxAuiToolBar(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_GRIPPER | wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTopTools->SetMinSize(FromDIP(wxSize(-1, 40)));
	mTopTools->AddTool(ID_SELECT, "Select", wxArtProvider::GetBitmap("action_check"), "Show Selections", wxITEM_CHECK);

	mTopTools->AddStretchSpacer();
	mTopTools->AddSeparator();
	mTopTools->AddTool(ID_ADD_PRODUCT, "Add Product", wxArtProvider::GetBitmap("action_add"), "Add product to order list");
	mTopTools->AddTool(ID_CLEAR_ORDERED, "Remove ordered", wxArtProvider::GetBitmap(wxART_REMOVABLE, wxART_TOOLBAR, FromDIP(wxSize(16, 16))), "Remove ordered items", wxITEM_NORMAL);
	mTopTools->AddTool(ID_CLEAR_ORDERLIST, wxT("Clear all"), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR, FromDIP(wxSize(16, 16))), "Clear order list");
	m_tool11 = mTopTools->AddTool(ID_PRINT_ORDER, wxT("Print Order"), wxArtProvider::GetBitmap("download"), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	mTopTools->Realize();

	bSizer2->Add(mTopTools, 0, wxEXPAND, FromDIP(5));

	m_panel3 = new wxPanel(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	m_panel4 = new wxPanel(m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	bSizer4->AddStretchSpacer();

	m_staticText1 = new wxStaticText(m_panel4, wxID_ANY, wxT("Total Orders:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText1->Wrap(-1);
	bSizer4->Add(m_staticText1, 0, wxALL, FromDIP(5));

	m_staticText2 = new wxStaticText(m_panel4, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText2->Wrap(-1);
	bSizer4->Add(m_staticText2, 0, wxALL, FromDIP(5));

	bSizer4->AddSpacer(FromDIP(5));

	bSizer4->Add(new wxStaticLine(m_panel4, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), wxSizerFlags().Expand());

	bSizer4->AddSpacer(FromDIP(5));


	m_staticText3 = new wxStaticText(m_panel4, wxID_ANY, wxT("Order Cost: "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText3->Wrap(-1);
	bSizer4->Add(m_staticText3, 0, wxALL, FromDIP(5));

	m_staticText4 = new wxStaticText(m_panel4, wxID_ANY, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->SetFont(wxFont(wxFontInfo().AntiAliased()));
	m_staticText4->Wrap(-1);
	bSizer4->Add(m_staticText4, 0, wxALL, FromDIP(5));


	m_panel4->SetSizer(bSizer4);
	m_panel4->Layout();
	bSizer4->Fit(m_panel4);
	bSizer3->Add(m_panel4, 1, wxEXPAND | wxALL, 0);


	m_panel3->SetSizer(bSizer3);
	m_panel3->Layout();
	bSizer3->Fit(m_panel3);
	//bSizer2->Add( m_panel3, 0, wxEXPAND | wxALL, 0 );

	mBook = new wxSimplebook(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mOrderView = new wxDataViewCtrl(mBook, ID_ORDER_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	mOrderView->AssociateModel(wxGetApp().mProductManager.GetOrderList().get());

	mProductCol = mOrderView->AppendTextColumn(wxT("Product"), pof::ProductManager::ORDER_PRODUCT_NAME, wxDATAVIEW_CELL_INERT, FromDIP(500));
	mQuanCol = mOrderView->AppendTextColumn(wxT("Quantity"), pof::ProductManager::ORDER_QUANTITY, wxDATAVIEW_CELL_EDITABLE, FromDIP(100));
	m_dataViewColumn3 = mOrderView->AppendTextColumn(wxT("Cost"), pof::ProductManager::ORDER_COST, wxDATAVIEW_CELL_INERT, FromDIP(100));
	m_dataViewColumn4 = mOrderView->AppendBitmapColumn(wxT("Ordered"), pof::ProductManager::ORDER_STATE, wxDATAVIEW_CELL_INERT, FromDIP(100));


	CreateEmptyPanel();
	bool empty = wxGetApp().mProductManager.GetOrderList()->GetDatastore().empty();
	mBook->AddPage(mOrderView, wxT("ORDER VIEW"), !empty);
	mBook->AddPage(mEmpty, wxT("EMPTY"), empty);
	mBook->Layout();

	bSizer2->Add(mBook, 1, wxEXPAND, 0);
	bSizer2->Add(m_panel3, 0, wxEXPAND | wxALL, FromDIP(2));

	m_panel1->SetSizer(bSizer2);
	m_panel1->Layout();
	bSizer2->Fit(m_panel1);
	bSizer1->Add(m_panel1, 1, wxEXPAND | wxALL, 0);


	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	CreateSpeicalCol();
	UpdateTexts();

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);

	wxGetApp().mPrintManager->printSig.connect(std::bind_front(&pof::OrderListView::OnPrintComplete, this));
}
