#include "Printout.h"
//#include "Application.h"
#include "PofPch.h"


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

size_t pof::Printout::WritePageHeader(wxPrintout* printout, wxDC* dc, const wxString& text, double mmToLogical)
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

	dc->SetFont(wxFont(wxFontInfo(12).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));
	wxCoord xExtent, yExtent;
	dc->GetTextExtent(text, &xExtent, &yExtent);

	int xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(text, xPos, topMarginLogical + border);

	dc->SetFont(mHeaderFont);
	std::string addy = app.MainPharmacy->GetAddressAsString();
	int yPos = yExtent + border + topMarginLogical; 
	dc->GetTextExtent(addy, &xExtent, &yExtent);
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(addy, xPos, yPos + border);

	//print contact
	std::string contact = app.MainPharmacy->GetContactAsString();
	yPos += yExtent + border;
	dc->GetTextExtent(contact, &xExtent, &yExtent);
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->SetFont(wxFont(wxFontInfo(10)));
	dc->DrawText(contact, xPos, yPos + border);



	dc->SetFont(mBodyFont);
	//date/time
	yPos += yExtent + border + 5;
	wxRect rect(leftMarginLogical, yPos, (rightMarginLogical - leftMarginLogical), yExtent);
	pof::base::data::datetime_t& date = boost::variant2::get<pof::base::data::datetime_t>(
			(wxGetApp().mSaleManager.GetSaleData()->GetDatastore().begin())->first[pof::SaleManager::SALE_DATE]);

	dc->DrawLabel(fmt::format("Date/Time: {:%Y-%m-%d %H:%M:%S}", date), rect, wxALIGN_LEFT);

	//account name and status
	yPos += yExtent + border;
	rect.SetPosition(wxPoint(leftMarginLogical, yPos));
	auto& acc = wxGetApp().MainAccount;
	std::string accName = fmt::format("{}, {} {}", acc->GetAccountTypeString(), acc->lastname, acc->name);
	dc->DrawLabel(accName, rect, wxALIGN_LEFT);

	//sale id
	yPos += yExtent + border;
	pof::base::data::duuid_t& d = boost::variant2::get<pof::base::data::duuid_t>(
		(wxGetApp().mSaleManager.GetSaleData()->GetDatastore().begin())->first[pof::SaleManager::SALE_UUID]);
	std::stringstream os;
	os << d;
	rect.SetPosition(wxPoint(leftMarginLogical, yPos));
	dc->DrawLabel(fmt::format("Receipt id: {}", os.str()), rect, wxALIGN_LEFT);

	yPos += yExtent + border;

	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(leftMarginLogical, yPos + border,
		rightMarginLogical, yPos + border);
	dc->DrawLine(leftMarginLogical, yPos + border + 5,
		rightMarginLogical, yPos + border + 5);

	//draw titile
	const std::string title = "INVOICE";
	dc->SetFont(wxFont(wxFontInfo(14).Bold()));
	dc->GetTextExtent(title, &xExtent, &yExtent);
	yPos += border + 20;
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(title, xPos, yPos);

	yPos += yExtent + border + 20;

	return yPos;
}

size_t pof::Printout::WriteSaleData(double mmToLogical, size_t y)
{
	wxDC* dc = GetDC();
	auto& saleData = wxGetApp().mSaleManager.GetSaleData();

	int pageWidthMM, pageHeightMM;
	GetPageSizeMM(&pageWidthMM, &pageHeightMM);

	int leftMarginLogical = int(mmToLogical * leftMargin);
	int topMarginLogical = int(mmToLogical * topMargin);
	int rightMarginLogical = int(mmToLogical * (pageWidthMM - rightMargin));
	int border = 5;
	int xPos = 0, yPos = y, xExtent = 0, yExtent = 18;
	int lineLength = rightMarginLogical - leftMarginLogical;
	int lineHeight = 18;

	//tite
	wxRect rect(leftMarginLogical, yPos, lineLength, lineHeight);
	dc->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));

	dc->DrawLabel("QUANTITY", rect, wxALIGN_LEFT);
	dc->DrawLabel("PRODUCT", rect, wxALIGN_CENTER);
	dc->DrawLabel("SUB-AMOUNT", rect, wxALIGN_RIGHT);

	yPos += lineHeight + 2;
	rect.SetPosition(wxPoint(leftMarginLogical, yPos + border));
	
	dc->SetFont(mBodyFont);
	pof::base::currency totalAmount;
	for (auto& r : saleData->GetDatastore())
	{
		auto& v = r.first;
		pof::base::data::text_t& name = boost::variant2::get<pof::base::data::text_t>(v[pof::SaleManager::PRODUCT_NAME]);
		pof::base::data::currency_t& amount = boost::variant2::get<pof::base::data::currency_t>(v[pof::SaleManager::PRODUCT_EXT_PRICE]);
		std::uint64_t quantity = boost::variant2::get<std::uint64_t>(v[pof::SaleManager::PRODUCT_QUANTITY]);
		
		dc->DrawLabel(fmt::to_string(quantity), rect, wxALIGN_LEFT);
		dc->DrawLabel(name, rect, wxALIGN_CENTER);
		dc->DrawLabel(fmt::format("{:cu}", amount), rect, wxALIGN_RIGHT);


		totalAmount += amount;
		yPos += lineHeight + 2;
		rect.SetPosition(wxPoint(leftMarginLogical, yPos + border));
	}

	//footer
	yPos += lineHeight;
	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(leftMarginLogical, yPos + border,
		rightMarginLogical, yPos + border);
	dc->DrawLine(leftMarginLogical, yPos + border + 5,
		rightMarginLogical, yPos + border + 5);
	dc->SetFont(wxFontInfo(14).Bold());
	std::string totalA = fmt::format("TOTAL: {:cu}", totalAmount);
	dc->GetTextExtent(totalA, &xExtent, &yExtent);
	yPos += border + 20;
	wxRect TotalRect(leftMarginLogical, yPos + border,  lineLength, yExtent);

	dc->DrawLabel(totalA, TotalRect, wxALIGN_RIGHT);

	yPos += yExtent + border;
	dc->SetFont(mBodyFont);
	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(leftMarginLogical, yPos + border + 5,
		rightMarginLogical, yPos + border + 5);
	dc->GetTextExtent(mFooterMessage, &xExtent, &yExtent);
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(mFooterMessage, xPos, yPos + border + 20);


	//
	yPos += yExtent + border;
	dc->SetFont(wxFontInfo(8).Bold().Italic());
	std::string copyRight = "Powered by PharmaOffice";
	dc->GetTextExtent(copyRight, &xExtent, &yExtent);
	xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(copyRight, xPos, yPos + border + 20);


	return yPos;
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
	int y = WritePageHeader(this, dc, app.MainPharmacy->name, logUnitsFactor);
	WriteSaleData(logUnitsFactor, y);


	return false;
}

void pof::Printout::SetDefaultFonts()
{
	mHeaderFont = std::move(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
	mBodyFont = std::move(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
	mFooterFont = std::move(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
}
