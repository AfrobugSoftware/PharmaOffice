#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // !WX_PRECOMP

#include "ThumbnailItem.h"
#include "ThumbnailEvent.h"
//#include "./Images/tick.xpm"
 
#define USE_BUFFERED_PAINT 1

//THESE COULD HAVE BEEN BETTER WITH ENUMS
//styles
#define TH_MULTIPLE_SELECT 0x0010
#define TH_SINGLE_SELECT 0x0000
#define TH_TEXT_LABEL 0x0020
#define TH_IMAGE_LABEL 0X0040
#define TH_EXTENSION_LABEL 0x0080

//flags
#define TH_SHIFT_DOWN 0X01
#define TH_CTRL_DOWN 0x02
#define TH_ALT_DOWN 0x04

#define TH_SORT_NAME_UP 1
#define TH_SORT_NAME_DOWN 2
#define TH_SORT_TIMESTAMP_UP 3
#define TH_SORT_TIMESTAMP_DOWN 4
#define TH_SORT_NUMERICALLY_UP 5
#define TH_SORT_NUMERICALLY_DOWN 6
#define TH_SORT_TYPE_UP 7
#define TH_SORT_TYPE_DOWN 8


//defaults
#define TH_DEFAULT_OVERALLSIZE wxSize(-1,-1)
#define TH_DEFAULT_IMAGE_SIZE wxSize(150,150);
#define TH_DEFAULT_SPACING 10
#define TH_DEFAULT_MARGIN 5
#define TH_DEFUALT_UNFOCUSED_COLOUR_BACKGROUND wxColour(255,255,255)
#define TH_DEFAULT_FOCUSED_COLOUR_BACKGROUND wxColour(102,225,255)
#define TH_DEFAULT_UNSELECTED_COLOUR_BACKGROUND wxColour(255,255,255)
//#define TH_DEFAULT_UNSELECTED_COLOUR_BACKGROUND wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)
#define TH_DEFAULT_TYPE_COLOUR wxColour(0,0,200)
#define TH_DEFAULT_TAG_COLOUR wxColour(0,0,255)
#define TH_DEFAULT_FOCUS_RECT_COLOUR wxColour(100,80,80)

//drawing styles / state
#define TH_SELECTED 0x01
#define TH_TAGGED 0x02
#define TH_FOCUSED 0x04
#define TH_IS_FOCUSED 0x08

//magics
#define TH_SCROLL_PIXEL_PER_UNIT 10

class ThumbnailCtrl : public wxScrolledWindow
{
public:
	ThumbnailCtrl();
	ThumbnailCtrl(wxWindow* parent, wxWindowID id = -1
			,const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize
			, long style = TH_TEXT_LABEL | TH_IMAGE_LABEL | TH_EXTENSION_LABEL);

	bool Create(wxWindow* parent, wxWindowID id = -1
		, const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize
		, long style = TH_TEXT_LABEL | TH_IMAGE_LABEL | TH_EXTENSION_LABEL);
	
	void Init();

	//to prevent refresh
	void Freeze();

	//call thaw to refresh
	void Thaw();

	//Scrolls into view if necessary
	void EnsureVisable(int n);

	//Finds reloads of this items thumbnail image
	void Reload(int n);

	int FindItemForFilename(const wxString& filename);

	void Sort(int sortMode);

	//Draw item, 
	virtual bool DrawItem(int n, wxDC& dc, const wxRect& rect, int style);

	virtual bool DrawItemBackground(int n, wxDC& dc, const wxRect& rect, const wxRect& imageRect, int style);

	//append
	virtual int Append(ThumbnailItem* item);
	virtual int Insert(ThumbnailItem* item, int pos = 0);

	virtual void Clear();
	virtual void Delete(int n);

	virtual int GetCount() const { return mItems.GetCount();  }
	inline bool IsEmpty() const { return mItems.GetCount() == 0;  }
	ThumbnailItem* GetItem(int n);

	//Get overall rect of the given item, if transform is true, rect is relative to the sroll viewport
	bool GetItemRect(int Item, wxRect& rect, bool Transform = true);

	//Get the image Rect
	bool GetItemRectImage(int item, wxRect& rect, bool transform = true);

	bool GetRowCol(int item, const wxSize& clientSize, int& row, int& col);

	int GetFocusItem() const;

	void SetFocusItem(int Item);

	void Select(int n, bool select = true);
	
	void SelectRange(int from, int to, bool select = true);

	void Tag(int n, bool tag = true);

	void SelectAll();

	void SelectNone();
	
	int GetSelection() const;

	inline const wxArrayInt& GetSelections() const { return mSelections; }

	inline const wxArrayInt& GetTags() const { return mTags; }

	bool IsSelected(int n) const;

	bool isTagged(int n) const;

	void ClearSelections();

	void ClearTags();


