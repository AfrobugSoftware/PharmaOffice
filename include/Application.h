#include <wx/app.h>


#include <filesystem>
#include <netmanager.h>

#include "Account.h"
#include "Pharmacy.h"

namespace pof {
	class Application : public wxApp
	{
	public:
		using signoutsig_t = boost::signals2::signal<void(void)>;

		Application();
		virtual ~Application() {}

		virtual bool OnInit() override;
		virtual int OnExit() override;

		void SignIn();

		pof::base::net_manager mNetManager;
		signoutsig_t mSignOutSig; //send signal to all modules about a sign out
	private:
		pof::Pharmacy MainPharamcy;
		pof::Account MainAccount;


	};
};
DECLARE_APP(pof::Application)