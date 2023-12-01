#include "controls/ThumbnailCtrl.h"
#include <wx/filename.h>
#include <wx/dcbuffer.h>
#include <wx/image.h>
static int ThumbnailCtrlCmpFunction(ThumbnailItem** item1, ThumbnailItem** item2);


ThumbnailCtrl* ThumbnailCtrl::smCurrentThumbnailCtrl = NULL;

IMPLEMENT_CLASS(ThumbnailCtrl , wxScrolledWindow)

BEGIN_EVENT_TABLE(ThumbnailCtrl, wxScrolledWindow)
EVT_PAINT(ThumbnailCtrl::OnPaint)
EVT_ERASE_BACKGROUND(ThumbnailCtrl::OnEraseBackground)
EVT_LEFT_DOWN(ThumbnailCtrl::OnLeftClick)
EVT_RIGHT_DOWN(ThumbnailCtrl::OnRightClick)
EVT_MIDDLE_DOWN(ThumbnailCtrl::OnMiddleClick)
EVT_LEFT_DCLICK(ThumbnailCtrl::OnLeftDClick)
EVT_CHAR(ThumbnailCtrl::OnChar)
EVT_SIZE(ThumbnailCtrl::OnSize)
EVT_SET_FOCUS(ThumbnailCtrl::OnSetFocus)
EVT_KILL_FOCUS(ThumbnailCtrl::OnKillFocus)

EVT_MENU(wxID_SELECTALL, ThumbnailCtrl::OnSelectAll)
EVT_UPDATE_UI(wxID_SELECTALL, ThumbnailCtrl::OnUpdateSelectAll)
END_EVENT_TABLE()

 
ThumbnailCtrl::ThumbnailCtrl()
{
	Init();
}

ThumbnailCtrl::ThumbnailCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	Init();
	Create(parent, id, pos, size, style);
}

bool ThumbnailCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	if (!wxScrolledWindow::Create(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE)) return false;
	if (!GetFont().Ok())
	{
		SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
	}
	CalculateOverallThumbnailSize();
	SetBackgroundColour(*wxWHITE);
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	mTagBitmap = wxArtProvider::GetBitmap("action_check");
	SetInitialSize(size);
#if USE_BUFFERED_PAINT
	RecreateBuffer(size);
	SetDoubleBuffered(true);
#endif
	return true;
}

void ThumbnailCtrl::Init()
{
	mThumbnailOverallSize = TH_DEFAULT_OVERALLSIZE;
	mThumbnailImageSize = TH_DEFAULT_IMAGE_SIZE;
	mFreezeCount = 0;
	mSpacing = TH_DEFAULT_SPACING;
	mMargin = TH_DEFAULT_MARGIN;
	mFirstSelection = -1;
	mLastSelection = -1;
	mSortMode = TH_SORT_NAME_UP;
	mFocusedThumbnailBkgColour = TH_DEFAULT_FOCUSED_COLOUR_BACKGROUND;
	mUnfocusedThumbnailBkgColour = TH_DEFUALT_UNFOCUSED_COLOUR_BACKGROUND;
	mUnselectedThumbnailBkgColour = TH_DEFAULT_UNSELECTED_COLOUR_BACKGROUND;
	mTypeColour = TH_DEFAULT_TYPE_COLOUR;
	mTagColour = TH_DEFAULT_TAG_COLOUR;
	mFocusedRectColour = TH_DEFAULT_FOCUS_RECT_COLOUR;
	mFocusItem = -1;

}

void ThumbnailCtrl::Freeze()
{
	mFreezeCount++;
}

void ThumbnailCtrl::Thaw()
{
	mFreezeCount--;
	if (mFreezeCount == 0)
	{
		//Adjust viewport and paint
		SetUpSrollBars();
		Refresh();
	}
}

void ThumbnailCtrl::EnsureVisable(int n)
{
	wxRect rect;
	//Get rectt in srcoll units
	GetItemRect(n, rect, false);

	int ppux, ppuy , startx, starty, virtualSizeX , virtualSizey;
	GetScrollPixelsPerUnit(&ppux, &ppuy);
	GetViewStart(&startx, &starty);
	GetVirtualSize(&virtualSizeX, &virtualSizey); //ibn decive unitsS

	startx = 0;
	starty *= ppuy;

	virtualSizeX = 0;
	if (ppuy != 0) virtualSizey /= ppuy;

	wxSize clientSize = GetClientSize();
	if ((rect.y + rect.height) > (clientSize.y + starty))
	{
		int y = rect.y - (clientSize.y - mThumbnailOverallSize.y - mSpacing);
		SetScrollbars(ppux, ppuy, virtualSizeX, virtualSizey, 0, (int)(0.5 + y / ppuy));
	}
	else if (rect.y < starty)
	{
		int y = rect.y;
		SetScrollbars(ppux, ppuy, virtualSizeX, virtualSizey, 0, (int)(0.5 + y / ppuy));
	}

}

