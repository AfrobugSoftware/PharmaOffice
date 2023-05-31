#pragma once
#include "database.h"

pof::base::database::database(const std::filesystem::path& path)
{
	int ret = sqlite3_open(path.string().c_str(), &m_connection);
	if (ret != SQLITE_OK) {
		throw std::logic_error(fmt::format("{}: Cannot open database file", path.string()));
	}
	stmt_t s = nullptr;
	std::array<std::string_view, 2> a{ "ZINO", "FERIFE" };
	bind_para(s, std::tuple<int, float>{1, 2.1}, std::forward<std::array<std::string_view, 2>>(a));
}

pof::base::database::~database()
{
	for(auto& v : m_stmap){
		sqlite3_finalize(v.second);
	}

	sqlite3_close(m_connection);
}

pof::base::database::database(database&& db) noexcept
{
}

database& pof::base::database::operator=(database&& db) noexcept
{
	// TODO: insert return statement here
}

std::optional<pof::base::database::stmt_t> pof::base::database::prepare(const query_t& query)
{
	return std::optional<stmt_t>();
}

void pof::base::database::reset(stmt_t stmt)
{
}

void pof::base::database::finalise(stmt_t stmt)
{
}

bool pof::base::database::add_map(const std::string& name, stmt_t stmt)
{
	return false;
}

bool pof::base::database::remove_map(const std::string& name)
{
	return false;
}

std::optional<pof::base::database::stmt_t> pof::base::database::get_map(const std::string& value)
{

	return std::nullopt;
}

bool pof::base::database::execute(const query_t& query)
{
	return false;
}

bool pof::base::database::execute(stmt_t stmt)
{
	return false;
}
