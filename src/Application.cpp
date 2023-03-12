#include "Application.h"
#include <net.h>
#include <errc.h>
#include <string>
#include <spdlog/spdlog.h>
#include <thread>
IMPLEMENT_APP(pof::Application)

//test
boost::asio::io_context io;
boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23_client};
using namespace std::literals::string_literals;

int test_main(int argc, char** const argv)
{
	spdlog::error("{:ec}", std::make_error_code(pof::base::errc::no_data));
	auto sess = std::make_shared<pof::base::ssl::session<http::string_body>>(io, ctx);
	std::string host = "www.example.com";
	std::string service = "https";

	auto f = sess->req<http::verb::post>(host, "/"s, service);

	auto w = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(io.get_executor());
	std::thread t{ [&]() {io.run(); } };
	try {
		auto data = f.get();
		std::cout << data << std::endl;
	}
	catch (...) {
		//ignore
	}
	io.stop();
	t.join();
	return 0;
}


pof::Application::Application()
{
}

bool pof::Application::OnInit()
{
	return false;
}

int pof::Application::OnExit()
{
	return 0;
}
