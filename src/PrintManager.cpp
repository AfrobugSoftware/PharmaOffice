#include "PrintManager.h"

pof::PrintManager::PrintManager()
{
	//mPageSetupData = std::make_unique<wxPageSetupData>();
	mPrintData = std::make_unique<wxPrintData>();
}

pof::PrintManager::~PrintManager()
{
}

void pof::PrintManager::PrintJob(wxWindow* parent, std::unique_ptr<wxPrintout> printout)
{
	wxPrintDialogData printDialogData(*mPrintData);
	wxPrinter printer(&printDialogData);
	if (printout) {
		if (!printer.Print(parent, &(*printout))) {
			if (GetLastError() == wxPRINTER_ERROR) {
				wxMessageBox("Problem printing the document", "Printing", wxICON_ERROR | wxOK);
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
		wxT("Label Print Preview"));
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show(true);
}

