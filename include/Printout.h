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
		bool DrawLabelPrint(int page);
		void SetDefaultFonts();

		std::string mFooterMessage;
		int startPage = 1, endPage = 1, pageNum = 1;
		wxFont mPharmacyNameFont;
		wxFont mHeaderFont;
		wxFont mBodyFont;
		wxFont mFooterFont;
		wxFont mContactFont;
		wxFont mInoiceHeaderFont;

		int leftMargin = 2;
		int topMargin = 2;
		int rightMargin = 2;
		int pageCount = 1;
		int minPage = 1;
		int maxPage = 1;
		int selPageFrom = 1;
		int selPageTo = 1;
		double logUnitsFactor = 0.0;
		std::vector<LabelInfo> mLabels;
		wxPrintDialogData* mPrintDialogData;
	};
};