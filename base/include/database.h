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
			else if constexpr (std::is_same_v<arg_type, pof::base::data::uuid_t>)
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

		template<size_t N>
		struct loop {
			template<typename tuple_t>
			static bool bind(sqlite3_stmt* stmt, const tuple_t& tuple) {
				bool ret = loop<N - 1>::template bind(stmt,tuple);

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

		};

		class database : public boost::noncopyable {
		public:
			using stmt_t = std::add_pointer_t<sqlite3_stmt>;
			using stmt_map = std::unordered_map<std::string, stmt_t>;
			using query_t = std::string;

			database(const std::filesystem::path& path);
			~database();

			std::optional<stmt_t> prepare(const query_t& query);
			void reset(stmt_t stmt);
			void finalise(stmt_t stmt);
			bool add_map(const std::string& name, stmt_t stmt);
			bool remove_map(const std::string& name);
			std::optional<stmt_t> get_map(const std::string& value);

			template<typename... Args>
			auto execute(const query_t& query) -> std::optional<pof::base::relation<Args...>>
			{
				
			}

			template<typename... Args>
			auto execute_stmt(const std::string& name) -> std::optional<pof::base::relation<Args...>>
			{

			}

			template<typename... Args>
			auto execute_stmt(stmt_t stmt) -> std::optional<pof::base::relation<Args...>>
			{

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
		private:
			sqlite3* m_connection;
			stmt_map m_stmap;
		};
	
	};
};