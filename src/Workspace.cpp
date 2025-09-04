#include "Workspace.h"
BEGIN_EVENT_TABLE(pof::Workspace, wxPanel)
	EVT_AUINOTEBOOK_PAGE_CLOSE(pof::Workspace::WORKSPACEBOOK, pof::Workspace::OnWorkspaceClose)
END_EVENT_TABLE()



pof::Workspace::Workspace( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	SetBackgroundColour(*wxWHITE);
	//SetDoubleBuffered(true);

	mWorkspacebook = new wxAuiNotebook( this, WORKSPACEBOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE|wxAUI_NB_MIDDLE_CLICK_CLOSE|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_EXTERNAL_MOVE|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT|wxAUI_NB_TOP|wxAUI_NB_WINDOWLIST_BUTTON |wxNO_BORDER );
	//mWorkspacebook->SetBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	//mWorkspacebook->SetDoubleBuffered(true);
	auto workspacetabart = new pof::WorkspaceTabArt;
	mWorkspacebook->SetArtProvider(workspacetabart);
	
	bSizer4->Add( mWorkspacebook, 1, wxEXPAND | wxALL, 0 );
	
	SetDropTarget(new pof::TreeItemDropTarget(new pof::TreeItemDataObject{pof::TreeItemDataObject::data_t{}},
		std::bind_front(&pof::Workspace::OnDroppedTreeITtem, this)));
	
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
	if (!space) return false;
	// check if already inserted
	auto pageidx = mWorkspacebook->GetPageIndex(space);
	if (pageidx != wxNOT_FOUND) {
		if (!space->IsShown()) space->Show();
		mWorkspacebook->SetSelection(pageidx);
		mSignal(pof::Workspace::Notif::ADDED, pageidx);
		return true;
	}
	auto ret = mWorkspacebook->AddPage(space, name, true, img);
	mSignal(pof::Workspace::Notif::ADDED, mWorkspacebook->GetSelection());
	return ret;
}



void pof::Workspace::Style()
{
}

void pof::Workspace::OnWorkspaceClose(wxAuiNotebookEvent& evt)
{
	auto pageIndex = evt.GetSelection();
	if (pageIndex != wxNOT_FOUND) {
		mWorkspacebook->RemovePage(pageIndex);
		mSignal(Notif::CLOSED, pageIndex);
	}
	evt.Veto();
}

void pof::Workspace::OnDroppedTreeITtem(const pof::TreeItemDataObject::data_t& item)
{
	AddSpace(std::get<1>(item), std::get<2>(item), std::get<3>(item));
}
