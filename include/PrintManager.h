#pragma once
#include <queue>
#include <memory>


#include <wx/print.h>
#include <wx/msgdlg.h>
#include <wx/printdlg.h>
#include "Printout.h"

namespace pof {
	class PrintManager
	{
	public:
		PrintManager();
		~PrintManager();
		void PrinterSetup();
		void PrintSaleReceipt(wxWindow* parent);
		void PrintJob(wxWindow* parent, std::unique_ptr<wxPrintout> printjob);
		void Preview(wxWindow* parent, wxPrintout* previeout, wxPrintout* printout);

		std::unique_ptr<wxPrintDialogData> mPrintDialogData;
		std::unique_ptr<wxPageSetupData> mPageSetupData;
		std::unique_ptr<wxPrintData> mPrintData;
	};
};

