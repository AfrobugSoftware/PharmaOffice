#pragma once

#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aboutdlg.h>
#include <wx/aui/dockart.h>
#include <wx/aui/auibar.h>
#include <wx/srchctrl.h>
#include <wx/frame.h>

using namespace std::literals::string_literals;
namespace pof {
	class MainFrame : public wxFrame
	{
	public:

		//menu ID
		enum
		{
			ID_BACK_UP_DATA,
			ID_NEW_PRODUCT,
			ID_PRODUCT_SEARCH,
			ID_LOG,
			ID_MODULE,
			ID_IMPORT_JSON,
			ID_USER_LOG_IN,
			ID_USER_LOG_OUT,
			ID_USER_PROFILE,
			ID_USER_CREATE_ACCOUNT,
		};

		MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
		virtual ~MainFrame();
	protected:
		void CreateToolBar();
		void CreateMenuBar();
		void CreateStatusBar();

	private:
		DECLARE_EVENT_TABLE()
	};
};