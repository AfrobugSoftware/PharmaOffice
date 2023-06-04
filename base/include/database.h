#pragma once
#include "relation.h"
#include <sqlite3.h>
#include <boost/noncopyable.hpp>

#include <unordered_map>
#include <optional>
#include <filesystem>
#include <exception>

namespace pof {
	namespace base {
		//for SQL with ? instead of a parameter
		namespace detail {
			template<typename T>
			inline bool do_bind(sqlite3_stmt* statement, const T& value, size_t position)
			{
				using arg_type = std::decay_t<T>;
				static_assert(is_database_type<arg_type>::value, "Tuple type is not a valid database type");
				if constexpr (std::is_integral_v<arg_type>)
				{
					//64-bit int
					if constexpr (sizeof(arg_type) == sizeof(std::uint64_t))
					{
						return (SQLITE_OK == sqlite3_bind_int64(statement, position, value));
					}
					return (SQLITE_OK == sqlite3_bind_int(statement, position, value));
				}
				else if constexpr (std::is_floating_point_v<arg_type>)
				{
					if constexpr (std::is_same_v<arg_type, float>)
					{
						return (SQLITE_OK == sqlite3_bind_double(statement, position, static_cast<double>(value)));
					}
					return (SQLITE_OK == sqlite3_bind_double(statement, position, value));
				}
				//need to add support for other string formats
				else if constexpr (std::is_same_v<arg_type, pof::base::data::text_t>)
				{
					if (!value.empty())
					{
						return (SQLITE_OK == sqlite3_bind_text(statement, position, value.c_str(), value.size(), SQLITE_TRANSIENT));
					}
					else {
						return (SQLITE_OK == sqlite3_bind_null(statement, position));
					}
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::blob_t>)
				{
					if (value.empty())
					{
						//write null??? or just leave it so that, we would just bind null on empty vector
						return (SQLITE_OK == sqlite3_bind_null(statement, position));
					}
					return (SQLITE_OK == sqlite3_bind_blob(statement, position, value.data(), value.size(), SQLITE_TRANSIENT));
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::datetime_t>)
				{
					auto rep = value.time_since_epoch().count();
					return (SQLITE_OK == sqlite3_bind_int64(statement, position, rep));
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::uuid_t>)
				{
					return (SQLITE_OK == sqlite3_bind_blob(statement, position, value.data, value.size(), SQLITE_TRANSIENT));
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::currency_t>) {
					return (SQLITE_OK == sqlite3_bind_blob(statement, position, value.data().data(), value.data().size(), SQLITE_TRANSIENT));
				}
				else
				{
					return false;
				}

			}
			/**
			* Parameters must be specified in the SQL query as either ? or :name
			* any other format would lead to undefiend behaviour
			*/
			template<typename T>
			bool do_bind_para(sqlite3_stmt* statement, const T& value, std::string_view name)
			{
				using arg_type = std::decay_t<T>;
				static_assert(is_database_type<arg_type>::value, "Tuple type is not a valid database type");
				const std::string p_name = fmt::format(":{}", name);
				int position = sqlite3_bind_parameter_index(statement, p_name.c_str());
				if (position == 0) return false;

				if constexpr (std::is_integral_v<arg_type>)
				{
					//64-bit integers have speical functions to upload in sqlite
					if constexpr (sizeof(arg_type) == sizeof(std::uint64_t))
					{
						return (SQLITE_OK == sqlite3_bind_int64(statement, position, value));
					}
					return (SQLITE_OK == sqlite3_bind_int(statement, position, value));
				}
				else if constexpr (std::is_floating_point_v<arg_type>)
				{
					if constexpr (std::is_same_v<arg_type, float>)
					{
						return (SQLITE_OK == sqlite3_bind_double(statement, position, static_cast<double>(value)));
					}
					return (SQLITE_OK == sqlite3_bind_double(statement, position, value));
				}
				//need to add support for other string formats
				else if constexpr (std::is_same_v<arg_type, pof::base::data::text_t>)
				{
					if (!value.empty())
					{
						return (SQLITE_OK == sqlite3_bind_text(statement, position, value.c_str(), value.size(), SQLITE_TRANSIENT));
					}
					return (SQLITE_OK == sqlite3_bind_null(statement, position));
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::blob_t>)
				{
					if (value.empty())
					{
						//write null??? or just leave it so that, we would just bind null on empty vector
						return (SQLITE_OK == sqlite3_bind_null(statement, position));
					}
					return (SQLITE_OK == sqlite3_bind_blob(statement, position, value.data(), value.size(), SQLITE_TRANSIENT));
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::datetime_t>)
				{
					pof::base::data::clock_t::rep rep = value.time_since_epoch().count();
					return (SQLITE_OK == sqlite3_bind_int64(statement, position, rep));
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::duuid_t>)
				{
					return (SQLITE_OK == sqlite3_bind_blob(statement, position, value.data, value.size(), SQLITE_TRANSIENT));
				}
				else if constexpr (std::is_same_v<arg_type, pof::base::data::currency_t>) {
					return (SQLITE_OK == sqlite3_bind_blob(statement, position, value.data().data(), value.data().size(), SQLITE_TRANSIENT));
				}
				else if constexpr (std::is_enum_v<arg_type>) {
					auto en = static_cast<std::uint32_t>(value);
					return (SQLITE_OK == sqlite3_bind_int(statement, position, en));
				}
				else
				{
					return false;
				}

			}

