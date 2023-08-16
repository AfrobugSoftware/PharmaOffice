#include "database.h"

void pof::base::cost_step_func(sqlite3_context* con, int row, sqlite3_value** vals) {
	using currency_ptr = std::add_pointer_t<pof::base::currency>;
	currency_ptr cur = reinterpret_cast<currency_ptr>(sqlite3_aggregate_context(con, sizeof(pof::base::currency)));
	if (cur) {
		pof::base::currency temp;
		const currency_ptr val = (const currency_ptr)(sqlite3_value_blob(vals[0]));
		std::copy(val->data().begin(), val->data().end(), temp.data().begin());
		*cur += temp;
	}
}

void pof::base::cost_final_func(sqlite3_context* conn) {
	using currency_ptr = std::add_pointer_t<pof::base::currency>;
	currency_ptr cur = reinterpret_cast<currency_ptr>(sqlite3_aggregate_context(conn, sizeof(pof::base::currency)));
	if (cur) {
		sqlite3_result_blob(conn, (const void*)cur, pof::base::currency::max, SQLITE_TRANSIENT);
	}
}


pof::base::database::database(const std::filesystem::path& path)
{
	int ret = sqlite3_open(path.string().c_str(), &m_connection);
	if (ret != SQLITE_OK) {
		throw std::logic_error(fmt::format("{}: Cannot open database file", path.string()));
	}
	err_msg();
	auto opt = prepare(std::string_view{ "BEGIN;" });
	if (opt.has_value()) {
		begin = opt.value();
	}

	opt = prepare(std::string_view{ "BEGIN IMMEDIATE;" });
	if (opt.has_value()) {
		begin_immidiate = opt.value();
	}

	opt = prepare(std::string_view{ "END;" });
	if (opt.has_value()) {
		end = opt.value();
	}

	opt = prepare(std::string_view{ "ROLLBACK;" });
	if (opt.has_value()) {
		rollback = opt.value();
	}
}

pof::base::database::~database()
{
	finalise(begin);
	finalise(begin_immidiate);
	finalise(end);
	finalise(rollback);

	for(auto& v : m_stmap){
		sqlite3_finalize(v.second);
	}
	m_stmap.clear();
	sqlite3_close(m_connection);
}

pof::base::database::database(database&& db) noexcept
{
	m_connection = db.m_connection;
	db.m_connection = nullptr;
	m_stmap = std::move(db.m_stmap);
}

pof::base::database& pof::base::database::operator=(database&& db) noexcept
{
	m_connection = db.m_connection;
	db.m_connection = nullptr;
	m_stmap = std::move(db.m_stmap);
	return (*this);
}

std::optional<pof::base::database::stmt_t> pof::base::database::prepare(const query_t& query) const
{
	if (query.empty()) return std::nullopt;
	auto c_str = query.c_str();
	const char* tail = nullptr;
	stmt_t stmt = nullptr;

	if (!sqlite3_complete(c_str)) return std::nullopt; //make sure statement has a ; at the end
	if (sqlite3_prepare_v2(m_connection, c_str, query.size(), &stmt, &tail) != SQLITE_OK) return std::nullopt;

	return stmt;
}

std::optional<pof::base::database::stmt_t> pof::base::database::prepare(std::string_view query) const
{
	if (query.empty()) return std::nullopt;
	const char* tail = nullptr;
	stmt_t stmt = nullptr;

	if (!sqlite3_complete(query.data())) return std::nullopt; //make sure statement has a ; at the end
	if (sqlite3_prepare_v2(m_connection, query.data(), query.size(), &stmt, &tail) != SQLITE_OK) return std::nullopt;

	return stmt;
}

void pof::base::database::reset(stmt_t stmt) const
{
	sqlite3_reset(stmt);
}

void pof::base::database::finalise(stmt_t stmt) const
{
	sqlite3_finalize(stmt);
}

bool pof::base::database::add_map(const std::string& name, stmt_t stmt)
{
	auto [iter, insert] = m_stmap.insert({ name, stmt });
	return insert;
}

bool pof::base::database::remove_map(const std::string& name)
{
	m_stmap.erase(name);
	return true;
}

std::optional<pof::base::database::stmt_t> pof::base::database::get_map(const std::string& value)
{
	auto iter = m_stmap.find(value);
	if(iter == m_stmap.end()) return std::nullopt;

	return iter->second;
}

void pof::base::database::set_commit_handler(commit_callback callback, void* UserData)
{
	sqlite3_commit_hook(m_connection, callback, UserData);
}

bool pof::base::database::set_trace_handler(trace_callback callback, std::uint32_t mask, void* UserData)
{
	return (sqlite3_trace_v2(m_connection, mask, callback, UserData) == SQLITE_OK);
}

bool pof::base::database::set_busy_handler(busy_callback callback, void* UserData)
{
	return (sqlite3_busy_handler(m_connection, callback, UserData) != SQLITE_ERROR);
}

void pof::base::database::set_rowback_handler(rollback_callback callback, void* UserData)
{
	sqlite3_rollback_hook(m_connection, callback, UserData);
}

void pof::base::database::set_update_handler(update_callback callback, void* UserData)
{
	sqlite3_update_hook(m_connection, callback, UserData);
}

bool pof::base::database::set_auth_handler(auth callback, void* UserData)
{
	return (sqlite3_set_authorizer(m_connection, callback, UserData) != SQLITE_ERROR);
}

void pof::base::database::set_progress_handler(progress_callback callback, void* UserData, int frq)
{
	sqlite3_progress_handler(m_connection, frq, callback, UserData);
}

bool pof::base::database::execute(const query_t& query) const
{
	char* errmessage;
	sqlite3_exec(m_connection, query.c_str(), [](void* prt, int a, char** v, char** data) -> int {
		return SQLITE_OK; //find out the appropriate return
	}, nullptr, &errmessage);
	return false;
}

bool pof::base::database::execute(stmt_t stmt) const
{
	int ret = sqlite3_step(stmt);
	if (ret == SQLITE_DONE) {
		clear_bindings(stmt);
		reset(stmt);
		return true;
	}
	else {
		finalise(stmt);
		return false;
	}
}

void pof::base::database::clear_bindings(stmt_t stmt) const
{
	int ret = sqlite3_clear_bindings(stmt);
}

bool pof::base::database::begin_trans() const 
{
	const int ret = sqlite3_step(begin_immidiate);
	if (ret != SQLITE_DONE) {
		reset(begin_immidiate);
		return false;
	}
}

bool pof::base::database::end_trans() const
{
	const bool ret = sqlite3_step(end) == SQLITE_DONE;
	reset(begin_immidiate);
	reset(end);
	return ret;
}

bool pof::base::database::flush_db()
{
	int ret = sqlite3_db_cacheflush(m_connection);
	return ret == SQLITE_OK;
}

bool pof::base::database::register_func(const func_aggregate& argg)
{
	int ret = sqlite3_create_function(m_connection, argg.name.c_str(), argg.arg_count, func_aggregate::encoding,
		argg.user_data, argg.func, argg.fstep, argg.ffinal);
	return ret == SQLITE_OK;
}
