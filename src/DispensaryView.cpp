#include "DispensaryView.h"
#include "PrescriptionView.h"

BEGIN_EVENT_TABLE(pof::DispensaryView, wxPanel)
EVT_DATAVIEW_SELECTION_CHANGED(pof::DispensaryView::DATA_VIEW, DispensaryView::OnDataViewItemSelected)
EVT_PG_CHANGED(pof::DispensaryView::PROPERTY_MANAGER, DispensaryView::OnPropertyChanged)
END_EVENT_TABLE()

pof::DispensaryView::DispensaryView(wxWindow* parent, wxWindowID id)
	: wxPanel(parent, id, wxDefaultPosition, wxDefaultSize), mPanelManager(this) {
	mDataModel = std::make_unique<pof::DataModel>();
	mDataModel->Adapt<
		std::uint64_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		std::int32_t,
		std::uint64_t
	>();


	CreateDataView();
	CreatePropertyGrid();
	SetupAuiTheme();

	mPanelManager.Update();

}

pof::DispensaryView::~DispensaryView()
{
	mDataView.release();
	mPropertyManager.release();
	mPanelManager.UnInit();
}

void pof::DispensaryView::OnAuiThemeChange()
{
	auto auiArtProvider = mPanelManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::DispensaryView::SetupAuiTheme()
{
	auto auiArtProvider = mPanelManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::DispensaryView::OnAuiThemeChange, this));
}

void pof::DispensaryView::Load(const pof::base::data::row_t& prescription)
{
	LoadPropertyGrid(prescription);
}


