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

std::string pof::Pharmacy::GetPharmacyTypeAsString() const
{
	int i = 0;
	for (; i < 5; i++) {
		if (pharmacyType.test(i)) break;
	}
	if (i >= 5) i = 0;
	return std::string(sPharamcyTypes[i]);
}
