#include "SignInDIalog.h"

BEGIN_EVENT_TABLE(pof::SignInDialog, wxDialog)
	
END_EVENT_TABLE()



pof::SignInDialog::SignInDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	RootPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	RootPane->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel2 = new wxPanel( RootPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	bSizer2->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	m_panel3 = new wxPanel( RootPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_bitmap1 = new wxStaticBitmap( m_panel3, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_bitmap1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mWelcomText = new wxStaticText( m_panel3, wxID_ANY, wxT("WELCOME TO PHARMAOFFICE"), wxDefaultPosition, wxDefaultSize, 0 );
	mWelcomText->Wrap( -1 );
	mWelcomText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer5->Add( mWelcomText, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	bSizer5->Add( 0, 30, 0, wxEXPAND, 5 );
	
	mUserName = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mUserName->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer5->Add( mUserName, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mPassword = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPassword->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer5->Add( mPassword, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	bSizer5->Add( 0, 30, 0, wxEXPAND, 5 );
	
	m_panel5 = new wxPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	mLogOn = new wxButton( m_panel5, ID_LOGON, wxT("Log on"), wxDefaultPosition, wxDefaultSize, 0 );
	mLogOn->SetDefault(); 
	bSizer6->Add( mLogOn, 0, wxALL, 5 );
	
	mSignup = new wxButton( m_panel5, ID_SIGNUP, wxT("Sign Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( mSignup, 0, wxALL, 5 );
	
	
	m_panel5->SetSizer( bSizer6 );
	m_panel5->Layout();
	bSizer6->Fit( m_panel5 );
	bSizer5->Add( m_panel5, 0, wxEXPAND | wxALL, 0 );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_panel3->SetSizer( bSizer5 );
	m_panel3->Layout();
	bSizer5->Fit( m_panel3 );
	bSizer2->Add( m_panel3, 1, wxEXPAND | wxALL, 5 );
	
	m_panel4 = new wxPanel( RootPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_panel4->SetSizer( bSizer4 );
	m_panel4->Layout();
	bSizer4->Fit( m_panel4 );
	bSizer2->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );
	
	
	RootPane->SetSizer( bSizer2 );
	RootPane->Layout();
	bSizer2->Fit( RootPane );
	bSizer1->Add( RootPane, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

pof::SignInDialog::~SignInDialog()
{
}
