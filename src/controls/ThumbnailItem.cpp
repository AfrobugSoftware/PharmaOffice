#include "controls/ThumbnailItem.h"
#include "controls/ThumbnailCtrl.h"
#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(ThumbnailItemArray);
IMPLEMENT_CLASS(ThumbnailItem, wxObject)
IMPLEMENT_CLASS(ImageThumbnailItem , ThumbnailItem)

static bool SplitPath(const wxString& filenameFull, wxString* filename, wxString* path, wxString* ext)
{
	wxString filenameFul = filenameFull;

#ifdef __WXMSW__
	wxString n("/");
	filenameFul.Replace(wxT("\\"), n);
#endif

	size_t pos = filenameFul.find_last_of('/');
	size_t posExt = filenameFul.find_last_of('.');
	if (pos != wxString::npos)
	{	
		*path = filenameFul.SubString(0, pos);
		*filename = filenameFul.SubString(pos + 1, filenameFul.Length());
	}

	if (posExt != wxString::npos)
	{
		*filename = filenameFul;
		*ext = filenameFul.SubString(posExt + 1, filenameFull.Length());
		return true;

	}

	return false;
}

bool ThumbnailItem::Draw(wxDC& WXUNUSED(dc), ThumbnailCtrl* WXUNUSED(control), const wxRect& WXUNUSED(rect), int WXUNUSED(style))
{
	return false;
}

bool ThumbnailItem::DrawBackground(wxDC& dc, ThumbnailCtrl* control, const wxRect& rect, const wxRect& ImageRect, int style, int index)
{
	wxColour Unselectedbackground = control->GetUnselectedThumbnailBkgColour();
	wxColour unfocusedColour = control->GetSelectedThumbnailUnfocusedBkgColour();
	wxColour focusedColour = control->GetSelectedThumbnailFocusedBkgColour();
	wxColour colour;
	if (style & TH_FOCUSED)
		colour = focusedColour;
	else colour = unfocusedColour;

	wxPen pen(*wxBLACK);
	dc.SetPen(pen);
	if (style & TH_SELECTED)
	{
		wxBrush brush(colour);
		dc.SetBrush(brush);
	}
	else
	{
		wxBrush brush(Unselectedbackground);
		dc.SetBrush(brush);
	}

	dc.DrawRoundedRectangle(rect, 5);
	dc.DrawRectangle(ImageRect);
	
	wxString filename = mFilename;
	if (!filename.IsEmpty() && (control->GetWindowStyle() & TH_TEXT_LABEL))
	{
		dc.SetFont(control->GetFont());
		if (style & TH_SELECTED) dc.SetTextForeground(*wxWHITE);
		else dc.SetTextForeground(*wxBLACK);
		dc.SetBackgroundMode(wxTRANSPARENT);

		int Margin = control->GetThumbnailMargin();

		wxRect frect; 
		frect.x = rect.x + Margin;
		frect.y = rect.y + rect.height - (rect.height - ImageRect.height) + Margin;
		frect.width = rect.width - 2 * Margin;
		frect.height = (rect.height - ImageRect.height) / 2 - 2 * Margin;

		wxCoord textw, texth;
		dc.GetTextExtent(filename, &textw, &texth);

		dc.SetClippingRegion(frect);
		int x = frect.x + 5; // +wxMax(0, (frect.width - textw) / 2);
		int y = frect.y;
		dc.DrawText(filename, x, y);
		dc.DestroyClippingRegion();
	}

	if (style & TH_TAGGED)
	{
		const wxBitmap& tag = control->GetTagBitmap();
		if (tag.Ok())
		{
			int x = rect.x + rect.width - tag.GetWidth() - control->GetThumbnailMargin();
			int y = rect.y + control->GetThumbnailMargin();
			dc.DrawBitmap(tag, x, y);
		}
	}
	return true;
}

bool ImageThumbnailItem::Draw(wxDC& dc, ThumbnailCtrl* control, const wxRect& rect , int style)
{
	if (mCachedBitmap.Ok())
	{
		int x = rect.x + (rect.width - mCachedBitmap.GetWidth()) / 2;
		int y = rect.y + (rect.height - mCachedBitmap.GetHeight()) / 2;

#ifdef __WXWINCE__
		wxBitmap tmpBitmap = mCachedBitmap;
		dc.DrawBitmap(tmpBitmap, x, y);
#else
		dc.DrawBitmap(mCachedBitmap, x, y);
#endif
	}
	return true;
}

bool ImageThumbnailItem::Load(ThumbnailCtrl* control, bool forcedLoad)
{

	wxBitmap image = wxArtProvider::GetBitmap("");
	wxBitmap newImage;
	wxSize thumbnailSize = control->GetThumbnailImageSize();
	double scalex = ((double)thumbnailSize.x) / ((double)image.GetWidth());
	double scaley = ((double)thumbnailSize.y) / ((double)image.GetHeight());
	if (scalex < 1.0 || scaley < 1.0)
	{
		double scale = wxMin(scalex, scaley);
		int newWidth = (int)(scale * image.GetWidth());
		int newHeight = (int)(scale * image.GetHeight());
		image.Rescale(newImage, wxSize(newWidth, newHeight));
		image = newImage;
	}
	mCachedBitmap = wxBitmap(image);
	return true;
}
 