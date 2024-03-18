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
		};



		ChartDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Charts"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1148, 584), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxRESIZE_BORDER);
		void CreateToolbar();

		void LoadWeeklySalesChart();
	private:
		wxAuiManager mManager;
		wxChartPanel* mChartPanel = nullptr;
		wxDatePickerCtrl* mDatePicker = nullptr;
		wxAuiToolBar* mTools = nullptr;
		wxAuiToolBarItem* mExportImage = nullptr;
		wxListCtrl* mListReport = nullptr;
		wxInfoBar* mInfoBar = nullptr;
	};

};