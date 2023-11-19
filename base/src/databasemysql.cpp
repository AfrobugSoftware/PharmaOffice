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
