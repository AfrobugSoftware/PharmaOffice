#include <wx/app.h>


#include <filesystem>
#include <netmanager.h>

#include <nlohmann/json.hpp>
#include "SignInDialog.h"
#include "Account.h"
#include "Pharmacy.h"

namespace js = nlohmann;
namespace fs = std::filesystem;
namespace pof {
	class Application : public wxApp
	{
	public:
		using signoutsig_t = boost::signals2::signal<void(void)>;

		Application();
		virtual ~Application() {}

		virtual bool OnInit() override;
		virtual int OnExit() override;

		bool SignIn();

		const fs::path& GetAssertsPath() const { return mAsserts; }
		
		pof::base::net_manager mNetManager;
	private:
		pof::Pharmacy MainPharamcy;
		pof::Account MainAccount;

		fs::path mAsserts;
		fs::path mModules; //plugin 

	};
};
DECLARE_APP(pof::Application)