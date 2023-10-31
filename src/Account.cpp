#include "Account.h"

pof::Account::Account()
{
	sessionDuration = date::days(5); //session lasts for 5 days
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

bool pof::Account::CreateSessionTable()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(CREATE TABLE IF NOT EXISTS session (sid blob, uid integer, date integer);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		bool status = mLocalDatabase->execute(*stmt);
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::Account::InsertSession()
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO session VALUES(?, ?, ?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		pof::base::data::duuid_t sid = boost::uuids::random_generator_mt19937{}();
		pof::base::data::datetime_t tt = pof::base::data::clock_t::now();
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(sid, accountID, tt));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		if (!status){
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::Account::CheckForUsername(const std::string& usersname)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT 1 FROM users WHERE username = ?;)";
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

bool pof::Account::SignInFromSession()
{
	if (mLocalDatabase)
	{
		//only one entry should be in this session table for personal build
		constexpr const std::string_view sql = R"(SELECT s.sid, s.date FROM session s;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto rsd = mLocalDatabase->retrive<pof::base::data::duuid_t, pof::base::data::datetime_t>(*stmt);
		assert(rsd);

		mLocalDatabase->finalise(*stmt);
		if (rsd->empty()){
			wxMessageBox("There is no active session, try signing in.", "Session", wxICON_INFORMATION | wxOK);
			return false;
		}
		auto& tup = *rsd->begin();
		//check if session has expired, TODO: FIX EXPIRE
		if (std::get<1>(tup) + sessionDuration < (std::chrono::system_clock::now())){
			wxMessageBox("Session has expired, please sign in again.", "Session", wxICON_INFORMATION | wxOK);
			RemoveSession();

			return false;
		}

		constexpr const std::string_view sql2 = R"(SELECT u.id, u.priv, u.name, u.last_name, 
		u.email, u.phonenumber, u.regnumber, u.username, u.password FROM users u, session s WHERE s.uid = u.id AND s.sid = ?;)";
		stmt = mLocalDatabase->prepare(sql2);
		assert(stmt);
		
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(std::get<0>(tup)));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			std::uint64_t, //ID
			std::uint32_t, //PRV
			pof::base::data::text_t, //NAME
			pof::base::data::text_t, //LAST NAME
			pof::base::data::text_t, //EMAIL
			pof::base::data::text_t, // PHONEUMBER
			pof::base::data::text_t, // REGNUMBER
			pof::base::data::text_t, //USERNAME
			pof::base::data::text_t>(*stmt); //PASSWORD IN BCRYPT

		if (!rel.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}

		auto& v = rel->front();
		signintime = pof::Account::clock_t::now();
		accountID = std::get<0>(v);
		priv = pof::Account::privilage_set_t(std::get<1>(v));
		name = std::get<2>(v);
		lastname = std::get<3>(v);
		email = std::get<4>(v);
		phonenumber = std::get<5>(v);
		regnumber = std::get<6>(v);
		username = std::get<7>(v);
		passhash = std::get<8>(v);
		SetSignInTime();
		
		mLocalDatabase->finalise(*stmt);
		return true;
	}

	return false;
}

bool pof::Account::RemoveSession()
{
	if (mLocalDatabase)
	{
		//removes all the sessions from the session table, should only have one record
		constexpr const std::string_view sql = R"(DELETE FROM session;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		assert(stmt);
		mLocalDatabase->finalise(*stmt);
		return status;
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

bool pof::Account::AddNewRole(const Privilage& p)
{
	if (priv.to_ullong() & static_cast<unsigned long long>(p)) return false;
	priv |= static_cast<unsigned long long>(p);
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE users set privilage = ? WHERE id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(priv.to_ulong(), accountID));
		assert(status);
		status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
	}
	
	return true;
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

bool pof::Account::DeleteAccount()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(
			BEGIN IMMEDIATE
				DELETE FROM users WHERE id = ?
				DELETE FROM users_info WHERE user_id = ?
			END;
		)";
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

bool pof::Account::UpdateAccount() {
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE users SET name = ?, last_name = ?, email = ?, 
		phonenumber = ?, regnumber = ? WHERE id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		if (!stmt.has_value()){
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		bool status = mLocalDatabase->bind(*stmt, 
			std::make_tuple(name, lastname, email, phonenumber, regnumber, accountID));
		assert(status);
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
			goto end;
		}
		updateSig(*this);
	end:
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
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
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(UPDATE users_info set sec_question = ?, set sec_ans_hash = ? WHERE user_id = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt.has_value());
		std::string ans = answer;
		std::transform(ans.begin(), ans.end(), ans.begin(), [&](char c) -> char { return std::tolower(c); });
		std::string hash = bcrypt::generateHash(std::move(ans));

		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(question, std::move(hash), accountID));
		assert(status);

		status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
	}
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
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, fmt::format("{} signed out at {:%d/%M/%Y %H:%M:%S}", username, signintime));
	wxGetApp().bKeepMeSignedIn = false;
	RemoveSession();
	signOutSig(*this);
}
