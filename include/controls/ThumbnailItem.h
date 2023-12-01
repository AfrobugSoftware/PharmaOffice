#pragma once
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif 

#include <wx/dynarray.h>

class ThumbnailCtrl;

//file is name
class ThumbnailItem : public wxObject
{
	DECLARE_CLASS(ThumbnailItem)
public:
	ThumbnailItem(const wxString& filename = wxEmptyString)
	:mFilename(filename), mState(0){  }

	inline void SetFilename(const wxString& filename) { mFilename = filename;  }
	inline const wxString& GetFilename() const { return mFilename;  }

	inline void SetState(int state) { mState = state; }
	inline int GetState() const { return mState;  }


	virtual bool Draw(wxDC& dc, ThumbnailCtrl* control, const wxRect& rect , int style);
	virtual bool DrawBackground(wxDC& dc, ThumbnailCtrl* control, const wxRect& rect, const wxRect& ImageRect, int style, int index);
	virtual bool Load(ThumbnailCtrl* WXUNUSED(control), bool WXUNUSED(forcedLoad)) { return false;  }
protected:
	wxString mFilename;
	int mState;
};

class ImageThumbnailItem : public ThumbnailItem
{
	DECLARE_CLASS(ImageThumbnailItem)
public:
	ImageThumbnailItem(const wxString& filename = wxEmptyString)
		:ThumbnailItem(filename)
	{
		mCachedBitmap = wxArtProvider::GetBitmap("product");
	}
	virtual bool Load(ThumbnailCtrl* control, bool forcedLoad) override;
	virtual bool Draw(wxDC& dc, ThumbnailCtrl* control, const wxRect& rect, int style) override;
protected:
	wxBitmap mCachedBitmap;
};



WX_DECLARE_OBJARRAY(ThumbnailItem, ThumbnailItemArray);
