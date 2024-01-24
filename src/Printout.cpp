#include "Printout.h"
//#include "Application.h"
#include "PofPch.h"
#include <wx/fontutil.h>

pof::Printout::Printout(wxPrintDialogData* printDlgData, const std::string& title)
: wxPrintout(title){
	mPrintDialogData = printDlgData;
	SetDefaultFonts();
	m_paper_type = static_cast<wxPaperSize>(wxGetApp().mPaperType);
	PerformPageSetup(wxGetApp().bShowPageSetup);
}

bool pof::Printout::OnPrintPage(int page)
{
	if (wxGetApp().mPrintManager->gPrintState == pof::PrintManager::LABELS)
	{
		DrawLabelPrint(page);
	}else{
		DrawSalePrint();
	}
	return true;
}

bool pof::Printout::HasPage(int page)
{
	if (wxGetApp().mPrintManager->gPrintState == pof::PrintManager::LABELS)
	{
		return (page <= mLabels.size() && page > 0);
	}
	return (pageNum == 1);
}

bool pof::Printout::OnBeginDocument(int startPage, int endPage)
{
	if (!wxPrintout::OnBeginDocument(startPage, endPage))
		return false;
	wxSize paperSize = m_page_setup.GetPaperSize();  // in millimeters

	// still in millimeters
	float large_side = std::max(paperSize.GetWidth(), paperSize.GetHeight());
	float small_side = std::min(paperSize.GetWidth(), paperSize.GetHeight());

	float large_side_cm = large_side  / 10.0f;  // in centimeters
	float small_side_cm = small_side / 10.0f;  // in centimeters

	float ratio = float(large_side - topMargin - bottomMargin) /
		float(small_side - leftMargin - rightMargin);

	m_coord_system_width = (int)((small_side_cm - topMargin / 10.f -
		bottomMargin / 10.0f) * 30);
	m_coord_system_height = m_coord_system_width * ratio;

	return true;
}

void pof::Printout::GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo)
{
		*minPage = this->minPage;
		*maxPage = this->maxPage;
		*selPageFrom = this->selPageFrom;
		*selPageTo = this->selPageTo;
}

void pof::Printout::PerformPageSetup(bool showSetup)
{
	// don't show page setup dialog, use default values
	wxPrintData printdata;
	printdata.SetPrintMode(wxPRINT_MODE_PRINTER);
	printdata.SetOrientation(wxPORTRAIT);
	printdata.SetNoCopies(1);
	printdata.SetPaperId(m_paper_type);

	m_page_setup = wxPageSetupDialogData(printdata);
	m_page_setup.SetMarginTopLeft(wxPoint(leftMargin, topMargin));
	m_page_setup.SetMarginBottomRight(wxPoint(rightMargin, bottomMargin));

	if (showSetup)
	{
		wxPageSetupDialog dialog(NULL, &m_page_setup);
		if (dialog.ShowModal() == wxID_OK)
		{

			m_page_setup = dialog.GetPageSetupData();
			m_paper_type = m_page_setup.GetPrintData().GetPaperId();
			
			wxPoint marginTopLeft = m_page_setup.GetMarginTopLeft();
			wxPoint marginBottomRight = m_page_setup.GetMarginBottomRight();
			leftMargin = marginTopLeft.x;
			rightMargin = marginBottomRight.x;
			topMargin = marginTopLeft.y;
			bottomMargin = marginBottomRight.y;
		}
	}

	spdlog::info("Paper type {:d}", m_paper_type);
	wxGetApp().mPaperType = static_cast<int>(m_paper_type);
}

