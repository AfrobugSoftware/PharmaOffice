#include "TranferView.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::TransferView, wxDialog)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::TransferView::ID_SELECT, pof::TransferView::OnSelect)
	EVT_TOOL(pof::TransferView::ID_UPDATE_TRANSFER, pof::TransferView::OnUpdateTransfer)
	EVT_TOOL(pof::TransferView::ID_REMOVE_TRANSFER, pof::TransferView::OnRemoveTransfer)
	EVT_TOOL(pof::TransferView::ID_ADD_TRANSFER, pof::TransferView::OnAddTransfer)
	EVT_TOOL(pof::TransferView::ID_DOWNLOAD_EXCEL, pof::TransferView::OnDownloadExcel)
	EVT_DATE_CHANGED(pof::TransferView::ID_DATE_SELECT, pof::TransferView::OnDateChanged)
END_EVENT_TABLE()

pof::TransferView::TransferView(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxDialog(parent, id, title, pos, size, style) {
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
	mTopTools->AddTool(ID_ADD_TRANSFER, "Add", wxArtProvider::GetBitmap("action_add"), "Add product to order list");
	mTopTools->AddTool(ID_UPDATE_TRANSFER, wxT("Update"), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR, FromDIP(wxSize(16, 16))), "Clear order list");
	mTopTools->AddTool(ID_REMOVE_TRANSFER, wxT("Remove"), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR, FromDIP(wxSize(16, 16))), "Clear order list");
	m_tool11 = mTopTools->AddTool(ID_DOWNLOAD_EXCEL, wxT(""), wxArtProvider::GetBitmap("download"), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

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


	m_staticText3 = new wxStaticText(m_panel4, wxID_ANY, wxT("Transfer Cost: "), wxDefaultPosition, wxDefaultSize, 0);
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
	mOrderView = new wxDataViewCtrl(mBook, ID_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	model = std::make_unique<pof::DataModel>();
	mOrderView->AssociateModel(model.get());

	mProductCol       = mOrderView->AppendTextColumn(wxT("Product"), 1, wxDATAVIEW_CELL_INERT, FromDIP(500));
	mQuanCol          = mOrderView->AppendTextColumn(wxT("Quantity"), 2, wxDATAVIEW_CELL_EDITABLE, FromDIP(100));
	m_dataViewColumn3 = mOrderView->AppendTextColumn(wxT("Cost"), 3, wxDATAVIEW_CELL_INERT, FromDIP(100));


	CreateEmptyPanel();
	bool empty = model->GetDatastore().empty();
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
}

pof::TransferView::~TransferView()
{
	model.release();
}

void pof::TransferView::CreateEmptyPanel()
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

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap("basket", wxART_OTHER, FromDIP(wxSize(64, 64))), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No product transfered"), wxDefaultPosition, wxDefaultSize, 0);
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
}

void pof::TransferView::CreateSpeicalCol()
{
	pof::DataModel::SpeicalColHandler_t quanCol;
	pof::DataModel::SpeicalColHandler_t SelectColHandler;

	auto& datastore = model->GetDatastore();
	quanCol.second = [&](size_t row, size_t col, const wxVariant& value) -> bool {
		wxBusyCursor cusor;
		std::uint32_t quan = static_cast<std::uint32_t>(value.GetInteger());
		auto& rowid   = datastore[row].first[0];
		auto& rowQuan = datastore[row].first[2];
		auto& cost    = datastore[row].first[3];
		rowQuan = quan;
		wxGetApp().mSaleManager.UpdateTransferQuantity(
			static_cast<std::uint32_t>(boost::variant2::get<uint32_t>(rowid)),
			static_cast<std::uint32_t>(boost::variant2::get<uint32_t>(rowQuan)));
		UpdateTexts();
		return true;
	};
	SelectColHandler.first = [&](size_t row, size_t col) -> wxVariant {
		auto found = mSelections.find(pof::DataModel::GetItemFromIdx(row));
		return wxVariant((found != mSelections.end()));
	};
	SelectColHandler.second = [&](size_t row, size_t col, const wxVariant& v) -> bool {
		if (v.GetBool()) {
			auto [iter, inserted] = mSelections.insert(pof::DataModel::GetItemFromIdx(row));
			return inserted;
		}
		else {
			mSelections.erase(pof::DataModel::GetItemFromIdx(row));
			return true;
		}
	};
	model->SetSpecialColumnHandler(2, std::move(quanCol));
	model->SetSpecialColumnHandler(100, std::move(SelectColHandler));
}

void pof::TransferView::OnAddTransfer(wxCommandEvent& evt)
{
	pof::SearchProduct dialog(this);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	wxBusyCursor cursor;
	auto& datastore = model->GetDatastore();
	if (dialog.HasMultipleSelections()) {
		const auto vec = dialog.GetSelectedProducts();
		for (const auto& v : vec) {
			auto& row = v.get();

		}
	}
}

void pof::TransferView::OnRemoveTransfer(wxCommandEvent& evt)
{
}

void pof::TransferView::OnUpdateTransfer(wxCommandEvent& evt)
{
}

void pof::TransferView::OnDownloadExcel(wxCommandEvent& evt)
{
}

void pof::TransferView::OnSelect(wxAuiToolBarEvent& evt)
{
}

void pof::TransferView::OnDateChanged(wxDateEvent& evt)
{
}

void pof::TransferView::ShowSelect()
{
}

void pof::TransferView::HideSelect()
{
}

void pof::TransferView::UpdateTexts()
{
}