//Reload of the thumbnail's image
void ThumbnailCtrl::Reload(int n)
{
	ThumbnailItem* item = GetItem(n);
	if (item)
	{
		item->Load(this, true);
	}

}

//if the items are sorted by filename, use a binary search on filename
//else default to a linear search
int ThumbnailCtrl::FindItemForFilename(const wxString& filename)
{
	wxString searchName = filename;
#ifdef __WXMSW__
	searchName.Replace(wxT("\\"), wxT("/"));
#endif
	if (mSortMode == TH_SORT_NAME_UP )
	{
		size_t  i;
		size_t last = mItems.GetCount(), first = 0;
		for (i = (int)last/ 2; i > 0 && mItems.GetCount();)
		{
			ThumbnailItem& item = mItems[i];
			wxString itemName = item.GetFilename();
#ifdef __WXMSW__
			itemName.Replace(wxT("\\"), wxT("/"));
#endif
			if (itemName > searchName)
			{
				first = i + 1;
				i = (int)((last + first) / 2);
			}

			else if (itemName == searchName)
			{
				return i;
			}

			else if (itemName < searchName)
			{
				last = i - 1;
				i = (int)((last + first) / 2);
			}
		}
		return -1;
	}
	else if (mSortMode == TH_SORT_NAME_DOWN)
	{
			size_t  i{};
			size_t last = mItems.GetCount(), first = 0;
			for (i = (int)last / 2; i >= 0 && mItems.GetCount();)
			{
				ThumbnailItem& item = mItems[i];
				wxString itemName = item.GetFilename();
#ifdef __WXMSW__
				itemName.Replace(wxT("\\"), wxT("/"));
#endif
				if (itemName < searchName)
				{
					first = i + 1;
					i = (int)((last + first) / 2);
				}

				else if (itemName == searchName)
				{
					return i;
				}

				else if (itemName > searchName)
				{
					last = i - 1;
					i = (int)((last + first) / 2);
				}
			}
			return -1;
		
	}
	else
	{
		size_t i;
		for (i = 0; i < mItems.GetCount(); i++)
		{
			ThumbnailItem& item = mItems[i];
			wxString itemName = item.GetFilename();
#ifdef __WXMSW__
			itemName.Replace(wxT("\\"), wxT("/"));
#endif
			if (itemName == searchName) return i;
		}
	}
	return -1;
}

void ThumbnailCtrl::Sort(int sortMode)
{
	mSortMode = sortMode;
	size_t i;
	size_t len = mItems.GetCount();
	for (i = 0; i < len; i++ )
	{
		ThumbnailItem& item = mItems[i];
		int state = 0;
		if (IsSelected(i))
		{
			state |= TH_SELECTED;
		}if (isTagged(i))
		{
			state |= TH_TAGGED;
		}
		item.SetState(state);
	}
	mSelections.Clear();
	mTags.Clear();
	mFirstSelection = -1;
	mLastSelection = -1;
	mFocusItem = -1;

	smCurrentThumbnailCtrl = this;
	mItems.Sort(ThumbnailCtrlCmpFunction);
	smCurrentThumbnailCtrl = NULL;

	Freeze();
	for (int i = 0; i < len; i++)
	{
		ThumbnailItem& item = mItems[i];
		if (item.GetState() & TH_SELECTED) Select(i);
		if (item.GetState() & TH_TAGGED) Tag(i);
	}

	Thaw();
}

bool ThumbnailCtrl::DrawItem(int n, wxDC& dc, const wxRect& rect, int style)
{
	ThumbnailItem* item = GetItem(n);
	if (item)
	{
		return item->Draw(dc, this, rect, style);
	}
	else return false;
}

bool ThumbnailCtrl::DrawItemBackground(int n, wxDC& dc, const wxRect& rect, const wxRect& imageRect, int style)
{
	ThumbnailItem* item = GetItem(n);
	if (item)
	{
		return item->DrawBackground(dc, this, rect, imageRect, style, n);

	}else
		return false;
}

//returns the size before the addition
int ThumbnailCtrl::Append(ThumbnailItem* item)
{
	int sz = (int)GetCount();
	mItems.Add(item);

	//Reset Selection
	mFirstSelection = -1;
	mLastSelection = -1;
	mFocusItem = -1;

	item->Load(this, false);
	if (mFreezeCount == 0)
	{
		SetUpSrollBars();
		Refresh();
	}
	return sz;
}

//Returns the position of insertion
int ThumbnailCtrl::Insert(ThumbnailItem* item, int pos)
{
	mItems.Insert(item, pos);
	mFirstSelection = -1;
	mLastSelection = -1;
	mFocusItem = -1;

	size_t i;
	for (i = 0; i < mSelections.GetCount(); i++)
	{
		if (mSelections[i] >= pos)
			mSelections[i] = mSelections[i] + 1;
	}
	for (i = 0; i < mTags.GetCount(); i++)
	{
		if (mTags[i] >= pos)
		{
			mTags[i] = mTags[i] + 1;
		}
	}

	item->Load(this, false);
	if (mFreezeCount == 0)
	{
		SetUpSrollBars();
		Refresh();
	}

	return pos;
}

