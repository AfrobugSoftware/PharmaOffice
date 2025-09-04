#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // !WX_PRECOMP



class ThumbnailEvent : public wxNotifyEvent
{
public:
	ThumbnailEvent(wxEventType commndType = wxEVT_NULL, int winID = 0) :
		wxNotifyEvent(commndType, winID), mItemIndex(-1), mFlags(0) {

	}

	ThumbnailEvent(const ThumbnailEvent& event)
		: wxNotifyEvent(event), mItemIndex(event.mItemIndex), mFlags(event.mFlags) {

	}

	int GetIndex() const { return mItemIndex;  }
	void SetIndex(int n) { mItemIndex = n; }


	int GetFlags() const { return mFlags;  }
	void SetFlags(int flag) { mFlags = flag; }


	virtual wxEvent* Clone() const override { return new ThumbnailEvent(*this); }
protected:
	int mItemIndex;
	int mFlags;

private:
	DECLARE_DYNAMIC_CLASS_NO_ASSIGN(ThumbnailEvent)
};
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_COMMAND_THUMBNAIL_ITEM_SELECTED, 2600)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_THUMBNAIL_ITEM_DESELECTED, 2601)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_THUMBNAIL_LEFT_CLICK, 2602)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_THUMBNAIL_RIGHT_CLICK, 2603)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_THUMBNAIL_MIDDLE_CLICK, 2604)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_THUMBNAIL_LEFT_DCLICK, 2605)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_THUMBNAIL_RETURN, 2606)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::* ThumbnailEventFunction)(ThumbnailEvent&);

#define EVT_THUMBNAIL_ITEM_SELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_THUMBNAIL_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( ThumbnailEventFunction, & fn ), NULL ),
#define EVT_THUMBNAIL_ITEM_DESELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_THUMBNAIL_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( ThumbnailEventFunction, & fn ), NULL ),
#define EVT_THUMBNAIL_LEFT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_THUMBNAIL_LEFT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( ThumbnailEventFunction, & fn ), NULL ),
#define EVT_THUMBNAIL_RIGHT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_THUMBNAIL_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( ThumbnailEventFunction, & fn ), NULL ),
#define EVT_THUMBNAIL_MIDDLE_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_THUMBNAIL_MIDDLE_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( ThumbnailEventFunction, & fn ), NULL ),
#define EVT_THUMBNAIL_LEFT_DCLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_THUMBNAIL_LEFT_DCLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( ThumbnailEventFunction, & fn ), NULL ),
