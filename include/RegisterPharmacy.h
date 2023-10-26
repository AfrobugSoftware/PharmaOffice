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
#include <wx/valtext.h>
#include <wx/statbox.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>
#include "Pharmacy.h"

namespace pof {
	class PharmacyRegistration : public wxDialog
	{
	private:

	protected:
		wxPanel* m_panel1;
		wxStaticText* mTitle;
		wxStaticText* mDescription;
		wxScrolledWindow* m_scrolledWindow1;
		wxPanel* m_panel3;
		wxStaticText* mPharmacyType;
		wxChoice* mPharamcyTypeValue;
		wxStaticText* mPharamcyName;
		wxTextCtrl* mPharmacyNameValue;
		wxStaticText* mPhoneNo;
		wxTextCtrl* mPhoneNoValue;
		wxStaticText* mEmail;
		wxTextCtrl* mEmailValue;
		wxStaticText* mWebsiteText;
		wxTextCtrl* mWebsiteValue;
		wxPanel* m_panel4;
		wxStaticText* mCountyText;
		wxTextCtrl* mCountryValue;
		wxStaticText* mLgaText;
		wxTextCtrl* mLgaValue;
		wxStaticText* mNoText;
		wxTextCtrl* mNoValue;
		wxStaticText* mStreetText;
		wxTextCtrl* mStreetValue;
		wxStaticText* mCityText;
		wxTextCtrl* mCityValue;
		wxStaticText* mStateText;
		wxTextCtrl* mStateValue;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;

	public:
		std::shared_ptr<pof::Pharmacy> mp;
		PharmacyRegistration(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(939, 657), long style = wxTAB_TRAVERSAL);
		~PharmacyRegistration();
		bool ValidateEmail(const std::string email);
		virtual bool TransferDataFromWindow() override;
	};
};