void ThumbnailCtrl::Clear()
{
	mFirstSelection = -1;
	mLastSelection = -1;
	mFocusItem = -1;
	mItems.Clear();
	mSelections.Clear();
	mTags.Clear();

	if (mFreezeCount == 0)
	{
		SetUpSrollBars();
		Refresh();
	}
}
static bool StripExtention(wxString& filepath)
{
	wxString result = filepath;
	size_t pos = result.find_last_of('.');
	if (pos == wxString::npos) return false;

	filepath = result.SubString(0, pos);
	return true;

}

static bool SplitPath( const wxString& filenameFull, wxString* filename, wxString* path, wxString* ext)
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


static bool GetIntergerFromFilename(const wxString& name, int& n)
{
	wxString Filename = name;
	StripExtention(Filename);
	wxString strNum;

	size_t len = Filename.Len();
	size_t i;

	//The for loop may run only once, Use a do-while loop  
	for(i = len; i == 0; i--)
	{
		if (wxIsdigit(Filename[i]) != 0)
		{
			strNum = Filename[i] + strNum;
		}
		else break;
	}
	if (strNum.IsEmpty()) return false;
	else
	{
		n = wxAtoi(strNum);
		return true;
	}
}

//The way i could have implemented this
//Define an operator<(ThumbnailItem& item) in the item class,
//Have a static varible that is the sort mode, public so that ThumbnailCtrl can set THe sortMode 
//Therefore the less function in the Sort algorithm STL can go ahead and do the sorting, without hassle

int ThumbnailCtrlCmpFunction(ThumbnailItem** item1, ThumbnailItem** item2)
{
	int SortMode = TH_SORT_NAME_UP;
	if (ThumbnailCtrl::GetThumbnailCtrl())
	{
		SortMode = ThumbnailCtrl::GetThumbnailCtrl()->GetSortMode();
	}
	if (SortMode == TH_SORT_NAME_UP || SortMode == TH_SORT_NAME_DOWN)
	{
		wxString filename1, filename2;
		if (SortMode == TH_SORT_NAME_UP)
		{
			filename1 = wxFileNameFromPath((*item1)->GetFilename());
			filename2 = wxFileNameFromPath((*item2)->GetFilename());
		}
		else
		{
			filename2 = wxFileNameFromPath((*item1)->GetFilename());
			filename1 = wxFileNameFromPath((*item2)->GetFilename());
		}
		return filename1.CmpNoCase(filename2);
	}
	else if (SortMode == TH_SORT_TIMESTAMP_UP || SortMode == TH_SORT_TIMESTAMP_DOWN)
	{
		wxFileName Fname1, Fname2;
		if (SortMode == TH_SORT_TIMESTAMP_UP)
		{
			Fname1 = ((*item1)->GetFilename());
			Fname2 = ((*item2)->GetFilename());
		}
		else
		{
			Fname2 = ((*item1)->GetFilename());
			Fname1 = ((*item2)->GetFilename());
		}
		wxDateTime lastMod1, lastMod2;
		if (Fname1.GetTimes(NULL, &lastMod1, NULL) && Fname2.GetTimes(NULL, &lastMod2, NULL))
		{
			if (lastMod1 < lastMod2)
				return -1;
			else if (lastMod1 > lastMod2) return 1;
			else return 0;
		}
		else
			return 0;
	}
	else if (SortMode == TH_SORT_NUMERICALLY_UP || SortMode == TH_SORT_NUMERICALLY_DOWN)
	{
		int n1, n2;
		bool success1, success2;
		if (SortMode == TH_SORT_NUMERICALLY_UP)
		{
			success1 = GetIntergerFromFilename((*item1)->GetFilename(), n1);
			success2 = GetIntergerFromFilename((*item2)->GetFilename(), n2);
		}
		else
		{
			success1 = GetIntergerFromFilename((*item2)->GetFilename(), n1);
			success2 = GetIntergerFromFilename((*item1)->GetFilename(), n2);
		}
		if (success1 && !success2)
			return -1;
		else if (!success1 && success2)
			return 1;
		else if (!success1 && !success2)
			return 0;

		if (n1 < n2)
			return -1;
		else if (n1 > n2)
			return 1;
		else
			return 0;
	}

	else if (SortMode == TH_SORT_TYPE_UP || SortMode == TH_SORT_TYPE_DOWN)
	{
		wxString filename1, path1, ext1;
		wxString filename2, path2, ext2;
		if (SortMode == TH_SORT_TYPE_UP)
		{
			SplitPath((*item1)->GetFilename(), &filename1, &path1, &ext1);
			SplitPath((*item2)->GetFilename(), &filename2, &path2, &ext2);
		}
		else
		{
			SplitPath((*item2)->GetFilename(), &filename1, &path1, &ext1);
			SplitPath((*item1)->GetFilename(), &filename2, &path2, &ext2);
		}
		return ext1.CmpNoCase(ext2);
	}
	return 0;
}


