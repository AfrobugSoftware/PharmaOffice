

#include "ResetPassword.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::ResetPassword, wxDialog)
	EVT_BUTTON(wxID_OK, pof::ResetPassword::OnOk)
END_EVENT_TABLE()


pof::ResetPassword::ResetPassword(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : 
	wxDialog(parent, id, "Reset password", pos, size, style)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Please reset your password"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	m_staticText1->Wrap(-1);
	bSizer1->Add(m_staticText1, 0, wxALL, 5);

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer(2, 2, 0, 0);

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Current Password"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	gSizer1->Add(m_staticText2, 0, wxALL, 5);

	m_textCtrl1 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(m_textCtrl1, 1, wxALL | wxEXPAND, 5);

	m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	gSizer1->Add(m_staticText3, 0, wxALL, 5);

	m_textCtrl2 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

	m_staticText4 = new wxStaticText(this, wxID_ANY, wxT("Re-enter password"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->Wrap(-1);
	gSizer1->Add(m_staticText4, 0, wxALL, 5);

	m_textCtrl3 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);


	bSizer1->Add(gSizer1, 0, wxALL | wxEXPAND, 5);


	bSizer1->Add(0, 0, 1, wxEXPAND, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bSizer1->Add(m_sdbSizer1, 0, wxALL | wxEXPAND, 5);

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	this->SetIcon(appIcon);

	this->SetSizer(bSizer1);
	this->Layout();
}

pof::ResetPassword::~ResetPassword()
{
}

void pof::ResetPassword::OnOk(wxCommandEvent& evt)
{
	const std::string currentPassword = m_textCtrl1->GetValue().ToStdString();
	const std::string pass = m_textCtrl2->GetValue().ToStdString();
	const std::string repass = m_textCtrl3->GetValue().ToStdString();

	//verify current pass
	const auto& acc = wxGetApp().MainAccount;
	if (!acc->ValidateCredentials(acc->username, currentPassword)) {
		wxMessageBox("Incorrect current password", "Reset password", wxICON_ERROR | wxOK);
		return;
	}
	if ((pass != repass))
	{
		wxMessageBox("Password mismatch", "Reset password", wxICON_INFORMATION | wxOK);
		return;
	}
	if (!acc->UpdateUserPassword(acc->username, bcrypt::generateHash(currentPassword))) {
		wxMessageBox("Cannot update user password", "Reset password", wxICON_INFORMATION | wxOK);
		return;
	}
	EndModal(wxID_OK);
}

