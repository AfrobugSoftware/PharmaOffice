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
#include <wx/srchctrl.h>

#include "SaleManager.h"
#include "DropTarget.h"
#include "SearchPopup.h"

#include <ranges>
#include <numeric>

namespace pof
{
	class SaleView : public wxPanel
	{
	private:

	protected:
		wxPanel* mMainPane;
		wxPanel* mTopTools;
		wxStaticText* mProductNameText;
		wxSearchCtrl* mProductNameValue;
		wxStaticText* mScanProduct;
		wxSearchCtrl* mScanProductValue;
		wxPanel* mDataPane;
		wxDataViewCtrl* m_dataViewCtrl1;
		wxDataViewColumn* mSerialNumber;
		wxDataViewColumn* mProductNameCol;
		wxDataViewColumn* mQuantityColumn;
		wxDataViewColumn* mExtPriceColumn;
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
		wxStaticText* mTotalAmountLabel;
		wxStaticText* mTotalAmount;
		wxPanel* mSalePaymentButtonsPane;
		wxButton* mClear;
		wxButton* mSave;
		wxButton* mCheckout = nullptr;
		pof::SearchPopup* mSearchPopup = nullptr;
		pof::base::database::stmt_t mExpiredStatement = nullptr;
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

		void CreateSpecialColumnHandlers();
		void CreateSearchPopup();
	protected:
		//sale operations
		void UpdateSaleDisplay();
		void Checkout();

		void OnClear(wxCommandEvent& evt);
		void OnCheckout(wxCommandEvent& evt);
		void OnSave(wxCommandEvent& evt);
		void OnDropPossible(wxDataViewEvent& evt);
		void OnDrop(wxDataViewEvent& evt);

		void OnBeginDrag(wxDataViewEvent& evt);
		void OnProductNameSearch(wxCommandEvent& evt);


		void OnValueChanged(wxDataViewEvent& evt);
		void OnEditingStarted(wxDataViewEvent& evt);
		void OnEditingDone(wxDataViewEvent& evt);

		void DropData(const pof::DataObject& dat);
		void OnSearchPopup(const pof::base::data::row_t& row);
	
		bool CheckInStock(const pof::base::data::row_t& product);
		bool CheckProductClass(const pof::base::data::row_t& product);
		bool CheckExpired(const pof::base::data::row_t& product);

		void ProductNameKeyEvent(); //test
	private:
		pof::base::data::row_t mDropRow; //dummy row required by pof::DataObject

		DECLARE_EVENT_TABLE();
	};
};