void ThumbnailCtrl::Delete(int n)
{
	if (mFirstSelection == n) mFirstSelection = -1;
	if (mLastSelection == n) mLastSelection = -1;
	if (mFocusItem = n) mFocusItem = -1;

	if (mSelections.Index(n) != wxNOT_FOUND) mSelections.Remove(n);
	if (mTags.Index(n) != wxNOT_FOUND) mTags.Remove(n);

	mItems.RemoveAt(n);
	size_t i;
	for (i = 0; i < mSelections.GetCount(); i++)
	{
		if (mSelections[i] > n) mSelections[i] = mSelections[i] - 1;
	}

	if (mFreezeCount == 0)
	{
		SetUpSrollBars();
		Refresh();
	}

}

ThumbnailItem* ThumbnailCtrl::GetItem(int n)
{
	wxASSERT(n < GetCount());
	if (n < GetCount()) return &mItems[(size_t)n];
	else return nullptr;
}

bool ThumbnailCtrl::GetItemRect(int n, wxRect& rect, bool Transform)
{
	wxASSERT(n < GetCount());
	if (n < GetCount())
	{
		int row, col;
		if (!GetRowCol(n, GetClientSize(), row, col)) return false;

		//spacing - Rect - spacing
		int x = col * (mThumbnailOverallSize.x + mSpacing) + mSpacing;
		int y = row * (mThumbnailOverallSize.y + mSpacing) + mSpacing;
		//Convert from logical units to device units, The rect position is in device cooridates; pixels
		if (Transform)
		{
			int startX, startY;
			int xppu, yppu;
			GetScrollPixelsPerUnit(&xppu, &yppu);
			GetViewStart(&startX, &startY);
			x = x - startX * xppu;
			y = y - startY * yppu;
		}
		rect.x = x;
		rect.y = y;
		rect.width = mThumbnailOverallSize.x;
		rect.height = mThumbnailOverallSize.y;
		return true;

	}
	return false;
}

bool ThumbnailCtrl::GetItemRectImage(int item, wxRect& rect, bool transform)
{
	wxASSERT(item < GetCount());
	wxRect OutterRect;
	if (!GetItemRect(item, OutterRect, transform))
	{
		return  false;
	}

	rect.width = mThumbnailImageSize.x;
	rect.height = mThumbnailImageSize.y;

	rect.x = OutterRect.x; // +(OutterRect.width - rect.width);
	rect.y = OutterRect.y; // +(OutterRect.height - rect.height);

	return true;

}

bool ThumbnailCtrl::GetRowCol(int item, const wxSize& clientSize, int& row, int& col)
{
	
	wxASSERT(item < GetCount());
	if (item >= GetCount()) return false;
	int perRow = clientSize.x / (mThumbnailOverallSize.x + mSpacing);
	if (perRow < 1) perRow = 1;

	row = item / perRow;
	col = item % perRow;

	return true;
}

int ThumbnailCtrl::GetFocusItem() const
{
	return 0;
}

void ThumbnailCtrl::SetFocusItem(int item)
{
	wxASSERT(item < GetCount());
	if (item < GetCount())
	{
		int oldFocusItem = mFocusItem;
		if (mFreezeCount == 0)
		{
			wxRect rect;
			if (oldFocusItem != -1)
			{
				GetItemRect(oldFocusItem, rect);
				RefreshRect(rect);
			}
			if (mFocusItem != -1)
			{
				GetItemRect(mFocusItem, rect);
				RefreshRect(rect);
			}
		}
	}
}

void ThumbnailCtrl::Select(int n, bool select)
{
	wxASSERT(n < GetCount());
	if (select)
	{
		if (mSelections.Index(n) == wxNOT_FOUND) mSelections.Add(n);
	}
	else
	{
		if (mSelections.Index(n) != wxNOT_FOUND)  mSelections.Remove(n);
	}

	mFirstSelection = n;
	mLastSelection = n;
	int oldFocusItem = mFocusItem;
	mFocusItem = n;

	if (mFreezeCount == 0)
	{
		wxRect rect;
		GetItemRect(n, rect);
		RefreshRect(rect);
		if (oldFocusItem != -1 && oldFocusItem != n)
		{
			GetItemRect(oldFocusItem, rect);
			RefreshRect(rect);
		}
	}

}

void ThumbnailCtrl::SelectRange(int from, int to, bool select)
{
	int first = from;
	int last = to;
	if (first < last)
	{
		first = to;
		last = from;
	}
	wxASSERT(first >= 0 && first < GetCount());
	wxASSERT(last >= 0 && last < GetCount());

	Freeze();
	int i;
	for (i = first; i < last; i++)
	{
		Select(i, select);
	}
	mFocusItem = to;
	Thaw();
}