			template<typename arg_type, size_t N>
			auto do_retrive(sqlite3_stmt* statement) -> std::tuple<arg_type>
			{
				const size_t col = (size_t)sqlite3_column_count(statement) - (N + 1);
				using arg_t = typename std::decay_t<arg_type>;

				static_assert(is_database_type<arg_t>::value, "Type in tuple is not a valid database type");
				if constexpr (std::is_integral_v<arg_t>)
				{
					//64 bit intergers has a special download function in sqlite
					if constexpr (sizeof(arg_t) == sizeof(std::uint64_t))
					{
						return std::make_tuple(sqlite3_column_int64(statement, col));
					}
					return std::make_tuple(sqlite3_column_int(statement, col));
				}
				else if constexpr (std::is_floating_point_v<arg_t>)
				{
					if constexpr (std::is_same_v<arg_t, float>)
					{
						//sqlite uses REAL type as double, need to safely cast, removes annoying warnings
						return std::make_tuple(static_cast<arg_t>(sqlite3_column_double(statement, col)));
					}
					return std::make_tuple(sqlite3_column_double(statement, col));
				}
				else if constexpr (std::is_same_v<arg_t, pof::base::data::blob_t>)
				{
					const pof::base::data::blob_t::value_type* val_ptr = static_cast<const pof::base::data::blob_t::value_type*>(sqlite3_column_blob(statement, col));
					if (val_ptr)
					{
						const size_t size = sqlite3_column_bytes(statement, col);
						pof::base::data::blob_t vec(size);
						std::copy(val_ptr, val_ptr + size, vec.begin());
						return std::make_tuple(std::move(vec));
					}
					return std::make_tuple(pof::base::data::blob_t{});
				}
				else if constexpr (std::is_same_v<arg_t, pof::base::data::text_t>)
				{
					const char* txt = (const char*)(sqlite3_column_text(statement, col));
					if (txt)
					{
						return std::make_tuple(pof::base::data::text_t(txt));
					}
					return std::make_tuple(pof::base::data::text_t{});
				}
				else if constexpr (std::is_same_v<arg_t, pof::base::data::datetime_t>)
				{
					pof::base::data::clock_t::rep rep = sqlite3_column_int64(statement, col);
					return std::make_tuple(pof::base::data::datetime_t(pof::base::data::clock_t::duration(rep)));
				}
				else if constexpr (std::is_same_v<arg_t, pof::base::data::duuid_t>)
				{
					const pof::base::data::blob_t::value_type* val_ptr =
						static_cast<const pof::base::data::blob_t::value_type*>(sqlite3_column_blob(statement, col));
					if (val_ptr)
					{
						const size_t size = sqlite3_column_bytes(statement, col);
						if (size == 16) //128 bit ids
						{
							pof::base::data::duuid_t id;
							std::copy(val_ptr, val_ptr + size, id.data);
							return std::make_tuple(std::move(id));
						}
					}
					return std::make_tuple(boost::uuids::nil_uuid());
				}
				else if constexpr (std::is_same_v<arg_t, pof::base::data::currency_t>) {
					const pof::base::data::blob_t::value_type* val_ptr =
						static_cast<const pof::base::data::blob_t::value_type*>(sqlite3_column_blob(statement, col));
					if (val_ptr)
					{
						const size_t size = sqlite3_column_bytes(statement, col);
						pof::base::data::currency_t buf;
						std::copy(val_ptr, val_ptr + size, buf.data().data());
						return std::make_tuple(std::move(buf));

					}
					return std::make_tuple(pof::base::data::currency_t{});
				}
				else if constexpr (std::is_enum_v<arg_t>) {
					auto en = static_cast<arg_t>(sqlite3_column_int(statement, col));
					return std::make_tuple(en);
				}
			}

