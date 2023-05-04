#include "ProductView.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::ProductView, wxPanel)
	EVT_SIZE(pof::ProductView::OnResize)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnProductActivated)
END_EVENT_TABLE()



// Using the construction of a static object to ensure that the help provider is set
class wxFBContextSensitiveHelpSetter
{
public:
wxFBContextSensitiveHelpSetter()
{
wxHelpProvider::Set( new wxSimpleHelpProvider );
}
};

static wxFBContextSensitiveHelpSetter s_wxFBSetTheHelpProvider;

pof::ProductView::ProductView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(AUIMGRSTYLE);
	SetBackgroundColour(*wxWHITE); //move to theme
	CreateToolBar();
	CreateDataView();
	CreateProductInfo();
	SetupAuiTheme();
	m_mgr.Update();
}

pof::ProductView::~ProductView()
{
	m_mgr.UnInit();
	
}

void pof::ProductView::ReSizeColumns()
{
	auto width = GetSize().GetWidth();
	//15, 55, 10, 10, 10 
	float fact = 1.0f / 100.0f;
	
	int colWidth = static_cast<int>(width * (15.0f * fact));
	colWidth = colWidth > mSerialNumCol->GetWidth() ? colWidth : mSerialNumCol->GetWidth();
	mSerialNumCol->SetWidth(colWidth);


	colWidth = static_cast<int>(width * (55.0f * fact));
	colWidth = colWidth > mProductNameCol->GetWidth() ? colWidth : mProductNameCol->GetWidth();
	mProductNameCol->SetWidth(colWidth);

	colWidth = static_cast<int>(width * (10.0f * fact));
	colWidth = colWidth > mProductClass ->GetWidth() ? colWidth : mProductClass->GetWidth();
	mProductClass->SetWidth(colWidth);

	colWidth = static_cast<int>(width * (10.0f * fact));
	colWidth = colWidth > mProductUnitPriceCol->GetWidth() ? colWidth : mProductUnitPriceCol->GetWidth();
	mProductUnitPriceCol->SetWidth(colWidth);

	colWidth = static_cast<int>(width * (10.0f * fact));
	colWidth = colWidth > mStockLevel->GetWidth() ? colWidth : mStockLevel->GetWidth();
	mProductUnitPriceCol->SetWidth(colWidth);

}

void pof::ProductView::SaveColumnWidths()
{
}

void pof::ProductView::SetupAuiTheme()
{
	auto auiArtProvider = m_mgr.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::ProductView::OnAuiThemeChange, this));
}

void pof::ProductView::OnResize(wxSizeEvent& evt)
{
	ReSizeColumns();
	evt.Skip();
}

void pof::ProductView::OnAuiThemeChange()
{
	auto auiArtProvider = m_mgr.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::ProductView::OnProductActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	
	auto Model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	if (Model) {
		auto& datastore = Model->GetDatastore();
		const size_t idx = pof::DataModel::GetIdxFromItem(item);
		const pof::base::data::row_t& row = datastore[idx];
		mProductinfo->Load(row);
		SwapCenterPane(true);
	}
}

void pof::ProductView::CreateDataView()
{

	m_dataViewCtrl1 = new wxDataViewCtrl(this, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES);
	auto& pm = wxGetApp().mProductManager;
	m_dataViewCtrl1->AssociateModel(pm.GetProductData().get());
	pm.GetProductData()->DecRef();
	
	mSerialNumCol = m_dataViewCtrl1->AppendTextColumn(wxT("Serial #"), pof::ProductManager::PRODUCT_SERIAL_NUM, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductClass = m_dataViewCtrl1->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductUnitPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("Stock Count"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mStockLevel = m_dataViewCtrl1->AppendTextColumn(wxT("Unit Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	m_dataViewCtrl1->AppendTextColumn(wxEmptyString, 100 , wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE); // null column

	m_mgr.AddPane(m_dataViewCtrl1, wxAuiPaneInfo().Name("DataView").CenterPane().CaptionVisible(false));
	
}

void pof::ProductView::CreateToolBar()
{
	m_auiToolBar1 = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	m_auiToolBar1->SetToolBitmapSize(wxSize(16,16));

	m_searchCtrl1 = new wxSearchCtrl(m_auiToolBar1, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(500, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	m_searchCtrl1->ShowSearchButton(true);
#endif
	m_searchCtrl1->ShowCancelButton(true);
	m_searchCtrl1->SetDescriptiveText("Search products by name");
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_CATEGORY, "Search by category");
	menu->Check(ID_SEARCH_BY_NAME, true);

	m_searchCtrl1->SetMenu(menu);
	m_auiToolBar1->AddControl(m_searchCtrl1);

	m_auiToolBar1->AddStretchSpacer();
	m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddTool(ID_ADD_PRODUCT, "Add product", wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR), "Add a new Product");


	m_auiToolBar1->Realize();

	m_mgr.AddPane(m_auiToolBar1, wxAuiPaneInfo().Name("ProductToolBar").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Resizable().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));

}

void pof::ProductView::CreateProductInfo()
{
	mProductinfo = new pof::ProductInfo(this, ID_PRODUCTINFO);
	mProductinfo->AttachBackSlot([&]() { SwapCenterPane(false); });

	m_mgr.AddPane(mProductinfo, wxAuiPaneInfo().Name("ProductInfo").CenterPane().Hide());

}

void pof::ProductView::SwapCenterPane(bool IsInventoryView)
{
	auto& Pane = m_mgr.GetPane("ProductInfo");
	auto& ProductPane = m_mgr.GetPane("DataView");
	auto& ProductToolBarPane = m_mgr.GetPane("ProductToolBar");
	if (!Pane.IsOk() || !ProductPane.IsOk() || !ProductToolBarPane.IsOk()) return;
	Pane.Show(IsInventoryView);
	ProductPane.Show(!IsInventoryView);
	ProductToolBarPane.Show(!IsInventoryView);
	m_mgr.Update();

}
