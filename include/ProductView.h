#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/cshelp.h>
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/infoBar.h>
#include <wx/itemattr.h>

#include "PackView.h"
#include "AuiTheme.h"
#include "ProductInfo.h"
#include "DataObject.h"
#include "AddProductDialog.h"
#include "InventoryDialog.h"
#include "OrderView.h"
#include "Reports.h"
#include "StockCheck.h"

#include <boost/signals2/signal.hpp>
#include <OpenXLSX.hpp>

namespace excel = OpenXLSX;
namespace pof
{
	class ProductView : public wxPanel
	{
	private:

	protected:
		enum {
			SELECTION_MODEL_COL = pof::ProductManager::PRODUCT_MAX + 2
		};
		enum
		{
			ID_TOOLBAR = 1000,
			ID_ADD_PRODUCT = wxID_HIGHEST + 2000,
			ID_REMOVE_PRODUCT,
			ID_ADD_ORDER_LIST,
			ID_ORDER_LIST,
			ID_REMOVE_FROM_CATEGORY,
			ID_PRODUCTINFO,
			ID_BACK,
			ID_FORWARD,
			ID_DATA_VIEW,
			ID_FUNCTIONS,
			ID_FUNCTION_BROUGHT_FORWARD,
			ID_FUNCTION_STOCK_CHECK,
			ID_FUNCTION_MARK_UP_PRODUCTS,
			ID_REPORTS,
			ID_REPORTS_CONSUMPTION_PATTERN,
			ID_REPORTS_ENDOFDAY,
			ID_SEARCH,
			ID_SEARCH_BY_CATEGORY,
			ID_SEARCH_BY_NAME,
			ID_OUT_OF_STOCK,
			ID_OPTIMISE_BY_FORMULARY,
			ID_ADD_CATEGORY,
			ID_ADD_INVENTORY,
			ID_REMOVE_GROUP_BY,
			ID_QUICK_SORT_TEST,
			ID_INVENTORY_VIEW,
			ID_INVENTORY_VIEW_TOOL_ADD,
			ID_INVENTORY_PRODUCT_NAME,
			ID_CATEGORY_LIST_CONTROL,
			ID_PRODUCT_CONTEXT_QUICKEDIT,
			ID_PRODUCT_CONTEXT_REMOVE,
			ID_PRODUCT_EXPIRE,
			ID_PRODUCT_MARKUP,
			ID_EXPIRY_VIEW,
			ID_SELECT_MULTIPLE,
			ID_UNSELECT_MULTIPLE,
			ID_SHOW_COST_PRICE,
			ID_TO_JSON,
			ID_FILE,
			ID_PACKS,
			ID_MOVE_PRODUCT_STOCK,
			ID_DOWNLOAD_EXCEL,
			ID_STOCK_CHECK_TIMER,
			ID_CREATE_CONTROLLED_BOOK,
			ID_ADD_VARIANT,
			ID_REPORTS_EOM,
			ID_STORE_SUMMARY,
			ID_INCR_PRICE,
			ID_INCR_PRODUCT_PRICE,
			ID_OPEN_PRODUCT_INFO,
		};

		size_t mRowHeights;
		wxItemAttr mHeaderAttr;
		wxFont mDataViewFont;


		wxAuiToolBar* m_auiToolBar1 = nullptr;
		wxSearchCtrl* m_searchCtrl1 = nullptr;
		wxAuiToolBarItem* mOutOfStockItem = nullptr;
		wxAuiToolBarItem* mExpireProductItem = nullptr;
		wxAuiToolBarItem* mReportItem = nullptr;
		wxAuiToolBarItem* mFuncDropItem = nullptr;
		wxDataViewCtrl* m_dataViewCtrl1 = nullptr;
		wxDataViewColumn* mSerialNumCol = nullptr;
		wxDataViewColumn* mProductNameCol = nullptr;
		wxDataViewColumn* mProductClass = nullptr;
		wxDataViewColumn* mProductUnitPriceCol = nullptr;
		wxDataViewColumn* mProductCostPriceCol = nullptr;
		wxDataViewColumn* mProductFormulation = nullptr;
		wxDataViewColumn* mStockLevel = nullptr;
		wxDataViewColumn* mSelectionCol = nullptr;
		wxPanel* mEmpty = nullptr;
		wxPanel* mNoResult = nullptr;
		wxStaticText* mNoResultText = nullptr;
		wxTimer mStockCheckTimer;
		pof::ProductInfo* mProductinfo = nullptr;
		wxInfoBar* mInfoBar = nullptr;
		std::string mActiveCategory;

		//attibutes
		std::shared_ptr<wxDataViewItemAttr> mUpdatedAttr;
		std::set<wxDataViewItem> mSelections;

