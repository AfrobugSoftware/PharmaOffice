
#pragma once
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/hyperlink.h>


#include <nlohmann/json.hpp>
#include <exception>

namespace pof {
	class SignInDialog : public wxDialog
	{
	private:

	protected:
		wxPanel* RootPane;
		wxPanel* m_panel2;
		wxPanel* m_panel3;
		wxStaticBitmap* m_bitmap1;
		wxStaticText* mWelcomText;
		wxTextCtrl* mUserName;
		wxTextCtrl* mPassword;
		wxCheckBox* mKeepMeSigned;
		wxPanel* m_panel5;
		wxButton* mLogOn;
		wxButton* mSignup;
		wxPanel* mPharmacySignupPanel;
		wxHyperlinkCtrl* mLoginPharmacyLink;
		wxHyperlinkCtrl* mHelpLink;
		wxPanel* m_panel4;

		std::string mUserData;

	public:

		enum {
			ID_LOGON,
			ID_SIGNUP
		};


		SignInDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(848, 584), long style = wxDEFAULT_DIALOG_STYLE);
		~SignInDialog();
	
		inline constexpr const std::string& GetUserData() const { return mUserData; }
	protected:
		void onLogon(wxCommandEvent& evt);
		void onSignup(wxCommandEvent& evt);

		bool ValidateLocal();
		bool ValidateGlobal(); 

		DECLARE_EVENT_TABLE()

	};
};


