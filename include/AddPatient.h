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
#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/datectrl.h>


#include "PatientManager.h"

namespace pof
{
	class AddPatient : public wxDialog 
	{
		private:
		
		protected:
			wxPanel* m_panel1;
			wxStaticText* TitleText;
			wxStaticText* m_staticText18;
			wxPanel* m_panel2;
			wxPanel* m_panel4;
			wxPanel* m_scrolledWindow2;
			wxStaticText* mPatientName;
			wxTextCtrl* mPatientNameValue;
			wxStaticText* mPatientLastName;
			wxTextCtrl* mPatientLastNameValue;
			wxStaticText* mDateOfBirth;
			wxDatePickerCtrl* mDOBValue;
			wxStaticText* mGenderLabel;
			wxChoice* mGenderValue;
			wxStaticText* mPhonenumber;
			wxTextCtrl* mPhonenumberValue;
			wxStaticText* mAddress;
			wxTextCtrl* mAddressValue;
			wxPanel* m_panel7;
			wxStdDialogButtonSizer* m_sdbSizer2;
			wxButton* m_sdbSizer2OK;
			wxButton* m_sdbSizer2Cancel;
		
			pof::base::data::row_t mPatientEntry;
			wxArrayString genChoies;
		public:
			
			AddPatient( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 891,423 ), long style = wxDEFAULT_DIALOG_STYLE ); 
			~AddPatient();
			virtual bool TransferDataFromWindow() override;
			inline const pof::base::data::row_t& GetPatientData() const { return mPatientEntry; }
	};
	
} // namespace pof