size_t pof::Printout::WritePageHeader(wxPrintout* printout, wxDC* dc, const wxString& text, double mmToLogical)
{
	auto& app = wxGetApp();
	
	int border = 5;
	int xPos = leftMargin, yPos = topMargin;
	int lineLength = m_coord_system_width; //rightMarginLogical - leftMarginLogical;
	int lineHeight = 18;

	//dc->SetFont(mPharmacyNameFont);
	//wxString name(wxT("Bits"));
	wxFont font(wxFONTSIZE_SMALL, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString);

	dc->SetFont(font);


	wxCoord xExtent = 0, yExtent = 0;
	dc->GetTextExtent(text, &xExtent, &yExtent);
	lineHeight = yExtent;

	dc->DrawText(text, lineLength / 2 - xExtent / 2 , yPos + border);

	std::string addy = app.MainPharmacy->GetAddressAsString();
	yPos += lineHeight + border;
	dc->GetTextExtent(addy, &xExtent, &yExtent);
	dc->DrawText(addy, lineLength/2 - xExtent/2, yPos + border);

	//print contact
	std::string contact = app.MainPharmacy->GetContactAsString();
	yPos += yExtent + border;
	dc->GetTextExtent(contact, &xExtent, &yExtent);
	dc->DrawText(contact, lineLength/2 - xExtent/2, yPos + border);



	//dc->SetFont(mBodyFont);
	//date/time
	yPos += yExtent + border + 5;
	wxRect rect(xPos, yPos, lineLength, lineHeight);
	pof::base::data::datetime_t& date = boost::variant2::get<pof::base::data::datetime_t>(
			(wxGetApp().mSaleManager.GetSaleData()->GetDatastore().begin())->first[pof::SaleManager::SALE_DATE]);

	dc->DrawLabel(fmt::format("Date/Time: {:%Y-%m-%d %H:%M:%S}", date), rect, wxALIGN_LEFT);

	//account name and status
	yPos += yExtent + border;
	rect.SetPosition(wxPoint(xPos, yPos));
	auto& acc = wxGetApp().MainAccount;
	std::string accName = fmt::format("{}, {} {}", acc->GetAccountTypeString(), acc->lastname, acc->name);
	dc->DrawLabel(accName, rect, wxALIGN_LEFT);

	//sale id
	yPos += yExtent + border;
	pof::base::data::duuid_t& d = boost::variant2::get<pof::base::data::duuid_t>(
		(wxGetApp().mSaleManager.GetSaleData()->GetDatastore().begin())->first[pof::SaleManager::SALE_UUID]);
	std::stringstream os;
	os << d;
	rect.SetPosition(wxPoint(xPos, yPos));
	dc->DrawLabel(fmt::format("Receipt id: {}", os.str()), rect, wxALIGN_LEFT);

	yPos += yExtent + border;

	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(xPos, yPos + border, lineLength, yPos + border);
	dc->DrawLine(xPos, yPos + border + 5, lineLength, yPos + border + 5);

	//draw titile
	const std::string title = "INVOICE";
	//dc->SetFont(mInoiceHeaderFont);
	dc->GetTextExtent(title, &xExtent, &yExtent);
	yPos += border + 10;
	//xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(title, lineLength / 2 - xExtent/2, yPos);

	yPos += yExtent + border + 2;

	return yPos;
}

