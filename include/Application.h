#include <wx/app.h>


#include <filesystem>


namespace pof {
	class Application : public wxApp
	{
	public:
		Application();
		virtual ~Application() {}

		virtual bool OnInit() override;
		virtual int OnExit() override;

	private:

	};
};
DECLARE_APP(pof::Application)