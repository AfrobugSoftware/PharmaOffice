#include "DropTarget.h"

pof::DropTarget::DropTarget(pof::DataObject* obj, TargetSignal::slot_type&& slot)
{
	SetDataObject(obj);
	mTargetSignal.connect(std::forward<TargetSignal::slot_type>(slot));
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
	auto dataobj = static_cast<pof::DataObject*>(GetDataObject());
	if (!dataobj) return wxDragNone;
	mTargetSignal(*dataobj);

}