size_t pof::Printout::WriteSaleData(double mmToLogical, size_t y)
{
	wxDC* dc = GetDC();
	auto& saleData = wxGetApp().mSaleManager.GetSaleData();

	int border = 5;
	int xPos = 0, yPos = y, xExtent = 0, yExtent = 18;
	int lineLength = m_coord_system_width; //rightMarginLogical - leftMarginLogical;
	dc->GetTextExtent("X", &xExtent, &yExtent);
	int lineHeight = yExtent;

	//tite
	wxRect rect(xPos + border, yPos, lineLength, lineHeight - border);
	//dc->SetFont(mInoiceHeaderFont);

	dc->DrawLabel("Quantity", rect, wxALIGN_LEFT);
	dc->DrawLabel("Product", rect, wxALIGN_CENTER);
	dc->DrawLabel("Sub-amount", rect, wxALIGN_RIGHT);

	yPos += lineHeight + 2;
	rect.SetPosition(wxPoint(xPos + border, yPos + border));
	
	//dc->SetFont(mBodyFont);
	auto& cachefont = dc->GetFont();

	//wxString name(wxT("Bits"));
	wxFont font(wxFONTSIZE_SMALL, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT,false, wxEmptyString);

	dc->SetFont(font);

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
		/*auto curtext = fmt::format("{:cu}", amount);
		auto quantext = fmt::to_string(quantity);

		dc->DrawText(quantext, xPos, yPos);
		dc->GetTextExtent(quantext, &xExtent, &yExtent);

		dc->DrawText(name, xPos + xExtent + 2, yPos);
		dc->GetTextExtent(name, &xExtent, &yExtent);

		dc->DrawText(curtext, xPos + xExtent + 2, yPos);*/

		totalAmount += amount;
		yPos += lineHeight + 2;
		rect.SetPosition(wxPoint(xPos + border, yPos + border));
	}

	dc->SetFont(cachefont);
	//footer
	yPos += lineHeight;
	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(xPos, yPos + border,lineLength, yPos + border);
	dc->DrawLine(xPos, yPos + border + 5, lineLength, yPos + border + 5);
	//dc->SetFont(mInoiceHeaderFont);
	std::string totalA = fmt::format("TOTAL: {:cu}", totalAmount);
	dc->GetTextExtent(totalA, &xExtent, &yExtent);
	yPos += border + 10;
	wxRect TotalRect(xPos, yPos + border,  lineLength - border, yExtent);

	dc->DrawLabel(totalA, TotalRect, wxALIGN_RIGHT);

	yPos += yExtent + border;
	//dc->SetFont(mFooterFont);
	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(xPos, yPos + border, lineLength, yPos + border);
	dc->GetTextExtent(mFooterMessage, &xExtent, &yExtent);
	//xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(mFooterMessage, lineLength/2 - xExtent /2, yPos + border +2);


	//
	yPos += yExtent + border;
	//dc->SetFont(mFooterFont);
	std::string copyRight = "Powered by PharmaOffice";
	dc->GetTextExtent(copyRight, &xExtent, &yExtent);
	xPos = lineLength / 2;//int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(copyRight, lineLength/2 - xExtent / 2 , yPos + border);
	dc->DrawText(" ", lineLength / 2 - xExtent / 2, yPos + border);


	return yPos;
}

size_t pof::Printout::WritePageHeaderSmall(wxPrintout* printout, wxDC* dc, const wxString& text, double mmToLogical)
{
	auto& app = wxGetApp();

	int border = 0;
	int xPos = leftMargin, yPos = topMargin;
	int lineLength = m_coord_system_width;
	int lineHeight = 18;

	//dc->SetFont(mPharmacyNameFont);
	wxString name(wxT("Bits"));
	wxFont font(wxFONTSIZE_SMALL, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, name);

	dc->SetFont(font);


	wxCoord xExtent = 0, yExtent = 0;
	dc->GetTextExtent(text, &xExtent, &yExtent);
	lineHeight = yExtent;

	dc->DrawText(text, lineLength / 2 - xExtent / 2, yPos + border);

	std::string addy = app.MainPharmacy->GetAddressAsString();
	yPos += lineHeight + border;
	dc->GetTextExtent(addy, &xExtent, &yExtent);
	dc->DrawText(addy, lineLength / 2 - xExtent / 2, yPos + border);

	//print contact
	std::string phone = fmt::format("No:{}", app.MainPharmacy->contact.phoneNumber);
	std::string mail = app.MainPharmacy->contact.email;
	if (!phone.empty()) {
		yPos += yExtent + border;
		dc->GetTextExtent(phone, &xExtent, &yExtent);
		dc->DrawText(phone, lineLength / 2 - xExtent / 2, yPos + border);
	}

	if (!mail.empty()) {
		yPos += yExtent + border;
		dc->GetTextExtent(mail, &xExtent, &yExtent);
		dc->DrawText(mail, lineLength / 2 - xExtent / 2, yPos + border);
	}
	//date/time
	yPos += yExtent + border + 5;
	wxRect rect(xPos, yPos, lineLength, lineHeight);
	pof::base::data::datetime_t& date = boost::variant2::get<pof::base::data::datetime_t>(
		(wxGetApp().mSaleManager.GetSaleData()->GetDatastore().begin())->first[pof::SaleManager::SALE_DATE]);

	dc->DrawLabel(fmt::format("{:%Y-%m-%d %H:%M:%S}", date), rect, wxALIGN_LEFT);

	//account name and status
	yPos += yExtent + border;
	rect.SetPosition(wxPoint(xPos, yPos));
	auto& acc = wxGetApp().MainAccount;
	std::string accName = fmt::format("{} {}", acc->lastname, acc->name);
	dc->DrawLabel(accName, rect, wxALIGN_LEFT);

	//sale id
	yPos += yExtent + border;
	pof::base::data::duuid_t& d = boost::variant2::get<pof::base::data::duuid_t>(
		(wxGetApp().mSaleManager.GetSaleData()->GetDatastore().begin())->first[pof::SaleManager::SALE_UUID]);
	std::stringstream os;
	os << d;
	rect.SetPosition(wxPoint(xPos, yPos));
	dc->DrawLabel(fmt::format("id: {}", os.str()), rect, wxALIGN_LEFT);

	yPos += yExtent + border + 10;

	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(xPos, yPos + border, lineLength, yPos + border);
	dc->DrawLine(xPos, yPos + border + 5, lineLength, yPos + border + 5);

	//draw titile
	const std::string title = "Invoice";
	dc->GetTextExtent(title, &xExtent, &yExtent);
	yPos += border + 5;
	dc->DrawText(title, lineLength / 2 - xExtent / 2, yPos);

	yPos += yExtent + border + 2;

	return yPos;
}

