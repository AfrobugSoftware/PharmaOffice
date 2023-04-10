#pragma once
#include <wx/window.h>
#include <wx/aui/tabart.h>

namespace pof {
	class WorkspaceTabArt : public wxAuiDefaultTabArt
	{
	public:
		WorkspaceTabArt() {
			m_baseColour = wxTheColourDatabase->Find("Aqua");
			m_baseColourBrush = wxBrush(m_baseColour);
			m_baseColourPen = wxPen(m_baseColour);
		}
		using wxAuiDefaultTabArt::DrawButton;
		using wxAuiDefaultTabArt::DrawBorder;
		using wxAuiDefaultTabArt::DrawTab;
		using wxAuiDefaultTabArt::DrawBackground;

		

		void SetBackgroundColour(const wxColour& c) { mBackgroundColour = c; }
		const wxColour& GetBackgroundColour() const { return mBackgroundColour; }
	private:
		wxColor mBackgroundColour;
	};
};