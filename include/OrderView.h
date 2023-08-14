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
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
#include <wx/simplebook.h>


namespace pof
{
	class OrderListView : public wxDialog
	{
	private:

	protected:
		wxPanel* m_panel1;
		wxAuiToolBar* mTopTools;
		wxAuiToolBarItem* m_tool11;
		//wxPanel* m_panel2;
		wxDataViewCtrl* mOrderView;
		wxDataViewColumn* mProductCol;
		wxDataViewColumn* mQuanCol;
		wxDataViewColumn* m_dataViewColumn3;
		wxDataViewColumn* m_dataViewColumn4;
		wxPanel* m_panel3;
		wxPanel* m_panel4;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxSimplebook* mBook;
		wxPanel* mEmpty;
	public:
		enum {
			ID_ORDER_VIEW = wxID_HIGHEST + 40,
			ID_PRINT_ORDER,
			ID_REMOVE_ORDER,
			ID_PLACE
		};

		OrderListView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Order List"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(766, 535), long style = wxDEFAULT_DIALOG_STYLE);
		~OrderListView();
		void CreateEmptyPanel();
		void CreateSpeicalCol();

		void OnPrintOrder(wxCommandEvent& evt);
		void OnContexMenu(wxDataViewEvent& evt);
		void OnRemoveOrder(wxCommandEvent& evt);
		DECLARE_EVENT_TABLE()
	};
}
