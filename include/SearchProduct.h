#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/frame.h>

#include <wx/dialog.h>


#include "DataModel.h"
#include <set>

namespace pof {
	class SearchProduct : public wxDialog
	{
	protected:
		wxAuiToolBar* SearchProductBar;
		wxSearchCtrl* SearchCtrl;
		wxAuiToolBarItem* m_tool1;
		wxPanel* m_panel1;
		wxDataViewCtrl* SearchData;
		wxDataViewColumn* ProductIdCol;
		wxDataViewColumn* ProductName;
		wxDataViewColumn* PriceCol;


		//id, name, price
		pof::DataModel* mModel;
		size_t mSelectedProduct;

		std::set<size_t> mSelectedProducts;
	public:
		enum {
			ID_SEARCH_BUTTON = wxID_HIGHEST + 3000,
			ID_SEARCH_CTRL,
			ID_SEARCH_VIEW,
			ID_ADD_PRODUCT,
		};


		SearchProduct(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Search Product"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(713, 523), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
		wxAuiManager m_mgr;

		~SearchProduct();

		void CreateToolBar();
		void CreateSearchView();
		void CreateViewManagerDefaultArt();

		void SetUpView();



		inline bool HasMultipleSelections() const { return !mSelectedProducts.empty(); }

		const pof::base::data::row_t& GetSelectedProduct() const;
		const std::vector<std::reference_wrapper<pof::base::data::row_t>> GetSelectedProducts() const;
	protected:


		void OnItemSelected(wxDataViewEvent& evt);
		void OnActivated(wxDataViewEvent& evt);
		void OnSearch(wxCommandEvent& evt);
		void OnSearchCancelled(wxCommandEvent& evt);
		void OnClose(wxCloseEvent& evt);
		void OnAddProduct(wxCommandEvent& evt);

		void DoSearch(const std::string& search_for);

		DECLARE_EVENT_TABLE()
	};

};

