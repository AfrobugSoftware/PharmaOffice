#pragma once

#include <wx/artprov.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/simplebook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/splitter.h>
#include <wx/datectrl.h>
#include <Data.h>

#include <bitset>
#include <unordered_map>
#include <optional>
#include <random>
#include <date/date.h>


#include <boost/signals2/signal.hpp>
#include "ProductManager.h"
#include "InventoryDialog.h"
#include "WarningViewDialog.h"

#include <wx/bars/barplot.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/categoryaxis.h>
#include <wx/xy/xyhistorenderer.h>
#include <wx/category/categorysimpledataset.h>


namespace pof
{
	//aslo use for adding a product to the database
	class ProductInfo : public wxPanel 
	{
		private:
		
		protected:
			wxSplitterWindow* m_splitter1;
			wxPanel* m_panel1;
			wxAuiToolBar* m_auiToolBar1;
			wxAuiToolBarItem* m_tool1;
			wxAuiToolBarItem* mProductHist;
			wxAuiToolBarItem* mProductNameText;
			wxAuiToolBarItem* mShowAddInfo;
			wxDataViewCtrl* InventoryView;
			wxSimplebook* mBook;
			wxDataViewCtrl* mHistView;
			wxDataViewColumn* mInputDate;
			wxDataViewColumn* mBactchNo;
			wxDataViewColumn* mExpiryDate;
			wxDataViewColumn* mStockCount;
			wxDataViewColumn* mManuFactureName;
			wxPanel* m_panel2;
			wxPropertyGridManager* m_propertyGridManager1;
			wxPropertyGridPage* m_propertyGridPage1;
			wxPGProperty* m_propertyGridItem1;
			wxPGProperty* mNameItem;
			wxPGProperty* mGenericNameItem;
			wxPGProperty* mPackageSizeItem;
			wxPGProperty* mProductClass;
			wxPGProperty* mFormulationItem;
			wxPGProperty* mStrengthValueItem;
			wxPGProperty* mStrengthTypeItem;
			wxPGProperty* mMoreProductInfo;
			wxPGProperty* mDirForUse;
			wxPGProperty* mHealthCond;
			wxPGProperty* mProductDescription;
			wxPGProperty* mSideEffects;
			wxPGProperty* mSettings;
			wxPGProperty* mMinStockCount;
			wxPGProperty* mExpDateCount;
			wxPGProperty* mExpDatePeriod;
			wxPGProperty* mSaleSettings;
			wxPGProperty* mUnitPrice;
			wxPGProperty* mCostPrice;
			wxPGProperty* mBarcode;
			wxDatePickerCtrl* mInventoryDate;
			double mStubPrice;
			wxPGChoices ProductClassChoices;
			wxPGChoices FormulationChoices;
			wxPGChoices ExpChoices;
			wxPGChoices StrengthChoices;
			wxPanel* mEmpty = nullptr;
			wxSimplebook* mHistBook = nullptr;
			wxChartPanel* mChartPanel = nullptr;
			DECLARE_EVENT_TABLE();

		public:
			struct PropertyUpdate {
				constexpr const static size_t elems = pof::ProductManager::PRODUCT_MAX;
				std::bitset<elems> mUpdatedElememts;
				pof::base::data::row_t mUpdatedElementsValues;
			};
			
			//pages
			enum {
				PAGE_INVENTORY = 0,
				PAGE_SALE_HIST,
				PAGE_EMPTY,
			};

			//hist pages
			enum {
				HIST_TABLE = 0,
				HIST_CHART = 1,
			};

			using back_signal_t = boost::signals2::signal<void(void)>;
			using update_signal_t = boost::signals2::signal<void(const PropertyUpdate&)>;
			using stock_remv_signal = boost::signals2::signal<void(const pof::base::data::duuid_t&, std::uint64_t)>;

