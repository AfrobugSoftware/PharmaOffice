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

namespace pof
{
	class ReportsDialog : public wxDialog
	{
	private:

	protected:
		wxPanel* m_panel5 = nullptr;
		wxAuiToolBar* mTools = nullptr;
		wxAuiToolBarItem* mPrint = nullptr;
		wxDatePickerCtrl* mDateFrom = nullptr;
		wxDatePickerCtrl* mDateTo = nullptr;
		wxChoice* mPeriodChoice = nullptr;
		wxGrid* mGrid = nullptr;

	public:
		enum {
			ID_PRINT = 4567,
			ID_DATE_FROM,
			ID_DATE_TO,
			ID_PEIOD_CHOICE,
		};

		//report type
		enum class reportType : std::uint8_t {
			EOD,
			CONSUMP,
		};

		struct ReportParams
		{
			int colCount = 5;
			int rowCount = 5;
			int colLabelSize = 30;
			int rowLabelSize = 80;

			bool enableEditing = false;
			bool enableDragGridSize = false;
			bool enableGridLines = true;

		};


		ReportsDialog(wxWindow* parent, const ReportParams& params = {},  wxWindowID id = wxID_ANY, const wxString & title = wxT("Report"), const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxSize(799, 611), long style = wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL);
		~ReportsDialog();

		constexpr inline wxGrid& GetGrid() { return *mGrid; }



	protected:
		//EVENTS
		void OnDateFromChanged(wxDateEvent& evt);
		void OnDateToChanged(wxDateEvent& evt);
		void OnPrint(wxCommandEvent& evt);
		void OnChoiceChanged(wxCommandEvent& evt);


		DECLARE_EVENT_TABLE()
	};

} // namespace pof
