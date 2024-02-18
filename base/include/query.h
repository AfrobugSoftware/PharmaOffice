#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <exception>
#include <future>

#include <boost/signals2.hpp>
#include <boost/mysql.hpp>
#include <boost/noncopyable.hpp>

#include <spdlog/spdlog.h>
#include <boost/asio/experimental/awaitable_operators.hpp>

#include "Data.h"
#include "errc.h"
#include "relation.h"

//represents data and query read or written to the database
using namespace boost::asio::experimental::awaitable_operators;
namespace pof {
	namespace base {
		template<typename manager>
		struct query : public std::enable_shared_from_this<query<manager>> {
			using default_token = boost::asio::as_tuple_t<boost::asio::use_awaitable_t<>>;
			using timer_t = default_token::as_default_on_t<boost::asio::steady_timer>;

			using shared_t = std::enable_shared_from_this<query<manager>>;
			static constexpr auto tuple_awaitable = boost::asio::as_tuple(boost::asio::use_awaitable);

			boost::signals2::signal<void(std::error_code, std::shared_ptr<query>)> m_sig;
			std::string m_sql;
			std::shared_ptr<pof::base::data> m_data;

			boost::mysql::diagnostics m_diag; //server diagonis
			std::shared_ptr<manager> m_manager; //must be the database manager
			std::promise<std::shared_ptr<pof::base::data>> m_promise;

			query(std::shared_ptr<manager> man = nullptr) : m_manager(man) {
				m_data = std::make_shared<pof::base::data>();
			}

			std::future<std::shared_ptr<pof::base::data>> get_future(){
				return m_promise.get_future();
			}
			//Text query
			virtual boost::asio::awaitable<void> operator()() {
				auto this_ = shared_t::shared_from_this(); //hold till we leave the coroutine

				boost::mysql::results result;

				timer_t timer(co_await boost::asio::this_coro::executor);
				timer.expires_after(std::chrono::minutes(1));
				try {
					std::error_code ec;
					auto complete
						= co_await(m_manager->connection().async_query(m_sql, result, tuple_awaitable)
							|| timer.async_wait());

					switch (complete.index())
					{
					case 0:
						timer.cancel();
						ec = std::get<0>(std::get<0>(complete));
						break;
					case 1:
						//what happens if we timeout ?
						//signal the query on timeout ...
						ec = boost::system::error_code(boost::asio::error::timed_out);
						throw std::system_error(ec);
						break; //
					default:
						break;
					}

					if (ec) {
						throw std::system_error(ec);
					}

					if (!result.has_value()) {
						//query did not return a value, is this an error ?
						m_promise.set_value(nullptr); //set an empty data ? 
						co_return;
					}
					else if (result.rows().empty()) {
						m_promise.set_value(m_data); //set an empty data ? 
						co_return;
					}
					else {
						const auto& meta = result.meta();
						auto& datameta = m_data->get_metadata();
						datameta.reserve(meta.size());
						for (const auto& m : meta) {
							auto k = m.type();
							switch (k)
							{
							case boost::mysql::column_type::bigint:
								datameta.emplace_back(pof::base::data::kind::uint64);
								break;
							case boost::mysql::column_type::smallint:
								datameta.emplace_back(pof::base::data::kind::uint32);
								break;
							case boost::mysql::column_type::text:
								datameta.emplace_back(pof::base::data::kind::text);
								break;
							case boost::mysql::column_type::json:
								datameta.emplace_back(pof::base::data::kind::text);
								break;
							case boost::mysql::column_type::double_:
								datameta.emplace_back(pof::base::data::kind::float64);
								break;
							case boost::mysql::column_type::float_:
								datameta.emplace_back(pof::base::data::kind::float32);
								break;
							case boost::mysql::column_type::datetime:
								datameta.emplace_back(pof::base::data::kind::datetime);
								break;
							case boost::mysql::column_type::blob:
								datameta.emplace_back(pof::base::data::kind::float32);
								break;
							case boost::mysql::column_type::int_:
								datameta.push_back(pof::base::data::kind::int32);
								break;
							case boost::mysql::column_type::unknown:
								datameta.push_back(pof::base::data::kind::null);
								break;
							default:
								break;
							}
						}
						const auto& rows = result.rows();
						const auto& meta = result.meta();

						m_data->reserve(rows.size());
						for (const auto& row : rows) {
							//copy data
							size_t i = 0;
							std::vector<pof::base::data::data_t> v;
							v.resize(meta.size()); //

							for (const auto& m : meta)
							{
								auto k = m.type();
								
								switch (k)
								{
								case boost::mysql::column_type::int_:
								case boost::mysql::column_type::bigint:
								case boost::mysql::column_type::decimal:
									v[i] = row.at(i).as_int64();
									break;
								case boost::mysql::column_type::float_:
									v[i] = row.at(i).as_float();
									break;
								case boost::mysql::column_type::double_:
									v[i] = row.at(i).as_double();
									break;
								
								case boost::mysql::column_type::bit:
								case boost::mysql::column_type::binary:
								case boost::mysql::column_type::varbinary:
								case boost::mysql::column_type::blob:
								{
									auto bv = row.at(i).as_blob();
									pof::base::data::blob_t blob;
									blob.reserve(bv.size());

									std::ranges::copy(bv, std::back_inserter(blob));
									v[i] = std::move(blob);
								}
									break;

								case boost::mysql::column_type::time:
								case boost::mysql::column_type::date:
								case boost::mysql::column_type::datetime:
								case boost::mysql::column_type::timestamp:
									v[i] = pof::base::data::datetime_t(row.at(i).as_datetime().as_time_point().time_since_epoch()); // wrong type
									break;
								case boost::mysql::column_type::char_:
								case boost::mysql::column_type::varchar:
								case boost::mysql::column_type::text:
								case boost::mysql::column_type::enum_:
								case boost::mysql::column_type::set:
								case boost::mysql::column_type::json:
									v[i] = pof::base::data::text_t(row.at(i).as_string());
									break;
								case boost::mysql::column_type::unknown:
									break;
								default:
									break;
								}
								i++; //next column
							}
							m_data->emplace(std::move(v));

						}
						m_promise.set_value(m_data); //set the data
					}
				}catch (...) {
					m_promise.set_exception(std::current_exception());
			    }
			}
		};

