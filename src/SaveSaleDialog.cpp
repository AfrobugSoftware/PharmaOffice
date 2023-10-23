#include "SaveSaleDialog.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::SaveSaleDialog, wxDialog)
	EVT_LIST_ITEM_RIGHT_CLICK(pof::SaveSaleDialog::ID_SAVE_SALE_VIEW, pof::SaveSaleDialog::OnContextMenu)
	EVT_LIST_ITEM_ACTIVATED(pof::SaveSaleDialog::ID_SAVE_SALE_VIEW, pof::SaveSaleDialog::OnItemActivated)
	EVT_LIST_ITEM_SELECTED(pof::SaveSaleDialog::ID_SAVE_SALE_VIEW, pof::SaveSaleDialog::OnItemSelected)
	EVT_MENU(pof::SaveSaleDialog::ID_REMOVE_SAVE_SALE, pof::SaveSaleDialog::OnRemove)
END_EVENT_TABLE()


pof::SaveSaleDialog::SaveSaleDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
 : wxDialog(parent, id, title, pos, size, style) {
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	auto saveText = new wxStaticText(this, wxID_ANY, wxT("Please Select a saved sale"), wxDefaultPosition, wxDefaultSize, 0);
	saveText->Wrap(-1);
	saveText->SetFont(wxFontInfo(9).Bold().AntiAliased());
	bSizer5->Add(saveText, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer5->AddSpacer(20);

	m_panel6 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	mSaveSaleView = new wxListCtrl(m_panel6, ID_SAVE_SALE_VIEW, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_REPORT | wxLC_VRULES | wxNO_BORDER);
	//mWarningView->EnableAlternateRowColours(true);
	wxItemAttr attr;
	attr.SetBackgroundColour(*wxBLACK);
	attr.SetFont(wxFontInfo().Bold().AntiAliased());
	mSaveSaleView->SetHeaderAttr(attr);

	bSizer7->Add(mSaveSaleView, 1, wxALL | wxEXPAND, 2);


	m_panel6->SetSizer(bSizer7);
	m_panel6->Layout();
	bSizer7->Fit(m_panel6);
	bSizer5->Add(m_panel6, 1, wxEXPAND | wxALL, 5);


	this->SetSizer(bSizer5);
	this->Layout();

	this->Centre(wxBOTH);
}

pof::SaveSaleDialog::~SaveSaleDialog()
{
}

bool pof::SaveSaleDialog::LoadSaveSales(const pof::base::relation<pof::base::data::datetime_t, boost::uuids::uuid, pof::base::currency>& relation)
{
	mSaveSaleView->AppendColumn("Save Date", wxLIST_FORMAT_LEFT, 100);
	mSaveSaleView->AppendColumn("Save ID", wxLIST_FORMAT_LEFT, 450);
	mSaveSaleView->AppendColumn("Save Total Price", wxLIST_FORMAT_LEFT, 150);
	size_t i = 0;
	std::ostringstream str;
	if (relation.empty()) return false;
	for (const auto& tup : relation){
		wxListItem item;
		item.SetColumn(0);
		item.SetId(i);
		item.SetText(fmt::format("{:%d/%m/%Y}", std::get<0>(tup)));
		item.SetData(new pof::base::data::duuid_t(std::get<1>(tup)));
		item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_DATA);
		mSaveSaleView->InsertItem(item);
		
		item.SetColumn(1);
		item.SetId(i);
		str << std::get<1>(tup);
		item.SetText(fmt::format("{}",str.str()));
		item.SetMask(wxLIST_MASK_TEXT);
		str.str({});
		mSaveSaleView->SetItem(item);


		item.SetColumn(2);
		item.SetId(i);
		item.SetText(fmt::format("{:cu}", std::get<2>(tup)));
		item.SetMask(wxLIST_MASK_TEXT);
		mSaveSaleView->SetItem(item);
		i++;
	}
	return true;
}

void pof::SaveSaleDialog::OnContextMenu(wxListEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto pp = menu->Append(ID_REMOVE_SAVE_SALE, "Remove save sale", nullptr);

	mSaveSaleView->PopupMenu(menu);
}

void pof::SaveSaleDialog::OnRemove(wxCommandEvent& evt)
{
	boost::uuids::uuid* uid = reinterpret_cast<boost::uuids::uuid*>(mItem.GetData());
	if (uid == nullptr) return;
	
	bool status = wxGetApp().mSaleManager.RemoveSaveSale(*uid);
	if (!status){
		wxMessageBox("Cannot remove save sale", "Sale", wxICON_ERROR | wxOK);
		return;
	}

	mSaveSaleView->Freeze();
	mSaveSaleView->DeleteItem(mItem);
	mSaveSaleView->Thaw();
	mSaveSaleView->Refresh();
	mItem = wxListItem{};
}

void pof::SaveSaleDialog::OnItemSelected(wxListEvent& evt)
{
	mItem = evt.GetItem();
}

void pof::SaveSaleDialog::OnItemActivated(wxListEvent& evt)
{
	boost::uuids::uuid* uid = reinterpret_cast<boost::uuids::uuid*>(evt.GetItem().GetData());
	if (uid == nullptr){
		EndModal(wxID_CANCEL);
		return;
	}
	mSuid = *uid;
	EndModal(wxID_OK);
}
