#include "AuiTheme.h"
boost::signals2::signal<void(void)> pof::AuiTheme::sSignal;
wxFont pof::AuiTheme::sFont;
wxFont pof::AuiTheme::sCaptionFont;

size_t pof::AuiTheme::sCaptionSize = 25;
size_t pof::AuiTheme::sGripperSize = 5;
size_t pof::AuiTheme::sSashSize = 5;
size_t pof::AuiTheme::sPaneBorderSize = 0;
size_t pof::AuiTheme::sGradientType = wxAUI_GRADIENT_HORIZONTAL;

wxColour pof::AuiTheme::sSashColour = *wxLIGHT_GREY;
wxColour pof::AuiTheme::sBackgroundColour = *wxWHITE;
wxColour pof::AuiTheme::sBorderColour = *wxBLACK;
wxColour pof::AuiTheme::sActiveCaptionColour = *wxLIGHT_GREY;
wxColour pof::AuiTheme::sActiveGradientColour = *wxWHITE;
wxColour pof::AuiTheme::sActiveTextCaptionColour = *wxBLACK;
wxColour pof::AuiTheme::sInactiveTextCaptionColour = *wxBLACK;
wxColour pof::AuiTheme::sInactiveCaptionColour = *wxLIGHT_GREY;
wxColour pof::AuiTheme::sInactiveGradientColour = *wxWHITE;

void pof::AuiTheme::Update(wxAuiDockArt* art)
{
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, sCaptionSize);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, sGripperSize);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, sSashSize);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, sPaneBorderSize);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, sGradientType);
	


	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, sSashColour);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, sBackgroundColour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, sBorderColour);
	art->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, sActiveCaptionColour);
	art->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, sActiveGradientColour);
	art->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, sActiveTextCaptionColour);
	art->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, sInactiveTextCaptionColour);
	art->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, sInactiveCaptionColour);
	art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, sInactiveGradientColour);
}

void pof::AuiTheme::DarkTheme()
{
}

void pof::AuiTheme::LightTheme()
{
}

void pof::AuiTheme::DefaultTheme()
{
}

boost::signals2::connection pof::AuiTheme::Register(pof::AuiTheme::Signal_t::slot_function_type&& func)
{
	return sSignal.connect(std::forward<pof::AuiTheme::Signal_t::slot_function_type>(func));
}