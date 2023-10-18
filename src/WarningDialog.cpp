#include "WarningViewDialog.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::Warning, wxDialog)
EVT_BUTTON(wxID_ADD, pof::Warning::OnAdd)
EVT_LIST_ITEM_RIGHT_CLICK(pof::Warning::ID_WARN_VIEW, pof::Warning::OnContextMenu)
EVT_MENU(wxID_REMOVE, pof::Warning::OnRemove)
EVT_MENU(pof::Warning::ID_CHANGE_LEVEL, pof::Warning::OnChangeLevel)
EVT_LIST_ITEM_SELECTED(pof::Warning::ID_WARN_VIEW, pof::Warning::OnItemSelected)
END_EVENT_TABLE()



pof::Warning::Warning( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) 
	: wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_panel5 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	mWarning = new wxStaticText( m_panel5, wxID_ANY, wxT("Warning"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	mWarning->Wrap( -1 );
	bSizer6->Add( mWarning, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mWarningText = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mWarningText->SetMinSize( wxSize( 250,-1 ) );
	
	bSizer6->Add( mWarningText, 1, wxALL|wxEXPAND, 5 );
	
	mWarningLevelText = new wxStaticText( m_panel5, wxID_ANY, wxT("Level"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	mWarningLevelText->Wrap( -1 );
	bSizer6->Add( mWarningLevelText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxArrayString m_choice2Choices;
	m_choice2Choices.Add("WARN");
	m_choice2Choices.Add("CRITICAL");
	m_choice2 = new wxChoice( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice2Choices, 0|wxNO_BORDER );
	m_choice2->SetSelection( 0 );
	m_choice2->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(m_choice2);
	wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

	dc.SetBrush(*wxWHITE);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRoundedRectangle(rect, 2.0f);
	dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
	auto sel = m_choice2->GetStringSelection();
	if (!sel.IsEmpty()) {
		dc.DrawLabel(sel, rect, wxALIGN_CENTER);
	}
	});
	bSizer6->Add( m_choice2, 0, wxALL|wxEXPAND, 5 );
	
	m_button1 = new wxButton( m_panel5, wxID_ADD, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_button1, 0, wxALL, 5 );
	
	m_panel5->SetSizer( bSizer6 );
	m_panel5->Layout();
	bSizer6->Fit( m_panel5 );
	bSizer5->Add( m_panel5, 0, wxEXPAND | wxALL, 2 );
	
	m_panel6 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	mWarningView = new wxListCtrl( m_panel6, ID_WARN_VIEW, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_VRULES|wxNO_BORDER );
	//mWarningView->EnableAlternateRowColours(true);
	wxItemAttr attr;
	attr.SetBackgroundColour(*wxBLACK);
	attr.SetFont(wxFontInfo().Bold().AntiAliased());
	mWarningView->SetHeaderAttr(attr);
	
	bSizer7->Add( mWarningView, 1, wxALL|wxEXPAND, 2 );
	
	
	m_panel6->SetSizer( bSizer7 );
	m_panel6->Layout();
	bSizer7->Fit( m_panel6 );
	bSizer5->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

pof::Warning::~Warning()
{
}

void pof::Warning::OnAdd(wxCommandEvent& evt)
{
	std::string message = mWarningText->GetValue().ToStdString();
	int level = m_choice2->GetSelection();
	if (message.empty() || level == wxNOT_FOUND){
		wxMessageBox("Cannot add empty warning, or the warning level is not set", "WARNING", wxICON_WARNING | wxOK);
		return;
	}
	mWarningView->Freeze();
	int i = mWarningView->GetItemCount();
	wxListItem item;
	item.SetColumn(0);
	item.SetId(i);
	item.SetText(message);
	item.SetMask(wxLIST_MASK_TEXT);
	mWarningView->InsertItem(item);

	item.SetColumn(1);
	item.SetId(i);
	switch(level)
	{
	case pof::ProductManager::SIMPLE:
		item.SetText("WARN");
		break;
	case pof::ProductManager::CRITICAL:
		item.SetText("CRITICAL");
		break;
	default:
		break;
	}
	item.SetMask(wxLIST_MASK_TEXT);
	mWarningView->SetItem(item);

	wxGetApp().mProductManager.InsertWarning(mPuid, level, message);
	mWarningView->Thaw();
	mWarningView->Refresh();

	mWarningText->Clear();
}

void pof::Warning::OnContextMenu(wxListEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto rmv = menu->Append(wxID_REMOVE, "Remove", nullptr);
	wxMenu* submenu = new wxMenu;
	submenu->Append(0, "WARN", nullptr);
	submenu->Bind(wxEVT_MENU, std::bind_front(&pof::Warning::OnChangeLevel, this), 0);

	submenu->Append(1, "CRITICAL", nullptr);
	submenu->Bind(wxEVT_MENU, std::bind_front(&pof::Warning::OnChangeLevel, this), 1);

	auto change = menu->Append(ID_CHANGE_LEVEL, "Change Level", submenu);
	mWarningView->PopupMenu(menu);
}

void pof::Warning::OnRemove(wxCommandEvent& evt)
{
	std::string message = mWarningView->GetItemText(mItem.GetId(), 0).ToStdString();
	wxGetApp().mProductManager.RemoveWarning(mPuid, message);
	mWarningView->Freeze();
	mWarningView->DeleteItem(mItem);
	mWarningView->Thaw();
	mWarningView->Refresh();
}

void pof::Warning::OnChangeLevel(wxCommandEvent& evt)
{
	//maybe you should remove and add back
	int level = evt.GetId();
	mWarningView->Freeze();
	wxListItem item;
	item.SetColumn(1);
	item.SetId(mItem.GetId());
	switch (level)
	{
	case pof::ProductManager::SIMPLE:
		item.SetText("WARN");
		break;
	case pof::ProductManager::CRITICAL:
		item.SetText("CRITICAL");
		break;
	default:
		break;
	}
	item.SetMask(wxLIST_MASK_TEXT);
	mWarningView->SetItem(item);

	mWarningView->Thaw();
	mWarningView->Refresh();
	std::string message = mWarningView->GetItemText(mItem.GetId(), 0).ToStdString();
	wxGetApp().mProductManager.UpdateWarnLevel(mPuid, level, message);	
}

void pof::Warning::OnItemSelected(wxListEvent& evt)
{
	mItem = evt.GetItem();
}

void pof::Warning::LoadWarnings(pof::base::data::duuid_t& pid)
{
	auto rel = wxGetApp().mProductManager.GetWarning(pid);
	if (!rel) {
		wxMessageBox("Error in getting warnings for product", "Warnings", wxICON_WARNING | wxOK);
	}

	mWarningView->AppendColumn("Warning", wxLIST_FORMAT_LEFT, 450);
	mWarningView->AppendColumn("Levels", wxLIST_FORMAT_LEFT, 100);
	size_t i = 0;
	mPuid = pid;
	for (auto& p : *rel){
		wxListItem item;
		item.SetColumn(0);
		item.SetId(i);
		item.SetText(p.second);
		item.SetMask(wxLIST_MASK_TEXT);
		mWarningView->InsertItem(item);

		item.SetColumn(1);
		item.SetId(i);
		switch (p.first)
		{
		case pof::ProductManager::SIMPLE:
			item.SetText("WARN");
			break;
		case pof::ProductManager::CRITICAL:
			item.SetText("CRITICAL");
			break;
		default:
			break;
		}
		item.SetMask(wxLIST_MASK_TEXT);
		mWarningView->SetItem(item);
		i++;
	}
}
