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

	if (style & TH_SELECTED)
	{
		wxBrush brush(colour);
		wxPen pen(colour);
		dc.SetBrush(brush);
		dc.SetPen(pen);
	}
	else
	{
		wxBrush brush(Unselectedbackground);
		wxPen pen(Unselectedbackground);
		dc.SetBrush(brush);
		dc.SetPen(pen);
	}

	dc.DrawRoundedRectangle(rect, 0.5);
	if (style & TH_TAGGED)
	{
		wxPen  TagPen(control->GetTagColour());
		dc.SetPen(TagPen);

		dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight() + 1, rect.GetBottom());

		dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetTop());
		dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
	}
	else if (style & TH_SELECTED)
	{
		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight() + 1, rect.GetBottom());

		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetTop());
		dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
	}
	else
	{
		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight() + 1, rect.GetBottom());

		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetTop());
		dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
	}

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
		frect.y = rect.y + rect.height - (rect.height - ImageRect.height) / 2 + Margin;
		frect.width = rect.width - 2 * Margin;
		frect.height = (rect.height - ImageRect.height) / 2 - 2 * Margin;

		wxCoord textw, texth;
		dc.GetTextExtent(filename, &textw, &texth);

		dc.SetClippingRegion(frect);
		int x = frect.x + wxMax(0, (frect.width - textw) / 2);
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
	if (style & TH_IS_FOCUSED)
	{
		wxPen dottenPen(control->GetFocusRectColour(), 1, wxDOT);
		dc.SetPen(dottenPen);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		wxRect rect = ImageRect;
		rect.x--;
		rect.y--;
		rect.height += 2;
		rect.width += 2;
		dc.DrawRoundedRectangle(rect, 0.5);
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
 