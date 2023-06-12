///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

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

#include "AuiTheme.h"
#include "ProductInfo.h"
#include "DataObject.h"
#include "AddProductDialog.h"

#include <boost/signals2/signal.hpp>

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
			ID_PRODUCTINFO,
			ID_BACK,
			ID_FORWARD,
			ID_DATA_VIEW,
			ID_SEARCH,
			ID_SEARCH_BY_CATEGORY,
			ID_SEARCH_BY_NAME,
			ID_OUT_OF_STOCK,
			ID_OPTIMISE_BY_FORMULARY,
			ID_ADD_CATEGORY,
			ID_REMOVE_GROUP_BY,
			ID_QUICK_SORT_TEST,
			ID_INVENTORY_VIEW,
			ID_INVENTORY_VIEW_TOOL_ADD,
			ID_INVENTORY_PRODUCT_NAME,
			ID_CATEGORY_LIST_CONTROL,
			ID_PRODUCT_CONTEXT_QUICKEDIT,
			ID_PRODUCT_CONTEXT_REMOVE,
			ID_PRODUCT_EXPIRE,
			ID_EXPIRY_VIEW,
			ID_SELECT_MULTIPLE,
			ID_UNSELECT_MULTIPLE,
			ID_SHOW_COST_PRICE,
			ID_TO_JSON,
			ID_FILE
		};

		wxAuiToolBar* m_auiToolBar1 = nullptr;
		wxSearchCtrl* m_searchCtrl1 = nullptr;
		wxDataViewCtrl* m_dataViewCtrl1 = nullptr;
		wxDataViewColumn* mSerialNumCol = nullptr;
		wxDataViewColumn* mProductNameCol = nullptr;
		wxDataViewColumn* mProductClass = nullptr;
		wxDataViewColumn* mProductUnitPriceCol = nullptr;
		wxDataViewColumn* mProductCostPriceCol = nullptr;
		wxDataViewColumn* mStockLevel = nullptr;
		wxDataViewColumn* mSelectionCol = nullptr;

		pof::ProductInfo* mProductinfo = nullptr;



		//attibutes
		std::shared_ptr<wxDataViewItemAttr> mUpdatedAttr;
		std::set<wxDataViewItem> mSelections;

	public:
		boost::signals2::signal<void(const std::string&)> CategoryAddSignal;
		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		ProductView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(727, 557), long style = wxNO_BORDER | wxTAB_TRAVERSAL); wxAuiManager m_mgr;

		~ProductView();
		void ReSizeColumns();
		void SaveColumnWidths();
		void SetupAuiTheme();


		//events
		void OnResize(wxSizeEvent& evt);
		void OnAuiThemeChange();

		void OnProductActivated(wxDataViewEvent& evt);
		void OnBeginDrag(wxDataViewEvent& evt);
		void OnExpiredProducts(wxCommandEvent& evt);
		void OnAddProduct(wxCommandEvent& evt);
		void OnAddCategory(wxCommandEvent& evt);
		void OnSearchFlag(wxCommandEvent& evt);
		void OnContextMenu(wxDataViewEvent& evt);
		void OnRemoveProduct(wxCommandEvent& evt);
		void OnAddProductToOrderList(wxCommandEvent& evt);
		void OnSearchProduct(wxCommandEvent& evt);
		void OnSearchCleared(wxCommandEvent& evt);
		void OnSelection(wxCommandEvent& evt);
		void OnShowCostPrice(wxCommandEvent& evt);



		void OnProductInfoUpdated(const pof::ProductInfo::PropertyUpdate&);
		void OnCategorySelected(const std::string& name);
		void ShowCostPriceColumn();
		void HideCostPriceColumn();
		void ShowSelectionColumn();
		void HideSelectionColumn();

		void OnCategoryActivated(const std::string& name);
	protected:
		void CreateDataView();
		void CreateToolBar();
		void CreateProductInfo();
		void CreateAttibutes();
		void CreateSpecialCols();
		void Style();
		void SwapCenterPane(bool IsInventoryView);

	private:
		DECLARE_EVENT_TABLE()

	};

}; // namespace pof

