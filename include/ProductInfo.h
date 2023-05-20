#pragma once

#include <wx/artprov.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/splitter.h>
#include <Data.h>

#include <bitset>
#include <unordered_map>
#include <optional>

#include <boost/signals2/signal.hpp>
#include "ProductManager.h"
#include "InventoryDialog.h"

namespace pof
{
	//aslo use for adding a product to the database
	class ProductInfo : public wxPanel 
	{
		private:
		
		protected:
			wxSplitterWindow* m_splitter1;
			wxPanel* m_panel1;
			wxAuiToolBar* m_auiToolBar1;
			wxAuiToolBarItem* m_tool1;
			wxAuiToolBarItem* mProductNameText;
			wxDataViewCtrl* InventoryView;
			wxDataViewColumn* mInputDate;
			wxDataViewColumn* mBactchNo;
			wxDataViewColumn* mExpiryDate;
			wxDataViewColumn* mStockCount;
			wxDataViewColumn* mManuFactureName;
			wxPanel* m_panel2;
			wxPropertyGridManager* m_propertyGridManager1;
			wxPropertyGridPage* m_propertyGridPage1;
			wxPGProperty* m_propertyGridItem1;
			wxPGProperty* mNameItem;
			wxPGProperty* mGenericNameItem;
			wxPGProperty* mPackageSizeItem;
			wxPGProperty* mProductClass;
			wxPGProperty* mFormulationItem;
			wxPGProperty* mStrengthValueItem;
			wxPGProperty* mStrengthTypeItem;
			wxPGProperty* mMoreProductInfo;
			wxPGProperty* mDirForUse;
			wxPGProperty* mHealthCond;
			wxPGProperty* mProductDescription;
			wxPGProperty* mSideEffects;
			wxPGProperty* mSettings;
			wxPGProperty* mMinStockCount;
			wxPGProperty* mExpDateCount;
			wxPGProperty* mExpDatePeriod;
			wxPGProperty* mSaleSettings;
			wxPGProperty* mUnitPrice;
			double mStubPrice;
			wxPGChoices ProductClassChoices;
			wxPGChoices FormulationChoices;
			wxPGChoices ExpChoices;
			wxPGChoices StrengthChoices;


			DECLARE_EVENT_TABLE();

		public:
			struct PropertyUpdate {
				constexpr const static size_t elems = pof::ProductManager::PRODUCT_MAX;
				std::bitset<elems> mUpdatedElememts;
				pof::base::data::row_t mUpdatedElementsValues;
			};
			
			using back_signal_t = boost::signals2::signal<void(void)>;
			using update_signal_t = boost::signals2::signal<void(const PropertyUpdate&)>;


			enum {
				ID_TOOL_GO_BACK = wxID_HIGHEST + 2000,
				ID_TOOL_ADD_INVENTORY,
				ID_TOOL_REMV_EXPIRE_BATCH,
				ID_PROPERTY_GRID,
			};

			ProductInfo( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 975,707 ), long style = wxTAB_TRAVERSAL ); 
			~ProductInfo();
			
			void StyleProductPropertyManager();
			void Load(const pof::base::data::row_t& row);
			void LoadProductProperty(const pof::base::data::row_t& row);
			boost::signals2::connection AttachBackSlot(back_signal_t::slot_type&& slot);
			boost::signals2::connection AttachPropertyUpdateSlot(update_signal_t::slot_type&& slot);
			void CreateNameToProductElemTable();


			void m_splitter1OnIdle( wxIdleEvent& )
			{
				m_splitter1->SetSashPosition( 700 );
				m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ProductInfo::m_splitter1OnIdle ), NULL, this );
			}
	protected:
		void OnGoBack(wxCommandEvent& evt);
		void OnAddInventory(wxCommandEvent& evt);
		void OnPropertyChanged(wxPropertyGridEvent& evt);

		void RemovePropertyModification();

		back_signal_t mBackSignal;
		update_signal_t mUpdatePropertySignal;
		pof::base::data::row_t mProductData;
		std::optional<PropertyUpdate> mPropertyUpdate;
		std::unordered_map<std::string, size_t> mNameToProductElem;
	};	
}