	//visual properties
		///The overal size of thumbnail, including decoration
		///normally be calculated from the setthumbnailImageSize
	//inline void SetThumbnailOverallSize(const wxSize& size) { mThumbnailOverallSize = size; }
	inline const wxSize& GetThumbnailOverallSize() const { return mThumbnailOverallSize;  }

	void SetThumbnailImageSize(const wxSize& size);
	inline const wxSize& GetThumbnailImageSize() const { return mThumbnailImageSize;  }

	inline void SetThumbnailSpacing(int spacing) { mSpacing = spacing;  }
	inline int GetThumbnailSpacing() const { return mSpacing;  }

	inline void SetThumbnailMargin(int margin) { mMargin = margin;  }
	inline int GetThumbnailMargin() const { return mMargin;  }

	inline void SetThumbnailTextHeight(int height) { mTextHeight = height; }
	inline int GetThumbnailTextHeight() const { return mTextHeight;  }

	void SetSelectedThumbnailBackgroundColour(const wxColour& focussedColour, const wxColour& unfocusedColour)
	{
		mFocusedThumbnailBkgColour = focussedColour;
		mUnfocusedThumbnailBkgColour = unfocusedColour;
	}


	inline const wxColour& GetSelectedThumbnailFocusedBkgColour() const { return mFocusedThumbnailBkgColour; }
	inline const wxColour& GetSelectedThumbnailUnfocusedBkgColour() const { return mUnfocusedThumbnailBkgColour;}

	inline void SetUnselectedThumbnailBkgColour(const wxColour& colour) {
		mUnselectedThumbnailBkgColour = colour;
	}

	inline const wxColour& GetUnselectedThumbnailBkgColour() const { return mUnselectedThumbnailBkgColour; }

	inline void SetTypeColour(const wxColour& colour)
	{
		mTypeColour = colour;
	}

	inline const wxColour& GetTypeColour() const { return mTypeColour; }

	inline void SetTagColour(const wxColour& colour) { mTagColour = colour;  }
	inline const wxColour& GetTagColour() const { return mTagColour;  }
	
	inline void SetFocusedRectColour(const wxColour& colour) {
		mFocusedRectColour = colour;
	}

	inline const wxColour GetFocusRectColour() const { return mFocusedRectColour;  }

	///command handlers
	void OnSelectAll(wxCommandEvent& event);
	void OnUpdateSelectAll(wxUpdateUIEvent& event);


	//control evnt handlers
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	
	void OnLeftClick(wxMouseEvent& event);
	void OnLeftDClick(wxMouseEvent& event);
	void OnRightClick(wxMouseEvent& event);
	void OnMiddleClick(wxMouseEvent& event);

	void OnChar(wxKeyEvent& event);
	void OnSize(wxSizeEvent& event);

	///setting and losing focus
	void OnSetFocus(wxFocusEvent& event);
	void OnKillFocus(wxFocusEvent& event);
//Implementation
	///Set up scroll bars after resize for example
	void SetUpSrollBars();

	void CalculateOverallThumbnailSize(); 

	//Do (de) selectiom ? 
	void DoSelection(int n, int flags);

	bool HitTest(const wxPoint& pt, int& n);

	//keyboard navigation
	virtual bool Navigate(int keycode, int flags);

	void ScrollIntoView(int n, int Keycode);

	void PaintBackground(wxDC& dc);

	bool RecreateBuffer(const wxSize& size = wxDefaultSize);

	const wxBitmap& GetTagBitmap() const { return mTagBitmap;  }

	inline void SetSortMode(int sortmode) { mSortMode = sortmode;  }

	inline int GetSortMode() const { return mSortMode; }


	static ThumbnailCtrl* GetThumbnailCtrl() { return smCurrentThumbnailCtrl;  }


	wxSize DoGetBestSize() const;
private:
	ThumbnailItemArray mItems;

	wxSize mThumbnailOverallSize;
	wxSize mThumbnailImageSize;
	int mSpacing;
	int mMargin;
	int mTextHeight;
	int mSortMode;
	int mFirstSelection;
	int mLastSelection;
	int mFocusItem;
	int mFreezeCount;

	wxColour mUnfocusedThumbnailBkgColour;
	wxColour mFocusedThumbnailBkgColour;
	wxColour mUnselectedThumbnailBkgColour;
	wxColour mTypeColour;
	wxColour mTagColour;
	wxColour mFocusedRectColour;
	wxBitmap mTagBitmap;


	wxArrayInt mSelections;
	wxArrayInt mTags;

	static ThumbnailCtrl* smCurrentThumbnailCtrl;
#if USE_BUFFERED_PAINT
	wxBitmap mBufferedBitmap;
#endif


	DECLARE_CLASS(ThumbnailCtrl)
	DECLARE_EVENT_TABLE()
};

