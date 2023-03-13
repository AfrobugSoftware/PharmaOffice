#include "Data.h"

pof::base::data::data()
: bModified(false){
}

pof::base::data::data(size_t count)
: value(count),
  bModified(false),
  created(ch::steady_clock::now()),
  modified(ch::steady_clock::now())
{}

pof::base::data::~data()
{

}

void pof::base::data::insert(row_t&& row)
{
	assert(row.first.size() == metadata.size()); //not less than or greater than the row count
	modified = ch::steady_clock::now();
	constexpr size_t pos = (std::underlying_type_t<state>)state::CREATED;

	value.emplace_back(std::forward<row_t>(row));
	value.back().second.set(pos);
}

void pof::base::data::insert(const typename row_t::first_type& vals)
{
	assert(vals.size() == metadata.size()); //not less than or greater than the row count
	modified = ch::steady_clock::now();
	constexpr size_t pos = (std::underlying_type_t<state>)state::CREATED;

	value.push_back({ vals, state_t{}});
	value.back().second.set(pos);
}

void pof::base::data::insert(const typename row_t::first_type& vals, const typename row_t::second_type& st)
{
	assert(vals.size() == metadata.size()); //not less than or greater than the row count
	modified = ch::steady_clock::now();
	constexpr size_t pos = (std::underlying_type_t<state>)state::CREATED;
	value.push_back({ std::forward<const row_t::first_type&>(vals), st });
	value.back().second.set(pos);
}

const pof::base::data::row_t & pof::base::data::at(size_t i) const
{
	return value.at(i);
}

const pof::base::data::row_t& pof::base::data::operator[](size_t i) const
{
	return value[i];
}
