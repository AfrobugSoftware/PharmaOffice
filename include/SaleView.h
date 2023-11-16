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
#include <wx/aui/auibar.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>

#include "PackView.h"
#include "SaleManager.h"
#include "DropTarget.h"
#include "SearchPopup.h"
#include "SaveSaleDialog.h"
#include "database.h"
#include "LabelInfo.h"

#include <ranges>
#include <unordered_map>
#include <numeric>
#include <nlohmann/json.hpp>


namespace pof
{
	class SaleView : public wxPanel
	{
	private:

	protected:
		wxPanel* mMainPane;
		wxAuiToolBar* mTopTools;
		wxAuiToolBar* mBottomTools;
		wxStaticText* mProductNameText;
		wxSearchCtrl* mProductNameValue;
		wxStaticText* mScanProduct;
		wxSearchCtrl* mScanProductValue;
		wxPanel* mProductViewPane = nullptr;
		wxPanel* mDataPane;
		wxPropertyGrid* mPropertyManager;
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
		std::string mCurPack;
		pof::base::data::duuid_t mCurPackID;
		wxInfoBar* mInfoBar = nullptr;
		wxStaticText* mActiveSaleId = nullptr;
		wxAuiToolBarItem* mActiveSaleTextItem = nullptr;

		//product properties
		wxStringProperty* productName = nullptr;
		wxStringProperty* genArray = nullptr;
		wxEditEnumProperty* dirArray = nullptr;
		wxIntProperty* stock = nullptr;
		wxStringProperty* strength = nullptr;
		wxStringProperty* strength_type = nullptr;
		wxEditEnumProperty* warning = nullptr;
		wxIntProperty* packageSize = nullptr;



		pof::base::data::duuid_t mCurSaleuuid;
		std::map<size_t, std::string> mCurDirForUse;
		std::map<size_t, std::string> mSideEffectWarnings; 

		boost::uuids::random_generator_mt19937 uuidGen;
		//pof::base::database::stmt_t mExpiredStatemet; 
	public:

		enum {
			ID_PRODUCT_SEARCH_NAME,
			ID_PRODUCT_SCAN,
			ID_SALE_DATA_VIEW,
			ID_CHECKOUT,
			ID_REMOVE_PRODUCT,
			ID_HIDE_PRODUCT_VIEW_PROPERTY,
			ID_SAVE,
			ID_CLEAR,
			ID_PRODUCT_VIEW_PROPERTY,
			ID_PRINT_LABELS,
			ID_PACKS,
			ID_FORM_M,
			ID_OPEN_SAVE_SALE,
			ID_ACTIVE_UI_TEXT,
			ID_REPRINT,
			ID_REPRINT_LAST_SALE,
			ID_RETURN_SALE,
			ID_FUNCTIONS,
			ID_FOCUS_SCAN,
		};

		std::vector<pof::LabelInfo> mProductLabels;

		SaleView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(917, 668), long style = wxTAB_TRAVERSAL);
		~SaleView();
		void SetupDropTarget();

		void CreateSpecialColumnHandlers();
		void CreateSearchPopup();
		void CreateProductDetails();
		void CreateAccelTable();

		bool OnAddMedicationsToSale(const pof::base::data& data);
	protected:
		//sale operations
		void UpdateSaleDisplay();
		void ResetSaleDisplay();
		void Checkout();

		void OnClear(wxCommandEvent& evt);
		void OnCheckout(wxCommandEvent& evt);
		void OnSaleComplete(bool status, size_t printState);
		void OnSave(wxCommandEvent& evt);
		void OnDropPossible(wxDataViewEvent& evt);
		void OnDrop(wxDataViewEvent& evt);

		void OnBeginDrag(wxDataViewEvent& evt);
		void OnProductNameSearch(wxCommandEvent& evt);
		void OnRemoveProduct(wxCommandEvent& evt);
		void OnSelected(wxDataViewEvent& evt);
		void OnHideProductViewProperty(wxCommandEvent& evt);
		void OnPrintAsLabels(wxCommandEvent& evt);
		void OnShowPacks(wxCommandEvent& evt);
		void OnFormM(wxCommandEvent& evt);
		void OnOpenSaveSale(wxCommandEvent& evt);

		void OnValueChanged(wxDataViewEvent& evt);
		void OnEditingStarted(wxDataViewEvent& evt);
		void OnEditingDone(wxDataViewEvent& evt);
		void OnSaleUuidTextUI(wxUpdateUIEvent& evt);
		void OnReturnSale(wxCommandEvent& evt);
		void OnReprintLastSale(wxCommandEvent& evt);
		void OnReprintSale(wxAuiToolBarEvent& evt);

		void DropData(const pof::DataObject& dat);
		void OnSearchPopup(const pof::base::data::row_t& row);
		void OnScanBarCode(wxCommandEvent& evt);
		void OnProductPropertyChanged(wxPropertyGridEvent& evt);
	

		bool CheckInStock(const pof::base::data::row_t& product);
		bool CheckProductWarning(const pof::base::data::duuid_t& pid);
		bool CheckProductClass(const pof::base::data::row_t& product);
		bool CheckExpired(const pof::base::data::row_t& product);
		std::optional<pof::base::data::iterator> CheckAlreadyAdded(const pof::base::data::text_t& productName);
		void ProductNameKeyEvent(); //test
		void LoadProductDetails(const pof::base::data::row_t& product);
		void LoadLabelDetails(LabelInfo& info, const pof::base::data::row_t& product);
		void OnProductUpdate(pof::base::data::const_iterator iter);
		void SetActiveSaleIdText(const boost::uuids::uuid& saleId);
		void SaveLabelInfo(const pof::base::data::duuid_t& suid);
		void ReloadLabelInfo(const pof::base::data::duuid_t& suid);
	private:
		pof::base::data::row_t mDropRow; //dummy row required by pof::DataObject
		std::unordered_map<std::add_pointer_t<wxPGProperty>, std::function<void(const wxVariant& value)>> mProperties;

		//used to select label data

		DECLARE_EVENT_TABLE();
	};
};
