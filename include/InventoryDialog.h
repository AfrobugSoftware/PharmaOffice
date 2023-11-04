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
		InventoryDialog(wxWindow* parent);

		virtual bool TransferDataFromWindow() override;
		virtual bool TransferDataToWindow() override;
		inline pof::base::data::row_t& GetData() { return mInventoryData; }

		pof::base::data::duuid_t mProductUuid;
		enum
		{
			ID_CALENDAR = wxID_HIGHEST + 2333,
			ID_IMPROPER_DATE,
			ID_DATE_PICKER,
		};
	private:
		void CreateDialog();
		void SizeDialog();

		wxStaticText* texts[8];
		wxSpinCtrl* mQuantityInControl;
		wxSpinCtrl* mCostControl;
		wxDatePickerCtrl* mExpiryDate;
		wxTextCtrl* mBatchNumber;
		wxTextCtrl* mManufactureName;
		wxBitmapButton* mCalenderButton;
		wxButton* mOkCancel[2];
		pof::base::data::row_t mInventoryData;

	private:
		void OnOk(wxCommandEvent& evt);
		void OnCancel(wxCommandEvent& evt);
		void OnCalendar(wxCommandEvent& evt);


		DECLARE_EVENT_TABLE()

	private:
	
	};
}

