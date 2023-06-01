#pragma once
#include "database.h"

pof::base::database::database(const std::filesystem::path& path)
{
	int ret = sqlite3_open(path.string().c_str(), &m_connection);
	if (ret != SQLITE_OK) {
		throw std::logic_error(fmt::format("{}: Cannot open database file", path.string()));
	}

	auto opt = prepare("BEGIN;");
	if (opt.has_value()) {
		begin = opt.value();
	}

	opt = prepare("BEGIN IMMIDATE;");
	if (opt.has_value()) {
		begin_immidiate = opt.value();
	}

	opt = prepare("END;");
	if (opt.has_value()) {
		end = opt.value();
	}

	opt = prepare("ROLLBACK;");
	if (opt.has_value()) {
		rollback = opt.value();
	}


	stmt_t s = nullptr;
	std::array<std::string_view, 2> a{ "ZINO", "FERIFE" };
	bind_para(s, std::tuple<int, float>{1, 2.1}, std::forward<std::array<std::string_view, 2>>(a));

	retrive<int, float>(s);
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

std::optional<pof::base::database::stmt_t> pof::base::database::prepare(const query_t& query)
{
	return std::optional<stmt_t>();
}

void pof::base::database::reset(stmt_t stmt)
{
	sqlite3_reset(stmt);
}

void pof::base::database::finalise(stmt_t stmt)
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

bool pof::base::database::execute(const query_t& query)
{
	return false;
}

bool pof::base::database::execute(stmt_t stmt)
{
	return false;
}
