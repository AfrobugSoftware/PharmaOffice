#include "currency.h"

pof::base::currency::currency()
{
	std::memset(m_data.data(), 0, m_data.size());
}

pof::base::currency::currency(const std::string& string)
{
	if ((string.size() - 1) > m_data.size()) {
		throw pof::base::currency_exception("double value is bigger than the max currency size");
	}
	auto pos = string.find_first_of(".");
	if (pos == std::string::npos) {
		throw pof::base::currency_exception("invalid conversion from double");
	}

	std::memset(m_data.data(), 0, m_data.size());
	std::copy_n(string.begin(), pos + 1, m_data.begin());
	std::copy_n(string.begin() + pos, string.size() - pos, m_data.begin() + 2);
}

pof::base::currency::currency(double cur)
{
	if (std::signbit(cur)) {
		//no negetiive currency
		throw pof::base::currency_exception("Currency cannot be negetive");
	}
	auto string = fmt::format("{:.2}", cur);
	if ((string.size() - 1) > m_data.size()) {
		throw pof::base::currency_exception("double value is bigger than the max currency size");
	}
	auto pos = string.find_first_of(".");
	if (pos == std::string::npos) {
		throw pof::base::currency_exception("invalid conversion from double");
	}

	std::memset(m_data.data(), 0, m_data.size());
	std::copy_n(string.begin(), pos + 1, m_data.begin());
	std::copy_n(string.begin() + pos, string.size() - pos, m_data.begin() + 2);
}

pof::base::currency::currency(const cur_t& cur_data)
{
	std::copy(cur_data.begin(), cur_data.end(), m_data.begin());
}

pof::base::currency::currency(const currency& rhs)
{
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
}

pof::base::currency::currency(currency&& rhs) noexcept
{
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
	rhs.m_data = { 0 };
}

pof::base::currency & pof::base::currency::operator=(const currency & rhs)
{
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
	return (*this);
}

pof::base::currency& pof::base::currency::operator=(currency&& rhs) noexcept
{
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
	rhs.m_data = { 0 };
	return (*this);
}

pof::base::currency& pof::base::currency::operator=(double rhs)
{
	*this = currency(rhs);
	return *this;
}

pof::base::currency pof::base::currency::operator/(double denomiator) const
{
	const double f = static_cast<double>(*this) / denomiator;
	return currency(f);
}

pof::base::currency pof::base::currency::operator+(const currency& rhs) const
{
	const double f = static_cast<double>(*this) + static_cast<double>(rhs);
	return currency(f);
}

pof::base::currency pof::base::currency::operator+(double rhs) const
{
	const double f = static_cast<double>(*this) + rhs;
	return currency(f);
}

pof::base::currency pof::base::currency::operator-(const currency& rhs) const
{
	const double f = static_cast<double>(*this) - static_cast<double>(rhs);
	return currency(f);
}

pof::base::currency pof::base::currency::operator-(double rhs) const
{
	const double f = static_cast<double>(*this) - rhs;
	return currency(f);
}

pof::base::currency pof::base::currency::operator*(double rhs) const
{
	const double f = static_cast<double>(*this) * rhs;
	return currency(f);
}

pof::base::currency& pof::base::currency::operator+=(const currency& rhs)
{
	*this = *this + rhs;
	return (*this);
}

pof::base::currency& pof::base::currency::operator-=(const currency& rhs)
{
	*this = *this - rhs;
	return (*this);
}

pof::base::currency& pof::base::currency::operator/=(double rhs)
{
	*this = *this / rhs;
	return (*this);
}

pof::base::currency& pof::base::currency::operator*=(double rhs)
{
	*this = *this * rhs;
	return (*this);
}

bool pof::base::currency::operator==(const currency& rhs) const
{
	for (int i = 0; i < max; i++) {
		if (m_data[i] != rhs.m_data[i]) return false;
	}
	return true;
}

bool pof::base::currency::operator<(const currency& rhs) const
{
	for (int i = 0; i < max; i++) {
		if (!(m_data[i] < rhs.m_data[i])) return false;
	}
	return true;
}

pof::base::currency::operator std::string() const
{
	return fmt::format("{:cu}", *this);
}

pof::base::currency::operator double() const
{
	const std::string_view lower(m_data.begin(), m_data.begin() + 2);
	const std::string_view upper(m_data.begin() + 2, m_data.end());
	double total = 0.0;
	try {
		total = atof(upper.data());
		total += (atof(lower.data()) * 0.001);
	}
	catch (const std::exception& exp) {
		//spdlog::critical(exp.what());
		total = 0.0;
	}
	return total;
}

pof::base::currency pof::base::operator""cu(long double fig)
{
	return currency(fig);
}
