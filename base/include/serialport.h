#pragma once
#pragma once
#include "errc.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include < boost/asio/serial_port.hpp>

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/cancellation_signal.hpp>
#include <boost/asio/bind_cancellation_slot.hpp>

#include <boost/signals2/signal.hpp>

#include <chrono>
#include <memory>
#include <vector>
#include <deque>
#include <shared_mutex>
#include <spdlog/spdlog.h>

using namespace std::literals::chrono_literals;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;

using boost::asio::use_awaitable_t;
using default_token = boost::asio::as_tuple_t<use_awaitable_t<>>;
using serial_port = default_token::as_default_on_t<boost::asio::serial_port>;
using timer_t = default_token::as_default_on_t<boost::asio::steady_timer>;


/**
* A base Class representing a connection via a serial port to the terminal
*/
namespace pof
{
	namespace base {
		//base class
		class serialport {
		public:
			constexpr static size_t max_buffer_size = 2048;
			constexpr const static std::string_view seq = "\x05\x0D";
			constexpr const static std::string_view awk = "\x06\x0D";
			constexpr const static std::string_view delim = "\r\n";

			boost::signals2::signal<void(bool)> written;
			serialport(boost::asio::io_context& ios);
			std::error_code open(const std::string& name);
			void close();
			std::error_code cancel();
			void setoptions();

			operator bool();

			awaitable<void> watchdog();
			/**
			* Use strings to the buffer of data,
			*/
			awaitable<void> write();
			awaitable<void> read();

			std::optional<std::string> readque();
			void writeque(std::string&& data);
			constexpr inline void setBaudRate(size_t baud) { mBaudRate = baud; }

			inline serial_port& get_underlying_port() { return mPort; }
			awaitable<void> do_read();
			awaitable<void> do_write();


			boost::asio::cancellation_signal mcancelsignal;
		private:
			//might not be necessery
			std::chrono::steady_clock::duration mPoolDuration = 5s;
			boost::asio::steady_timer mPollTimer;
			//why not a stringstream, then a poll would read whatever is in the buffer
			std::shared_mutex mQueMut;
			std::shared_mutex mWriteQueMut;

			std::deque<std::string> mReadQue;
			std::deque<std::string> mWriteQue;
			serial_port mPort;
			size_t mBaudRate;
		};
	}
};