#pragma once

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <boost/signals2/signal.hpp>
#include <wx/withimages.h>
#include <array>
#include <unordered_map>
#include <ranges>
#include <algorithm>

#include <spdlog/spdlog.h>
#include "ArtProvider.h"
#include "DataObject.h"

namespace std {
	template<>
	struct hash<wxTreeItemId> {
		size_t operator()(const wxTreeItemId& item) const noexcept {
			size_t i = reinterpret_cast<size_t>(item.GetID());
			return std::hash<size_t>{}(i);
		}
	};
};


namespace pof {
	class Modules : public wxPanel
	{
	public:
		using iterator = std::unordered_map<wxTreeItemId, wxWindow*>::iterator;
		using const_iterator = std::unordered_map<wxTreeItemId, wxWindow*>::const_iterator;
		using value_type = std::unordered_map<wxTreeItemId, wxWindow*>::value_type;
		enum {
			ID_TREE = wxID_HIGHEST + 1000

		};

		enum class Evt {
			ACTIVATED,
			COLLAPASED,
			SEL_CHANGED,
			DRAG_END
		};

		enum {
			FONT_MAIN,
			FONT_CHILD,
			FONT_ACCOUNT
		};


		enum {
			CONTEXT_MENU_EDIT = wxID_HIGHEST + 1,
			CONTEXT_MENU_REMOVE,
			CONTEXT_MENU_ADD_PROPERTIES,

		};

		using signal_t = boost::signals2::signal<void(const_iterator, Evt)>;
		using childtree_signal_t = boost::signals2::signal<void(const std::string&)>;
		using childEditTree_signal_t = boost::signals2::signal<void(const std::string&, const std::string&)>;

		Modules(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(248, 680), long style = wxNO_BORDER | wxTAB_TRAVERSAL);
		virtual ~Modules();

		void CreateTree();
		void Style();

		void activateModule(wxTreeItemId mod);

		std::string GetText(const_iterator item) const;
		int GetImage(const_iterator item) const;
		const_iterator::value_type GetModuleItem(wxTreeItemId item) const;
		void ReloadAccountDetails();

		bool CheckPrivilage();
		inline void SetImageList(wxImageList* imglist) { mModuleTree->SetImageList(imglist); }
		boost::signals2::connection SetSlot(signal_t::slot_type&& slot);
		boost::signals2::connection SetChildTreeSlot(childtree_signal_t::slot_type&& slot);
		boost::signals2::connection SetChildTreeRemoveSlot(childtree_signal_t::slot_type&& slot);
		boost::signals2::connection SetChildTreeEditSlot(childEditTree_signal_t::slot_type&& slot);
	protected:
		void OnActivated(wxTreeEvent& evt);
		void OnSelected(wxTreeEvent& evt);
		void OnBeginDrag(wxTreeEvent& evt);
		void OnEndDrag(wxTreeEvent& evt);
		void OnContextMenu(wxTreeEvent& evt);
		void OnBeginEditLabel(wxTreeEvent& evt);
		void OnEndEditLabel(wxTreeEvent& evt);
		void OnAccountUpdated(const pof::Account& acc);
		void OnPharmacyUpdated(const pof::Pharmacy& pharm);
		//menu handlers
		void OnContextEdit(wxCommandEvent& evt);
		void OnContextRemove(wxCommandEvent& evt);

		void SetupFont();
		void AppendChildTreeId(wxTreeItemId parent, const std::string& name, int img = -1);
		void RemoveChildTreeId(const std::string& name);
	private:
		friend class MainFrame;
		std::array<wxFont, 3> mFonts;


		//who has the windows ? the module or the workspace 
		//std::array<wxWindow*, 6> mModuleWindows;
		wxTreeItemId mPharmacy;
		wxTreeItemId mTransactions;

		wxTreeItemId mPrescriptions;
		wxTreeItemId mPaitents;
		wxTreeItemId mPoisionBook;
		wxTreeItemId mProducts;
		wxTreeItemId mSales;
		wxTreeItemId mOrders;
		wxTreeItemId mRequisitions;
		wxTreeItemId mAuditTrails;



		wxPanel* m_panel1;
		wxStaticBitmap* m_bitmap1;
		wxPanel* m_panel2;
		wxTreeCtrl* mModuleTree;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText4;
		signal_t mSig;
		childtree_signal_t mChildSignal;
		childtree_signal_t mChildRemoveSignal;
		childEditTree_signal_t mChildEditedSignal;

		std::unordered_map<wxTreeItemId, wxWindow*> mModuleViews;
		std::vector<wxTreeItemId> mChildId; //used for childs
		DECLARE_EVENT_TABLE()
	};
}
