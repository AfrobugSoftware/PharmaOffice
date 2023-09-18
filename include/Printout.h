#pragma once
#include <wx/window.h>
#include <wx/font.h>
#include <wx/print.h>

namespace pof
{
	class Printout : public wxPrintout
	{
	public:
		Printout(wxPrintDialogData* printDlgData, const std::string& title = "My printout");

		virtual bool OnPrintPage(int page) override;
		virtual bool HasPage(int page) override;
		virtual bool OnBeginDocument(int startPage, int endPage) override;
		virtual void GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo) override;
		//virtual bool IsPageSelected(int pageNum) override;

		size_t WritePageHeader(wxPrintout* printout, wxDC* dc, const wxString& text, double mmToLogical);
		size_t WriteSaleData(double mToLogical, size_t ypos);

		bool DrawSalePrint();
		void SetDefaultFonts();

		std::string mFooterMessage;
		int startPage = 1, endPage = 1, pageNum = 1;
		wxFont mHeaderFont;
		wxFont mBodyFont;
		wxFont mFooterFont;

		int leftMargin = 10;
		int topMargin = 10;
		int rightMargin = 10;

	};
};