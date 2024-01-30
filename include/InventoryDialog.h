#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/calctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/sizer.h>
#include <wx/valnum.h>
#include <wx/datectrl.h>

#include "ArtProvider.h"
#include <Data.h>
#include <random>

namespace pof {
	class InventoryDialog : public wxDialog
	{
	public:
		InventoryDialog(wxWindow* parent, const pof::base::data::duuid_t& uid);

		virtual bool TransferDataFromWindow() override;
		virtual bool TransferDataToWindow() override;
		inline pof::base::data::row_t& GetData() { return mInventoryData; }

		pof::base::data::duuid_t mProductUuid;
		enum
		{
			ID_CALENDAR = wxID_HIGHEST + 2333,
			ID_IMPROPER_DATE,
			ID_DATE_PICKER,
			ID_SUPPLIER_NAME_COMBO,
			ID_INVOICE_NAME_COMBO,
		};
	private:
		void CreateDialog();
		void SizeDialog();

		wxStaticText* texts[9] = {0};
		wxSpinCtrl* mQuantityInControl = nullptr;
		wxTextCtrl* mCostControl = nullptr;
		wxDatePickerCtrl* mExpiryDate = nullptr;
		wxTextCtrl* mBatchNumber = nullptr;
		//wxTextCtrl* mManufactureName = nullptr;
		wxBitmapButton* mCalenderButton = nullptr;
		wxButton* mOkCancel[2] = {0};
		wxComboBox* mManufacturersName = nullptr;
		wxTextCtrl* mInvoiceName = nullptr;
		wxCheckBox* mAddSupplier = nullptr;
		pof::base::data::row_t mInventoryData;
		float mFloatValidator = 0.0f;
	private:
		void OnOk(wxCommandEvent& evt);
		void OnCancel(wxCommandEvent& evt);
		void OnCalendar(wxCommandEvent& evt);


		wxArrayString SetupManufacturerName();
		wxArrayString SetupInvoicesName();

		DECLARE_EVENT_TABLE()

	private:
	
	};
}

