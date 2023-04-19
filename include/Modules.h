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

#include "ArtProvider.h"

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
			SEL_CHANGED
		};

		enum {
			FONT_MAIN,
			FONT_CHILD
		};


		using signal_t = boost::signals2::signal<void(const_iterator, Evt)>;
		Modules(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(248, 680), long style = wxNO_BORDER | wxTAB_TRAVERSAL);
		virtual ~Modules();

		void CreateTree();
		void Style();


		std::string GetText(const_iterator item) const;
		int GetImage(const_iterator item) const;

		bool CheckPrivilage();
		inline void SetImageList(wxImageList* imglist) { mModuleTree->SetImageList(imglist); }
		boost::signals2::connection SetSlot(signal_t::slot_type&& slot);
	protected:
		void OnActivated(wxTreeEvent& evt);
		void OnSelected(wxTreeEvent& evt);


		void SetupFont();

	private:
		friend class MainFrame;
		std::array<wxFont, 2> mFonts;

		//who has the windows ? the module or the workspace 
		//std::array<wxWindow*, 6> mModuleWindows;
		wxTreeItemId mPharmacy;
		wxTreeItemId mTransactions;
		wxTreeItemId mWarehouse;

		wxTreeItemId mPrescriptions;
		wxTreeItemId mPaitents;
		wxTreeItemId mPoisionBook;
		wxTreeItemId mProducts;
		wxTreeItemId mSales;
		wxTreeItemId mOrders;
		wxTreeItemId mRequisitions;
		wxTreeItemId mReports;
		wxTreeItemId mSupply;



		wxPanel* m_panel1;
		wxStaticBitmap* m_bitmap1;
		wxPanel* m_panel2;
		wxTreeCtrl* mModuleTree;
		signal_t mSig;

		std::unordered_map<wxTreeItemId, wxWindow*> mModuleViews;

		DECLARE_EVENT_TABLE()
	};
}
