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

#include "ProductManager.h"

namespace pof {

	class Warning : public wxDialog
	{
	private:

	protected:
		wxPanel* m_panel5;
		wxStaticText* mWarning;
		wxTextCtrl* mWarningText;
		wxStaticText* mWarningLevelText;
		wxChoice* m_choice2;
		wxButton* m_button1;
		wxPanel* m_panel6;
		wxListCtrl* mWarningView;
		pof::base::data::duuid_t mPuid;
		wxListItem mItem;

	public:
		enum {
			ID_LEVEL = wxID_HIGHEST + 10,
			ID_WARN_VIEW,
			ID_CHANGE_LEVEL,
		};

		Warning(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Warnings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(780, 459), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
		virtual ~Warning();

		void OnAdd(wxCommandEvent& evt);
		void OnContextMenu(wxListEvent& evt);
		void OnRemove(wxCommandEvent& evt);
		void OnChangeLevel(wxCommandEvent& evt);
		void OnItemSelected(wxListEvent& evt);


		void LoadWarnings(pof::base::data::duuid_t& pid);
		DECLARE_EVENT_TABLE()
	};
};
