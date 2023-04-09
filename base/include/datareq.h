#pragma once
#include "net.h"
#include "packages.h"
#include <future>
#include <memory>
#include <type_traits>

namespace pof {
	namespace base {

		//sends and recieves packages
		//post for sending a package and get for recieving a package
		template<boost::beast::http::verb verb, typename req_body
				= std::conditional_t<verb == http::verb::post, http::vector_body<pack_t::value_type>, http::empty_body>>
		class datareq  : public std::enable_shared_from_this<datareq> {
		public:
			using session_t = pof::base::ssl::session< std::conditional_t<verb == http::verb::post,
			http::string_body, http::vector_body<pack_t::value_type>> ,req_body>;
			using future_t = typename session_t::future_t;
			using clock_t = std::chrono::system_clock;

			datareq(const std::string hst, const std::string& tar, const std::string& port):
				m_host(hst),
				m_port(port),
				m_target(tar) {}

			//blocks 
			void operator()() const {
				auto req = std::make_shared<session_t>();
				req_future = std::move(req->req<verb>(host, target, port));
			}
			void operator()(const pack_t& pack) {
				if constexpr (verb != http::verb::post) return;
				auto req = std::make_shared<session_t>();
				req_future = std::move(req->req<verb>(host, target, port, std::move(pack)));
			}

			inline std::future_status wait_for(clock_t::duration dur) {
				return req_future.wait_for(dur);
			}
			const std::string& get_taget() const { return m_target; }
		private:
			std::string m_host;
			std::string m_port;
			std::string m_target;
			future_t req_future;

		};

	};
};
