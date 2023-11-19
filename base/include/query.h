#pragma once

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <boost/signals2.hpp>
#include <boost/mysql.hpp>
#include <boost/noncopyable.hpp>

#include <spdlog/spdlog.h>
#include <boost/asio/experimental/awaitable_operators.hpp>

#include "errc.h"

//represents data and query read or written to the database
namespace pof {
	template<typename Manager>
	struct query : public std::enable_shared_from_this<query<Manager>> {
		using shared_t = std::enable_shared_from_this<query<Manager>>;
		static constexpr auto tuple_awaitable = boost::asio::as_tuple(boost::asio::use_awaitable);

		boost::signals2::signal<void(std::error_code, std::shared_ptr<query>)> m_sig;
		std::string m_sql;
		std::vector<boost::mysql::metadata> m_metadata;
		std::vector<boost::mysql::row> m_data;
		boost::mysql::diagnostics m_diag; //server diagonis
		Manager& m_manager; //must be the database manager

		query(Manager& man) : m_manager(man) {}

		//Text query
		virtual boost::asio::awaitable<void> operator()() {
			boost::mysql::results result;
			auto [ec] = co_await m_manager.connection().async_query(m_sql, result, tuple_awaitable);
			if (ec) {
				spdlog::error("{:ec}", std::error_code(ec));
				m_sig(ec, shared_t::shared_from_this());
				co_return;
			}
			if (!result.has_value()) {
				//query did not return a value
				spdlog::info("{:ec}, {}", std::error_code(ec), result.info());
				m_sig(ec, shared_t::shared_from_this());
			}
			else {
				const auto& meta = result.meta();
				m_metadata.reserve(meta.size());
				for (const auto& m : meta) {
					m_metadata.emplace_back(std::move(m));
				}
				const auto& rows = result.rows();
				m_data.reserve(rows.size());
				for (const auto& row : rows) {
					m_data.emplace_back(std::move(row));
				}
				m_sig(ec, shared_t::shared_from_this()); //data moved into the datamodels cache
			}
		}

	};

	//Statement queries may require arguments
	template<typename Manager, typename... Args>
	struct querystmt : public query<Manager> {
		using base_t = query<Manager>;
		std::tuple<Args...> m_arguments;

		querystmt(Manager& manager) : DataQuery<Manager>(manager) {}

		virtual boost::asio::awaitable<void> operator()() override {
			boost::mysql::statement stmt;
			std::error_code ec;
			auto& conn = base_t::m_manager.connection();

			std::tie(ec) = co_await conn.async_prepare_statement(base_t::m_sql, stmt, base_t::m_diag, base_t::tuple_awaitable);
			if (ec) {
				base_t::mSig(ec, base_t::shared_t::shared_from_this());
				co_return;
			}
			boost::mysql::results result;
			std::tie(ec) = co_await conn.async_execute(m_arguments, result, base_t::mDiag, base_t::tuple_awaitable);
			if (ec) {
				base_t::mSig(ec, base_t::shared_t::shared_from_this());
				co_return;
			}
			if (!result.has_value()) {
				//query did not return a value
				spdlog::info("{:ec}, {}", ec, result.info());
				base_t::m_sig(ec, base_t::shared_t::shared_from_this());
			}
			else {
				spdlog::info("Reading Query meta data");
				const auto& meta = result.meta();

				base_t::m_metadata.reserve(meta.size());
				for (const auto& m : meta) {
					base_t::m_metadata.emplace_back(std::move(m));
				}
				spdlog::info("Reading Query row data");
				const auto& rows = result.rows();
				for (const auto& row : rows) {
					base_t::m_data.push_back(row);
				}
				base_t::m_sig(ec, base_t::shared_t::shared_from_this()); //data moved into the datamodels cache
			}
		}
	};

};