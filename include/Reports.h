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
	public:
		enum {
			ID_PRINT = 4567,
			ID_EXCEL,
		};

		enum {
			IMAGE_CHECK = 0,
			IMAGE_DELETE
		};

		enum class ReportType
		{
			COMSUMPTION_PATTARN,
			EOD,
		};

		ReportsDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString & title = wxT("Report"), const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxSize(848, 584), long style = wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL | wxRESIZE_BORDER);
		~ReportsDialog();

		constexpr inline wxListCtrl& GetListReport() { return *mListReport; }

		bool LoadReport(ReportType repType);

		//different report loading function
		bool LoadConsumptionPattern();
		bool LoadEndOFDay();
	protected:
		//EVENTS
		void OnPrint(wxCommandEvent& evt);

		ReportType mCurReportType;
		DECLARE_EVENT_TABLE()
	};

} // namespace pof
