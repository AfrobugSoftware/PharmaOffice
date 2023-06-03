#pragma once

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>


#include <Data.h>
#include "Account.h"
#include <../base/bcrypt/include/bcrypt.h> //to avoid clashing
#include <regex>

namespace pof {
	class RegistrationDialog : public wxDialog
	{
	private:

	protected:
		wxPanel* MainPane;
		wxPanel* CreateAccount;
		wxScrolledWindow* m_scrolledWindow1;
		wxPanel* m_panel5;
		wxStaticText* m_staticText12;
		wxStaticText* mLastNameLabel;
		wxTextCtrl* mLastNameValue;
		wxStaticText* mFirstNameLabel;
		wxTextCtrl* mFirstNameValue;
		wxStaticText* mUserNameLabel;
		wxTextCtrl* mUserNameValue;
		wxStaticText* mAccountTypeLabel;
		wxChoice* mAccountType;
		wxStaticText* mEmailLabel;
		wxTextCtrl* mEmailValue;
		wxStaticText* mPhoneNo;
		wxTextCtrl* mPhoneNoValue;
		wxStaticText* mPasswordLabel;
		wxTextCtrl* mPasswordValue;
		wxStaticText* mConfirmPasswordLabel;
		wxTextCtrl* mConfirmPasswordValue;
		wxCheckBox* m_checkBox1;
		wxStaticText* mRegNumberLabel;
		wxTextCtrl* mRegNumValue;
		wxRadioBox* m_radioBox2;
		wxStdDialogButtonSizer* m_sdbSizer3;
		wxButton* m_sdbSizer3Save;
		wxButton* m_sdbSizer3Cancel;

		Account mAccount;

	public:
		//window ids
		enum {
			ID_SHOW_PASSWORD = wxID_HIGHEST + 4500,
			ID_ACCOUNT_TYPE,

		};


		//account type
		enum {
			PHARAMCIST = 0,
			PHARM_TECH,
			DISPENSER,
			SALES_ASSISTANT,
			INTERN_PHARAMCIST,
			STUDENT_PHARMACIST,
			MANAGER,
		};

		//PHARAMCY ROLE
		enum {
			PHARMACIST,
			LOCCUM
		};

		RegistrationDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Registration"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(848, 584), long style = wxDEFAULT_DIALOG_STYLE);
		~RegistrationDialog();

		inline const Account& GetAccount() const { return mAccount; }
		virtual bool TransferDataFromWindow() override;
		bool ValidateEmail(const std::string email);

		void OnShowPassword(wxCommandEvent& evt);
		void OnAccountTypeSelect(wxCommandEvent& evt);

	private:
		DECLARE_EVENT_TABLE()

	};
}