void ThumbnailCtrl::Tag(int n, bool tag)
{
	wxASSERT(n < GetCount());
	if (tag)
	{
		if (mTags.Index(n) == wxNOT_FOUND) mTags.Add(n);
	}
	else
	{
		if (mTags.Index(n) != wxNOT_FOUND) mTags.Remove(n);
	}
	if (mFreezeCount == 0)
	{
		wxRect rect;
		GetItemRect(n, rect);
		RefreshRect(rect);
	}

}

void ThumbnailCtrl::SelectAll()
{
	Freeze();
	int i;
	for (i = 0; i < GetCount(); i++)
	{
		Select(i, true);
	}
	if (GetCount() > 0)
	{
		mFocusItem = GetCount() - 1;
	}
	else
	{
		mFocusItem = -1;
	}
	Thaw();
}

void ThumbnailCtrl::SelectNone()
{
	Freeze();
	int i;
	for (i = 0; i < GetCount(); i++)
	{
		Select(i, false);
	}
	Thaw();
}


//Get the index of a single selection, if not multiselect
int ThumbnailCtrl::GetSelection() const
{
	if (GetCount() > 0)
		return mSelections[0u];
	else return -1;
}

bool ThumbnailCtrl::IsSelected(int n) const
{
	return (mSelections.Index(n) != wxNOT_FOUND);
}

bool ThumbnailCtrl::isTagged(int n) const
{
	return (mTags.Index(n) != wxNOT_FOUND);
}

void ThumbnailCtrl::ClearSelections()
{
	int count = GetCount();
	mSelections.Clear();
	mFirstSelection = -1;
	mLastSelection = -1;
	mFocusItem = -1; 

	if (count > 0 && mFreezeCount == 0)
	{
		Refresh();
	}

}

void ThumbnailCtrl::ClearTags()
{
	int count = GetCount();
	mTags.Clear();
	if (count > 0 && mFreezeCount == 0)
	{
		Refresh();
	}
}

void ThumbnailCtrl::SetThumbnailImageSize(const wxSize& size)
{
	mThumbnailImageSize = size;
	CalculateOverallThumbnailSize();
	if (GetCount() > 0 && mFreezeCount == 0)
	{
		SetUpSrollBars();
		Refresh();
	}

}

void ThumbnailCtrl::OnSelectAll(wxCommandEvent& event)
{
	SelectAll();
}

void ThumbnailCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
	event.Enable(GetCount() > 0);
}

void ThumbnailCtrl::OnPaint(wxPaintEvent& event)
{
#if USE_BUFFERED_PAINT
	wxBufferedPaintDC dc(this, mBufferedBitmap);
#else 
	wxPaintDC dc(this);
#endif

	PrepareDC(dc);

	//freeze drawing
	if (mFreezeCount > 0) return;
	PaintBackground(dc);

	//nothing to draw
	if (GetCount() == 0) return;

	wxRegion dirtyRegion = GetUpdateRegion();
	bool isFocused = (FindFocus() == this);
	int i, count = GetCount(), style = 0; 
	wxRect rect, untransformedRect, imageRect, untransformedImageRect;
	for(i = 0; i < count; i++)
	{
		GetItemRect(i, rect);

		wxRegionContain c = dirtyRegion.Contains(rect);
		if (c != wxOutRegion)
		{
			GetItemRectImage(i, imageRect);
			style = 0;
			if (IsSelected(i)) style |= TH_SELECTED;
			if (isTagged(i)) style |= TH_TAGGED;
			if (isFocused) style |= TH_FOCUSED;
			if (isFocused && i == mFocusItem) style |= TH_IS_FOCUSED;

			GetItemRect(i, untransformedRect, true);
			GetItemRectImage(i, untransformedImageRect, true);

			DrawItemBackground(i, dc, untransformedRect, untransformedImageRect, style);
			DrawItem(i, dc, untransformedImageRect, style);
		}
	}

}

//implemented to prevent fliker in drawing 
void ThumbnailCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void ThumbnailCtrl::OnLeftClick(wxMouseEvent& event)
{
	SetFocus();
	int n;
	if (HitTest(event.GetPosition(), n))
	{
		int flags = 0;
		if (event.ControlDown()) flags |= TH_CTRL_DOWN;
		if (event.AltDown()) flags |= TH_ALT_DOWN;
		if (event.ShiftDown()) flags |= TH_SHIFT_DOWN;
		EnsureVisable(n);
		DoSelection(n, flags);


		ThumbnailEvent cmdEvent(wxEVT_COMMAND_THUMBNAIL_LEFT_CLICK, GetId());
		cmdEvent.SetEventObject(this);
		cmdEvent.SetIndex(n);
		cmdEvent.SetFlags(flags);
		GetEventHandler()->ProcessEvent(cmdEvent);
	}
	else {
		int x = GetSelection();
		if (x != wxNOT_FOUND) {
			Select(x, false);
		}
	}


}

