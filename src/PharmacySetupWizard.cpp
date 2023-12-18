#include "PharmacySetupWizard.h"

pof::PharmacySetupWizard::PharmacySetupWizard(wxFrame* frame)
{
   // SetExtraStyle(wxWIZARD_EX_HELPBUTTON);

    Create(frame, wxID_ANY, "PharmaOffice - Setup pharmacy",
        wxNullBitmap,
        wxDefaultPosition,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    SetBackgroundColour(*wxWHITE);
    SetSize(wxSize(700, 400));
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
    SetIcon(appIcon);

    SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE);
    SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

    CreateFirstPage();


   GetPageAreaSizer()->Add(mFirstPage);
}

bool pof::PharmacySetupWizard::ValidateEmail(const std::string email)
{
    return false;
}

bool pof::PharmacySetupWizard::TransferDataFromWindow()
{
	return true;
}

void pof::PharmacySetupWizard::CreateFirstPage()
{
    // a wizard page may be either an object of predefined class
    mFirstPage = new wxWizardPageSimple(this);
    mFirstPage->SetSize(wxSize(1100, 400));
    /* wxStaticText *text = */ new wxStaticText(mFirstPage, wxID_ANY,
        "This wizard doesn't help you\nto do anything at all.\n"
        "\n"
        "The next pages will present you\nwith more useless controls.",
        wxPoint(5, 5)
        , wxSize(700, 400)
    );

}