			template<size_t N>
			struct loop {
				template<typename tuple_t>
				static bool bind(sqlite3_stmt* stmt, const tuple_t& tuple) {
					bool ret = detail::loop<N - 1>::template bind(stmt, tuple);

					const auto& val = std::get<N>(tuple);
					bool ret2 = do_bind(stmt, val, N);
					return (ret2 && ret);
				}

				template<typename tuple_t, typename array_t>
				static bool bind_para(sqlite3_stmt* stmt, const tuple_t& tuple, array_t&& arr)
				{
					bool ret = loop<N - 1>::template bind_para(stmt, tuple, std::forward<array_t>(arr));
					const auto& val = std::get<N>(tuple);
					bool ret2 = do_bind_para(stmt, val, arr[N]);
					return (ret2 && ret);
				}

				template<typename tuple_t>
				static auto retrive(sqlite3_stmt* statement)
				{
					constexpr size_t col = (std::tuple_size_v<tuple_t> -(N + 1));
					using arg_type = std::tuple_element_t<col, tuple_t>;
					auto t1 = do_retrive<arg_type, col>(statement);
					auto t2 = detail::loop<N - 1>::template retrive<tuple_t>(statement);

					return std::tuple_cat(std::move(t1), std::move(t2));
				}
			};

			template<>
			struct loop<0>
			{
				template<typename tuple_t>
				static bool bind(sqlite3_stmt* stmt, const tuple_t& tuple) {
					const auto& val = std::get<0>(tuple);
					return  do_bind(stmt, val, 0);
				}

				template<typename tuple_t, typename array_t>
				static bool bind_para(sqlite3_stmt* stmt, const tuple_t& tuple, array_t&& arr)
				{
					const auto& val = std::get<0>(tuple);
					return do_bind_para(stmt, val, arr[0]);
				}

				template<typename tuple_t>
				static auto retrive(sqlite3_stmt* statement) {
					constexpr size_t col = (std::tuple_size_v<tuple_t> -1);
					using arg_type = std::tuple_element_t<col, tuple_t>;

					return do_retrive<arg_type, col>(statement);
				}

			};
		}
		class database : public boost::noncopyable {
		public:
			using stmt_t = std::add_pointer_t<sqlite3_stmt>;
			using stmt_map = std::unordered_map<std::string, stmt_t>;
			using query_t = std::string;

			database(const std::filesystem::path& path);
			~database();
			database(database&& db) noexcept;
			database& operator=(database&& db) noexcept;


			std::optional<stmt_t> prepare(const query_t& query);
			std::optional<stmt_t> prepare(std::string_view query);
			void reset(stmt_t stmt);
			void finalise(stmt_t stmt);
			bool add_map(const std::string& name, stmt_t stmt);
			bool remove_map(const std::string& name);
			std::optional<stmt_t> get_map(const std::string& value);

			bool execute(const query_t& query);
			bool execute(stmt_t stmt);

			inline std::string_view err_msg() const { return std::string_view(sqlite3_errmsg(m_connection)); }
			inline int err_code() const { return sqlite3_errcode(m_connection); }

			template<typename... Args>
			auto retrive(const std::string& name) -> std::optional<pof::base::relation<Args...>>
			{
				using type_list = std::tuple<Args...>;
				auto iter = m_stmap.find(name);
				if (iter == m_stmap.end()) return std::nullopt;
				stmt_t stmt = iter->second;

				return retrive(stmt);
			}

