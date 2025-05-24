#include "TranferView.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::TransferView, wxDialog)
	EVT_TOOL(pof::TransferView::ID_SELECT, pof::TransferView::OnSelect)
	EVT_TOOL(pof::TransferView::ID_UPDATE_TRANSFER, pof::TransferView::OnUpdateTransfer)
	EVT_TOOL(pof::TransferView::ID_REMOVE_TRANSFER, pof::TransferView::OnRemoveTransfer)
	EVT_TOOL(pof::TransferView::ID_ADD_TRANSFER, pof::TransferView::OnAddTransfer)
	EVT_TOOL(pof::TransferView::ID_DOWNLOAD_EXCEL, pof::TransferView::OnDownloadExcel)
	EVT_DATE_CHANGED(pof::TransferView::ID_DATE_SELECT, pof::TransferView::OnDateChanged)
	EVT_UPDATE_UI(pof::TransferView::ID_VIEW, pof::TransferView::OnUpdateUi)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::TransferView::ID_VIEW, pof::TransferView::OnHeaderClick)
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
	mSelItem = mTopTools->AddTool(ID_SELECT, "Select", wxArtProvider::GetBitmap("select_check", wxART_OTHER, FromDIP(wxSize(16,16))), "Show Selections", wxITEM_CHECK);

	mTopTools->AddTool(ID_ADD_TRANSFER, "Add", wxArtProvider::GetBitmap("add", wxART_OTHER, FromDIP(wxSize(16,16))), "Add product to order list");
	mTopTools->AddTool(ID_UPDATE_TRANSFER, wxT("Update"), wxArtProvider::GetBitmap("add_task", wxART_OTHER, FromDIP(wxSize(16, 16))), "Clear order list");
	mTopTools->AddTool(ID_REMOVE_TRANSFER, wxT("Remove"), wxArtProvider::GetBitmap("delete", wxART_TOOLBAR, FromDIP(wxSize(16, 16))), "Clear order list");
	m_tool11 = mTopTools->AddTool(ID_DOWNLOAD_EXCEL, wxT("Download Excel"), wxArtProvider::GetBitmap("download_down", wxART_OTHER, FromDIP(wxSize(16,16))), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	mTopTools->AddStretchSpacer();
	mTopTools->AddSeparator();
	mTopTools->AddSpacer(FromDIP(2));
	mDatePicker = new wxDatePickerCtrl(mTopTools, ID_DATE_SELECT, wxDateTime::Now(), wxDefaultPosition, FromDIP(wxSize(200, -1)), wxDP_DROPDOWN);
	mDatePicker->SetRange(wxDateTime{}, wxDateTime::Now());
	auto dd = mDatePicker->GetValue();
	std::chrono::month month = static_cast<std::chrono::month>(dd.GetMonth() + 1);
	std::chrono::year year = static_cast<std::chrono::year>(dd.GetYear());
	std::chrono::day day = static_cast<std::chrono::day>(dd.GetDay());
	const std::chrono::year_month_day ymd{ month / day / year };
	mSelectDay = static_cast<std::chrono::sys_days>(ymd);
	mTopTools->AddControl(mDatePicker);

	mTopTools->Realize();

	bSizer2->Add(mTopTools, 0, wxEXPAND, FromDIP(5));

	m_panel3 = new wxPanel(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	m_panel4 = new wxPanel(m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	bSizer4->AddStretchSpacer();

	m_staticText1 = new wxStaticText(m_panel4, wxID_ANY, wxT("Total Transfer:"), wxDefaultPosition, wxDefaultSize, 0);
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
	model->Adapt<
		boost::uuids::uuid,
		std::string,
		std::uint32_t,
		pof::base::currency,
		std::chrono::system_clock::time_point,
		boost::uuids::uuid
	>();
	mOrderView->AssociateModel(model.get());

	mProductCol       = mOrderView->AppendTextColumn(wxT("Product"),  1, wxDATAVIEW_CELL_INERT,    FromDIP(500));
	mQuanCol          = mOrderView->AppendTextColumn(wxT("Quantity"), 2, wxDATAVIEW_CELL_EDITABLE, FromDIP(100));
	m_dataViewColumn3 = mOrderView->AppendTextColumn(wxT("Cost"),     3, wxDATAVIEW_CELL_INERT,    FromDIP(100));
	m_dataViewColumn5 = mOrderView->AppendTextColumn(wxT("Total Cost"),400, wxDATAVIEW_CELL_INERT,    FromDIP(100));
	m_dataViewColumn4 = mOrderView->AppendTextColumn(wxT("Date"),     4, wxDATAVIEW_CELL_INERT,    FromDIP(100));


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
	LoadTransfer();

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
	pof::DataModel::SpeicalColHandler_t totalCost;

	auto& datastore = model->GetDatastore();
	quanCol.second = [&](size_t row, size_t col, const wxVariant& value) -> bool {
		wxBusyCursor cusor;
		std::uint32_t quan = static_cast<std::uint32_t>(value.GetInteger());
		auto& rowid   = boost::variant2::get<boost::uuids::uuid>(datastore[row].first[0]);
		auto& curQuan = boost::variant2::get<std::uint32_t>(datastore[row].first[2]);
		auto& puid    = boost::variant2::get<boost::uuids::uuid>(datastore[row].first[5]);
		std::uint32_t sc = 0;
		bool isRemv = false;
		if (curQuan > quan) {
			//we arr reducing
			isRemv = true;
			sc = curQuan - quan;
		}
		else if (quan > curQuan) {
			//we are are increaing
			sc = quan - curQuan;
		}
		else {
			//do nothing 
			return false;
		}
		if (!UpdateStockCount(puid, sc, isRemv)) {
			wxMessageBox("Cannot update transfer", "Transfer", wxICON_INFORMATION | wxOK);
			return false;
		}
		datastore[row].first[2] = quan;
		wxGetApp().mSaleManager.UpdateTransferQuantity(rowid, sc);
		UpdateTexts();
		return true;
	};
	totalCost.first = [&](size_t row, size_t col) -> wxVariant {
		auto& rowQuan = datastore[row].first[2];
		auto& cost =  boost::variant2::get<pof::base::currency>(datastore[row].first[3]);

		pof::base::currency total = cost * static_cast<double>(boost::variant2::get<std::uint32_t>(rowQuan));
		return wxVariant(fmt::format("{:cu}", total));
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
	model->SetSpecialColumnHandler(selcol, std::move(SelectColHandler));
	model->SetSpecialColumnHandler(400, std::move(totalCost));
}

void pof::TransferView::LoadTransfer()
{
	auto opt = wxGetApp().mSaleManager.GetTransferByDate(mSelectDay);
	if (!opt.has_value()) {
		wxMessageBox("Cannot load transfers, please call administator", "Transfer", wxICON_ERROR | wxOK);
		return;
	}
	const auto& data = opt.value();
	if (data.empty()) {
		mBook->SetSelection(1);
	}
	else {
		mOrderView->Freeze();
		model->Clear();
		for (const auto& t : data) {
			pof::base::data::row_t r;
			auto& back = r.first;
			back.resize(6);
			back[0] = t.transferID;
			back[1] = t.productName;
			back[2] = t.quantity;
			back[3] = t.amount;
			back[4] = t.date;
			back[5] = t.puid;

			model->EmplaceData(std::move(r));
		}
		UpdateTexts();
		mOrderView->Thaw();
		mBook->SetSelection(0);
	}

}

void pof::TransferView::OnAddTransfer(wxCommandEvent& evt)
{
	pof::SearchProduct dialog(this);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	wxBusyCursor cursor;
	auto& datastore = model->GetDatastore();
	const auto now = std::chrono::system_clock::now();
	if (dialog.HasMultipleSelections()) {
		const auto vec = dialog.GetSelectedProducts();
		std::vector<pof::SaleManager::transfer> transfers;
		transfers.reserve(vec.size());
		for (const auto& v : vec) {
			auto& row = v.get();
			pof::SaleManager::transfer t;
			t.transferID = boost::uuids::random_generator_mt19937{}();
			t.productName = boost::variant2::get<std::string>(row.first[pof::ProductManager::PRODUCT_NAME]);
			t.quantity = 1;
			t.amount = boost::variant2::get<pof::base::currency>(row.first[pof::ProductManager::PRODUCT_COST_PRICE]);
			t.date = mSelectDay;
			t.puid = boost::variant2::get<boost::uuids::uuid>(row.first[pof::ProductManager::PRODUCT_UUID]);

			auto found = std::ranges::find_if(datastore.tab(), [&](const auto& item) -> bool {
				return (t.puid == boost::variant2::get<boost::uuids::uuid>(item.first[5]));
			});
			//a duplicate
			if (found != datastore.tab().end()) {
				auto& i = *found;
				const auto quan = boost::variant2::get<std::uint32_t>(i.first[2]) + 1;
				const auto& id = boost::variant2::get<boost::uuids::uuid>(i.first[0]);
				const auto& name = boost::variant2::get<std::string>(i.first[1]);
				if (!UpdateStockCount(t.puid, 1)) {
					wxMessageBox(fmt::format("Cannot add transfer of {}", t.productName), "Transfer", wxICON_INFORMATION | wxOK);
					continue;
				}
				wxGetApp().mSaleManager.UpdateTransferQuantity(id, quan);
				wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Updated Transfer {} product quantity: {:d}", name, quan));

				i.first[2] = quan;
				mOrderView->Refresh();
				mOrderView->Update();
				continue;
			}
			
			if (!UpdateStockCount(t.puid, t.quantity)) {
				wxMessageBox(fmt::format("Cannot add transfer of {}", t.productName), "Transfer", wxICON_INFORMATION | wxOK);
				continue;
			}

			pof::base::data::row_t r;
			auto& back = r.first;
			back.resize(6);
			back[0] = t.transferID;
			back[1] = t.productName;
			back[2] = t.quantity;
			back[3] = t.amount;
			back[4] = t.date;
			back[5] = t.puid;

			model->EmplaceData(std::move(r));
			transfers.emplace_back(std::move(t));
		}
		wxGetApp().mSaleManager.AddTransfer(transfers);
	}
	else {
		const auto& row = dialog.GetSelectedProduct();
		pof::SaleManager::transfer t;
		t.transferID  = boost::uuids::random_generator_mt19937{}();
		t.productName = boost::variant2::get<std::string>(row.first[pof::ProductManager::PRODUCT_NAME]);
		t.quantity    = 1;
		t.amount      = boost::variant2::get<pof::base::currency>(row.first[pof::ProductManager::PRODUCT_COST_PRICE]);
		t.date        = now;
		t.puid        = boost::variant2::get<boost::uuids::uuid>(row.first[pof::ProductManager::PRODUCT_UUID]);
		
		auto found = std::ranges::find_if(datastore.tab(), [&](const auto& item) -> bool {
			return (t.puid == boost::variant2::get<boost::uuids::uuid>(item.first[5]));
			});
		//a duplicate
		if (found != datastore.tab().end()) {
			auto& i = *found;
			const auto quan = boost::variant2::get<std::uint32_t>(i.first[2]) + 1;
			const auto& id = boost::variant2::get<boost::uuids::uuid>(i.first[0]);
			if (!UpdateStockCount(t.puid, 1)) {
				wxMessageBox(fmt::format("Cannot add transfer of {}", t.productName), "Transfer", wxICON_INFORMATION | wxOK);
				return;
			}
			wxGetApp().mSaleManager.UpdateTransferQuantity(id, quan);
			i.first[2] = quan;
			mOrderView->Refresh();
			mOrderView->Update();
			return;
		}
		
		if (!UpdateStockCount(t.puid, t.quantity)) {
			wxMessageBox(fmt::format("Cannot add transfer of {}", t.productName), "Transfer", wxICON_INFORMATION | wxOK);
			return;
		}

		pof::base::data::row_t r;
		auto& back = r.first;
		back.resize(6);
		back[0] = t.transferID;
		back[1] = t.productName;
		back[2] = t.quantity;
		back[3] = t.amount;
		back[4] = t.date;
		back[5] = t.puid;

		model->EmplaceData(std::move(r));
		wxGetApp().mSaleManager.AddTransfer({ std::move(t) });
	}
	UpdateTexts();
	if (mBook->GetSelection() != 0)
		mBook->SetSelection(0);
}

void pof::TransferView::OnRemoveTransfer(wxCommandEvent& evt)
{
	if (wxMessageBox("Are you sure you want to remove transfer?", "Transfer", wxICON_INFORMATION | wxYES_NO) == wxNO) return;
	wxBusyCursor cur;
	if (mSelections.empty()) {
		const auto& item = mOrderView->GetSelection();
		if (!item.IsOk()) {
			wxMessageBox("Please select a product to remove from transafer", "Transfer", wxICON_INFORMATION | wxOK);
			return;
		}
		int index       = pof::DataModel::GetIdxFromItem(item);
		const auto& row = model->GetDatastore().at(index);
		const auto& puid          = boost::variant2::get<boost::uuids::uuid>(row.first[5]);
		const std::uint32_t& quan = boost::variant2::get<std::uint32_t>(row.first[2]);
		const std::string& name   = boost::variant2::get<std::string>(row.first[1]);
		if (!UpdateStockCount(puid, quan)) {
			wxMessageBox(fmt::format("Cannot remove transfer of {}", name), "Transfer", wxICON_INFORMATION | wxOK);
			return;
		}
		wxGetApp().mSaleManager.RemoveTransfer({ boost::variant2::get<boost::uuids::uuid>(row.first[0]) });
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Removed Transfer of {} product quantity: {:d}", name, quan));

		mOrderView->Freeze();
		model->RemoveData(item);
		mOrderView->Thaw();
	}
	else {
		std::vector<boost::uuids::uuid> revs;
		revs.reserve(mSelections.size());
		mOrderView->Freeze();
		wxDataViewItemArray items;
		items.reserve(mSelections.size());
		for (const auto& item : mSelections) {
			if (!item.IsOk()) continue;
			int index       = pof::DataModel::GetIdxFromItem(item);
			const auto& row = model->GetDatastore().at(index);
			const auto& puid         = boost::variant2::get<boost::uuids::uuid>(row.first[5]);
			const std::uint32_t quan = boost::variant2::get<std::uint32_t>(row.first[2]);
			const std::string& name  = boost::variant2::get<std::string>(row.first[1]);
			if (!UpdateStockCount(puid, quan, true)) {
				wxMessageBox(fmt::format("Cannot remove transfer of {}", name), "Transfer", wxICON_INFORMATION | wxOK);
				continue;
			}
			revs.emplace_back(boost::variant2::get<boost::uuids::uuid>(row.first[0]));
			items.push_back(item);
			wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Removed Transfer of {} product quantity: {:d}", name, quan));

		}
		model->RemoveData(items);
		UpdateTexts();
		mOrderView->Thaw();
		wxGetApp().mSaleManager.RemoveTransfer(revs);
	}
	if (model->GetDatastore().empty())
		mBook->SetSelection(1);
}

void pof::TransferView::OnUpdateTransfer(wxCommandEvent& evt)
{
	const auto& item = mOrderView->GetSelection();
	if (!item.IsOk()) {
		wxMessageBox("Please select an item to update", "Transfer", wxICON_INFORMATION | wxOK);
		return;
	}
	try {
		int index = pof::DataModel::GetIdxFromItem(item);
		auto& row = model->GetDatastore()[index];
		const std::string& name     = boost::variant2::get<std::string>(row.first[1]);
		const std::uint32_t curQuan = boost::variant2::get<std::uint32_t>(row.first[2]);

		std::uint32_t quan = boost::lexical_cast<std::uint32_t>(
			wxGetTextFromUser(fmt::format("Please enter quantity for {}", name)).ToStdString());
		std::uint32_t sc = 0;
		bool isRemv = false;
		if (curQuan > quan) {
			//we arr reducing
			isRemv = true;
			sc = curQuan - quan;
		}
		else if (quan > curQuan) {
			//we are are increaing
			sc = quan - curQuan;
		}
		else {
			//do nothing 
			return;
		}

		const auto& uuid = boost::variant2::get<boost::uuids::uuid>(row.first[0]);
		const auto& puid = boost::variant2::get<boost::uuids::uuid>(row.first[5]);
		if (!UpdateStockCount(puid, sc, isRemv)) {
			wxMessageBox(fmt::format("Cannot update transfer of {}", name), "Transfer", wxICON_INFORMATION | wxOK);
			return;
		}
		wxGetApp().mSaleManager.UpdateTransferQuantity(uuid, quan);
		mOrderView->Freeze();
		row.first[2] = quan;

		UpdateTexts();
		mOrderView->Thaw();
	}
	catch (const std::exception& exp) {
		wxMessageBox("Invalid quantity, please enter a number", "Transfer", wxICON_ERROR | wxOK);
	}
}

void pof::TransferView::OnDownloadExcel(wxCommandEvent& evt)
{
	const auto& data = model->GetDatastore().tab();
	if (data.empty()) {
		wxMessageBox("No transaction to download", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}
	wxFileDialog dialog(this, "Save inventory month report excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}
	wxBusyCursor cursor;
	excel::XLDocument doc;
	try {
		doc.create(fullPath.string());
		if (!doc.isOpen()) {
			spdlog::error("Canont open xlsx file");
			return;
		}
	}
	catch (excel::XLException& exp) {
		spdlog::error(exp.what());
		wxMessageBox("Error in creating and excel file, check if file with the same name is opened.", "Export excel", wxICON_ERROR | wxOK);
		return;
	}
	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName(fmt::format("INVENTORY for {:%m/%Y}", mSelectDay));
	const size_t colSize = 4 ;
	const size_t rowSize = data.size() + 3; //plus title and total row
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
		};
	writeHeader("Product");
	writeHeader("Quantity");
	writeHeader("Amount");
	writeHeader("Date");

	for (auto it = data.begin(); it != data.end() && iter != range.end(); it++) {
		auto& row = it->first;
		iter->value().set(boost::variant2::get<pof::base::data::text_t>(row[1]));
		iter++;

		iter->value().set(boost::variant2::get<std::uint32_t>(row[2]));
		iter++;

		iter->value().set(fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(row[3])));
		iter++;

		iter->value().set(fmt::format("{:%m/%Y}", boost::variant2::get<std::chrono::system_clock::time_point>(row[4])));
		iter++;
	}


	iter->value().set(fmt::format("Total Quantity: {:d}", model->GetDatastore().size()));
	iter++;

	iter->value().set(fmt::format("Total Amount: {:cu}", mTotalAmount));
	iter++;


	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Transfers", wxICON_INFORMATION | wxOK);
}

void pof::TransferView::OnSelect(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		ShowSelect();
	}
	else {
		HideSelect();
	}
}

void pof::TransferView::OnDateChanged(wxDateEvent& evt)
{
	auto dd = evt.GetDate();
	std::chrono::month month = static_cast<std::chrono::month>(dd.GetMonth() + 1);
	std::chrono::year year   = static_cast<std::chrono::year>(dd.GetYear());
	std::chrono::day day     = static_cast<std::chrono::day>(dd.GetDay());

	const std::chrono::year_month_day ymd{ month / day / year };
	mSelectDay = static_cast<std::chrono::sys_days>(ymd);

	LoadTransfer();
}

void pof::TransferView::OnUpdateUi(wxUpdateUIEvent& evt)
{
	auto id = evt.GetId();
	switch (id)
	{
	case ID_VIEW:
	{
		static size_t count = 0;
		if (!mSelections.empty() && count != mSelections.size()) {
			wxDataViewItemArray arr;
			arr.reserve(mSelections.size());
			for (auto& sel : mSelections) {
				arr.push_back(sel);
			}
			mOrderView->SetSelections(arr);
		}
	}
	case ID_SELECT:
	{
		if (mSelectCol == nullptr)
		{
			mTopTools->ToggleTool(ID_SELECT, false);
			mTopTools->Refresh();
			mTopTools->Update();
		}
	}
	default:
		break;
	}
}

void pof::TransferView::OnHeaderClick(wxDataViewEvent& evt)
{
	if (mSelectCol == evt.GetDataViewColumn()) {
		static bool sel = true;
		mOrderView->Freeze();
		auto& items = model->GetDataViewItems();
		if (sel) {
			if (!mSelections.empty()) {
				mSelections.clear();
				sel = false;
				mOrderView->SetSelections({});
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
		mOrderView->Thaw();
		evt.Veto();
	}
	else {
		evt.Skip();
	}
}

bool pof::TransferView::UpdateStockCount(const boost::uuids::uuid& puid, std::uint32_t quan, bool isRemv)
{
	wxBusyCursor cur;
	auto& prodDatastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto pIter = std::ranges::find_if(prodDatastore, [&](const pof::base::data::row_t& row)->bool {
		return (puid == boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]));
	});
	auto& v = pIter->first; 
	auto& sc = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT]);
	if (!isRemv && quan > sc)
		return false;

	if (isRemv)
		sc += quan;
	else
		sc -= quan;

	wxGetApp().mProductManager.UpdateProductQuan({std::make_pair(puid, sc)});
	return true;
}

