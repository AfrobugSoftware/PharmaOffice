#pragma once
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <currency.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/valnum.h>
#include <wx/dcclient.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <fmt/format.h>
#include <wx/statline.h>


#include <tuple>
namespace pof
{
	class CheckoutDialog : public wxDialog
	{
	public:
		enum {
			ID_TOTAL_AMOUNT = wxID_HIGHEST + 10,
			ID_TOTAL_PAID,
			ID_TOTAL_CHANGE,
		};

		CheckoutDialog(wxWindow* parent,const pof::base::currency& cur, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
		virtual ~CheckoutDialog() = default;

		virtual bool TransferDataFromWindow() override;
		wxArrayString paymentTypes;
		pof::base::currency mTotalPayment;

		void OnUpdate(wxUpdateUIEvent& evt);
		std::tuple<std::string, pof::base::currency, std::string,
			pof::base::currency> mPayments;
		pof::base::currency mChange;
	private:
		wxChoice* paytype1  = nullptr;
		wxTextCtrl* amount1 = nullptr;
		wxChoice* paytype2  = nullptr;
		wxTextCtrl* amount2 = nullptr;
		double fv = 0.0;

		wxStaticText* totalAmount = nullptr;
		wxStaticText* totalPaid   = nullptr;
		wxStaticText* totalChange = nullptr;
		wxStaticText* totalOverPaid = nullptr;
		wxPanel* summary = nullptr;

		DECLARE_EVENT_TABLE()
	};
}
