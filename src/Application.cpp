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
	//lunch mainframe
	return true;
}

int pof::Application::OnExit()
{
	return 0;
}

bool pof::Application::SignIn()
{
	pof::SignInDialog Dialog(nullptr);
	if (Dialog.ShowModal() == wxID_OK){
		auto username = Dialog.GetOfficeUserName();
		auto pass = Dialog.GetOfficePassword();

		//do verification how ??
		//send to chws?
		js::json payload;
		payload["Username"] = username;
		payload["Password"] = pass;
		
		http::string_body body;
		body

		auto sess = std::make_shared<pof::base::ssl::session<http::string_body>>(mNetManager.io(), mNetManager.ssl());

		auto fut = sess->req<http::verb::post>("chws.com", "/accounts/signin", "https", );


		//cache the sign in if the keep signed in was checked.

		return true;
	}
	else {
		spdlog::info("Sign in was cancelled");
		return false;
	}



}