void pof::TransferView::ShowSelect()
{
	mSelectCol = mOrderView->PrependToggleColumn(wxT("Select"), selcol,
		wxDATAVIEW_CELL_ACTIVATABLE, FromDIP(50));
}

void pof::TransferView::HideSelect()
{
	if (mSelectCol != nullptr) {
		mSelections.clear();

		mOrderView->Freeze();
		mOrderView->DeleteColumn(mSelectCol);
		mOrderView->Thaw();
		mSelectCol = nullptr;
	}
}

void pof::TransferView::UpdateTexts()
{
	pof::base::currency cur;
	auto& datastore = model->GetDatastore();
	cur = std::accumulate(datastore.begin(), datastore.end(), cur, [&](const pof::base::currency&, const pof::base::data::row_t& row) {
		auto& orderAmount = boost::variant2::get<pof::base::currency>(row.first[3]);
		auto& rowQuan = boost::variant2::get<std::uint32_t>(row.first[2]);

		return(cur = cur + (orderAmount * static_cast<double>(rowQuan)));
	});

	m_panel4->Freeze();
	m_staticText2->SetLabel(fmt::format("{:d}", datastore.size()));
	m_staticText4->SetLabel(fmt::format("{:cu}", cur));
	mTotalAmount = cur;


	m_panel4->Layout();
	m_panel4->Thaw();
	m_panel4->Refresh();
}
