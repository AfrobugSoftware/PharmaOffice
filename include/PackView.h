#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/gdicmn.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/dataview.h>
#include <wx/stattext.h>
#include <wx/simplebook.h>
#include <wx/dialog.h>

#include "ProductManager.h"
#include "SearchProduct.h"

#include <ranges>
#include <algorithm>
#include <numeric>

namespace pof
{
	class PackView : public wxDialog
	{
	private:

	protected:
		wxAuiToolBar* mTopTools;
		wxSimplebook* mBook;
		wxPanel* mPackView;
		wxPanel* mEmptyPack;
		wxPanel* mEmpty;
		wxListCtrl* mPackSelect;
		wxPanel* mPackData;
		wxDataViewCtrl* m_dataViewCtrl3;
		wxDataViewColumn* mProductName;
		wxDataViewColumn* mProductQuantity;
		wxDataViewColumn* mPackageSize;
		wxDataViewColumn* mPrice;
		wxDataViewColumn* mExtPrice;
		wxPanel* m_panel4;
		wxStaticText* m_staticText1;
		wxStaticText* mTotalQuantity;
		wxStaticText* mTotalAmountText;
		wxStaticText* mTotalAmount;
		wxStaticText* mPackText = nullptr;
		wxAuiToolBarItem* mTextItem = nullptr;
	public:
		enum {
			PACK_EMPTY = 1,
			PACK_VIEW,
			PACK_DATA,
			PACK_PRODUCT_EMPTY,
		};

		PackView(wxWindow* parent, bool showSale = false, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(957, 542), long style = wxDEFAULT_DIALOG_STYLE);
		virtual ~PackView();

		enum {
			ID_PACK_SELECT = wxID_HIGHEST + 10,
			ID_BOOK,
			ID_TOOL,
			ID_PACK_DATA,
			ID_TOOL_CREATE_PACK,
			ID_TOOL_ADD_PACK,
			ID_TOOL_REMOVE_PACK,
			ID_TOOL_ADD_PRODUCT_PACK,
			ID_TOOL_REMOVE_PRODUCT_PACK,
			ID_SALE_PACK,
			ID_RENAME_PACK,
			ID_OPEN_PACK,

		};

		virtual bool TransferDataFromWindow() override; 

		std::vector<pof::ProductManager::packType> GetPackProducts() const;
		inline bool& sale() { return mShowSale; }

	protected:
		void CreateEmptyPackPanel();
		void CreateEmptyPanel(); //
		void CreatePackTools();
		void CreateTopTools(); //naming is hard
		void CreateSpeicalCols();
		void ShowPack();

		void OnPackActivate(wxListEvent& evt);
		void OnEditPackName(wxListEvent& evt);
		void OnPackSelected(wxListEvent& evt);
		void OnAddPack(wxCommandEvent& evt);
		void OnCreatePack(wxCommandEvent& evt);
		void OnRemovePack(wxCommandEvent& evt);
		void OnAddProductPack(wxCommandEvent& evt);
		void OnRemoveProductPack(wxCommandEvent& evt);
		void OnBack(wxCommandEvent& evt);
		void OnSalePack(wxCommandEvent& evt);
		void OnColEdited(wxDataViewEvent& evt);
		void OnRightClick(wxListEvent& evt);
		void OnRenamePack(wxCommandEvent& evt);
		void OnOpenPack(wxCommandEvent& evt);

		bool mShowSale = false;
		void UpdateTotals();
		void LoadPackDescSelect();
		void LoadPackModel();
		pof::DataModel* mPackModel = nullptr;
		boost::uuids::uuid* mCurPackId = nullptr;
		std::string mPackName;
		wxListItem mSelItem;
		DECLARE_EVENT_TABLE()
	};

};