void ThumbnailCtrl::OnLeftDClick(wxMouseEvent& event)
{
	int n;
	if (HitTest(event.GetPosition(), n))
	{
		int flags = 0;
		if (event.ControlDown()) flags |= TH_CTRL_DOWN;
		if (event.AltDown()) flags |= TH_ALT_DOWN;
		if (event.ShiftDown()) flags |= TH_SHIFT_DOWN;
		

		ThumbnailEvent cmdEvent(wxEVT_COMMAND_THUMBNAIL_LEFT_DCLICK, GetId());
		cmdEvent.SetEventObject(this);
		cmdEvent.SetIndex(n);
		cmdEvent.SetFlags(flags);
		GetEventHandler()->ProcessEvent(cmdEvent);
	}
}

void ThumbnailCtrl::OnRightClick(wxMouseEvent& event)
{
	SetFocus();
	int n;
	if (HitTest(event.GetPosition(), n))
	{
		int flags = 0;
		if (event.ControlDown()) flags |= TH_CTRL_DOWN;
		if (event.AltDown()) flags |= TH_ALT_DOWN;
		if (event.ShiftDown()) flags |= TH_SHIFT_DOWN;

		if (mFocusItem != n) SetFocusItem(n);

		ThumbnailEvent cmdEvent(wxEVT_COMMAND_THUMBNAIL_RIGHT_CLICK, GetId());
		cmdEvent.SetEventObject(this);
		cmdEvent.SetIndex(n);
		cmdEvent.SetFlags(flags);
		GetEventHandler()->ProcessEvent(cmdEvent);
	}
}

void ThumbnailCtrl::OnMiddleClick(wxMouseEvent& event)
{
	int n;
	if (HitTest(event.GetPosition(), n))
	{
		int flags = 0;
		if (event.ControlDown()) flags |= TH_CTRL_DOWN;
		if (event.AltDown()) flags |= TH_ALT_DOWN;
		if (event.ShiftDown()) flags |= TH_SHIFT_DOWN;


		ThumbnailEvent cmdEvent(wxEVT_COMMAND_THUMBNAIL_MIDDLE_CLICK, GetId());
		cmdEvent.SetEventObject(this);
		cmdEvent.SetIndex(n);
		cmdEvent.SetFlags(flags);
		GetEventHandler()->ProcessEvent(cmdEvent);
	}
}

void ThumbnailCtrl::OnChar(wxKeyEvent& event)
{
	int flags = 0;
	if (event.ControlDown())
		flags |= TH_CTRL_DOWN;
	if (event.ShiftDown())
		flags |= TH_SHIFT_DOWN;
	if (event.AltDown())
		flags |= TH_ALT_DOWN;

	if (event.GetKeyCode() == WXK_LEFT ||
		event.GetKeyCode() == WXK_RIGHT ||
		event.GetKeyCode() == WXK_UP ||
		event.GetKeyCode() == WXK_DOWN ||
		event.GetKeyCode() == WXK_HOME ||
		event.GetKeyCode() == WXK_PAGEUP ||
		event.GetKeyCode() == WXK_PAGEDOWN ||
		event.GetKeyCode() == WXK_END)
	{
		Navigate(event.GetKeyCode(), flags);
	}
	else if (event.GetKeyCode() == WXK_RETURN)
	{
		ThumbnailEvent cmdEvent(
			wxEVT_COMMAND_THUMBNAIL_RETURN,
			GetId());
		cmdEvent.SetEventObject(this);
		cmdEvent.SetFlags(flags);
		GetEventHandler()->ProcessEvent(cmdEvent);
	}
	else
		event.Skip();

}

void ThumbnailCtrl::OnSize(wxSizeEvent& event)
{
	SetUpSrollBars();
	RecreateBuffer(); //if double buffered
	event.Skip();

}

void ThumbnailCtrl::OnSetFocus(wxFocusEvent& event)
{
	if (GetCount() > 0) Refresh();


}

void ThumbnailCtrl::OnKillFocus(wxFocusEvent& event)
{
	if (GetCount() > 0) Refresh();
}

void ThumbnailCtrl::SetUpSrollBars()
{
	if (mFreezeCount) return;
	if (GetCount() == 0)
	{
		SetScrollbars(0, 0, 0, 0, 0, 0);
		return;
	}

	int lastItem = wxMax(0, GetCount() - 1);
	int pixelsPerUnit = TH_SCROLL_PIXEL_PER_UNIT;
	wxSize clientSize = GetClientSize();

	int row, col;
	GetRowCol(lastItem, clientSize, row, col);

	//last row plus one extra row is the max height
	int maxHeight = (row + 1) * (mThumbnailOverallSize.y + mSpacing) + mSpacing;
	
	int unitsY = maxHeight / pixelsPerUnit;

	int startx, starty;
	GetViewStart(&startx, &starty);

	int maxPosX = 0; 
	int maxPosY = (wxMax(maxHeight - clientSize.y, 0)) / pixelsPerUnit;

	//move to scroll position if posible
	SetScrollbars(0, pixelsPerUnit, 0, unitsY, wxMin(maxPosX, startx), wxMin(maxPosY, starty));


}

