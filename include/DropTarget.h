#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <wx/dnd.h>
#include "DataObject.h"
namespace pof {
	class DropTarget : wxDropTarget
	{
	public:
		DropTarget(pof::DataObject* obj);
		~DropTarget() {}

		virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def) override
		{
			return OnDragOver(x, y, def);
		}

		virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult defResult) override;
		virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) override;
		virtual void OnLeave() override;
	private:

	};
};