size_t pof::Printout::WriteSaleDataSmall(double mToLogical, size_t y)
{
	wxDC* dc = GetDC();
	auto& saleData = wxGetApp().mSaleManager.GetSaleData();

	int border = 5;
	int xPos = 0, yPos = y, xExtent = 0, yExtent = 18;
	int lineLength = m_coord_system_width; //rightMarginLogical - leftMarginLogical;
	dc->GetTextExtent("X", &xExtent, &yExtent);
	
	int lineHeight = yExtent;

	//tite
	wxRect rect(xPos, yPos, lineLength, lineHeight - border);

	dc->DrawLabel("Qty Product", rect, wxALIGN_LEFT);
	dc->DrawLabel("Sub-amount", rect, wxALIGN_RIGHT);

	yPos += lineHeight + 2;
	rect.SetPosition(wxPoint(xPos + border, yPos + border));

	//dc->SetFont(mBodyFont);
	auto& cachefont = dc->GetFont();

	wxString name(wxT("Bits"));
	wxFont font(wxFONTSIZE_SMALL, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false, name);

	dc->SetFont(font);

	pof::base::currency totalAmount;
	for (auto& r : saleData->GetDatastore())
	{
		auto& v = r.first;
		pof::base::data::text_t& name = boost::variant2::get<pof::base::data::text_t>(v[pof::SaleManager::PRODUCT_NAME]);
		pof::base::data::currency_t& amount = boost::variant2::get<pof::base::data::currency_t>(v[pof::SaleManager::PRODUCT_EXT_PRICE]);
		std::uint64_t quantity = boost::variant2::get<std::uint64_t>(v[pof::SaleManager::PRODUCT_QUANTITY]);

		auto quantext = fmt::to_string(quantity);

		auto productText = fmt::format("{:d} {}", quantity, name);
		auto amountText = fmt::format("{:cu}", amount);

		dc->GetTextExtent(amountText, &xExtent, &yExtent);
		int xSize = m_coord_system_width - xExtent - 10;
		dc->SetClippingRegion(wxRect{ xPos, yPos, xSize, lineHeight });
		
		dc->DrawText(productText, { xPos, yPos });

		dc->DestroyClippingRegion();

		dc->DrawText(amountText, xPos + xSize + 10, yPos);

		totalAmount += amount;
		yPos += lineHeight + 2;
		rect.SetPosition(wxPoint(xPos + border, yPos + border));
	}

	dc->SetFont(cachefont);
	//footer
	yPos += lineHeight;
	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(xPos, yPos + border, lineLength, yPos + border);
	dc->DrawLine(xPos, yPos + border + 5, lineLength, yPos + border + 5);
	//dc->SetFont(mInoiceHeaderFont);
	std::string totalA = fmt::format("TOTAL: {:cu}", totalAmount);
	dc->GetTextExtent(totalA, &xExtent, &yExtent);
	yPos += border + 5;
	wxRect TotalRect(xPos, yPos + border, lineLength - border, yExtent);

	dc->DrawLabel(totalA, TotalRect, wxALIGN_RIGHT);

	yPos += yExtent + border;
	//dc->SetFont(mFooterFont);
	dc->SetPen(wxPenInfo(*wxBLACK).Style(wxPENSTYLE_SHORT_DASH));
	dc->DrawLine(xPos, yPos + border, lineLength, yPos + border);
	dc->GetTextExtent(mFooterMessage, &xExtent, &yExtent);
	//xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(mFooterMessage, lineLength / 2 - xExtent / 2, yPos + border + 2);


	//
	yPos += yExtent + border;
	//dc->SetFont(mFooterFont);
	std::string copyRight = "Powered by PharmaOffice";
	dc->GetTextExtent(copyRight, &xExtent, &yExtent);
	xPos = lineLength / 2;//int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
	dc->DrawText(copyRight, lineLength / 2 - xExtent / 2, yPos + border);


	return yPos;
}