		//Statement queries may require arguments
		template<typename manager, typename... args>
		struct querystmt : public query<manager> {
			using base_t = query<manager>;
			pof::base::relation<args...> m_arguments;

			querystmt(std::shared_ptr<manager> manager = nullptr) : base_t(manager) {}
			querystmt(std::shared_ptr<manager> manager,
				const std::string& sql, std::tuple<args...>&& arguments) : base_t(manager), base_t::m_sql(sql){
				m_arguments.emplace_back(std::forward<std::tuple<args...>>(arguments));
				
				}

			virtual boost::asio::awaitable<void> operator()() override {
				auto this_ = base_t::shared_t::shared_from_this(); //hold till we leave the coroutine

				boost::mysql::statement stmt;
				std::error_code ec;
				auto& conn = base_t::m_manager->connection();
				try{
					base_t::timer_t timer(co_await boost::asio::this_coro::executor);
					timer.expires_after(std::chrono::minutes(1));

					auto complete = co_await(conn.async_prepare_statement(base_t::m_sql, stmt, base_t::m_diag, base_t::tuple_awaitable) || timer.async_wait());
					switch (complete.index())
					{
					case 0:
						timer.cancel();
						ec = std::get<0>(std::get<0>(complete));
						break;
					case 1:
						//what happens if we timeout ?
						//signal the query on timeout ...
						ec = boost::system::error_code(boost::asio::error::timed_out);
						throw std::system_error(ec);
					default:
						break;
					}

					if (ec) {
						throw std::system_error(ec);
					}


					for(auto& tup : m_arguments){
						boost::mysql::results result;
						timer.expires_after(std::chrono::minutes(1));
						auto comp = co_await (conn.async_execute(tup, result, base_t::mDiag, base_t::tuple_awaitable) ||
							 timer.async_wait());
						switch (comp.index())
						{
						case 0:
							timer.cancel();
							ec = std::get<0>(std::get<0>(comp));
							break;
						case 1:
							//what happens if we timeout ?
							//signal the query on timeout ...
							ec = boost::system::error_code(boost::asio::error::timed_out);
							throw std::system_error(ec);
						default:
							break;
						}

						if (ec) {
							throw std::system_error(ec);
						}
						if (!result.has_value()) {
							base_t::m_promise.set_value(nullptr);
							co_return;
						}
						else if (result.empty()) {
							base_t::m_promise.set_value(base_t::m_data);
						}
						else {
					const auto& meta = result.meta();
					auto& datameta = base_t::m_data->get_metadata();
					datameta.reserve(meta.size());
					for (const auto& m : meta) {
						auto k = m.type();
						switch (k)
						{
						case boost::mysql::column_type::bigint:
							datameta.emplace_back(pof::base::data::kind::uint64);
							break;
						case boost::mysql::column_type::smallint:
							datameta.emplace_back(pof::base::data::kind::uint32);
							break;
						case boost::mysql::column_type::text:
							datameta.emplace_back(pof::base::data::kind::text);
							break;
						case boost::mysql::column_type::json:
							datameta.emplace_back(pof::base::data::kind::text);
							break;
						case boost::mysql::column_type::double_:
							datameta.emplace_back(pof::base::data::kind::float64);
							break;
						case boost::mysql::column_type::float_:
							datameta.emplace_back(pof::base::data::kind::float32);
							break;
						case boost::mysql::column_type::datetime:
							datameta.emplace_back(pof::base::data::kind::datetime);
							break;
						case boost::mysql::column_type::blob:
							datameta.emplace_back(pof::base::data::kind::float32);
							break;
						case boost::mysql::column_type::int_:
							datameta.push_back(pof::base::data::kind::int32);
							break;
						case boost::mysql::column_type::unknown:
							datameta.push_back(pof::base::data::kind::null);
							break;
						default:
							break;
						}
					}
					const auto& rows = result.rows();
					const auto& meta = result.meta();

					base_t::m_data->reserve(rows.size());
					for (const auto& row : rows) {
						//copy data
						size_t i = 0;
						std::vector<pof::base::data::data_t> v;
						v.resize(meta.size()); //

						for (const auto& m : meta)
						{
							auto k = m.type();

							switch (k)
							{
							case boost::mysql::column_type::int_:
							case boost::mysql::column_type::bigint:
							case boost::mysql::column_type::decimal:
								v[i] = row.at(i).as_int64();
								break;
							case boost::mysql::column_type::float_:
								v[i] = row.at(i).as_float();
								break;
							case boost::mysql::column_type::double_:
								v[i] = row.at(i).as_double();
								break;

							case boost::mysql::column_type::bit:
							case boost::mysql::column_type::binary:
							case boost::mysql::column_type::varbinary:
							case boost::mysql::column_type::blob:
							{
								auto bv = row.at(i).as_blob();
								pof::base::data::blob_t blob;
								blob.reserve(bv.size());

								std::ranges::copy(bv, std::back_inserter(blob));
								v[i] = std::move(blob);
							}
							break;

							case boost::mysql::column_type::time:
							case boost::mysql::column_type::date:
							case boost::mysql::column_type::datetime:
							case boost::mysql::column_type::timestamp:
								v[i] = pof::base::data::datetime_t(row.at(i).as_datetime().as_time_point().time_since_epoch()); // wrong type
								break;
							case boost::mysql::column_type::char_:
							case boost::mysql::column_type::varchar:
							case boost::mysql::column_type::text:
							case boost::mysql::column_type::enum_:
							case boost::mysql::column_type::set:
							case boost::mysql::column_type::json:
								v[i] = pof::base::data::text_t(row.at(i).as_string());
								break;
							case boost::mysql::column_type::unknown:
								break;
							default:
								break;
							}
							i++; //next column
						}
						base_t::m_data->emplace(std::move(v));
					}
				}
					base_t::m_promise.set_value(base_t::m_data); //data moved into the datamodels cache
				}
				} catch (...) {
					base_t::m_promise.set_exception(std::current_exception());
				}

			}
		};
	}
};