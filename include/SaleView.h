#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/button.h>

#include "SaleManager.h"
#include "DropTarget.h"

namespace pof
{
	class SaleView : public wxPanel
	{
	private:

	protected:
		wxPanel* mMainPane;
		wxPanel* mTopTools;
		wxStaticText* mProductNameText;
		wxTextCtrl* mProductNameValue;
		wxStaticText* mScanProduct;
		wxTextCtrl* mScanProductValue;
		wxPanel* mDataPane;
		wxDataViewCtrl* m_dataViewCtrl1;
		wxDataViewColumn* mSerialNumber;
		wxDataViewColumn* mProductNameCol;
		wxDataViewColumn* mQuantityColumn;
		wxDataViewColumn* mPriceCol;
		wxPanel* mSaleOutputPane;
		wxPanel* mSaleDisplayPane;
		wxPanel* mTextOutPut;
		wxStaticText* mQuantity;
		wxStaticText* mQuantityValue;
		wxStaticText* mExtQuantity;
		wxStaticText* mExtQuantityItem;
		wxStaticText* mDiscountAmount;
		wxStaticText* mDiscountValue;
		wxStaticText* mTotalQuantity;
		wxStaticText* mTotalQuantityValue;
		wxPanel* mSalePaymentButtonsPane;
		wxButton* mClear;
		wxButton* mSave;
		wxButton* mCheckout;

	public:

		enum {
			ID_PRODUCT_SEARCH_NAME,
			ID_PRODUCT_SCAN,
			ID_SALE_DATA_VIEW,
			ID_CHECKOUT,
			ID_SAVE,
			ID_CLEAR,
		};


		SaleView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(917, 668), long style = wxTAB_TRAVERSAL);
		~SaleView();
		void SetupDropTarget();
	protected:
		void OnClear(wxCommandEvent& evt);
		void OnCheckout(wxCommandEvent& evt);
		void OnSave(wxCommandEvent& evt);
		void OnDropPossible(wxDataViewEvent& evt);
		void OnDrop(wxDataViewEvent& evt);

		void DropData(const pof::DataObject& dat);
	private:
		pof::base::data::row_t mDropRow; //dummy row required by pof::DataObject

		DECLARE_EVENT_TABLE();
	};
};
