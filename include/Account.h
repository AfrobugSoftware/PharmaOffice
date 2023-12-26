#pragma once
#include <string>
#include <chrono>
#include <boost/signals2.hpp>
#include <nlohmann/json.hpp>
#include <array>
#include <cstdint>
#include <boost/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <bitset>
#include <fmt/format.h>

#include "database.h"
#include <../base/bcrypt/include/bcrypt.h> //to avoid clashing

namespace nl = nlohmann;
namespace pof {
	class Account : private boost::noncopyable {
	public:
		using clock_t = std::chrono::system_clock;
		using datetime_t = std::chrono::system_clock::time_point;
		using signal_t = boost::signals2::signal<void(const Account&)>;
		using privilage_set_t = std::bitset<8>;
		constexpr static const size_t max_account_type = 7;
		constexpr static const std::array<std::string_view, max_account_type> account_type_string = {
			"SUPERINTENDENT PHARMACIST", "PHARMTECH", "DISPENSER", "SALES ASSISTANT", "INTERN PHARMACIST", "STUDENT PHARMACIST", 
				"MANAGER"
		};
		enum class Privilage: std::uint8_t {
				PHARMACIST = 0,
				PHARMTECH,
				DISPENSER,
				SALES_ASSISTANT,
				INTEN_PHARMCIST,
				STUDENT_PHARMACIST,
				MANAGER
		};
		
		Account();
		~Account();


		nl::json Pack() const;
		void UnPack(const nl::json& package);

		void DoSignOut();

		inline constexpr const std::string& GetName() const { return name; }
		inline constexpr std::uint64_t GetID() const { return accountID; }
		inline constexpr const std::string& GetEmail() const { return email; }
		inline constexpr const std::string& GetPhonenumber() const { return phonenumber; }
		inline constexpr const std::string& GetRegnumber() const { return regnumber; }
		inline constexpr const boost::uuids::uuid& GetSessionUUID() const { return sessionID; }

		std::string GetAccountTypeString();
		inline constexpr datetime_t GetSignInTime() const { return signintime; }
		inline bool TestPriv(Privilage p) const {
			return priv.test(std::underlying_type_t<Privilage>(p));
		}

		//queries
		std::optional<pof::base::relation<
				std::uint64_t,
				std::uint64_t,
				pof::base::data::text_t,
				pof::base::data::text_t,
				pof::base::data::text_t,
				pof::base::data::text_t,
				pof::base::data::text_t,
				pof::base::data::text_t
			>> GetUsers() const;

		//deactivate an account
		bool DeactivateAccount(const std::string& username) const;

		//log out

		//session
		bool CreateSessionTable();
		bool InsertSession();

		bool SignInFromSession();
		bool RemoveSession();
		bool UpdateSession();

		bool CreateAccountInfoTable();
		bool CreateAccountInfo();
		bool ValidateCredentials(const std::string& username, const std::string& password);
		bool CheckForUsername(const std::string& usersname);
		bool ChangePassword(const std::string& newPass);
		bool AddNewRole(const Privilage& p);
		std::string GetSecurityQuestion(const std::string& username);
		std::uint64_t GetLastId() const;
		bool DeleteAccount();
		bool UpdateAccount();
		void SetSignInTime();
		void SetSecurityQuestion(const std::string& question, const std::string& answer);
		std::shared_ptr<pof::base::database> mLocalDatabase;
		signal_t signOutSig;
		signal_t updateSig;
		boost::uuids::uuid sessionID;
		privilage_set_t priv;
		datetime_t signintime;
		datetime_t::duration sessionDuration;
		std::uint64_t accountID = 0;
		std::string name;
		std::string lastname;
		std::string username; //username should be unique
		std::string email;
		std::string phonenumber;
		std::string regnumber;
		std::string passhash;
		bool isLoccum = false; 
		//account details?
	}; 
};