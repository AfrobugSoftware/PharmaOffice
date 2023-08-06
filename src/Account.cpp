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

bool pof::Account::CreateAccountInfoTable()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS users_info
					 (user_id integer unique, 
					 sec_question text,
					 sec_ans_hash text, 
					 signin_time integer, 
					 signout_time integer);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		bool status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
		return true;
	}
	return false;
}

bool pof::Account::CreateAccountInfo()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(IF NOT EXISTS (SELECT 1 FROM users_info WHERE user_id = ?)
			BEGIN IMMEDIATE
				INSERT INTO users_info (user_id) VALUES (?)
			END;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());
		
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(accountID, accountID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
		return true;
	}
	return false;
}

bool pof::Account::CheckForUsername(const std::string& usersname)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT 1 FROM users WHERE username = ?)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(usersname));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel.has_value());
		
		mLocalDatabase->finalise(*stmt);

		return (!rel->empty());

	}
	return false;
}

bool pof::Account::ChangePassword(const std::string& newPass)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE users set password = ? WHERE id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& s = stmt.value();
		const auto hash = bcrypt::generateHash(newPass);
		bool status = mLocalDatabase->bind(s, std::make_tuple(hash, accountID));
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		status = mLocalDatabase->execute(s);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		mLocalDatabase->finalise(s);
	}
	return false;
}

std::string pof::Account::GetSecurityQuestion(const std::string& un)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(SELECT ui.sec_question 
					FROM users u, users_info ui
					WHERE u.id = ui.user_id AND u.username = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());
		
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(un));
		assert(status);

		auto rel = mLocalDatabase->retrive<pof::base::data::text_t>(*stmt);
		assert(rel.has_value() && !rel->empty());
		mLocalDatabase->finalise(*stmt);
		return std::get<0>(*rel->begin());
	}
	return std::string();
}

std::uint64_t pof::Account::GetLastId() const
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT MAX(id) FROM users;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		auto rel = mLocalDatabase->retrive<std::uint64_t>(*stmt);
		assert(rel.has_value());

		return (std::get<0>(*rel->begin()));
	}
	return std::uint64_t(0);
}

//save the current sign in time
void pof::Account::SetSignInTime()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE users_info set signin_time = ? WHERE user_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(signintime, accountID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		assert(status);
		mLocalDatabase->finalise(*stmt);
	}
}

void pof::Account::SetSecurityQuestion(const std::string& question, const std::string& answer)
{
}


nl::json pof::Account::Pack() const
{
	return nl::json();
}

void pof::Account::UnPack(const nl::json& package)
{
}

void pof::Account::DoSignOut()
{
	auto sotime = pof::base::data::clock_t::now();
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(UPDATE users_info set signout_time = ? WHERE user_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(sotime, accountID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
	}
	signOutSig(*this);
}
