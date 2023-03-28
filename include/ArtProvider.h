#pragma once
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <map>

#include <wx/artprov.h>

#include "errc.h"

namespace fs = std::filesystem;
namespace pof {
	class ArtProvider : public wxArtProvider
	{
	public:
		ArtProvider();

	protected:
		void CreateArtStore();
		virtual wxSize DoGetSizeHint(const wxArtClient& client)  override;
		virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& clinet, const wxSize& size) override;
	private:
		std::unordered_map<std::string, wxBitmap> mArtMap;
	};
};