		std::chrono::system_clock::time_point mExpireProductWatchDog;
		std::chrono::system_clock::time_point mOutOfStockProductWatchDog;
		std::chrono::system_clock::duration mWatchDogDuration = std::chrono::minutes(30);
	public:
		boost::signals2::signal<void(const std::string&)> CategoryAddSignal;
		boost::signals2::signal<void(pof::base::data::const_iterator)> removeSignal;
		boost::signals2::signal<void(const pof::base::data::row_t&)> addPoisonBook;

		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		ProductView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(727, 557), long style = wxNO_BORDER | wxTAB_TRAVERSAL); wxAuiManager m_mgr;

		virtual ~ProductView();
		void ReSizeColumns();
		void SaveColumnWidths();
		void SetupAuiTheme();
		void SetupDataViewStyle();

		bool ProductAdd(pof::base::data::row_t&& productRow);

		inline void SetHeaderAttr(const wxItemAttr& attr) { mHeaderAttr = attr; }
		inline void SetDataViewRowHeight(size_t rowHeight) { mRowHeights = rowHeight; }
		inline void SetDataViewFont(const wxFont& font) { mDataViewFont = font; }

		void ReloadProductView();
		void CreateAccTable();
		void CreateEmptyPanel();
		//events
		void OnResize(wxSizeEvent& evt);
		void OnAuiThemeChange();

		void OnProductActivated(wxDataViewEvent& evt);
		void OnBeginDrag(wxDataViewEvent& evt);
		void OnHeaderClicked(wxDataViewEvent& evt);
		void OnExpiredProducts(wxCommandEvent& evt);
		void OnAddProduct(wxCommandEvent& evt);
		void OnAddCategory(wxCommandEvent& evt);
		void OnRemoveFromCategory(wxCommandEvent& evt);
		void OnSearchFlag(wxCommandEvent& evt);
		void OnContextMenu(wxDataViewEvent& evt);
		void OnRemoveProduct(wxCommandEvent& evt);
		void OnAddProductToOrderList(wxCommandEvent& evt);
		void OnAddItemsToCategory(wxCommandEvent& evt);
		void OnSearchProduct(wxCommandEvent& evt);
		void OnSearchCleared(wxCommandEvent& evt);
		void OnSelection(wxCommandEvent& evt);
		void OnShowCostPrice(wxCommandEvent& evt);
		void OnOutOfStock(wxCommandEvent& evt);
		void OnAddInventory(wxCommandEvent& evt);
		void OnReportDropdown(wxAuiToolBarEvent& evt);
		void OnConsumptionPattern(wxCommandEvent& evt);
		void OnEndOfDayReport(wxCommandEvent& evt);
		void OnPacks(wxCommandEvent& evt);
		void OnFunctions(wxAuiToolBarEvent& evt);
		void OnBFFunction(wxCommandEvent& evt);
		void OnSCFunction(wxCommandEvent& evt);
		void OnShowOrderList(wxCommandEvent& evt);
		void OnMarkUp(wxCommandEvent& evt);
		void OnMarkUpProducts(wxCommandEvent& evt);
		void OnMoveExpiredStock(wxCommandEvent& evt);
		void OnUpdateUI(wxUpdateUIEvent& evt);
		void OnDownloadExcel(wxCommandEvent& evt);
		void OnCacheHint(wxDataViewEvent& evt);
		void OnStockCheckTimer(wxTimerEvent& evt);
		void OnCreateControlBook(wxCommandEvent& evt);
		void OnAddVariant(wxCommandEvent& evt);
		void OnEndOfMonth(wxCommandEvent& evt);
		void OnStoreSummary(wxCommandEvent& evt);
		void OnIncrPrice(wxCommandEvent& evt);
		void OnOpenProductInfo(wxCommandEvent& evt);


		inline bool IsActiveCategory() const { return !mActiveCategory.empty(); }


		void OnProductInfoUpdated(const pof::ProductInfo::PropertyUpdate&);
		void OnProductStockRemoved(const pof::base::data::duuid_t& uuid, std::uint64_t newstock);
		void OnCategorySelected(const std::string& name);
		void ShowCostPriceColumn();
		void HideCostPriceColumn();
		void ShowSelectionColumn();
		void HideSelectionColumn();

		void OnCategoryActivated(const std::string& name);
		void OnCategoryRemoved(const std::string& name);
		void OnCategoryEdited(const std::string& oldName, const std::string& newName);
	protected:
		void CreateDataView();
		void CreateToolBar();
		void CreateProductInfo();
		void CreateAttibutes();
		void CreateSpecialCols();
		void CreateNoResultPane();
		void Style();
		void SwapCenterPane(bool IsInventoryView);
		void CreateCategoryMenu(wxMenu* menu);
		void DoBroughtForward();
		void RemoveCheckedState(wxAuiToolBarItem* item);
		void CheckIfStockCheckIsComplete();
		void CheckEmpty();
		void ShowNoResult(const std::string& search);
	private:
		DECLARE_EVENT_TABLE()

	};

}; // namespace pof

