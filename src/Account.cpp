#include "Account.h"

pof::Account::Account()
{
}

pof::Account::~Account()
{
}

std::string pof::Account::GetAccountTypeString()
{
	std::vector<std::string_view> strs;
	strs.reserve(max_account_type);
	for (int i = 0; i < max_account_type; i++){
		if (priv.test(i)) {
			strs.emplace_back(account_type_string[i]);
		}
	}
	strs.shrink_to_fit();
	return fmt::format("{}", fmt::join(strs, ","));
}


nl::json pof::Account::Pack() const
{
	return nl::json();
}

void pof::Account::UnPack(const nl::json& package)
{
}
