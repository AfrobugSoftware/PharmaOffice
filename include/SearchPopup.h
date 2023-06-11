#pragma once
#include <wx/dataview.h>
#include <wx/popupwin.h>
#include "DataModel.h"


namespace pof {
	class SearchPopup : public wxPopupWindow {
	public:
		SearchPopup(wxWindow* parent);

	private:


		wxDataViewCtrl* mTable;
		pof::DataModel* mTableModel;
		DECLARE_EVENT_TABLE()
	};
};
