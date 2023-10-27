#pragma once

#include <ranges>
#include <algorithm>
#include <numeric>

#include "SearchProduct.h"

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/dataview.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/choice.h>
#include <wx/simplebook.h>
#include <wx/listctrl.h>


namespace pof
{
	class StockCheck : public wxDialog 
	{
		private:
		
		protected:
			wxAuiToolBar* mTools;
			wxPanel* mMainPane;
			wxDataViewCtrl* mStockData;
			wxDataViewColumn* mProductName;
			wxDataViewColumn* mCurrenctStock;
			wxDataViewColumn* mCheckedStock;
			wxDataViewColumn* mExpiredStock;
			wxDataViewColumn* mShortage;
			wxDataViewColumn* mStatus;
			wxDataViewColumn* mDateAdded;
			wxPanel* mSummary;
			wxStaticText* mTotalStockCheckedLabel;
			wxStaticText* mTotalStockCheckedValue;
			wxStaticText* mTotalShortageLabel;
			wxStaticText* mTotalShortageValue;
			wxStaticText* mShortageAmountLabel;
			wxStaticText* mShortageAmountValue;
			wxDatePickerCtrl* mStockCheckMonth;
			wxChoice* mCategorySelect;
			wxListCtrl* mStockSelect;
			wxPanel* mEmptyPanel;
			wxSimplebook* mBook;
			wxAuiToolBarItem* mBackButton;
			wxAuiToolBarItem* mAddButton;
			pof::base::data::datetime_t* mSelectedMonth = nullptr;
			std::optional<std::vector<std::pair<pof::base::data::duuid_t, std::uint64_t>>> mExpiredStockValues;
		public:
			enum {
				PAGE_STOCK_EMPTY,
				PAGE_STOCK_LIST,
				PAGE_STOCK_SELECT,
			};

			enum {
					ID_TOOL = wxID_HIGHEST + 1,
					ID_ADD_PRODUCT,
					ID_STOCK_DATA,
					ID_DATE,
					ID_CATEGORY_SELECT,
					ID_SHOW_SUMMARY,
					ID_STOCK_CHECK,
					ID_BOOK,
					ID_MARK_COMPLETE,
					ID_STOCK_SELECT,
					ID_REMOVE_STOCK,
					ID_STOCK_CONSUMPTION_PATTERN,
					ID_STOCK_MARK_AS_COMPLETE,
					ID_STOCK_MARK_PROD_AS_COMPLETE,
			};

			//status
			enum {
				PENDING,
				DONE
			};

			//stock columns
			enum {
				STOCK_PRODUCT_UUID = 0,
				STOCK_PRODUCT_NAME,
				STOCK_CURRENT_STOCK,
				STOCK_CHECKED_STOCK,
				STOCK_STATUS,
				STOCK_DATE_ADDED,
				STOCK_SHORTAGE,
				STOCK_EXPIRED,
			};

			StockCheck( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Stock Check"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1178,689 ), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
			wxAuiManager m_mgr;
			void CreateToolBar();
			void CreateEmptyStockCheck();
			void CreateStockSelect();
			void LoadStockSelect();
			void SwitchToolBar(int page);
			void AddSpecialCols();
			virtual bool TransferDataFromWindow() override;
			~StockCheck();


		protected:
			void UpdateSummary();
			void OnAddProduct(wxCommandEvent& evt);
			//void OnMonthChange(wxDateEvent& evt);
			//void OnMonthChange(wxDateEvent& evt);
			void OnDate(wxDateEvent& evt);
			void OnDialogInit(wxInitDialogEvent& evt);
			void OnEditingStarted(wxDataViewEvent& evt);
			void OnCategorySelected(wxCommandEvent& evt);
			void OnShowSummary(wxCommandEvent& evt);
			void OnAuiThemeChange();
			void OnStockActivated(wxListEvent& evt);
			void OnStockSelected(wxListEvent& evt);
			void OnBack(wxCommandEvent& evt);
			void OnRemoveStock(wxCommandEvent& evt);
			void OnContextMenu(wxDataViewEvent& evt);
			void OnReset(wxCommandEvent& evt);
			void OnStockSelectRightClick(wxListEvent& evt);
			void OnStockConsumptionPattern(wxCommandEvent& evt);
			void OnToolUpdateUI(wxUpdateUIEvent& evt);
			void OnMarkAsComplete(wxCommandEvent& evt);
			DECLARE_EVENT_TABLE()
		
	};	
}
