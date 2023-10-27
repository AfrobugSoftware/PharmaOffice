#pragma once
#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>


#include "SaleManager.h"

namespace pof {

	class SaveSaleDialog : public wxDialog
	{
	private:

	protected:
		wxPanel* m_panel5;
		wxStaticText* mWarning;
		wxStaticText* mWarningLevelText;
		wxChoice* m_choice2;
		wxButton* m_button1;
		wxPanel* m_panel6;
		wxListCtrl* mSaveSaleView;
		pof::base::data::duuid_t mSuid;
		wxListItem mItem;
		wxAuiToolBar* mToolBar;

	public:
		enum {
			ID_SAVE_SALE_VIEW,
			ID_REMOVE_SAVE_SALE,
		};

		SaveSaleDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Save Sale"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(780, 459), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
		~SaveSaleDialog();
		bool LoadSaveSales(const pof::base::relation<pof::base::data::datetime_t, boost::uuids::uuid, pof::base::currency>& relation);
		void OnContextMenu(wxListEvent& evt);
		void OnRemove(wxCommandEvent& evt);
		void OnItemSelected(wxListEvent& evt);
		void OnItemActivated(wxListEvent& evt);
		inline const boost::uuids::uuid& GetSaveSaleId() const { return mSuid; }

		DECLARE_EVENT_TABLE()
	};
};