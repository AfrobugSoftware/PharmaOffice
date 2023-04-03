#include "Workspace.h"

pof::Workspace::Workspace( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	mWorkspacebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE|wxAUI_NB_MIDDLE_CLICK_CLOSE|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_EXTERNAL_MOVE|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT|wxAUI_NB_TOP|wxAUI_NB_WINDOWLIST_BUTTON|wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxNO_BORDER );
	mWorkspacebook->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	m_panel3 = new wxPanel( mWorkspacebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mWorkspacebook->AddPage( m_panel3, wxT("a page"), true, wxNullBitmap );
	
	bSizer4->Add( mWorkspacebook, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
}

pof::Workspace::~Workspace()
{
}

boost::signals2::connection pof::Workspace::AddNotifSlot(signal_t::slot_type&& slot)
{
	return mSignal.connect(std::forward<signal_t::slot_type>(slot));
}

bool pof::Workspace::AddSpace(wxWindow* space, const std::string& name, int img)
{
	return mWorkspacebook->AddPage(space, name, false, img);
}



void pof::Workspace::Style()
{
}