			template<typename... Args>
			auto retrive(stmt_t stmt) -> std::optional<pof::base::relation<Args...>>
			{
				using rel_t = relation<Args...>;
				using type_list = std::tuple<Args...>;
				constexpr const size_t s = sizeof...(Args);

				if (stmt == nullptr) return std::nullopt;
				rel_t optrel;

				if (sqlite3_step(begin) != SQLITE_DONE) {
					//finalise begin?
					sqlite3_step(rollback);
					reset(begin);
					reset(rollback);
					return std::nullopt;
				}
				int ret = 0;
				while((ret = sqlite3_step(stmt)) == SQLITE_ROW)
				{
					auto tup = detail::loop<s - 1>::template retrive<type_list>(stmt);
					optrel.emplace_back(std::move(tup));
				}
				if (ret != SQLITE_DONE) {
					//SOME ERROR OCCURED ? HOW TO HANDLE
					sqlite3_step(rollback);
					reset(stmt); //or finalise
					reset(begin);
					reset(rollback);
					return std::nullopt;
				}

				if (sqlite3_step(end) != SQLITE_DONE) {
					//finalise end ? 
					sqlite3_step(rollback);
					
					reset(begin);
					reset(end);
					reset(rollback);
					return std::nullopt;
				}

				reset(end);
				reset(stmt);
				reset(begin);
				return optrel;
			}

			template<typename... Args>
			bool bind(stmt_t stmt, const std::tuple<Args...>& args) {
				using tuple_t = std::tuple<Args...>;
				constexpr const size_t s = sizeof...(Args);
				return loop<s - 1>::template bind(stmt, args);
			}

			template<typename... Args>
			bool bind_para(stmt_t stmt, const std::tuple<Args...>& args, std::array<std::string_view, sizeof...(Args)>&& para)
			{
				using tuple_t = std::tuple<Args...>;
				using array_t = std::array<std::string_view, sizeof...(Args)>;
				constexpr const size_t s = sizeof...(Args);
				return loop<s - 1>::template bind_para(stmt, args, std::forward<array_t>(para));
			}

			//for insert statments that inserts an entire relation
			template<typename... Args>
			bool store(stmt_t stmt, pof::base::relation<Args...>&& rel)
			{
				using type_list = std::tuple<Args...>;
				if (sqlite3_step(begin_immidiate) != SQLITE_DONE) {
					sqlite3_step(rollback);
					reset(begin_immidiate);
					reset(rollback);
					return false;
				}
				for (auto& row : rel) {
					if (!bind(stmt, std::forward<pof::base::relation<Args...>>(row))) {
						sqlite3_step(rollback);
						reset(stmt);
						reset(begin_immidiate);
						reset(rollback);
						return false;
					}

					if (sqlite3_step(stmt) != SQLITE_DONE) {
						sqlite3_step(rollback);
						reset(stmt);
						reset(begin_immidiate);
						reset(rollback);
						return false;
					}
				}

				if (sqlite3_step(end) != SQLITE_DONE) {
					sqlite3_step(rollback);
					reset(stmt);
					reset(begin_immidiate);
					reset(end);
					reset(rollback);
					return false;
				}
				reset(end);
				reset(stmt);
				reset(begin_immidiate);
				return true;
			}

			//for update statements 
			template<typename... Args>
			bool update(stmt_t stmt,
				std::array<std::string_view, sizeof...(Args)>&& paras, pof::base::relation<Args...>&& rel)
			{
				if (sqlite3_step(begin_immidiate) != SQLITE_DONE) {
					sqlite3_step(rollback);
					reset(begin_immidiate);
					reset(rollback);
					return false;
				}
				for (auto& row : rel) {
					if (!bind(stmt, std::forward<pof::base::relation<Args...>>(row), 
							std::forward<std::array<std::string_view, sizeof...(Args)>>(paras))) {
						sqlite3_step(rollback);
						reset(stmt);
						reset(begin_immidiate);
						reset(rollback);
						return false;
					}

					if (sqlite3_step(stmt) != SQLITE_DONE) {
						sqlite3_step(rollback);
						reset(stmt);
						reset(begin_immidiate);
						reset(rollback);
						return false;
					}
				}

				if (sqlite3_step(end) != SQLITE_DONE) {
					sqlite3_step(rollback);
					reset(stmt);
					reset(begin_immidiate);
					reset(end);
					reset(rollback);
					return false;
				}
				reset(end);
				reset(stmt);
				reset(begin_immidiate);
				return true;
			}
		private:
			sqlite3* m_connection;
			stmt_map m_stmap;

			stmt_t begin = nullptr;
			stmt_t begin_immidiate = nullptr;
			stmt_t end = nullptr;
			stmt_t rollback = nullptr;

		};
	
	};
};