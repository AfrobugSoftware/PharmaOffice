#include "Printout.h"
#include "Application.h"



pof::Printout::Printout(wxPrintDialogData* printDlgData, const std::string& title)
: wxPrintout(title){
	SetDefaultFonts();
}

bool pof::Printout::OnPrintPage(int page)
{
	DrawSalePrint();
	return true;
}

bool pof::Printout::HasPage(int page)
{
	return (pageNum == 1);

}

bool pof::Printout::OnBeginDocument(int startPage, int endPage)
{
	if (!wxPrintout::OnBeginDocument(startPage, endPage))
		return false;

	return true;
}

void pof::Printout::GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo)
{
}

bool pof::Printout::WritePageHeader(wxPrintout* printout, wxDC* dc, const wxString& text, double mmToLogical)
{
	auto& app = wxGetApp();
	int pageWidthMM, pageHeightMM;

	printout->GetPageSizeMM(&pageWidthMM, &pageHeightMM);
	wxUnusedVar(pageHeightMM);
	dc->SetFont(mHeaderFont);

	int leftMarginLogical = int(mmToLogical * leftMargin);
	int topMarginLogical = int(mmToLogical * topMargin);
	int rightMarginLogical = int(mmToLogical * (pageWidthMM - rightMargin));
	int border = 5;

	wxCoord xExtent, yExtent;
	dc->GetTextExtent(text, &xExtent, &yExtent);

	int xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(text, xPos, topMarginLogical + border);

	std::string addy = app.MainPharmacy->GetAddressAsString();
	int yPos = yExtent + border + topMarginLogical; 
	dc->GetTextExtent(addy, &xExtent, &yExtent);
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(addy, xPos, yPos);

	//print contact
	std::string contact = app.MainPharmacy->GetContactAsString();
	yPos += yExtent + border;
	dc->GetTextExtent(contact, &xExtent, &yExtent);
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->SetFont(wxFont(wxFontInfo(8)));
	dc->DrawText(contact, xPos, yPos);

	yPos += yExtent;

	dc->SetPen(*wxBLACK_PEN);
	dc->DrawLine(leftMarginLogical, yPos + border,
		rightMarginLogical, yPos + border);

	//draw titile
	const std::string title = "INVOICE";
	dc->GetTextExtent(title, &xExtent, &yExtent);
	yPos += border;
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));

	dc->SetFont(wxFont(wxFontInfo(12).Bold()));
	dc->DrawText(title, xPos, yPos);

	dc->SetDeviceOrigin(xPos, yPos + topMarginLogical + (border * 2)); //set the origin for the ext
	return true;
}

bool pof::Printout::WriteSaleData(double mmToLogical)
{
	wxDC* dc = GetDC();
	auto& saleData = wxGetApp().mSaleManager.GetSaleData();

	int pageWidthMM, pageHeightMM;
	GetPageSizeMM(&pageWidthMM, &pageHeightMM);

	int leftMarginLogical = int(mmToLogical * leftMargin);
	int topMarginLogical = int(mmToLogical * topMargin);
	int rightMarginLogical = int(mmToLogical * (pageWidthMM - rightMargin));
	int border = 5;
	int xPos = 0, yPos = 0, xExtent = 0, yExtent = 0;
	int lineLength = rightMarginLogical - leftMarginLogical;
	int lineHeight = 0;

	for (auto& v : saleData->GetDatastore())
	{



	/*	int yPos = yExtent + border + topMarginLogical;
		dc->GetTextExtent(addy, &xExtent, &yExtent);
		xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
		dc->DrawText(addy, xPos, yPos);*/


		
	}


	return false;
}

bool pof::Printout::DrawSalePrint()
{

	wxDC* dc = GetDC();
	auto& app = wxGetApp();

	int ppiScreenX = 0 , ppiScreenY = 0;
	int ppiPrinterX = 0, ppiPrinterY = 0;
	GetPPIScreen(&ppiScreenX, &ppiScreenY);
	GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

	double scale = double(ppiPrinterX) / ppiScreenX;
	
	int pageWidth = 0, pageHeight = 0, textX = 0, textY = 0, marginX = 10, marginY = 10;
	int w = 0, h = 0;
	dc->GetSize(&w, &h);
	GetPageSizePixels(&pageWidth, &pageHeight);
	
	double overallScale = scale * w / pageWidth;
	double logUnitsFactor = ppiPrinterX / (scale * 25.4);

	dc->SetUserScale(overallScale, overallScale);


	dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
	dc->SetBrush(*wxTRANSPARENT_BRUSH);
	//calculate the length of a line
	WritePageHeader(this, dc, app.MainPharmacy->name, logUnitsFactor);
	WriteSaleData(logUnitsFactor);


	return false;
}

void pof::Printout::SetDefaultFonts()
{
	mHeaderFont = std::move(wxFont(wxFontInfo(12).AntiAliased().Family(wxFONTFAMILY_SWISS)));
	mBodyFont = std::move(wxFont(wxFontInfo(12).AntiAliased().Family(wxFONTFAMILY_SWISS)));
	mFooterFont = std::move(wxFont(wxFontInfo(12).AntiAliased().Family(wxFONTFAMILY_SWISS)));
}
