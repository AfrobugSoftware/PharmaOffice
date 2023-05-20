#pragma once
#include <wx/wx.h>
#include <wx/dialog.h>
#include "Data.h"
#include <wx/statline.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>
#include <wx/valnum.h>
#include <algorithm>
#include <sstream>

namespace pof {
	class AddProductDialog : public wxDialog
	{
	public:
		virtual bool TransferDataFromWindow() override;
		virtual bool TransferDataToWindow() override;
	private:
		wxComboBox* mProductCategoryCtrl;
		wxChoice* mProductClassCtrl;
		wxTextCtrl* mProductNameCtrl;
		wxTextCtrl* mProductActiveIngredentCtrl;
		wxTextCtrl* mProductDescCtrl;
		wxTextCtrl* mProductDirForUseCtrl;
		wxTextCtrl* mProductUnitPriceCtrl;
		wxTextCtrl* mProductCostPriceCtrl;
		wxSpinCtrl* mProductPackageSizeCtrl;
		wxStaticText* descp[10];
		wxButton* mOkButton;
		wxButton* mCancelButton;
		wxWindow* wPane;
		wxCollapsiblePane* pane;
		float validate_float;
		wxArrayString choices;


		pof::base::data::row_t mProductData;
	};
};