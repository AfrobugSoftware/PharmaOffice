#include "databasemysql.h"

pof::databasemysql::databasemysql(boost::asio::io_context& ios, boost::asio::ssl::context& ssl)
	: m_resolver(boost::asio::make_strand(ios.get_executor())), 
	  m_connection(boost::asio::make_strand(ios.get_executor()), ssl), 
	  m_timer(ios.get_executor()) {
}

boost::asio::awaitable<std::error_code> pof::databasemysql::connect(const std::string& hostname, const std::string& port, const std::string& user, const std::string& pwd)
{
	co_return std::error_code{};
}

bool pof::databasemysql::push(std::shared_ptr<pof::query<databasemysql>> query)
{
	std::unique_lock<std::shared_mutex> lock(m_querymut);
	bool is_running = !m_queryque.empty();
	m_queryque.push_back(query);
	lock.unlock();
	if (!is_running) {
		boost::asio::co_spawn(m_connection.get_executor(), runquery(), boost::asio::detached);
	}
	return false;
}

void pof::databasemysql::setupssl()
{
}

boost::asio::awaitable<void> pof::databasemysql::runquery()
{
	for (;;) {
		std::shared_lock<std::shared_mutex> lock(m_querymut);
		std::shared_ptr<dataquerybase> dq = m_queryque.front();
		lock.unlock();
		std::error_code ec;
		try {

			timer_t timer(co_await boost::asio::this_coro::executor);
			timer.expires_after(1s);

			auto complete = co_await( (*dq)() || timer.async_wait());
			switch (complete.index())
			{
			case 0:
				timer.cancel();
				break;
			case 1:
				//what happens if we timeout ?
				//signal the query on timeout ...
				continue; //
			default:
				break;
			}

			lock.lock();
			m_queryque.pop_front();
			if (m_queryque.empty()) break;
			lock.unlock();

			continue; //run the next query
		}
		catch (boost::system::system_error& err) {
			ec = err.code();
			spdlog::error("{:ec}", ec);
		}
		catch (std::system_error& exp) {
			ec = exp.code();
			spdlog::error("{:ec}", ec);
		}

		std::unique_lock<std::shared_mutex> l(m_querymut);
		m_queryque.pop_front();
		if (m_queryque.empty()) break;
		l.unlock();
	}
}


void pof::databasemysql::disconnect()
{
}
