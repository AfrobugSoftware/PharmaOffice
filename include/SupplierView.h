#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/button.h>
#include <wx/srchctrl.h>
#include <wx/aui/auibar.h>
#include <wx/aui/aui.h>
#include <wx/simplebook.h>

#include "ProductManager.h"
#include <ranges>
#include <unordered_map>
#include <numeric>
#include <boost/signals2.hpp>

namespace pof
{
	class SupplierView : public wxPanel
	{
	public:
		enum {
			SUPPLIER_VIEW,
			INVOICE_VIEW,
			INVOICE_PRODUCT_VIEW,
			EMPTY,
		};

		//ids
		enum {
			ID_BACK = wxID_HIGHEST + 1,
			ID_ADD_SUPPLIER,
			ID_REMV_SUPPLIER,
			ID_TOOLS,
			ID_ADD_INVOICE,
			ID_REMV_INVOICE,
			ID_SEARCH,
			ID_ADD_PRODUCT,
			ID_SUPPLIER_VIEW,
			ID_INVOICE_VIEW,
			ID_INVOICE_PRODUCT_VIEW,
			ID_REMOVE_INVOICE,
			ID_COPY_INVOICE_NAME,
		};
		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		SupplierView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(917, 668), long style = wxTAB_TRAVERSAL | wxNO_BORDER);
		virtual ~SupplierView();

		void CreateToolbar();
		void CreateViews();

		void LoadSuppliers();
		void LoadInvoices(std::uint64_t supid);
		void LoadInvoiceProducts(std::uint64_t sid, const std::string& in);

		wxPanel* CreateEmptyPanel(const std::string& text = {});
		void CheckEmpty(int viewIdx); //switch on book index
		
		void ShowEmpty(const std::string& text);
		void SwitchTool(int viewIdx);
		void UpdateTotals();
		void OnAuiThemeChange();
		void ShowTitle(const std::string& text);
		void OnChangeFont(const wxFont& font);
	protected:
		void OnSupplierActivated(wxDataViewEvent& evt);
		void OnInvoiceActivated(wxDataViewEvent& evt);

		void OnBack(wxCommandEvent& evt);
		void OnCreateSupplier(wxCommandEvent& evt);
		void OnRemoveSupplier(wxCommandEvent& evt);
		void OnRemoveInvoice(wxCommandEvent& evt);
		void OnContextMenu(wxDataViewEvent& evt);
		void OnInvoiceContextMenu(wxDataViewEvent& evt);
		void OnSearch(wxCommandEvent& evt);
		void OnSearchCleared(wxCommandEvent& evt);
		void OnCopyInvoice(wxCommandEvent& evt);
	private:
		wxAuiManager mManager;

		wxStaticText* mSupplierName = nullptr;
		wxSimplebook* mBook = nullptr;
		wxAuiToolBar* mTools = nullptr;
		wxAuiToolBar* mInvoiceTools = nullptr;
		wxAuiToolBar* mInvoiceProductTools = nullptr;
		wxSearchCtrl* mSupplierSearch = nullptr;
		wxAuiToolBarItem* mSupplierNameItem = nullptr;
		wxDataViewCtrl* mView = nullptr;
		wxDataViewCtrl* mInvoiceView = nullptr;
		wxDataViewCtrl* mInvoiceProductView = nullptr;

		wxPanel* mEmpty = nullptr;
		wxStaticText* mEmptyStr = nullptr;

		wxPanel* mCSPanel = nullptr;
		wxStaticText* mTotalStock = nullptr;
		wxStaticText* mTotalAmount = nullptr;

		int page = -1;
		std::uint64_t mCurSupplier;
		pof::DataModel* mInvoiceProductModel = nullptr; //the data for the products

		DECLARE_EVENT_TABLE();
	};
};