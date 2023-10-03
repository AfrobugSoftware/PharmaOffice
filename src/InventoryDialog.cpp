#include "InventoryDialog.h"
//#include "Application.h"
#include "PofPch.h"
#include "ProductManager.h" //circular dependcies ? 

BEGIN_EVENT_TABLE(pof::InventoryDialog, wxDialog)
EVT_BUTTON(wxID_OK, InventoryDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, InventoryDialog::OnCancel)
EVT_BUTTON(InventoryDialog::ID_CALENDAR, InventoryDialog::OnCalendar)
END_EVENT_TABLE()

pof::InventoryDialog::InventoryDialog(wxWindow* parent)
: wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(800, 348), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL){
	CreateDialog();
	SizeDialog();
}

bool pof::InventoryDialog::TransferDataFromWindow()
{
	static std::mt19937 engine(std::random_device{}());
	static std::uniform_int_distribution<std::uint64_t> distrb(std::numeric_limits<std::uint64_t>::min(),
		std::numeric_limits<uint64_t>::max());
	static auto rrand = std::bind(distrb, engine);


	mInventoryData.first.resize(pof::ProductManager::INVENTORY_MAX);
	mInventoryData.first[pof::ProductManager::INVENTORY_ID] = rrand();
	mInventoryData.first[pof::ProductManager::INVENTORY_INPUT_DATE] = pof::Application::clock_t::now();
	mInventoryData.first[pof::ProductManager::INVENTORY_LOT_NUMBER] = mBatchNumber->GetValue().ToStdString();
	mInventoryData.first[pof::ProductManager::INVENTORY_STOCK_COUNT] = static_cast<std::uint64_t>(mQuantityInControl->GetValue());
	mInventoryData.first[pof::ProductManager::INVENTORY_COST] = pof::base::currency(static_cast<float>(mCostControl->GetValue()));
	mInventoryData.first[pof::ProductManager::INVENTORY_EXPIRE_DATE] = std::chrono::system_clock::from_time_t(mExpiryDate->GetValue().GetTicks());
	mInventoryData.first[pof::ProductManager::INVENTORY_MANUFACTURER_NAME] = mManufactureName->GetValue().ToStdString();
	return true;
}

bool pof::InventoryDialog::TransferDataToWindow()
{
	return true;
}

void pof::InventoryDialog::CreateDialog()
{
	SetBackgroundColour(*wxWHITE);
	ClearBackground();
	texts[0] = new wxStaticText;
	texts[1] = new wxStaticText;
	texts[2] = new wxStaticText;
	texts[3] = new wxStaticText;
	texts[4] = new wxStaticText;
	texts[5] = new wxStaticText;
	texts[6] = new wxStaticText;

	texts[0]->Create(this, wxID_ANY, "ENTER INVENTORY", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[0]->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	texts[1]->Create(this, wxID_ANY, "Batch Number: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[2]->Create(this, wxID_ANY, "Quantity In: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[3]->Create(this, wxID_ANY, "Expiry Date: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[4]->Create(this, wxID_ANY, "Please Enter Inventory For Product", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[5]->Create(this, wxID_ANY, fmt::format("Cost Price per unit ({}): ", pof::base::data::currency_t::cur_type), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[6]->Create(this, wxID_ANY, "Supplier's Name: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	
	mOkCancel[0] = new wxButton;
	mOkCancel[1] = new wxButton;
	mOkCancel[0]->Create(this, wxID_OK, "OK");
	mOkCancel[1]->Create(this, wxID_CANCEL, "Cancel");
	//look for a calender icon

	mQuantityInControl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxSP_ARROW_KEYS| wxALIGN_LEFT, 0,
		std::numeric_limits<int>::max());
	mCostControl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxSP_ARROW_KEYS| wxALIGN_LEFT, 0,
		999999999);
	mExpiryDate = new wxDatePickerCtrl(this, ID_DATE_PICKER, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	mBatchNumber = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mBatchNumber->SetValidator(wxTextValidator{ wxFILTER_DIGITS  | wxFILTER_EMPTY});
	mManufactureName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mManufactureName->SetValidator(wxTextValidator{  wxFILTER_EMPTY });
}

void pof::InventoryDialog::SizeDialog()
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okCancleSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer* flexSizer = new wxFlexGridSizer(5,3, 5,5);
	flexSizer->AddGrowableCol(1);
	flexSizer->SetFlexibleDirection(wxBOTH);
	flexSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);


	okCancleSizer->AddStretchSpacer();
	okCancleSizer->Add(mOkCancel[0], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	okCancleSizer->Add(mOkCancel[1], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	flexSizer->Add(texts[1], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mBatchNumber, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[2], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mQuantityInControl, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[3], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mExpiryDate, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[5], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mCostControl, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[6], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mManufactureName, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, 5));
	flexSizer->AddStretchSpacer();

	boxSizer->Add(texts[0], wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(texts[4], wxSizerFlags().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(flexSizer, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, 5));
	boxSizer->Add(okCancleSizer, wxSizerFlags().Expand().Border(wxALL, 5));

	topSizer->Add(boxSizer, wxSizerFlags().Expand().Border(wxALL, 5));
	SetSizer(topSizer);
	//topSizer->SetSizeHints(this);
	Center();
}

void pof::InventoryDialog::OnOk(wxCommandEvent& evt)
{
	wxWindowID ret_code;
	if (Validate() && TransferDataFromWindow()){
		if (IsModal()) {
			EndModal(wxID_OK);
		}
	}
}

void pof::InventoryDialog::OnCancel(wxCommandEvent& evt)
{
	if (wxMessageBox(wxT("Are you sure you want to cancel Inventory entry"), wxT("Inventory entry"), wxYES | wxNO) == wxYES)
	{
		if (IsModal()) EndModal(wxID_CANCEL);
		else
		{
			SetReturnCode(wxID_CANCEL);
			this->Show(false);
		}
	}
}

void pof::InventoryDialog::OnCalendar(wxCommandEvent& evt)
{
	
}

