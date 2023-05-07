#include "DropTarget.h"

pof::DropTarget::DropTarget(pof::DataObject* obj)
{
	SetDataObject(obj);
}

void pof::DropTarget::OnLeave()
{
}

wxDragResult pof::DropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
	if (!GetData()) {
		spdlog::error("Cannot get data on drop");
		return wxDragNone;
	}

}

wxDragResult pof::DropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult defResult) {
	
}