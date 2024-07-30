#pragma once
#include <wx/aui/aui.h>
#include <wx/aui/tabart.h>
#include <wx/aui/dockart.h>
#include <wx/font.h>

#include <memory>
#include <boost/signals2.hpp>


namespace pof {
	//Monostate class to provide a theme for the Aui managed frames
	//also add dataviewctrl theme here
	class AuiTheme {
	public:
		using Signal_t = boost::signals2::signal<void(void)>;

		static Signal_t sSignal;
		static wxFont sFont;
		static wxFont sCaptionFont;
		static wxColour sFontColour;

		static size_t sCaptionSize;
		static size_t sGripperSize;
		static size_t sSashSize;
		static size_t sPaneBorderSize;
		static size_t sGradientType;

		static wxColour sSashColour;
		static wxColour sBackgroundColour;
		static wxColour sBorderColour;
		static wxColour sActiveCaptionColour;
		static wxColour sActiveGradientColour;
		static wxColour sActiveTextCaptionColour;
		static wxColour sInactiveTextCaptionColour;
		static wxColour sInactiveCaptionColour;
		static wxColour sInactiveGradientColour;



		//theme upates
		static boost::signals2::connection Register(Signal_t::slot_function_type&& sig);
		static void Update(wxAuiDockArt* art);
		static void DarkTheme();
		static void LightTheme();
		static void DefaultTheme();

		static void SaveTheme(const std::string& name);
		static void LoadTheme(const std::string& name);

		AuiTheme() {};


	};
};