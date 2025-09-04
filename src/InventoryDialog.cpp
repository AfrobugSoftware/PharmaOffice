#include "InventoryDialog.h"
//#include "Application.h"
#include "PofPch.h"
#include "ProductManager.h" //circular dependcies ? 

BEGIN_EVENT_TABLE(pof::InventoryDialog, wxDialog)
EVT_BUTTON(wxID_OK, InventoryDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, InventoryDialog::OnCancel)
EVT_BUTTON(InventoryDialog::ID_CALENDAR, InventoryDialog::OnCalendar)
END_EVENT_TABLE()

pof::InventoryDialog::InventoryDialog(wxWindow* parent, const pof::base::data::duuid_t& uid)
: wxDialog(parent, wxID_ANY, "Add Stock", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL),
	mProductUuid(uid){
	this->SetSize(FromDIP(wxSize(591, 398)));
	CreateDialog();
	SizeDialog();
}

bool pof::InventoryDialog::TransferDataFromWindow()
{
	try {
		wxBusyCursor cursor;
		mInventoryData.first.resize(pof::ProductManager::INVENTORY_MAX);
		mInventoryData.first[pof::ProductManager::INVENTORY_ID] = pof::GenRandomId();
		mInventoryData.first[pof::ProductManager::INVENTORY_INPUT_DATE] = pof::Application::clock_t::now();
		mInventoryData.first[pof::ProductManager::INVENTORY_LOT_NUMBER] = mBatchNumber->GetValue().ToStdString();
		std::uint64_t stok = static_cast<std::uint64_t>(mQuantityInControl->GetValue());
		if (stok == 0) {
			wxMessageBox("Quantity in cannot be 0, please try again", "Add stock", wxICON_WARNING | wxOK);
			return false;
		}
		mInventoryData.first[pof::ProductManager::INVENTORY_STOCK_COUNT] = stok;
		mInventoryData.first[pof::ProductManager::INVENTORY_COST] = pof::base::currency(boost::lexical_cast<float>(mCostControl->GetValue().ToStdString()));
		auto expDate = date::floor<date::days>(std::chrono::system_clock::from_time_t(mExpiryDate->GetValue().GetTicks()));
		auto nowDate = date::floor<date::days>(pof::base::data::clock_t::now()) - date::days(1);
		if (expDate == nowDate) {
			wxMessageBox("Expiry date cannot be today's date, check and try again", "Add Stock", wxICON_INFORMATION | wxOK);
			return false;
		}
		mInventoryData.first[pof::ProductManager::INVENTORY_EXPIRE_DATE] = std::chrono::system_clock::from_time_t(mExpiryDate->GetValue().GetTicks());
		mInventoryData.first[pof::ProductManager::INVENTORY_MANUFACTURER_NAME] = mManufacturersName->GetValue().ToStdString();

		if (mAddSupplier->IsChecked())
		{
			auto manuName = mManufacturersName->GetValue().ToStdString();

			if (manuName.empty()) {
				wxMessageBox("Supplier name cannot be empty if Add supplier is checked.\nPlease add a supplier name", "Add stock", wxICON_WARNING | wxOK);
				return false;
			}

			if (mInvoiceName->IsEmpty())
			{
				wxMessageBox("Invoice id cannot be empty if Add supplier is checked.\nPlease add an invoice id", "Add stock", wxICON_WARNING | wxOK);
				return false;
			}
			auto& suppliers = wxGetApp().mProductManager.GetSupplier()->GetDatastore();
			auto it = std::ranges::find_if(suppliers, [&](const pof::base::data::row_t& row) -> bool {
				return boost::variant2::get<std::string>(row.first[pof::ProductManager::SUPPLIER_NAME]) == manuName;
			});

			if (it == suppliers.end()) {
				//new supplier
				pof::base::data::row_t row;
				auto& v = row.first;
				v.resize(pof::ProductManager::SUPPLIER_MAX);
				auto dt = pof::base::data::clock_t::now();

				v[pof::ProductManager::SUPPLIER_ID] = pof::GenRandomId();
				v[pof::ProductManager::SUPPLIER_NAME] = manuName;
				v[pof::ProductManager::SUPPLIER_DATE_CREATED] = dt;
				v[pof::ProductManager::SUPPLIER_DATE_MODIFIED] = dt;
				v[pof::ProductManager::SUPPLIER_INFO] = ""s;


				//Add invoice entry for this stock
				pof::base::data::row_t inv;
				auto& i = inv.first;
				i.resize(pof::ProductManager::INVOICE_MAX);

				i[pof::ProductManager::INVOICE_SUPP_ID] = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::SUPPLIER_ID]);
				i[pof::ProductManager::INVOICE_ID] = mInvoiceName->GetValue().ToStdString();
				i[pof::ProductManager::INVOICE_PROD_UUID] = mProductUuid;
				i[pof::ProductManager::INVOICE_INVENTORY_ID] = boost::variant2::get<std::uint64_t>(mInventoryData.first[pof::ProductManager::INVENTORY_ID]);


				wxGetApp().mProductManager.GetInvoices()->StoreData(std::move(inv));
				wxGetApp().mProductManager.GetSupplier()->StoreData(std::move(row));
			}
			else {
				pof::base::data::row_t inv;
				auto& i = inv.first;
				i.resize(pof::ProductManager::INVOICE_MAX);

				i[pof::ProductManager::INVOICE_SUPP_ID] = boost::variant2::get<std::uint64_t>(it->first[pof::ProductManager::SUPPLIER_ID]);
				i[pof::ProductManager::INVOICE_ID] = mInvoiceName->GetValue().ToStdString();
				i[pof::ProductManager::INVOICE_PROD_UUID] = mProductUuid;
				i[pof::ProductManager::INVOICE_INVENTORY_ID] = boost::variant2::get<std::uint64_t>(mInventoryData.first[pof::ProductManager::INVENTORY_ID]);

				//
				it->second.second.set(pof::ProductManager::SUPPLIER_DATE_MODIFIED);
				it->first[pof::ProductManager::SUPPLIER_DATE_MODIFIED] = std::chrono::system_clock::now();

				wxGetApp().mProductManager.UpdateSupplier(it);
				wxGetApp().mProductManager.GetInvoices()->StoreData(std::move(inv));
			}
		}
		
		return true;
	}
	catch (const std::exception& exp){
		spdlog::error(exp.what());
		wxMessageBox("Invalid input in inventory form", "Add stock", wxICON_ERROR | wxOK);
		return false;
	}
}