void ThumbnailCtrl::CalculateOverallThumbnailSize()
{
	wxCoord w;

	wxClientDC dc(this);
	dc.SetFont(GetFont());
	dc.GetTextExtent(wxT("X"), &w, &mTextHeight);

	mThumbnailOverallSize.y = mMargin * 4 + mTextHeight * 3 + mThumbnailImageSize.y;
}

void ThumbnailCtrl::DoSelection(int n, int flags)
{
	bool isSelected = IsSelected(n);
	wxArrayInt stateChanged;
	bool multiSelect = (GetWindowStyle() & TH_MULTIPLE_SELECT) != 0;
	if (multiSelect && (flags & TH_CTRL_DOWN) == TH_CTRL_DOWN)
	{
		Select(n ,!isSelected);
		stateChanged.Add(n);
	}
	else if (multiSelect && (flags & TH_SHIFT_DOWN) == TH_SHIFT_DOWN)
	{
		int first = mFirstSelection;
		bool keepFirstSelection = false;
		wxArrayInt oldSelection = mSelections;
		mSelections.Clear();


		if (mFirstSelection != -1 && mFirstSelection < GetCount() && mFirstSelection != n)
		{
			int step = (n < mFirstSelection) ? -1 : 1;
			int i;
			for (i = mFirstSelection; i != n; i += step)
			{
				if (!IsSelected(i))
				{
					mSelections.Add(i);
					stateChanged.Add(i);

					wxRect rect;
					GetItemRect(i, rect);
					RefreshRect(rect);
				}
			}
			keepFirstSelection = true;
		}
		
		size_t i;
		for (i = 0; i < oldSelection.GetCount(); i++)
		{
			if (IsSelected(oldSelection[i]))
			{
				wxRect rect;
				GetItemRect(oldSelection[i], rect);
				RefreshRect(rect);
			}
		}

		Select(n, true);
		if (stateChanged.Index(n) == wxNOT_FOUND)
		{
			stateChanged.Add(n);
		}

		if (keepFirstSelection) mFirstSelection = first;
	}
	else
	{
		size_t i;
		for (i = 0; i < mSelections.GetCount(); i++)
		{
			wxRect rect;
			GetItemRect(mSelections[i], rect);
			RefreshRect(rect);

			stateChanged.Add(i);
		}

		mSelections.Clear();
		Select(n, true);
		if (stateChanged.Index(n) == wxNOT_FOUND)
		{
			stateChanged.Add(n);
		}
	}

	size_t i;
	for (i = 0; i < stateChanged.GetCount(); i++)
	{
		ThumbnailEvent event((mSelections.Index(stateChanged[i]) != wxNOT_FOUND
			? wxEVT_COMMAND_THUMBNAIL_ITEM_SELECTED : wxEVT_COMMAND_THUMBNAIL_ITEM_DESELECTED), GetId());
		event.SetEventObject(this);
		event.SetIndex(stateChanged[i]);
		GetEventHandler()->ProcessEvent(event);
	}

}

bool ThumbnailCtrl::HitTest(const wxPoint& pt, int& n)
{
	wxSize clientSize = GetClientSize();
	int startX, startY;
	int ppux, ppuy;
	GetViewStart(&startX, &startY);
	GetScrollPixelsPerUnit(&ppux, &ppuy);
	int perRow = clientSize.x / (mThumbnailOverallSize.x + mSpacing);
	if (perRow < 1) perRow = 1;
	//convert from pixel coodinate to index into item array
	int colPos = (int)(pt.x / (mThumbnailOverallSize.x + mSpacing));
	int rowPos = (int)(pt.y + startY * ppuy) / (mThumbnailOverallSize.y + mSpacing);

	int ItemN = (rowPos * perRow + colPos);
	if (ItemN >= GetCount())
	{
		return false;
	}

	wxRect rect; 
	GetItemRect(ItemN, rect);
	if (rect.Contains(pt))
	{
		n = ItemN;
		return true;
	}
	return false;
}

