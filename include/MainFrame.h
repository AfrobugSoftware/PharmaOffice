#pragma once

#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aboutdlg.h>
#include <wx/aui/dockart.h>
#include <wx/aui/auibar.h>
#include <wx/srchctrl.h>
#include <wx/frame.h>

#include "LogSink.h"
#include "ArtProvider.h"
#include "Modules.h"
#include "Workspace.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <functional>


namespace pof {
	class MainFrame : public wxFrame
	{
	public:

		//menu ID
		enum
		{
			ID_BACK_UP_DATA = wxID_HIGHEST + 100,
			ID_MODULE,
			ID_USER_PROFILE,
			ID_USER_CREATE_ACCOUNT,
			ID_MENU_VIEW_LOG,
			ID_STATUS_BAR,
			ID_TOOL_BAR


		};

		MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
		virtual ~MainFrame();
	protected:
		void CreateMenuBar();
		void CreateStatusBar();
		void CreateTheAcceleratorTable();
		void CreateLogView();
		void CreateModules();
		void CreateWorkSpace();

		void OnAbout(wxCommandEvent& evt);
		void OnClose(wxCloseEvent& evt);


	private:
		wxAuiManager mAuiManager;
		pof::Modules* mModules = nullptr;
		pof::Workspace* mWorkspace = nullptr;
		std::shared_ptr<wxTextCtrl> mLogView = nullptr;

		DECLARE_EVENT_TABLE()
	};
};