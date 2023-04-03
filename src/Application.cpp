#include "Application.h"
#include <net.h>
#include <errc.h>
#include <string>
#include <spdlog/spdlog.h>
#include <thread>

#include <fstream>

#include <random>

#include <data.h>
#include <data_tuple.h>
#include <packages.h>

#include "DataModel.h"

#include "datatree.h"

IMPLEMENT_APP(pof::Application)

//test
boost::asio::io_context io;
boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23_client};
using namespace std::literals::string_literals;

namespace ar = boost::archive;

static auto const flags = boost::archive::no_header | boost::archive::no_tracking;


void test_data()
{
	using namespace pof::base;
	pof::base::data data;
	pof::base::data data2;
	pof::base::adapt<int, float, pof::base::data::datetime_t>(data);

	pof::base::packer p(data);
	pof::base::unpacker up(data2);

	std::random_device d{};
	std::uniform_int_distribution<int > int_distro;
	std::uniform_real_distribution<float> float_distro;

	std::mt19937 gen(d());

	auto rd = std::bind(int_distro, gen);
	auto rd2 = std::bind(float_distro, gen);
	data.reserve(1000);
	for (int i = 0; i < 100000; i++) {
		data.insert({ rd(), rd2(), pof::base::data::clock_t::now() });
	}
	
	
	pof::DataModel<int, std::string, std::string> rl;


	auto& [r, s] = data[0];
	std::cout << boost::variant2::get<0>(r[0]) << std::endl;
	
	auto vec = p();
	
	std::cout << "PACK SIZE: " << vec.size() << std::endl;

	up(vec);
	//package classes
	//packager(const pof::base::data& data)
	//unpackager(pof::base::data& data)


	auto& [ro, so] = data2[0];
	std::cout << boost::variant2::get<0>(ro[0]) << std::endl;

}	

void test_tree()
{
	using string_node = pof::base::node<std::string>;
	using string_leaf = pof::base::leaf<std::string>;

	auto np = std::make_shared<string_node>("zino"s);
	auto lp = std::make_shared<string_leaf>("ferife"s);

	np->add_child(lp);

	auto v = np->get_value("zino", 0);

}


int test_main(int argc, char** const argv)
{
	test_data();
	return 0;
}


pof::Application::Application()
{
}

bool pof::Application::OnInit()
{
	//read settings 
	//set up network
	//sign in
	// 
	//set up other things
	// check for updates
	//lunch mainframe
	SetUpColorTable();


	bool ret = false;
	ret = SignIn();
	//test ret
	ret = CreateMainFrame();

	return ret;
}

int pof::Application::OnExit()
{
	mNetManager.stop();
	return 0;
}

void pof::Application::SetUpColorTable()
{
	wxTheColourDatabase->AddColour("Aqua", wxColour(240, 255, 255));
	wxTheColourDatabase->AddColour("Navajo_white", wxColour(255, 222, 173));
	wxTheColourDatabase->AddColour("Tomato", wxColour(255, 99, 71));
	wxTheColourDatabase->AddColour("Papaya whip", wxColour(255, 239, 213));
}

bool pof::Application::CreateMainFrame()
{
	mMainFrame = new pof::MainFrame(nullptr,
		wxID_ANY, wxDefaultPosition, wxSize(700, 500));

	mMainFrame->Center(wxBOTH);
	mMainFrame->Show();
	return true;
}

bool pof::Application::CheckForUpdate()
{
	//check everytime or just after some days
	return false;
}

bool pof::Application::LoadSettings(const fs::path& fp)
{
	if (fp.empty() || !fs::exists(fp)) return false;
	std::fstream inifile(fp, std::ios::in);
	if (!inifile.is_open()) false;
	try {
		boost::property_tree::ini_parser::read_ini(inifile, mSettings);
	}
	catch (const boost::property_tree::ini_parser_error& error) {
		//need to find where to log errors
		//cannot parse ini file,format error
		return false;
	}
	return true;
}

bool pof::Application::SaveSettings(const fs::path& fp)
{
	if (fp.empty() || !fs::exists(fp)) return false;
	std::fstream inifile(fp, std::ios::out);
	if (!inifile.is_open()) false;

	try {
		boost::property_tree::write_ini(inifile, mSettings);
	}
	catch (const boost::property_tree::ini_parser_error& error) {
		return false;
	}
	return true;
}

bool pof::Application::RegisterPharmacy()
{
	return false;
}

bool pof::Application::RegisterAccount()
{
	return false;
}

bool pof::Application::SignIn()
{
	pof::SignInDialog Dialog(nullptr);
	while (1) {
		if (Dialog.ShowModal() == wxID_OK) {
				//handle response from server 
			auto& data = Dialog.GetUserData();
			if (data.empty()) {
				continue;
			}

			js::json jsdata = js::json::parse(data);
				/**
				* PUT IN DOCUMENTATION
				*  {
				*		"id" : 12324255453434256354,
				*		"priv" : 345, #this number is a bitset
				*		"name" : "Zino",
				*		"last_name" : "ferife",
				*		"email" : "ferife_zino@yahoo.co.uk",
				*		"phonenumber" : "",
				*		"regnumber" : "",
				*		"session_uuid" : "effgrgfsfe-afefe-fge33-feefegweeg"
				*
				*  }
				*/
			MainAccount.accountID = jsdata["id"];
			MainAccount.name = jsdata["name"] + " "s + jsdata["last_name"];
			MainAccount.email = jsdata["email"];
			MainAccount.phonenumber = jsdata["phonenumber"];
			MainAccount.regnumber = jsdata["regnumber"];
			MainAccount.priv = Account::privilage_set_t{ jsdata["priv"] };
			MainAccount.signintime = clock_t::now();

			std::stringstream ss;
			ss << jsdata["session_uuid"];
			ss >> MainAccount.sessionID;


			return true;
		}
		else {
			spdlog::info("Sign in was cancelled");
			return false;
		}
	}
}

boost::property_tree::ptree& pof::Application::operator[](const std::string& path)
{
	//path should point to an actual path to a setting
	if (path.empty()) throw std::logic_error("CANNOT HAVE AN EMPTY PATH TO SETTINGS");
	return mSettings.get_child(path);
}
