#pragma once
#include "net.h"

#include <boost/noncopyable.hpp>

#include <thread>
#include <mutex>


namespace pof
{
	namespace base {
		class net_manager : private boost::noncopyable
		{
		public:
			net_manager();
			net_manager(net_manager&& manage);
			net_manager& operator=(net_manager&& manage);

			bool stop();


		private:
			std::unique_ptr<net::executor_work_guard<boost::asio::io_context::executor_type>> m_workgaurd;
			net::io_context m_io;
			net::ssl::context m_ssl;
			std::thread m_netthread;

		};
	};
};