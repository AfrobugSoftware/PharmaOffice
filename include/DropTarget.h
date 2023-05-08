#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <wx/dnd.h>
#include "DataObject.h"
#include <boost/signals2.hpp>
namespace pof {
	class DropTarget : public wxDropTarget
	{
	public:
		using TargetSignal = boost::signals2::signal<void(const pof::DataObject&)>;
		DropTarget(pof::DataObject* obj, TargetSignal::slot_type&& slot);
		~DropTarget() {}

		virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def) override
		{
			return OnDragOver(x, y, def);
		}

		virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) override;
		virtual void OnLeave() override;
	private:
		TargetSignal mTargetSignal;
	};
};