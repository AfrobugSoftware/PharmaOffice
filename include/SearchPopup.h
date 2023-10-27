#pragma once
#include <wx/dataview.h>
#include <wx/aui/aui.h>
#include <wx/popupwin.h>
#include <wx/sizer.h>
#include "DataModel.h"

#include <boost/signals2/signal.hpp>
#include <vector>
#include <utility>

namespace pof {
	class SearchPopup : public wxPopupTransientWindow {
	public:
		boost::signals2::signal<void(const pof::base::data::row_t&)> sSelectedSignal;
		enum {
			ID_DATA_VIEW = 10,
		};
		SearchPopup(wxWindow* parent, std::shared_ptr<pof::base::data> ptrData,
			const std::vector<std::pair<std::string, size_t>>& colNames = {}, const std::vector<size_t>& colSizes = {});

		void SearchString(size_t col, const std::string& searchString);
		void CaptureFocus();
	private:
		void CreateDataView(const std::vector<std::pair<std::string, size_t>>& colNames, const std::vector<size_t>& colSizes);
		void OnDataItemSelected(wxDataViewEvent& evt);


		wxAuiManager mPopManager;
		wxDataViewCtrl* mTable = nullptr;
		pof::DataModel* mTableModel = nullptr;
		DECLARE_EVENT_TABLE()
	};
};