void pof::DispensaryView::CreateToolBar()
{
	auto bar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW);

	bar->Realize();
	mPanelManager.AddPane(bar, wxAuiPaneInfo().Name(wxT("Tool")).Caption(wxT("Tool bar")).ToolbarPane().Top()
		.Resizable().MinSize(wxSize(-1, 30)).DockFixed()
		.LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::DispensaryView::CreateDataView()
{
	mDataModel = std::make_unique<pof::DataModel>();
	mDataView = std::make_unique<wxDataViewCtrl>(this, DATA_VIEW, wxDefaultPosition, wxDefaultSize,
		wxDV_ROW_LINES | wxNO_BORDER);
	mDataView->AssociateModel(mDataModel.get());
	mDataModel->DecRef();
	//columns
	mDataView->AppendToggleColumn("Approve", 10, wxDATAVIEW_CELL_ACTIVATABLE, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Id", 0, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Medication", 1, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Dosage form", 2, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Strength", 3, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Direction for use", 4, wxDATAVIEW_CELL_INERT, 400, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendTextColumn("Quantity", 5, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mDataView->AppendBitmapColumn("Status", 11, wxDATAVIEW_CELL_INERT, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	SetSpecialCol();

	mPanelManager.AddPane(mDataView.get(), wxAuiPaneInfo().Name("DataView").Caption("Medications").CenterPane().PinButton());
}

void pof::DispensaryView::SetSpecialCol()
{
	pof::DataModel::SpeicalColHandler_t handler;
	pof::DataModel::SpeicalColHandler_t statusHandler;

	handler.first = [self = this](size_t col, size_t row) -> wxVariant {
		return (self->mSelections.find(pof::DataModel::GetItemFromIdx(row)) != self->mSelections.end());
	};

	handler.second = [&](size_t col, size_t row, const wxVariant& var) ->bool {
		auto item = pof::DataModel::GetItemFromIdx(row);
		if (var.GetBool()) {
			mSelections.insert(item);
		}
		else {
			mSelections.erase(item);
		}
		return true;
	};

	statusHandler.first = [&](size_t col, size_t row) -> wxVariant {
		auto& datastore = mDataModel->GetDatastore();
		std::uint64_t status = boost::variant2::get<std::uint64_t>(datastore[row].first[col]);
		switch (status)
		{
		case STATUS_DISPENSED:
			return wxVariant(ArtProvider::GetBitmap("action_check"));
		case STATUS_REJECTED:
			return wxVariant(ArtProvider::GetBitmap("action_delete"));
		case STATUS_PEND:
			return wxVariant(ArtProvider::GetBitmap("action_remove"));
		default:
			return {};
			break;
		}
	};


	mDataModel->SetSpecialColumnHandler(DISPENSE_STATUS, std::move(handler));
	mDataModel->SetSpecialColumnHandler(11, std::move(statusHandler));
}

void pof::DispensaryView::CreatePropertyGrid()
{
	mPropertyManager = std::make_unique<wxPropertyGridManager>(this, PROPERTY_MANAGER,
		wxDefaultPosition, wxSize(500, -1), wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPGMAN_DEFAULT_STYLE);
	CreatePropertyGridToolBar();
	auto page = mPropertyManager->AddPage("Prescription detail", wxArtProvider::GetBitmap("user"));
	Freeze();
	page->Append(new wxPropertyCategory("Patient details"));
	page->Append(new wxStringProperty("Patients name", wxPG_LABEL));
	page->Append(new wxIntProperty("Patients age", wxPG_LABEL));
	page->Append(new wxStringProperty("Patients weight", wxPG_LABEL));
	page->Append(new wxStringProperty("Patients address", wxPG_LABEL));
	page->Append(new wxPropertyCategory("Prescribers details"));
	page->Append(new wxStringProperty("Prescribers name", wxPG_LABEL));
	page->Append(new wxStringProperty("Prescribers licience number", wxPG_LABEL));
	page->Append(new wxStringProperty("Prescribers health center", wxPG_LABEL));
	CreatePropertyGridArt();
	CreateEditProperty();
	Thaw();
	mPanelManager.AddPane(mPropertyManager.get(), wxAuiPaneInfo().Name("PrescriptionDetail").Caption("Prescription detail").Bottom().MinSize(500, -1).Layer(1).Position(1).PinButton().MinimizeButton());
	mPanelManager.Update();
}

void pof::DispensaryView::LoadPropertyGrid(const pof::base::data::row_t& prescription)
{
	auto page = mPropertyManager->GetPage("Prescription detail");
	auto& v = prescription.first;
	if (page) {
		mPropertyManager->Freeze();
		page->GetProperty("Patients name")->SetValue(boost::variant2::get<pof::base::data::text_t>(v[pof::PrescriptionView::PRESCRIPTION_PATIENT_NAME]));
		page->GetProperty("Patients age")->SetValue(boost::variant2::get<pof::base::data::text_t>(v[pof::PrescriptionView::PRESCRIPTION_PATIENT_AGE]));
		page->GetProperty("Patients weight")->SetValue(boost::variant2::get<pof::base::data::text_t>(v[pof::PrescriptionView::PRESCRIPTION_PATIENT_WEIGHT]));
		page->GetProperty("Patients address")->SetValue(boost::variant2::get<pof::base::data::text_t>(v[pof::PrescriptionView::PRESCRIPTION_PATIENT_ADDRESS]));
		page->GetProperty("Prescribers name")->SetValue(boost::variant2::get<pof::base::data::text_t>(v[pof::PrescriptionView::PRESCRIPTION_PATIENT_NAME]));
		page->GetProperty("Prescribers licience number")->SetValue(boost::variant2::get<pof::base::data::text_t>(v[pof::PrescriptionView::PRESCRIPTION_PATIENT_NAME]));
		page->GetProperty("Prescribers health center")->SetValue(boost::variant2::get<pof::base::data::text_t>(v[pof::PrescriptionView::PRESCRIPTION_PATIENT_NAME]));
		mPropertyManager->Thaw();
		mPanelManager.GetPane("PrescriptionDetail").Show();
		mPanelManager.Update();
	}
}
void pof::DispensaryView::CreatePropertyGridToolBar()
{
	auto tool = mPropertyManager->GetToolBar();
	if (tool) {
		tool->SetWindowStyleFlag(wxTB_HORZ_TEXT);
		tool->Realize();
		Update();
	}

}

void pof::DispensaryView::CreatePropertyGridArt()
{
	auto grid = mPropertyManager->GetPage("Prescription detail")->GetGrid();
	grid->SetBackgroundColour(*wxWHITE);
	grid->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->SetCaptionTextColour(*wxBLACK);

}

void pof::DispensaryView::CreateEditProperty()
{
	auto EditPrescription = mPropertyManager->AddPage("Prescription edit", wxArtProvider::GetBitmap("action_add"));
	EditPrescription->Append(new wxPropertyCategory("Prescribed Medication details", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Medication", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Dosage form", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Strength", wxPG_LABEL));
	EditPrescription->Append(new wxStringProperty("Direction for use", wxPG_LABEL));
	EditPrescription->Append(new wxIntProperty("Quantity", wxPG_LABEL));
}

void pof::DispensaryView::ResetViewData()
{
	for (int i = 0; i < 2; i++) {
		auto iter = mPropertyManager->GetPage(i)->GetIterator();
		while (!iter.AtEnd()) {
			auto prop = iter.GetProperty();
			if (prop && !prop->IsCategory()) {
				prop->SetValue(wxVariant());
			}
			(void)iter++;
		}
	}
	mSelections.clear();
}

void pof::DispensaryView::ResetModifiedStatus()
{
	auto grid = mPropertyManager->GetPage("Prescription edit")->GetGrid();
	wxPropertyGridIterator iter = grid->GetIterator();
	while (!iter.AtEnd()) {
		iter.GetProperty()->SetFlagRecursively(wxPG_PROP_MODIFIED, false);
		iter.Next();
	}
	grid->ClearModifiedStatus();
}

void pof::DispensaryView::Dispense()
{
	if (mSelections.empty()) {
		wxMessageBox("No item selected to be dispensed", "DISPENSARY", wxOK | wxICON_INFORMATION);
		return;
	}
	if (!CheckDispensed()) return; //refuse to dispens already dispensed

	UpdatePrescription();
}

void pof::DispensaryView::PreviewLabel()
{
	
}

bool pof::DispensaryView::CheckDispensed()
{
	std::vector<std::string> stringvec;
	stringvec.reserve(mSelections.size());
	for (auto& medIndex : mSelections) {
		size_t idx = pof::DataModel::GetIdxFromItem(medIndex);
		auto& med = mDataModel->GetDatastore()[idx];
		if (boost::variant2::get<std::uint64_t>(med.first[DISPENSE_STATUS]) == STATUS_DISPENSED) {
			stringvec.emplace_back(boost::variant2::get<pof::base::data::text_t>(med.first[DISPENSE_MEDIACTION]));
		}
	}
	stringvec.shrink_to_fit();
	bool ret = true;
	if (!stringvec.empty()) {
		std::string message = fmt::format("The following medications have been dispensed \n {} \n Do you wish to continue?", fmt::join(stringvec, "\n"));
		ret = wxMessageBox(message, "DISPENSED", wxICON_WARNING | wxYES_NO);
	}
	return ret;
}



void pof::DispensaryView::UpdatePrescription()
{
	
	//update the prescription table 
}


void pof::DispensaryView::OnDataViewItemSelected(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& med = mDataModel->GetDatastore()[idx].first;
	auto edp = mPropertyManager->GetPage("Prescription edit");
	if (edp) {
		mPropertyManager->Freeze();
		edp->GetProperty("Medication")->SetValue(boost::variant2::get<pof::base::data::text_t>(med[DISPENSE_MEDIACTION]));
		edp->GetProperty("Dosage form")->SetValue(boost::variant2::get<pof::base::data::text_t>(med[DISPENSE_DOSAGE_FORM]));
		edp->GetProperty("Strength")->SetValue(boost::variant2::get<pof::base::data::text_t>(med[DISPENSE_STRENGTH]));
		edp->GetProperty("Direction for use")->SetValue(boost::variant2::get<pof::base::data::text_t>(med[DISPENSE_DIR_FOR_USE]));
		edp->GetProperty("Quantity")->SetValue(boost::variant2::get<std::int32_t>(med[DISPENSE_QUANTITY]));
		ResetModifiedStatus();
		mPropertyManager->Thaw();
	}
}

void pof::DispensaryView::OnPropertyChanged(wxPropertyGridEvent& evt)
{
	
}

void pof::DispensaryView::OnPropertyChanging(wxPropertyGridEvent & evt)
{
	spdlog::info("Changing: {}", evt.GetPropertyName().ToStdString());
	auto prop = evt.GetProperty();
	if (prop) {
		if (evt.GetPropertyName() == "Direction for use") {
			prop->SetValue(wxVariant(EditText(evt.GetValue().GetString().ToStdString())));
		}
	}
}

std::string pof::DispensaryView::EditText(const std::string& text)
{
	return std::string("This is a test");
}
