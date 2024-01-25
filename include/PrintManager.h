#pragma once
#include <queue>
#include <memory>


#include <wx/print.h>
#include <wx/msgdlg.h>
#include <wx/printdlg.h>
#include "Printout.h"
#include "LabelInfo.h"

#include <boost/signals2.hpp>
namespace pof {
	class PrintManager
	{
	public:
		//print types
		enum {
			RECEIPT,
			REPRINT_RECEIPT,
			LABELS,
			ORDERlIST,
		};

		PrintManager();
		~PrintManager();
		void PrinterSetup();
		void PrintSaleReceipt(wxWindow* parent);
		void PrintOrderList(wxWindow* parent);
		void PrintSaleReceiptHtml(wxHtmlPrintout* print, wxHtmlPrintout* preview);
		void PrintLabels(const std::vector<pof::LabelInfo>& labels, wxWindow* parent);
		void PrintJob(wxWindow* parent, wxPrintout* printjob);
		void Preview(wxWindow* parent, wxPrintout* previeout, wxPrintout* printout);
		 
		size_t gPrintState; 
		boost::signals2::signal<void(bool, size_t)> printSig;
		std::unique_ptr<wxPrintDialogData> mPrintDialogData;
		std::unique_ptr<wxPageSetupData> mPageSetupData;
		std::unique_ptr<wxPrintData> mPrintData;

		pof::Printout* po = nullptr;
		pof::Printout* po2 = nullptr;

	};
};

