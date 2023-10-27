#include "Pharmacy.h"

pof::Pharmacy::Pharmacy()
{
}

pof::Pharmacy::~Pharmacy()
{
}

std::string pof::Pharmacy::GetAddressAsString() const
{
	return fmt::format("No.{}, {} street, {}, {}",
		addy.number, addy.street, addy.city, addy.state, addy.country);
}

std::string pof::Pharmacy::GetContactAsString() const
{
	return fmt::format("Phone: {}     Email: {}", contact.phoneNumber, contact.email);
}

nl::json pof::Pharmacy::Pack() const
{
	return nl::json();
}

void pof::Pharmacy::UnPack(const nl::json& js)
{
}
