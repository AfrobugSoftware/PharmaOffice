#pragma once
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/aui/aui.h>


namespace pof
{
	class AuditView : public wxPanel {
		enum {
			ID_DATA_VIEW = wxID_HIGHEST + 10,
		};
		AuditView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
			const wxSize& size = wxDefaultSize);


	private:
		wxAuiManager mAuiManager;
		wxDataViewCtrl* mDataView = nullptr;
	};
};