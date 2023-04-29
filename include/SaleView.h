#ifndef __NONAME_H__
#define __NONAME_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/button.h>


namespace pof
{
	class SaleView : public wxPanel 
	{
		private:
		
		protected:
			wxPanel* mMainPane;
			wxPanel* mTopTools;
			wxStaticText* mProductNameText;
			wxTextCtrl* mProductNameValue;
			wxStaticText* mScanProduct;
			wxTextCtrl* mScanProductValue;
			wxPanel* mDataPane;
			wxDataViewCtrl* m_dataViewCtrl1;
			wxDataViewColumn* mSerialNumber;
			wxDataViewColumn* mProductNameCol;
			wxDataViewColumn* mQuantityColumn;
			wxDataViewColumn* mPriceCol;
			wxPanel* mSaleOutputPane;
			wxPanel* mSaleDisplayPane;
			wxPanel* mTextOutPut;
			wxStaticText* mQuantity;
			wxStaticText* mQuantityValue;
			wxStaticText* mExtQuantity;
			wxStaticText* mExtQuantityItem;
			wxStaticText* mDiscountAmount;
			wxStaticText* mDiscountValue;
			wxStaticText* mTotalQuantity;
			wxStaticText* mTotalQuantityValue;
			wxPanel* mSalePaymentButtonsPane;
			wxButton* mClear;
			wxButton* mSave;
			wxButton* mCheckout;
		
		public:
			
			SaleView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 917,668 ), long style = wxTAB_TRAVERSAL ); 
			~SaleView();
		
	};
	
} // namespace pof

#endif //__NONAME_H__
