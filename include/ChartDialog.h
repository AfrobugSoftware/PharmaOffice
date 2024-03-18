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
#include <wx/infoBar.h>


#include <wx/bars/barplot.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/categoryaxis.h>
#include <wx/xy/xyhistorenderer.h>
#include <wx/category/categorysimpledataset.h>



namespace pof {
	class ChartDialog : public wxDialog
	{
	public:
		enum {
			ID_DATE_PICKER = wxID_HIGHEST + 100,
			ID_EXPORT_IMAGE,
			ID_BOOK,
			ID_TOOLBAR,
			ID_CHART,
		};

		enum {
			EMPTY = 0,
			WEEKLY_SALES,
		};


		ChartDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Charts"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1148, 584), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxRESIZE_BORDER);
		~ChartDialog();
		void CreateToolbar();
		void CreateChartPanel();
		void OnAuiThemeChange();
		void SetupAuiTheme();


		bool LoadWeeklySalesChart();
		wxPanel* CreateEmptyPanel(const std::string& text = {});
		void CheckEmpty(int viewIdx); //switch on book index
		void ShowEmpty(const std::string& text);
	private:
		void OnExportChartImage(wxCommandEvent& evt);
		void OnDateChanged(wxDateEvent& evt);


		wxAuiManager mManager;
		wxChartPanel* mChartPanel = nullptr;
		wxDatePickerCtrl* mDatePicker = nullptr;
		wxAuiToolBar* mTools = nullptr;
		wxAuiToolBarItem* mExportImage = nullptr;
		wxListCtrl* mListReport = nullptr;
		Chart* mCurrentChart = nullptr;
		wxInfoBar* mInfoBar = nullptr;
		wxPanel* mEmpty = nullptr;
		wxStaticText* mEmptyStr = nullptr;
		int mCurrentChartIdx = wxNOT_FOUND;
		wxSimplebook* mBook = nullptr;

		pof::base::data::datetime_t mSelectDay;
		std::optional<pof::base::data> data;
		DECLARE_EVENT_TABLE()
	};

};