bool pof::Printout::DrawSalePrint()
{

	wxDC* dc = GetDC();
	auto& app = wxGetApp();

	
	FitThisSizeToPageMargins(wxSize(m_coord_system_width, m_coord_system_height), m_page_setup);

	wxRect fitRect = GetLogicalPageMarginsRect(m_page_setup);

	wxCoord xoff = (fitRect.width - m_coord_system_width) / 2;
	wxCoord yoff = (fitRect.height - m_coord_system_height) / 2;
	OffsetLogicalOrigin(xoff, yoff);

	dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
	dc->SetBrush(*wxTRANSPARENT_BRUSH);
	//calculate the length of a line
	int y = 0;

	switch (m_paper_type)
	{
	case wxPAPER_NONE:
		y = WritePageHeaderSmall(this, dc, app.MainPharmacy->name, logUnitsFactor);
		WriteSaleDataSmall(logUnitsFactor, y);
		break;
	case wxPAPER_LETTER:
	case wxPAPER_LEGAL:
	case wxPAPER_A4:
		y = WritePageHeader(this, dc, app.MainPharmacy->name, logUnitsFactor);
		WriteSaleData(logUnitsFactor, y);
		break;
	case wxPAPER_CSHEET:
		break;
	case wxPAPER_DSHEET:
		break;
	case wxPAPER_ESHEET:
		break;
	case wxPAPER_LETTERSMALL:
		break;
	case wxPAPER_TABLOID:
		break;
	case wxPAPER_LEDGER:
		break;
	case wxPAPER_STATEMENT:
		break;
	case wxPAPER_EXECUTIVE:
		break;
	case wxPAPER_A3:
		break;
	case wxPAPER_A4SMALL:
	    y = WritePageHeaderSmall(this, dc, app.MainPharmacy->name, logUnitsFactor);
		WriteSaleDataSmall(logUnitsFactor, y);
		break;
	case wxPAPER_A5:
		break;
	case wxPAPER_B4:
		break;
	case wxPAPER_B5:
		break;
	case wxPAPER_FOLIO:
		break;
	case wxPAPER_QUARTO:
		break;
	case wxPAPER_10X14:
		break;
	case wxPAPER_11X17:
		break;
	case wxPAPER_NOTE:
		break;
	case wxPAPER_ENV_9:
		break;
	case wxPAPER_ENV_10:
		break;
	case wxPAPER_ENV_11:
		break;
	case wxPAPER_ENV_12:
		break;
	case wxPAPER_ENV_14:
		break;
	case wxPAPER_ENV_DL:
		break;
	case wxPAPER_ENV_C5:
		break;
	case wxPAPER_ENV_C3:
		break;
	case wxPAPER_ENV_C4:
		break;
	case wxPAPER_ENV_C6:
		break;
	case wxPAPER_ENV_C65:
		break;
	case wxPAPER_ENV_B4:
		break;
	case wxPAPER_ENV_B5:
		break;
	case wxPAPER_ENV_B6:
		break;
	case wxPAPER_ENV_ITALY:
		break;
	case wxPAPER_ENV_MONARCH:
		break;
	case wxPAPER_ENV_PERSONAL:
		break;
	case wxPAPER_FANFOLD_US:
		break;
	case wxPAPER_FANFOLD_STD_GERMAN:
		break;
	case wxPAPER_FANFOLD_LGL_GERMAN:
		break;
	case wxPAPER_ISO_B4:
		break;
	case wxPAPER_JAPANESE_POSTCARD:
		break;
	case wxPAPER_9X11:
		break;
	case wxPAPER_10X11:
		break;
	case wxPAPER_15X11:
		break;
	case wxPAPER_ENV_INVITE:
		break;
	case wxPAPER_LETTER_EXTRA:
		break;
	case wxPAPER_LEGAL_EXTRA:
		break;
	case wxPAPER_TABLOID_EXTRA:
		break;
	case wxPAPER_A4_EXTRA:
		break;
	case wxPAPER_LETTER_TRANSVERSE:
		break;
	case wxPAPER_A4_TRANSVERSE:
		break;
	case wxPAPER_LETTER_EXTRA_TRANSVERSE:
		break;
	case wxPAPER_A_PLUS:
		break;
	case wxPAPER_B_PLUS:
		break;
	case wxPAPER_LETTER_PLUS:
		break;
	case wxPAPER_A4_PLUS:
		break;
	case wxPAPER_A5_TRANSVERSE:
		break;
	case wxPAPER_B5_TRANSVERSE:
		break;
	case wxPAPER_A3_EXTRA:
		break;
	case wxPAPER_A5_EXTRA:
		break;
	case wxPAPER_B5_EXTRA:
		break;
	case wxPAPER_A2:
		break;
	case wxPAPER_A3_TRANSVERSE:
		break;
	case wxPAPER_A3_EXTRA_TRANSVERSE:
		break;
	case wxPAPER_DBL_JAPANESE_POSTCARD:
		break;
	case wxPAPER_A6:
		break;
	case wxPAPER_JENV_KAKU2:
		break;
	case wxPAPER_JENV_KAKU3:
		break;
	case wxPAPER_JENV_CHOU3:
		break;
	case wxPAPER_JENV_CHOU4:
		break;
	case wxPAPER_LETTER_ROTATED:
		break;
	case wxPAPER_A3_ROTATED:
		break;
	case wxPAPER_A4_ROTATED:
		break;
	case wxPAPER_A5_ROTATED:
		break;
	case wxPAPER_B4_JIS_ROTATED:
		break;
	case wxPAPER_B5_JIS_ROTATED:
		break;
	case wxPAPER_JAPANESE_POSTCARD_ROTATED:
		break;
	case wxPAPER_DBL_JAPANESE_POSTCARD_ROTATED:
		break;
	case wxPAPER_A6_ROTATED:
		break;
	case wxPAPER_JENV_KAKU2_ROTATED:
		break;
	case wxPAPER_JENV_KAKU3_ROTATED:
		break;
	case wxPAPER_JENV_CHOU3_ROTATED:
		break;
	case wxPAPER_JENV_CHOU4_ROTATED:
		break;
	case wxPAPER_B6_JIS:
		break;
	case wxPAPER_B6_JIS_ROTATED:
		break;
	case wxPAPER_12X11:
		break;
	case wxPAPER_JENV_YOU4:
		break;
	case wxPAPER_JENV_YOU4_ROTATED:
		break;
	case wxPAPER_P16K:
		break;
	case wxPAPER_P32K:
		break;
	case wxPAPER_P32KBIG:
		break;
	case wxPAPER_PENV_1:
		break;
	case wxPAPER_PENV_2:
		break;
	case wxPAPER_PENV_3:
		break;
	case wxPAPER_PENV_4:
		break;
	case wxPAPER_PENV_5:
		break;
	case wxPAPER_PENV_6:
		break;
	case wxPAPER_PENV_7:
		break;
	case wxPAPER_PENV_8:
		break;
	case wxPAPER_PENV_9:
		break;
	case wxPAPER_PENV_10:
		break;
	case wxPAPER_P16K_ROTATED:
		break;
	case wxPAPER_P32K_ROTATED:
		break;
	case wxPAPER_P32KBIG_ROTATED:
		break;
	case wxPAPER_PENV_1_ROTATED:
		break;
	case wxPAPER_PENV_2_ROTATED:
		break;
	case wxPAPER_PENV_3_ROTATED:
		break;
	case wxPAPER_PENV_4_ROTATED:
		break;
	case wxPAPER_PENV_5_ROTATED:
		break;
	case wxPAPER_PENV_6_ROTATED:
		break;
	case wxPAPER_PENV_7_ROTATED:
		break;
	case wxPAPER_PENV_8_ROTATED:
		break;
	case wxPAPER_PENV_9_ROTATED:
		break;
	case wxPAPER_PENV_10_ROTATED:
		break;
	case wxPAPER_A0:
		break;
	case wxPAPER_A1:
		break;
	default:
		break;
	}

	return true;
}

