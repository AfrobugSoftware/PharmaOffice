#pragma once
#include <queue>
#include <memory>


#include <wx/print.h>
#include <wx/msgdlg.h>


namespace pof {
	class PrintManager
	{
	public:
		PrintManager();
		~PrintManager();
		void PrintJob(wxWindow* parent, std::unique_ptr<wxPrintout> printjob);
		void Preview(wxWindow* parent, wxPrintout* previeout, wxPrintout* printout);
	private:
		std::unique_ptr<wxPageSetupData> mPageSetupData;
		std::unique_ptr<wxPrintData> mPrintData;
	};
};

