#include "Data.h"

pof::base::Data::Data()
: bModified(false){
}

pof::base::Data::Data(size_t count)
: value(count),
  bModified(false),
  created(ch::steady_clock::now()),
  modified(ch::steady_clock::now())
{}

pof::base::Data::~Data()
{

}

void pof::base::Data::insert(row_t&& row)
{
	modified = ch::steady_clock::now();
	constexpr size_t pos = (std::underlying_type_t<state>)state::MODIFIED;

	value.emplace_back(std::forward<row_t>(row));
	value.back().second.set(pos);
}

void pof::base::Data::insert(const typename row_t::first_type& vals)
{
	modified = ch::steady_clock::now();
	constexpr size_t pos = (std::underlying_type_t<state>)state::MODIFIED;

	value.push_back({ vals, state_t{}});
	value.back().second.set(pos);
}

void pof::base::Data::insert(const typename row_t::first_type& vals, const typename row_t::second_type& st)
{
	modified = ch::steady_clock::now();
	constexpr size_t pos = (std::underlying_type_t<state>)state::MODIFIED;
	value.push_back({ std::forward<const row_t::first_type&>(vals), st });
	value.back().second.set(pos);
}

const pof::base::Data::row_t & pof::base::Data::at(size_t i) const
{
	return value.at(i);
}

const pof::base::Data::row_t& pof::base::Data::operator[](size_t i) const
{
	return value[i];
}