bool pof::Printout::DrawLabelPrint(int page)
{
	wxDC* dc = GetDC();

	int ppiScreenX = 0, ppiScreenY = 0;
	int ppiPrinterX = 0, ppiPrinterY = 0;
	GetPPIScreen(&ppiScreenX, &ppiScreenY);
	GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

	double scale = double(ppiPrinterX) / ppiScreenX;

	int pageWidth = 0, pageHeight = 0, textX = 0, textY = 0, marginX = 10, marginY = 10;
	int w = 0, h = 0;
	dc->GetSize(&w, &h);
	GetPageSizePixels(&pageWidth, &pageHeight);

	double overallScale = scale * w / pageWidth;
	logUnitsFactor = ppiPrinterX / (scale * 25.4);

	dc->SetUserScale(overallScale, overallScale);

	dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
	dc->SetBrush(*wxTRANSPARENT_BRUSH);

	auto& info = mLabels[(page - 1)];
	int pageWidthMM, pageHeightMM;
	GetPageSizeMM(&pageWidthMM, &pageHeightMM);


	int leftMarginLogical = int(logUnitsFactor * leftMargin);
	int topMarginLogical = int(logUnitsFactor * topMargin);
	int bottomMarginLogical = int(logUnitsFactor * (pageHeightMM - topMargin));
	int rightMarginLogical = int(logUnitsFactor * (pageWidthMM - rightMargin));
	int border = 5;
	int xPos = 0, yPos = topMarginLogical, xExtent = 0, yExtent = 18;
	int lineLength = rightMarginLogical - leftMarginLogical;
	int lineHeight = 18;

	auto date = fmt::format("{:%d-%m-%y}", pof::base::data::clock_t::now());

	wxRect rect(leftMarginLogical, yPos, lineLength, lineHeight);
	dc->SetFont(mInoiceHeaderFont);
	dc->DrawRectangle(wxRect(rect.x, rect.y, rect.width, rect.height));
	dc->DrawLabel(wxGetApp().MainPharmacy->name, rect, wxALIGN_CENTER);
	
	dc->GetTextExtent(date, &xExtent, &yExtent);
	dc->DrawLine(wxPoint((rect.x + rect.width) - xExtent - border , rect.y), wxPoint((rect.x + rect.width) - xExtent - border, rect.y + rect.height));
	dc->DrawLabel(date, rect, wxALIGN_RIGHT);

	wxRect infoBox; 
	infoBox.x = rect.x;
	infoBox.y = rect.y + lineHeight + border;
	infoBox.width = rect.width;
	yPos += lineHeight + 2;
	rect.SetPosition(wxPoint(leftMarginLogical, yPos + border));

	std::transform(info.mFormulation.begin(), info.mFormulation.end(), info.mFormulation.begin(), [&](char c) {
		return std::tolower(c);
	});
	auto st = fmt::format("{}{}", info.mStrength, info.mStrengthType);
	
	dc->DrawLabel(fmt::format("Description: {}, {} {}", info.mProductName, st, info.mFormulation), rect, wxALIGN_CENTER);

	yPos += lineHeight + 2;
	rect.SetPosition(wxPoint(leftMarginLogical, yPos + border));

	dc->DrawLabel(fmt::format("Quantity: {:d}", info.mQuantity), rect, wxALIGN_CENTER);

	yPos += lineHeight + 2;
	rect.SetPosition(wxPoint(leftMarginLogical, yPos + border));

	dc->DrawLabel(fmt::format("Dosage: {}", info.mDirForUse), rect, wxALIGN_CENTER);


	yPos += lineHeight + 10;
	rect.SetPosition(wxPoint(leftMarginLogical, yPos + border));

	dc->DrawLabel(fmt::format("Warning: {}", info.mWarning), rect, wxALIGN_CENTER);

	infoBox.height = yPos + 5;
	dc->DrawRectangle(infoBox);

	return true;
}

void pof::Printout::SetDefaultFonts()
{
	
}

