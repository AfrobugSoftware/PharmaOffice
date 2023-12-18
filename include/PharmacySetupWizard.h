#pragma once
#include "wx/frame.h"
#include "wx/stattext.h"
#include "wx/log.h"
#include "wx/app.h"
#include "wx/checkbox.h"
#include "wx/checklst.h"
#include "wx/msgdlg.h"
#include "wx/radiobox.h"
#include "wx/menu.h"
#include "wx/sizer.h"

#include "wx/textctrl.h"
#include "wx/wizard.h"
#include <wx/config.h>
#include <wx/artprov.h>


namespace pof
{
	class PharmacySetupWizard : public wxWizard
	{
	public:
		enum {
				ID_FIRST_PAGE = wxID_HIGHEST + 100,
		};

		PharmacySetupWizard(wxFrame* frame);
		virtual ~PharmacySetupWizard() {};

		bool ValidateEmail(const std::string email);
		virtual bool TransferDataFromWindow() override;

		void CreateFirstPage();

		wxWizardPageSimple* GetFirstPage() const { return mFirstPage; }
	private:
		wxWizardPageSimple* mFirstPage = nullptr;

		//DECLARE_EVENT_TABLE()
	};
}