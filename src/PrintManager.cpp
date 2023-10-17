#include "PrintManager.h"

pof::PrintManager::PrintManager()
{
	//mPageSetupData = std::make_unique<wxPageSetupData>();
	mPrintData = std::make_unique<wxPrintData>();
	mPrintDialogData = std::make_unique<wxPrintDialogData>(*mPrintData);

	mPrintDialogData->EnableSelection(true);
	mPrintDialogData->EnablePageNumbers(true);
	mPrintDialogData->SetMinPage(1);
	mPrintDialogData->SetMaxPage(2);
	mPrintDialogData->SetFromPage(1);
	mPrintDialogData->SetToPage(2);
	mPrintDialogData->SetAllPages(true);
}

pof::PrintManager::~PrintManager()
{
}

void pof::PrintManager::PrinterSetup()
{
	if (!mPrintDialogData) return;
	(*mPageSetupData) = *mPrintData;

	wxPageSetupDialog pageSetupDialog(nullptr, mPageSetupData.get());
	pageSetupDialog.ShowModal();

	(*mPrintData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
	(*mPageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}

void pof::PrintManager::PrintSaleReceipt(wxWindow* parent)
{
	pof::Printout* po = new pof::Printout(mPrintDialogData.get());
	pof::Printout* po2 = new pof::Printout(mPrintDialogData.get());
	po->mFooterMessage = "THANK YOU FOR YOUR PATRONAGE!";
	po2->mFooterMessage = "THANK YOU FOR YOUR PATRONAGE!";
	Preview(parent, po, po2);
}

void pof::PrintManager::PrintJob(wxWindow* parent, std::unique_ptr<wxPrintout> printout)
{
	wxPrintDialogData printDialogData(*mPrintData);
	wxPrinter printer(&printDialogData);
	if (printout) {
		if (!printer.Print(parent, &(*printout))) {
			if (GetLastError() == wxPRINTER_ERROR) {
				wxMessageBox("Problem printing", "Printing", wxICON_ERROR | wxOK);
			}
			else {
				wxMessageBox("Printing cancelled", "Printing", wxOK);
			}
		}
	}
	(*mPrintData) = printer.GetPrintDialogData().GetPrintData();
}

void pof::PrintManager::Preview(wxWindow* parent,wxPrintout* previewout, wxPrintout* printout)
{
	//copy the print out so that the copy is deleted.
	wxPrintDialogData printDialogData(*mPrintData);
	//cannnout use the same pointer this is weird
	wxPrintPreview* preview = new wxPrintPreview( previewout, printout, &printDialogData);
	if (!preview->Ok())
	{
		delete preview;
		wxMessageBox(wxT("There was a problem previewing.\nPerhaps your current printer is not set correctly?"),
			wxT("Previewing"), wxOK);
		return;
	}

	wxPreviewFrame* frame = new wxPreviewFrame(preview, parent,
		wxT("Label Print Preview"), wxDefaultPosition, wxSize(878, 689));
	frame->Centre(wxBOTH);
	frame->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& evt) {
		printSig(true, gPrintState);
		evt.Skip();
	});

	frame->Initialize();
	frame->Show();
}