bool ThumbnailCtrl::Navigate(int keycode, int flags)
{
	if(GetCount() == 0) return false;

	wxSize clientSize = GetClientSize();
	int perRow = clientSize.x / (mThumbnailOverallSize.x + mSpacing);
	if (perRow < 1) perRow = 1;

	int rowsInview = clientSize.y / (mThumbnailOverallSize.y + mSpacing);
	if (rowsInview < 1) rowsInview = 1;

	int focus = mFocusItem;
	if (focus == -1) focus = mLastSelection;
	if (focus == -1 || focus >= GetCount())
	{
		mLastSelection = 0;
		DoSelection(mLastSelection, flags);
		ScrollIntoView(mLastSelection, keycode);
		return true;
	}
	if (keycode == WXK_RIGHT)
	{
		int next = focus + 1;
		if (next < GetCount())
		{
			DoSelection(next, flags);
			ScrollIntoView(next, keycode);
		}
	}
	else if (keycode == WXK_LEFT)
	{
		int next = focus - 1;
		if (next >= 0)
		{
			DoSelection(next, flags);
			ScrollIntoView(next, keycode);
		}
	}
	else if (keycode == WXK_DOWN)
	{
		int next = focus + perRow;
		if (next < GetCount())
		{
			DoSelection(next, flags);
			ScrollIntoView(next, keycode);
		}
	}

	else if (keycode == WXK_UP)
	{
		int next = focus - perRow;
		if (next < GetCount())
		{
			DoSelection(next, flags);
			ScrollIntoView(next, keycode);
		}
	}

	else if (keycode == WXK_PAGEUP)
	{
		int next = focus - (perRow * rowsInview);
		if (next > 0)
		{
			DoSelection(next, flags);
			ScrollIntoView(next, keycode);
		}
	}

	else if (keycode == WXK_PAGEDOWN)
	{
		int next = focus + (perRow + rowsInview);
		if (next < GetCount())
		{
			DoSelection(next, flags);
			ScrollIntoView(next, keycode);
		}
	}
	else if (keycode == WXK_HOME)
	{
		DoSelection(0, flags);
		ScrollIntoView(0, keycode);
	}
	else if (keycode == WXK_END)
	{
		DoSelection(GetCount() - 1, flags);
		ScrollIntoView(GetCount(), keycode);
	}

	return true;
}

void ThumbnailCtrl::ScrollIntoView(int n, int keyCode)
{
	wxRect rect;
	GetItemRect(n, rect, false);

	int ppux, ppuy;
	GetScrollPixelsPerUnit(&ppux, &ppuy);

	int startX, startY;
	GetViewStart(&startX, &startY);
	startX = 0;
	startY = startY * ppuy;

	int sx, sy;
	GetVirtualSize(&sx, &sy);
	sx = 0;
	if (ppuy != 0) sy = sy / ppuy;

	wxSize clientSize = GetClientSize();
	if (keyCode == WXK_DOWN || keyCode == WXK_RIGHT || keyCode == WXK_END || keyCode == WXK_PAGEDOWN)
	{
		if ((rect.y + rect.height) > (clientSize.y + startY))
		{
			//Scrool so that item it at the bottom of the window
			int y = rect.y - (clientSize.y - mThumbnailOverallSize.y - mSpacing);
			SetScrollbars(ppux, ppuy, sx, sy, 0, (int)(0.5 + y / ppuy));
		}
		else if (rect.y < startY)
		{
			int y = rect.y;
			//Sets the scroll bar to the position of the rect y in scroll units
			SetScrollbars(ppux, ppuy, sx, sy, 0, (int)(0.5 + y / ppuy));
		}
	}
	else if (keyCode == WXK_UP || keyCode == WXK_LEFT || keyCode == WXK_HOME || keyCode == WXK_PAGEUP)
	{
		if (rect.y > startY)
		{
			int y = rect.y;
			SetScrollbars(ppux, ppuy, sx, sy, 0, (int)(0.5 + y / ppuy));
		}
		else if ((rect.y + rect.height) > (clientSize.y + startY))
		{
			//Calcualte the distance of the rect.y from the clinentSize height - the size of the Itemsize and spacing
			//Set the scroll bars  y there
			int y = rect.y - (clientSize.y - mThumbnailOverallSize.y - mSpacing);
			SetScrollbars(ppux, ppuy, sx, sy, 0, (int)(0.5 + y / ppuy));
		}
	}
}

void ThumbnailCtrl::PaintBackground(wxDC& dc)
{
	wxColour backgroundColour = GetBackgroundColour();
	if (!backgroundColour.Ok())
	{
		backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	}
	//CLEAR THE BACKGROUND
	dc.SetBrush(wxBrush(backgroundColour));
	dc.SetPen(*wxTRANSPARENT_PEN);

	wxRect rect(wxPoint(0, 0), GetClientSize());
	rect.width += 4;
	rect.width += 4;

	CalcUnscrolledPosition(rect.x, rect.y, & rect.x, &rect.y);
	dc.DrawRectangle(rect);
}

bool ThumbnailCtrl::RecreateBuffer(const wxSize& size)
{
	wxSize sz = size;
	if (sz == wxDefaultSize) sz = GetClientSize();
	if (sz.x < 1 || sz.y < 1) return false;
#if USE_BUFFERED_PAINT
	if (!mBufferedBitmap.Ok() || mBufferedBitmap.GetWidth() < sz.x ||
		mBufferedBitmap.GetHeight() < sz.y)
		mBufferedBitmap = wxBitmap(sz.x, sz.y);
	return mBufferedBitmap.Ok();
#else
		return false;
#endif
}

wxSize ThumbnailCtrl::DoGetBestSize() const
{
	return wxWindow::DoGetBestSize();
}
