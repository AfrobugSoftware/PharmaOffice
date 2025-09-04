#include "CheckoutDialog.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::CheckoutDialog, wxDialog)
	EVT_UPDATE_UI_RANGE(pof::CheckoutDialog::ID_TOTAL_AMOUNT, 
		 pof::CheckoutDialog::ID_TOTAL_CHANGE, pof::CheckoutDialog::OnUpdate)
END_EVENT_TABLE()


pof::CheckoutDialog::CheckoutDialog(wxWindow* parent, const pof::base::currency& cur, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxDialog(parent, id, "Checkout", pos, size, style), mTotalPayment(cur) {
	SetSize(FromDIP(wxSize(591, 398)));
	SetBackgroundColour(*wxWHITE);
	ClearBackground();

	wxSizer* ms = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* flexSizer = new wxFlexGridSizer(8, 3, FromDIP(2), FromDIP(5));
	flexSizer->AddGrowableCol(1);
	flexSizer->SetFlexibleDirection(wxBOTH);
	flexSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	wxFloatingPointValidator<double> val(2, &fv, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 999999999999);

	auto TitleText = new wxStaticText(this, wxID_ANY, wxT("Checkout\nPlease select payment type"), wxDefaultPosition, wxDefaultSize, 0);
	TitleText->Wrap(-1);
	paymentTypes.push_back("Cash");
	paymentTypes.push_back("Transfer");
	paymentTypes.push_back("POS");
	paymentTypes.push_back("No payment option");

	paytype1 = new wxChoice(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(150, 20)), paymentTypes);
	paytype1->SetSelection(paymentTypes.size() - 1);

	flexSizer->Add(paytype1, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, FromDIP(5)));

	flexSizer->Add(new wxStaticText(this, wxID_ANY, "Amount: "), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, FromDIP(5)));
	amount1 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(200, -1)));
	amount1->Clear();

	amount1->SetValidator(val);
	flexSizer->Add(amount1, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));

	paytype2 = new wxChoice(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(150, 20)), paymentTypes);
	paytype2->SetSelection(paymentTypes.size() - 1);
	

	flexSizer->Add(paytype2, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, FromDIP(5)));

	flexSizer->Add(new wxStaticText(this, wxID_ANY, "Amount: "), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, FromDIP(5)));
	amount2 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(200, -1)));
	amount2->Clear();
	amount2->SetValidator(val);
	flexSizer->Add(amount2, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));

	wxPanel* m_panel3 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxSizer* bLineSizer = new wxBoxSizer(wxHORIZONTAL);
	bLineSizer->AddSpacer(FromDIP(20));
	auto line = new wxStaticLine(m_panel3, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	line->SetBackgroundColour(wxTheColourDatabase->Find("module"));

	bLineSizer->Add(line, wxSizerFlags().Proportion(1).Expand());
	bLineSizer->AddSpacer(FromDIP(20));

	m_panel3->SetSizer(bLineSizer);
	m_panel3->Layout();
	bLineSizer->Fit(m_panel3);
	m_panel3->SetBackgroundColour(wxTheColourDatabase->Find("module"));


	summary = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridSizer* gsizer = new wxGridSizer(4, 2, 5, 5);
	wxFont valueFont(wxFontInfo(FromDIP(10)).AntiAliased());

	auto s1 = new wxStaticText(summary, wxID_ANY, wxT("Total: "), wxDefaultPosition, wxDefaultSize, 0);
	s1->Wrap(-1);
	gsizer->Add(s1, 0, wxALIGN_RIGHT | wxALL, FromDIP(5));

	totalAmount = new wxStaticText(summary, ID_TOTAL_AMOUNT, fmt::format("{:cu}", mTotalPayment), wxDefaultPosition, wxDefaultSize, 0);
	totalAmount->Wrap(-1);
	gsizer->Add(totalAmount, 0, wxALIGN_RIGHT | wxALL, FromDIP(5));

	auto s2 = new wxStaticText(summary, wxID_ANY, wxT("Paid: "), wxDefaultPosition, wxDefaultSize, 0);
	s1->Wrap(-1);
	gsizer->Add(s2, 0, wxALIGN_RIGHT | wxALL, FromDIP(5));

	totalPaid = new wxStaticText(summary, ID_TOTAL_PAID, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	totalPaid->Wrap(-1);
	gsizer->Add(totalPaid, 0, wxALIGN_RIGHT | wxALL, FromDIP(5));

	auto s3 = new wxStaticText(summary, wxID_ANY, wxT("Amount left: "), wxDefaultPosition, wxDefaultSize, 0);
	s1->Wrap(-1);
	gsizer->Add(s3, 0, wxALIGN_RIGHT | wxALL, FromDIP(5));

	totalChange = new wxStaticText(summary, ID_TOTAL_CHANGE, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	totalChange->Wrap(-1);
	gsizer->Add(totalChange, 0, wxALIGN_RIGHT | wxALL, FromDIP(5));
	
	summary->SetSizer(gsizer);
	summary->Layout();

	wxStdDialogButtonSizer* mButtonSizer = new wxStdDialogButtonSizer();
	wxButton* m_sdbSizer2OK = new wxButton(this, wxID_OK);
	mButtonSizer->AddButton(m_sdbSizer2OK);
	wxButton* m_sdbSizer2Cancel = new wxButton(this, wxID_CANCEL);
	mButtonSizer->AddButton(m_sdbSizer2Cancel);
	mButtonSizer->Realize();

	ms->Add(TitleText, wxSizerFlags().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	ms->Add(flexSizer, wxSizerFlags().Expand().Align(wxLEFT).Border(wxALL, FromDIP(5)));
	ms->AddSpacer(FromDIP(15));
	ms->Add(m_panel3, wxSizerFlags().Expand().Border(wxALL, FromDIP(5)));
	ms->AddSpacer(FromDIP(5));
	ms->Add(summary, wxSizerFlags().Expand().Border(wxALL, FromDIP(5)));
	ms->Add(mButtonSizer, wxSizerFlags().Expand().Border(wxALL, FromDIP(5)));

	this->SetSizer(ms);
	this->Layout();
	ms->Fit(this);

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

bool pof::CheckoutDialog::TransferDataFromWindow()
{
	if (amount1->IsEmpty() && amount2->IsEmpty()) {
		wxMessageBox("No amount was entered", "Checkout", wxICON_WARNING | wxOK);
		return false;
	} 
	
	if (paytype1->GetSelection() == paytype2->GetSelection())
	{
		wxMessageBox("Cannot have both option set to the same payment type", "Checkout", wxICON_WARNING | wxOK);
		return false;
	}

	pof::base::currency total;
	if (paytype1->GetSelection() != paymentTypes.size() - 1) {
		const auto am1 = amount1->GetValue().ToStdString();
		total += pof::base::currency(am1);
		std::get<0>(mPayments) = paytype1->GetStringSelection().ToStdString();
		std::get<1>(mPayments) = pof::base::currency(am1);

	}

	if (paytype2->GetSelection() != paymentTypes.size() - 1) {
		const auto am2 = amount2->GetValue().ToStdString();
		total += pof::base::currency(am2);
		std::get<2>(mPayments) = paytype2->GetStringSelection().ToStdString();
		std::get<3>(mPayments) = pof::base::currency(am2);
	}

	if (std::get<0>(mPayments).empty() && 
		std::get<2>(mPayments).empty()) {
		wxMessageBox("No payment option was selected", "Checkout", wxICON_WARNING | wxOK);
		return false;
	}

	if (mTotalPayment > total) {
		wxMessageBox("Incomplete payment, please complete payment", "Checkout", wxICON_WARNING | wxOK);
		return false;
	}
	else if (total > mTotalPayment) {
		wxMessageBox("Cannot pay more than what was asked to pay.\nPlease reduce amount in one of the options.", "Checkout", wxICON_WARNING | wxOK);
		return false;
	}


	return true;
}

void pof::CheckoutDialog::OnUpdate(wxUpdateUIEvent& evt)
{
	auto am1 = amount1->GetValue().ToStdString();
	auto am2 = amount2->GetValue().ToStdString();
	
	if (am1.empty() && am2.empty()) return;

	pof::base::currency a1(am1);
	pof::base::currency a2(am2);

	/*auto t = (static_cast<double>(a1) + 
			static_cast<double>(a2)) - static_cast<double>(mTotalPayment);*/
	auto t =  static_cast<double>(mTotalPayment) - 
		(static_cast<double>(a1) + static_cast<double>(a2));
	if (std::signbit(static_cast<double>(t))) {
		t = pof::base::currency{};
	}

	totalPaid->SetLabelText(fmt::format("{:cu}", (a1 + a2)));
	totalChange->SetLabelText(fmt::format("{:cu}", pof::base::currency(t)));
	summary->Layout();
}
