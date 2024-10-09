#include "currency.h"

pof::base::currency::currency()
{
	issigned = false;
	std::memset(m_data.data(), 0, m_data.size());
}

pof::base::currency::currency(const std::string& string)
{
	try {
		double d = atof(string.c_str());
		issigned = std::signbit(d);
		auto integer = std::abs(static_cast<std::int64_t>(std::floor(d * 100)));
		fmt::format_to(m_data.begin(), "{:d}", integer);
	}
	catch (const std::exception& exp) {
		std::rethrow_exception(std::current_exception());
	}
}

pof::base::currency::currency(double cur)
{
	issigned = std::signbit(cur);
	auto integer = std::abs(static_cast<std::int64_t>(std::floor(cur * 100)));
	fmt::format_to(m_data.begin(), "{:d}", integer);
}

pof::base::currency::currency(const cur_t& cur_data)
{
	issigned = false;
	std::copy(cur_data.begin(), cur_data.end(), m_data.begin());
}

pof::base::currency::currency(const currency& rhs)
{
	issigned = rhs.issigned;
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
}

pof::base::currency::currency(currency&& rhs) noexcept
{
	issigned = rhs.issigned;
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
	rhs.m_data = { 0 };
	rhs.issigned = false;
	
}

pof::base::currency & pof::base::currency::operator=(const currency & rhs)
{
	issigned = rhs.issigned;
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
	return (*this);
}

pof::base::currency& pof::base::currency::operator=(currency&& rhs) noexcept
{
	issigned = rhs.issigned;
	std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
	rhs.m_data = { 0 };
	rhs.issigned = false;
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
	double total = 0.0;
	std::int64_t d = 0;
	for (int i = 0; m_data[i] != '\0' && i < m_data.size(); i++) {
		d = (d * 10) + m_data[i] - '0';
	}
	total = (static_cast<double>(d) * 0.01);
	if (issigned) total = std::copysign(total, -0.0);
	return total;
}

void pof::base::currency::nearest_hundred()
{
	auto f = static_cast<double>(*this);
	f = 10 * round(f / 10.0);
	*this = f;
}

pof::base::currency pof::base::operator""_cu(long double fig)
{
	return currency(fig);
}
