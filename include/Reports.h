#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dialog.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/listctrl.h>
#include <OpenXLSX.hpp>
#include <wx/simplebook.h>
#include <wx/clipbrd.h>
#include <wx/srchctrl.h>
#include <wx/statline.h>
#include <wx/choicdlg.h>

#include <unordered_map>

namespace excel = OpenXLSX;
namespace pof
{
	class ReportsDialog : public wxDialog
	{
	private:

	protected:
		wxPanel* m_panel5 = nullptr;
		wxAuiToolBar* mTools = nullptr;
		wxAuiToolBarItem* mPrint = nullptr;
		wxListCtrl* mListReport = nullptr;
		wxListCtrl* mReportSelect = nullptr;
		wxStaticText* text = nullptr;
		wxAuiToolBarItem* textItem = nullptr;
		wxSimplebook* mBook = nullptr;
		wxDatePickerCtrl* mEodDate = nullptr;
		wxPanel* mEmptyEod = nullptr;
		wxStaticText* mTotalQuantity = nullptr;
		wxStaticText* mTotalAmount = nullptr;
		wxStaticText* mTotalAmountCash = nullptr;
		wxStaticText* mTotalAmountTransfer = nullptr;
		wxStaticText* mTotalAmountPos = nullptr;

		wxTextCtrl* mSaleIdSearch = nullptr;
		
		wxPanel* mCSPanel = nullptr;
		wxStaticText* mTotalClosingStock = nullptr;
		wxStaticText* mTotalExpiredStock = nullptr;
		wxStaticText* mTotalInventoryIn = nullptr;
		wxStaticText* mTotalAmountIn = nullptr;
		wxStaticText* mTotalInventoryOut = nullptr;
		wxStaticText* mTotalAmountOut = nullptr;


		wxPanel* mSPanel = nullptr;
		pof::base::data::datetime_t mSelectedMonth;
		pof::base::data::datetime_t mSelectDay;
		wxPanel* panel = nullptr;
	public:
		//pages
		enum {
			REPORT_VIEW,
			REPORT_EMPTY_EOD,
		};

		enum {
			ID_PRINT = 4567,
			ID_EXCEL,
			ID_EOD_DATE,
			ID_BOOK,
			ID_SHOW_SALE_ID,
			ID_REPORT_LIST,
			ID_COPY_RECIEPT_ID,
			ID_SEARCH_SALEID,
			ID_CHANGE_PAYMENT_OPT,
			ID_RETURN_SALE,
			ID_REMOVE_RETURNS,
			ID_ADD_RETURNS,
			ID_REMOVE_SALE,
		};

		enum {
			IMAGE_CHECK = 0,
			IMAGE_DELETE
		};

		enum class ReportType
		{
			COMSUMPTION_PATTARN,
			EOD,
			EOM,
			IM,
			PL, //PROFIT/LOSS
			PSM, //PRODUCT SOLD FOR MONTH
			EOW,
		};

		ReportsDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString & title = wxT("Report"), const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxSize(1148, 584), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxRESIZE_BORDER);
		virtual ~ReportsDialog();

		constexpr inline wxListCtrl& GetListReport() { return *mListReport; }

		bool LoadReport(ReportType repType, pof::base::data::datetime_t month);

		//different report loading function
		bool LoadConsumptionPattern(pof::base::data::datetime_t month);
		bool LoadEndOFDay();
		bool LoadEndOfMonth();
		bool LoadInventoryMonth();
		bool LoadProfitLoss();
		bool LoadProductSoldForMonth();

	protected:
		//EVENTS
		void OnPrint(wxCommandEvent& evt);
		void OnDownloadExcel(wxCommandEvent& evt);
		void OnReportSelectSelected(wxListEvent& evt);
		void OnDateChange(wxDateEvent& evt);
		void OnEodRightClick(wxListEvent& evt);

		void OnSaleIdSearch(wxCommandEvent& evt);
		void OnSaleIdCleared(wxCommandEvent& evt);
		void OnChangePaymentOption(wxCommandEvent& evt);
		void OnDoReturn(wxCommandEvent& evt);
		void OnFilterReturns(wxCommandEvent& evt);
		void OnAddReturns(wxCommandEvent& evt);
		void OnRemoveSale(wxCommandEvent& evt);

		void CreateToolBar();
		void CreateComsumptionPatternToolBar();
		void CreateEODToolBar();
		void CreateIMToolBar();
		void CreateEmptyEodPage();


		//download for the report type
		void ConsumptionPatternExcel(pof::base::data::datetime_t month);
		void EODExcel();
		void EODSummarizeExcel();
		void InventoryMonthReportExcel();
		void ProfitLossExcel();
		void ProductSoldExcel();

		void UpdateTotals(const pof::base::data& data);
		void UpdateConsumptionTotals(const pof::base::data& data);
		bool bShowSaleID = false;
		bool bFilterReturns = false;

		wxStaticLine* l1 = nullptr;
		wxStaticLine* l2 = nullptr;
		wxStaticLine* l3 = nullptr;
		wxStaticLine* l4 = nullptr;

		std::unordered_map<boost::uuids::uuid, nl::json> mPaymentMap;

		ReportType mCurReportType;
		wxListItem mSelItem;
		std::optional<pof::base::data> data = std::nullopt;
		DECLARE_EVENT_TABLE()
	};

} // namespace pof
