#pragma once
#include <queue>
#include <memory>


#include <wx/print.h>
#include <wx/msgdlg.h>
#include <wx/printdlg.h>
#include "Printout.h"

#include <boost/signals2.hpp>
namespace pof {
	class PrintManager
	{
	public:
		//print types
		enum {
			RECEIPT,
			REPRINT_RECEIPT,
		};

		PrintManager();
		~PrintManager();
		void PrinterSetup();
		void PrintSaleReceipt(wxWindow* parent);
		void PrintJob(wxWindow* parent, std::unique_ptr<wxPrintout> printjob);
		void Preview(wxWindow* parent, wxPrintout* previeout, wxPrintout* printout);

		size_t gPrintState; 
		boost::signals2::signal<void(bool, size_t)> printSig;
		std::unique_ptr<wxPrintDialogData> mPrintDialogData;
		std::unique_ptr<wxPageSetupData> mPageSetupData;
		std::unique_ptr<wxPrintData> mPrintData;
	};
};

