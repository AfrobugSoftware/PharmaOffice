#include "serialport.h"

#include "SerialPort.h"

pof::base::serialport::serialport(boost::asio::io_context& ios)
	: mPort(boost::asio::make_strand(ios)), mBaudRate(115200), mPollTimer(ios.get_executor()) {
}

std::error_code pof::base::serialport::open(const std::string& name)
{
	boost::system::error_code ec;
	mPort.open(name, ec);
	return ec;
}

void pof::base::serialport::close() {
	boost::system::error_code ec;
	mPort.cancel(ec);
	if (!ec) mPort.close();
}

std::error_code pof::base::serialport::cancel()
{
	boost::system::error_code ec;
	mPort.cancel(ec);
	return ec;
}

void pof::base::serialport::setoptions() {
	mPort.set_option(boost::asio::serial_port_base::baud_rate(mBaudRate));
	mPort.set_option(boost::asio::serial_port_base::character_size(8));
	mPort.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
	mPort.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	mPort.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}


pof::base::serialport::operator bool() {
	return mPort.is_open();
}

/**
* WatchDog keeps a
*/
awaitable<void> pof::base::serialport::watchdog()
{
	auto deadline = std::chrono::steady_clock::now() + 6s;
	std::error_code ec;
	timer_t timer(co_await boost::asio::this_coro::executor);

	timer.expires_at(deadline);
	std::tie(ec) = co_await timer.async_wait();
	if (ec.value() == boost::asio::error::operation_aborted) {
		//spdlog::info("Cancelled");
		co_return; //cancel 
	}
	//timeout ??
	//use exceptions? exceptions are not used for || awaitable operators 
	//spdlog::info("timeout");
}

awaitable<void> pof::base::serialport::write()
{
	//mWriteQue is a critical region
	while (!mWriteQue.empty()) {
		auto&& data = std::move(mWriteQue.front());
		mWriteQue.pop_front();

		std::uint8_t c = '\0';
		size_t count = 0;
		while (count < data.size()) {
			c = data[count];
			auto&& [ec, size] = co_await mPort.async_write_some(boost::asio::buffer(&c, 1));
			if (ec) {
				written(false);
				throw std::system_error(ec);
			}
			count++;
		}

		//write the delimeter
		c = '\0';
		auto&& [ec, size] = co_await mPort.async_write_some(boost::asio::buffer(&c, 1));
		written(true);
	}
}

awaitable<void> pof::base::serialport::read()
{
	/**
	* Read until delim?? read some might not capture all the data
	*/
	static std::string data;
	data.reserve(1024);

	std::uint8_t c;
	for (;;) {
		//read a single characeter
		auto [ec, size] = co_await mPort.async_read_some(boost::asio::buffer(&c, 1));
		if (ec) {
			//spdlog::get("Log")->info("Could not read from socket from socket");
			throw std::system_error(ec);
		}
		switch (c)
		{
		case '\r':
		case '\n':
		case '\0':
		{
			std::unique_lock<std::shared_mutex> lock(mQueMut);
			mReadQue.push_back(data);
			lock.unlock();
			data.clear();
		}
		break;
		default:
			data += c;
			break;
		}
		/*mPollTimer.expires_from_now(mPoolDuration);
		co_await mPollTimer.async_wait(boost::asio::use_awaitable);*/
	}
}


std::optional<std::string> pof::base::serialport::readque()
{
	//using a condition variable??
	std::shared_lock<std::shared_mutex> lock(mQueMut);
	if (mReadQue.empty()) return std::nullopt;
	auto s = std::move(mReadQue.front());
	mReadQue.pop_front();
	return s;
}

void pof::base::serialport::writeque(std::string&& data)
{
	std::unique_lock<std::shared_mutex>(mWriteQueMut);
	const bool isRunning = !mWriteQue.empty();
	mWriteQue.push_back(std::forward<std::string>(data));
	if (!isRunning) {
		boost::asio::co_spawn(mPort.get_executor(), write(), [&](std::exception_ptr ptr) {
			if (ptr) {
				return;
			}
		});
	}
}

awaitable<void> pof::base::serialport::do_read()
{
	boost::asio::streambuf b;

	for (;;) {
		auto [ec, size] = co_await boost::asio::async_read_until(mPort, b, '\0');
		if (ec) {
			//spdlog::error("{:ec}", std::error_code(ec));
			throw std::system_error(ec);
		}
		std::string s;
		s.resize(size);
		b.commit(size);
		auto buf = static_cast<const char*>(b.data().data());
		std::copy(buf, buf + b.data().size(), s.begin());
		b.consume(size);

		std::unique_lock<std::shared_mutex> lock(mQueMut);
		mReadQue.emplace_back(std::move(s));
	}
}





