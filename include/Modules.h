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



#include "ArtProvider.h"
namespace pof {
	class Modules : public wxPanel
	{
	public:
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


		using signal_t = boost::signals2::signal<void(const wxTreeItemId&, Evt)>;
		Modules(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(248, 680), long style = wxNO_BORDER | wxTAB_TRAVERSAL);
		virtual ~Modules();

		void CreateTree();
		void Style();

		boost::signals2::connection SetSlot(signal_t::slot_type&& slot);
	protected:
		void OnActivated(wxTreeEvent& evt);
		void OnSelected(wxTreeEvent& evt);


		void SetupFont();

	private:
		std::array<wxFont, 2> mFonts;

		wxTreeItemId mPharmacy;
		wxTreeItemId mTransactions;
		wxTreeItemId mApplication;

		wxTreeItemId mPrescriptions;
		wxTreeItemId mPaitents;
		wxTreeItemId mPoisionBook;
		wxTreeItemId mProducts;
		wxTreeItemId mSales;
		wxTreeItemId mOrders;
		wxTreeItemId mReports;




		wxPanel* m_panel1;
		wxStaticBitmap* m_bitmap1;
		wxPanel* m_panel2;
		wxTreeCtrl* mModuleTree;
		signal_t mSig;


		DECLARE_EVENT_TABLE()
	};
}
