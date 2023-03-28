#include "Account.h"

pof::Account::Account()
{
}

pof::Account::~Account()
{
}

void pof::Account::SetName(const std::string& n)
{

}

void pof::Account::SetEmail(const std::string& e)
{
}

void pof::Account::SetPhoneNumber(const std::string& pn)
{
}

const std::string& pof::Account::GetName() const
{
	// TODO: insert return statement here
	return std::string();
}

const std::string& pof::Account::GetEmail() const
{
	// TODO: insert return statement here
	return std::string();
}

const std::string& pof::Account::GetPhoneNumber() const
{
	// TODO: insert return statement here
	return std::string();
}

nl::json pof::Account::Pack() const
{
	return nl::json();
}

void pof::Account::UnPack(const nl::json& package)
{
}
