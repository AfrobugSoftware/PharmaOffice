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
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>

///////////////////////////////////////////////////////////////////////////

#define ID_TREE 1000

///////////////////////////////////////////////////////////////////////////////
/// Class Modules
///////////////////////////////////////////////////////////////////////////////
class Modules : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		wxStaticBitmap* m_bitmap1;
		wxPanel* m_panel2;
		wxDataViewTreeCtrl* mModuleTree;
	
	public:
		
		Modules( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 248,680 ), long style = wxNO_BORDER|wxTAB_TRAVERSAL ); 
		~Modules();
	
};

#endif //__NONAME_H__
