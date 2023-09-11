///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NONAME_H__
#define __NONAME_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>

///////////////////////////////////////////////////////////////////////////

namespace pof
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class StockCheck
	///////////////////////////////////////////////////////////////////////////////
	class StockCheck : public wxDialog 
	{
		private:
		
		protected:
			wxPanel* mMainPane;
			wxDataViewCtrl* mStockData;
			wxDataViewColumn* mProductName;
			wxDataViewColumn* mCurrenctStock;
			wxDataViewColumn* mCheckedStock;
			wxDataViewColumn* mShortage;
			wxDataViewColumn* mStatus;
			wxPanel* mSummary;
			wxStaticText* mTotalStockCheckedLabel;
			wxStaticText* mTotalStockCheckedValue;
			wxStaticText* mTotalShortageLabel;
			wxStaticText* mTotalShortageValue;
			wxStaticText* mShortageAmountLabel;
			wxStaticText* mShortageAmountValue;
		
		public:
			
			StockCheck( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Stock Check"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 878,689 ), long style = wxDEFAULT_DIALOG_STYLE ); wxAuiManager m_mgr;
			
			~StockCheck();
		
	};
	
} // namespace pof

#endif //__NONAME_H__
