#include "PrintManager.h"

pof::PrintManager::PrintManager()
{
	//mPageSetupData = std::make_unique<wxPageSetupData>();
	mPrintData = std::make_unique<wxPrintData>();
	//mPrintData->SetQuality(wxPRINT_QUALITY_HIGH);
	mPrintDialogData = std::make_unique<wxPrintDialogData>(*mPrintData);

	mPrintDialogData->EnableSelection(true);
	mPrintDialogData->EnablePageNumbers(true);
	mPrintDialogData->SetMinPage(1);
	mPrintDialogData->SetMaxPage(2);
	mPrintDialogData->SetFromPage(1);
	mPrintDialogData->SetToPage(2);
	mPrintDialogData->SetAllPages(false);
	
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
	//PrinterSetup();
	pof::Printout* po = new pof::Printout(mPrintDialogData.get());
	pof::Printout* po2 = new pof::Printout(mPrintDialogData.get());
	po->mFooterMessage = "THANK YOU FOR YOUR PATRONAGE!";
	po2->mFooterMessage = "THANK YOU FOR YOUR PATRONAGE!";
	if (wxGetApp().bShowPreviewOnSale){
		Preview(parent, po, po2);
	}else PrintJob(parent, po);
}

void pof::PrintManager::PrintJob(wxWindow* parent, wxPrintout* printout)
{
	wxPrinter printer(mPrintDialogData.get());
	if (printout) {
		if (!printer.Print(parent, &(*printout))) {
			if (GetLastError() == wxPRINTER_ERROR) {
				wxMessageBox("Problem printing", "Printing", wxICON_ERROR | wxOK);
			}
		}else {
			printSig(true, gPrintState);
			(*mPrintData) = printer.GetPrintDialogData().GetPrintData();
		}
	}
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

