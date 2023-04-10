#pragma once

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/aui/auibook.h>

#include <vector>
#include <boost/signals2/signal.hpp>
#include "ArtProvider.h"
#include "WorkspaceTabArt.h"

namespace pof {
	class Workspace : public wxPanel
	{
	private:

	protected:
		wxAuiNotebook* mWorkspacebook;
		wxPanel* m_panel3;

	public:
		enum {
			WORKSPACEBOOK = wxID_HIGHEST + 3000
		};

		enum class Notif {
			CLOSED,
			OPENED,
			DELETED,
			ADDED,
			SHOWN,
			HIDEN
		};
		using signal_t = boost::signals2::signal<void(Notif notif, size_t page)>;

		Workspace(wxWindow* parent, wxWindowID id = WORKSPACEBOOK, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(829, 644), long style = wxTAB_TRAVERSAL);
		~Workspace();

		wxAuiNotebook& GetWorkspacebook() { return *mWorkspacebook; }
		size_t GetLastPage() const { return mWorkspacebook->GetPageCount() - 1; }
		size_t GetPageCount() const { return mWorkspacebook->GetPageCount(); }

		inline void SetImageList(wxImageList* imglist) { mWorkspacebook->SetImageList(imglist); }

		boost::signals2::connection AddNotifSlot(signal_t::slot_type&& slot);
		bool AddSpace(wxWindow* space, const std::string& name = "EMPTY", int img = -1);
		void Style();
	protected:
		void OnWorkspaceClose(wxAuiNotebookEvent& evt);


		signal_t mSignal;

		DECLARE_EVENT_TABLE()
	};
}