			enum {
				ID_DATA_VIEW,
				ID_TOOL_GO_BACK = wxID_HIGHEST + 2000,
				ID_TOOL_ADD_INVENTORY,
				ID_TOOL_REMV_EXPIRE_BATCH,
				ID_TOOL_SHOW_PRODUCT_INFO,
				ID_PROPERTY_GRID = 8004,
				ID_SPLIT_WINDOW,
				ID_SHOW_PRODUCT_SALE_HISTORY,
				ID_DATE,
				ID_INVEN_MENU_REMOVE,
				ID_WARNINGS = 9000,
				ID_RESET,
				ID_ADD_BARCODE,
				ID_INVEN_MENU_CREATE_INVOICE,
				ID_INVEN_MENU_CHANGE_SUPPLIER_NAME,
				ID_SAVE_CHART_IMAGE,
				ID_SHOW_HIST_TABLE,
				ID_SHOW_HIST_CHART,
			};

			ProductInfo( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 975,707 ), long style = wxTAB_TRAVERSAL ); 
			~ProductInfo();
			
			void StyleProductPropertyManager();
			void Load(const pof::base::data::row_t& row);
			void LoadProductProperty(const pof::base::data::row_t& row);
			boost::signals2::connection AttachBackSlot(back_signal_t::slot_type&& slot);
			boost::signals2::connection AttachPropertyUpdateSlot(update_signal_t::slot_type&& slot);
			void SignalUpdate(const PropertyUpdate& update);
			void CreateNameToProductElemTable();
			void UpdateDropDowns();
			void LoadChart();

			void m_splitter1OnIdle(wxIdleEvent&);

			stock_remv_signal mStockRemvSig;
			void OnChangeFont(const wxFont& font);
	protected:
		void CreateInventoryView();
		void CreateHistoryView();
		void CreateEmptyPanel();
		void CreatChartPanel();
		Chart* CreateChart();



		void RemoveCheckedState(wxAuiToolBarItem* item);
		void OnGoBack(wxCommandEvent& evt);
		void OnAddInventory(wxCommandEvent& evt);
		void OnPropertyChanged(wxPropertyGridEvent& evt);
		void OnPropertyChanging(wxPropertyGridEvent& evt);
		void StyleSheet();

		void OnSashDoubleClick(wxSplitterEvent& evt);
		void OnUnspilt(wxSplitterEvent& evt);
		void OnShowProductInfo(wxCommandEvent& evt);
		void OnShowProducSaleHistory(wxCommandEvent& evt);
		void OnDateChange(wxDateEvent& evt);
		void OnRemoveInventory(wxCommandEvent& evt);
		void OnInvenContextMenu(wxDataViewEvent& evt);
		void OnWarnings(wxCommandEvent& evt);
		void OnReset(wxCommandEvent& evt);
		void OnAddBarcode(wxCommandEvent& evt);
		void OnCreateInvoice(wxCommandEvent& evt);
		void OnChangeSupplierName(wxCommandEvent& evt);
		void OnSaveChartImage(wxCommandEvent& evt);

		void OnShowHist(wxCommandEvent& evt);

		void RemovePropertyModification();
		std::uint64_t PeriodTime(int periodCount) const;
		pof::base::data::text_t CreatePeriodString();
		void SplitPeriodString(const pof::ProductManager::relation_t::tuple_t& tup);

		void LoadInventoryByDate(const pof::base::data::datetime_t& dt);
		void LoadHistoryByDate(const pof::base::data::datetime_t& dt);
		void UpdateWarnings();
		bool iscontrolled = false;
		back_signal_t mBackSignal;
		update_signal_t mUpdatePropertySignal;
		pof::base::data::row_t mProductData;
		std::optional<PropertyUpdate> mPropertyUpdate;
		std::unordered_map<std::string, size_t> mNameToProductElem;

		wxAuiToolBarItem* mShowHistChart = nullptr;
		wxAuiToolBarItem* mShowHistTable = nullptr;
	};	
}