bool pof::InventoryDialog::TransferDataToWindow()
{
	if (wxGetApp().bAutomaticBatchNumber) {

		auto lotNum = wxGetApp().mProductManager.GetLastInventoryBatchNumber(mProductUuid);
		//only work if we get valid data from the database
		if (lotNum) {
			auto& v = lotNum.value();
			if (std::ranges::all_of(v, [&](char c) -> bool {return std::isdigit(c); })) {
				//if it a digit
				try {
					size_t c = boost::lexical_cast<size_t>(v);
					c++;
					mBatchNumber->SetValue(std::to_string(c));
				}
				catch (std::exception& exp) {
					spdlog::error(exp.what());
				}
			}
		}
	}
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
	texts[7] = new wxStaticText;

	texts[0]->Create(this, wxID_ANY, "Enter new stock for product", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[0]->SetFont(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS).Bold()));

	texts[1]->Create(this, wxID_ANY, "Batch Number: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[2]->Create(this, wxID_ANY, "Quantity In: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[3]->Create(this, wxID_ANY, "Expiry Date: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[4]->Create(this, wxID_ANY, "Please fill in the form to enter stock for product", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[5]->Create(this, wxID_ANY, fmt::format("Cost Price per unit ({}): ", pof::base::data::currency_t::cur_type), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[6]->Create(this, wxID_ANY, "Supplier's Name: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	texts[7]->Create(this, wxID_ANY, "Invoice number: ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	
	mOkCancel[0] = new wxButton;
	mOkCancel[1] = new wxButton;
	mOkCancel[0]->Create(this, wxID_OK, "OK");
	mOkCancel[1]->Create(this, wxID_CANCEL, "Cancel");
	//look for a calender icon
	wxFloatingPointValidator<float> val(2, &mFloatValidator, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 999999999999);


	mQuantityInControl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(200, -1)), wxSP_ARROW_KEYS| wxALIGN_LEFT, 0,
		std::numeric_limits<int>::max());
	mCostControl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(200, -1)));
	mCostControl->SetValidator(val);

	mExpiryDate = new wxDatePickerCtrl(this, ID_DATE_PICKER, wxDateTime::Now(), wxDefaultPosition, FromDIP(wxSize(200, -1)), wxDP_DROPDOWN);
	mExpiryDate->SetRange(wxDateTime::Now(), wxDateTime{});
	mBatchNumber = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(200, -1)));
	mBatchNumber->SetValidator(wxTextValidator{ wxFILTER_DIGITS  | wxFILTER_EMPTY});
	//mManufactureName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	//mManufactureName->SetValidator(wxTextValidator{  wxFILTER_EMPTY });

	auto choices = SetupManufacturerName();
	mManufacturersName = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices);
	mManufacturersName->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	mInvoiceName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	mAddSupplier = new wxCheckBox(this, wxID_ANY, "Add supplier", wxDefaultPosition, wxDefaultSize);
}

