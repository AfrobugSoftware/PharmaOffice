#include "databasemysql.h"


boost::mysql::datetime pof::base::to_mysql_datetime(const pof::base::data::datetime_t& tt) {
	auto dt = std::chrono::time_point_cast<boost::mysql::datetime::time_point::duration,
		pof::base::data::clock_t, std::chrono::system_clock::duration>(tt);
	return boost::mysql::datetime(dt);
}

boost::mysql::blob pof::base::to_mysql_uuid(const pof::base::data::duuid_t& duuid) {
	return boost::mysql::blob(duuid.begin(), duuid.end());
}


pof::base::databasemysql::databasemysql(boost::asio::io_context& ios, boost::asio::ssl::context& ssl)
	: m_resolver(boost::asio::make_strand(ios.get_executor())), 
	  m_connection(boost::asio::make_strand(ios.get_executor()), ssl), 
	  m_timer(ios.get_executor()) {
}

boost::asio::awaitable<std::error_code> pof::base::databasemysql::connect(std::string hostname, std::string port, std::string user, std::string pwd)
{
	if (hostname.empty() || port.empty() || user.empty() || pwd.empty())
			throw std::system_error(std::make_error_code(pof::base::errc::no_database_hostname));

	auto [ec1, endpoints] = co_await m_resolver.async_resolve(hostname, port, tuple_awaitable);
	if (ec1) 
		throw std::system_error(ec1);
	
	boost::mysql::handshake_params params{ user, pwd };
	auto [ec] = co_await m_connection.async_connect(*endpoints.begin(), params, tuple_awaitable);
	if (ec)
		throw std::system_error(ec);
	co_return std::error_code{};
}

bool pof::base::databasemysql::push(std::shared_ptr<pof::base::query<databasemysql>> query)
{
	std::unique_lock<std::shared_mutex> lock(m_querymut);
	bool is_running = !m_queryque.empty();
	m_queryque.push_back(query);
	lock.unlock();
	if (!is_running) {
		boost::asio::co_spawn(m_connection.get_executor(), runquery(), boost::asio::detached);
	}
	return true;
}

void pof::base::databasemysql::setupssl()
{
}

boost::asio::awaitable<void> pof::base::databasemysql::runquery()
{
	for (;;) {
		if (m_isconnected) {
			std::unique_lock<std::shared_mutex> lock(m_querymut);
			std::shared_ptr<dataquerybase> dq = m_queryque.front();
			lock.unlock();
			std::error_code ec;
			try {
				//execute the query
				co_await(*dq)();
			}
			catch (...) {
				std::rethrow_exception(std::current_exception());
			}

			std::unique_lock<std::shared_mutex> lk(m_querymut);
			m_queryque.pop_front();
			if (m_queryque.empty()) break;
		}
		else {
			//need to suspend the corouine here
			boost::asio::steady_timer timer(co_await boost::asio::this_coro::executor);
			timer.expires_after(std::chrono::seconds(1));

			auto[ec] = co_await timer.async_wait(tuple_awaitable);
		}
	}
}


void pof::base::databasemysql::disconnect()
{
}
