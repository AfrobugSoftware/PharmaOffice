#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/dialog.h>


namespace pof {
	class ResetPassword : public wxDialog
	{
	private:

	protected:
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl2;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl3;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;

	public:

		ResetPassword(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(621, 270), long style = wxTAB_TRAVERSAL);
		~ResetPassword();

		void OnOk(wxCommandEvent& evt);

		DECLARE_EVENT_TABLE();
	};
}