void pof::InventoryDialog::SizeDialog()
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* okCancleSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer* flexSizer = new wxFlexGridSizer(7,3, FromDIP(2),FromDIP(5));
	flexSizer->AddGrowableCol(1);
	flexSizer->SetFlexibleDirection(wxBOTH);
	flexSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);


	okCancleSizer->AddStretchSpacer();
	okCancleSizer->Add(mOkCancel[0], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, FromDIP(5)));
	okCancleSizer->Add(mOkCancel[1], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, FromDIP(5)));

	flexSizer->Add(texts[1], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mBatchNumber, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[2], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mQuantityInControl, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[3], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mExpiryDate, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[5], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mCostControl, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(mAddSupplier, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	flexSizer->AddStretchSpacer();
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[6], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mManufacturersName, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	flexSizer->AddStretchSpacer();

	flexSizer->Add(texts[7], wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));
	flexSizer->Add(mInvoiceName, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	flexSizer->AddStretchSpacer();


	boxSizer->Add(texts[0], wxSizerFlags().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	boxSizer->Add(texts[4], wxSizerFlags().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	boxSizer->Add(flexSizer, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	boxSizer->Add(okCancleSizer, wxSizerFlags().Expand().Border(wxALL, FromDIP(5)));

	topSizer->Add(boxSizer, wxSizerFlags().Expand().Border(wxALL, FromDIP(5)));
	SetSizer(topSizer);
	//topSizer->SetSizeHints(this);
	Center();

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

void pof::InventoryDialog::OnOk(wxCommandEvent& evt)
{
	wxWindowID ret_code = wxID_OK;
	if (Validate() && TransferDataFromWindow()){
		if (IsModal()) {
			EndModal(wxID_OK);
		}
	}
}

void pof::InventoryDialog::OnCancel(wxCommandEvent& evt)
{
	if (wxMessageBox(wxT("Are you sure you want to cancel stock entry"), wxT("Stock entry"), wxYES | wxNO) == wxYES)
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

wxArrayString pof::InventoryDialog::SetupManufacturerName()
{
	wxArrayString ret;
	ret.push_back("ENTRY");

	auto suppliers = wxGetApp().mProductManager.GetSupplierNames();
	if (suppliers.has_value()) {
		for (auto& s : suppliers.value()) {
			ret.push_back(s);
		}
	}
	return ret;
}

wxArrayString pof::InventoryDialog::SetupInvoicesName()
{
	return wxArrayString();
}

