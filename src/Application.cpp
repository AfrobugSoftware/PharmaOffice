#include "Application.h"
#include <net.h>
#include <errc.h>
#include <string>
#include <spdlog/spdlog.h>
#include <thread>

#include <fstream>
#include <poflz4.h>

IMPLEMENT_APP(pof::Application)

//test
boost::asio::io_context io;
boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23_client};
using namespace std::literals::string_literals;

int test_main(int argc, char** const argv)
{
	spdlog::error("{:ec}", std::make_error_code(pof::base::errc::no_data));
	auto sess = std::make_shared<pof::base::ssl::session<http::string_body>>(io, ctx);
	std::string host = "www.boost.org";
	std::string service = "https";

	auto f = sess->req<http::verb::post>(host, "/"s, service);

	auto w = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(io.get_executor());
	std::thread t{ [&]() {io.run(); } };
	try {
		std::ofstream out("test.html");
		auto data = f.get();
		out << data.body();
		out.flush();
		out.close();

		std::ifstream in("test.html");
		std::ofstream o("testcompress");

		spdlog::info("Compressing...");
		auto start = std::chrono::system_clock::now();
		lz4_stream::ostream stream(o);
		std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(stream));
		auto stop = std::chrono::system_clock::now();
		auto ti = std::chrono::duration<float, std::chrono::milliseconds::period>(stop - start).count();
		spdlog::info("Done compressing into testcompress took {:.2f}ms", ti);
		
	}
	catch (const std::exception& exp) {
		//ignore
		spdlog::critical(exp.what());
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
