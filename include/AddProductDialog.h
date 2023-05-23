#pragma once
#include <wx/artprov.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/valtext.h>
#include <wx/valnum.h>


#include "Data.h"
#include "ProductManager.h"


namespace pof
{
	class AddProdutDialog : public wxDialog
	{
	private:

	protected:
		wxPanel* m_panel1;
		wxStaticText* TitleText;
		wxPanel* m_panel2;
		wxPanel* m_panel4;
		wxScrolledWindow* m_scrolledWindow2;
		wxStaticText* mProductName;
		wxTextCtrl* mProductNameValue;
		wxStaticText* mProductGenericName;
		wxTextCtrl* m_textCtrl12;
		wxStaticText* mFormulation;
		wxChoice* mFormulationValue;
		wxStaticText* mStrength;
		wxTextCtrl* mStrengthValue;
		wxStaticText* mStrengthType;
		wxChoice* mStrengthTypeValue;
		wxStaticText* mClassLabel;
		wxChoice* mClassValue;
		wxPanel* m_panel5;
		wxScrolledWindow* m_scrolledWindow1;
		wxPanel* m_panel71;
		wxStaticText* mCostPriceLabel;
		wxTextCtrl* mCostPriceValue;
		wxStaticText* mSalePriceLabel;
		wxTextCtrl* m_textCtrl7;
		wxCheckBox* mDoMarkup;
		wxStaticText* mCategoryLabel;
		wxChoice* mCategoryValue;
		wxStaticText* mPackageSizeLabel;
		wxTextCtrl* m_textCtrl121;
		wxStaticText* mProductDescription;
		wxTextCtrl* mProductDescriptionValue;
		wxBitmapButton* mMoreDescription;
		wxStaticText* mDirectionForUse;
		wxTextCtrl* mDirForUseValue;
		wxBitmapButton* mMoreDirForUse;
		wxStaticText* mHealthConditions;
		wxTextCtrl* mHealthConditionsValue;
		wxBitmapButton* mMoreHealthConditons;
		wxStaticText* mSideEffectsLabel;
		wxTextCtrl* mSideEffectsValue;
		wxBitmapButton* mMoreSideffects;
		wxPanel* m_panel8;
		wxButton* mScanProduct;
		wxPanel* m_panel6;
		wxScrolledWindow* m_scrolledWindow3;
		wxStaticText* mBacthNumber;
		wxTextCtrl* mBatchNumbeValue;
		wxStaticText* m_staticText8;
		wxDatePickerCtrl* m_datePicker1;
		wxStaticText* mQuntity;
		wxTextCtrl* mQunatityValue;
		wxPanel* m_panel7;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		wxButton* m_sdbSizer2Cancel;

		pof::base::data::row_t datum;
		wxArrayString ProductClassChoices;
		wxArrayString FormulationChoices;
		wxArrayString ExpChoices;
		wxArrayString StrengthChoices;
		double mFloatValidator;

		void WriteToDatabase();


	public:
		//controls id
		enum {
			ID_PRODUCT_NAME,
		};
		AddProdutDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("ADD PRODUCT"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1122, 762), long style = wxDEFAULT_DIALOG_STYLE);
		~AddProdutDialog();

		virtual bool TransferDataFromWindow() override final;
		inline constexpr pof::base::data::row_t GetAddedProduct() const { return datum; }
	};


};
