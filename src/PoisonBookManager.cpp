#include "PoisonBookManager.h"
#include "PofPch.h"

pof::PoisonBookManager::PoisonBookManager()
{
	mPoisonBook = std::make_unique<pof::DataModel>();

	mPoisonBook->Adapt<
		pof::base::data::duuid_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		pof::base::data::text_t,
		std::uint64_t,
		std::uint64_t,
		std::uint64_t,
		std::uint64_t,
		pof::base::data::datetime_t
	>();

	mPoisonBook->ConnectSlot(std::bind_front(&pof::PoisonBookManager::OnAddRecord, this), pof::DataModel::Signals::STORE);
	mPoisonBook->ConnectSlot(std::bind_front(&pof::PoisonBookManager::OnRemoveRecord, this), pof::DataModel::Signals::REMOVED);
	mPoisonBook->ConnectSlot(std::bind_front(&pof::PoisonBookManager::OnUpdateRecord, this), pof::DataModel::Signals::UPDATE);
}

pof::PoisonBookManager::~PoisonBookManager()
{
}

void pof::PoisonBookManager::CreatePoisonBookTable()
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql =
			R"(CREATE TABLE IF NOT EXISTS poison_book (
			puid blob,
			patient_name text,
			patient_addy text,
			pharmacist_name text,
			is_verified integer,
			quantity integer,
			start_stock integer,
			running_balance integer,
			date integer 
			);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->execute(*stmt);
		assert(status);

		mLocalDatabase->finalise(*stmt);
	}
}

bool pof::PoisonBookManager::CreateNewBook(pof::base::data::row_t&& row)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO poison_book VALUES (?,?,?,?,?,?,?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		std::tuple <
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::datetime_t
		> tup;
		pof::base::build(tup, row);
		bool status = mLocalDatabase->bind(*stmt, tup);
		assert(status);
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		mPoisonBook->Signal(pof::DataModel::Signals::STORE_LOAD, 0); // USED TO SIGNAL A RELOAD OF BOOKS
		return status;
	}
	return false;
}

bool pof::PoisonBookManager::LoadBook(const pof::base::data::duuid_t& puid)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT * FROM poison_book WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return false;
		mPoisonBook->Clear();
		mPoisonBook->GetDatastore().tab().reserve(rel.value().size());
		for (const auto& tup : rel.value()){
			pof::base::data::row_t row;
			row.first = pof::base::make_row_from_tuple(tup);
			mPoisonBook->EmplaceData(std::move(row));
		}
		mPoisonBook->GetDatastore().shrink_to_fit();
		return true;
	}
	return false;
}

bool pof::PoisonBookManager::LoadBook(const pof::base::data::duuid_t& puid, const pof::base::data::datetime_t& d)
{
	if (mLocalDatabase) {
		constexpr const std::string_view sql = R"(SELECT * FROM poison_book WHERE puid = ? AND Months(date) = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		auto mth = std::chrono::duration_cast<date::months>(d.time_since_epoch());
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid, pof::base::data::datetime_t(mth)));
		assert(status);

		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::datetime_t
		>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		if (rel->empty()) return false;
		mPoisonBook->Clear();
		mPoisonBook->GetDatastore().tab().reserve(rel.value().size());
		for (const auto& tup : rel.value()) {
			pof::base::data::row_t row;
			row.first = pof::base::make_row_from_tuple(tup);
			mPoisonBook->EmplaceData(std::move(row));
		}
		mPoisonBook->GetDatastore().shrink_to_fit();
		return true;
	}
	return false;
}

bool pof::PoisonBookManager::OnAddRecord(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase){
		constexpr const std::string_view sql = R"(INSERT INTO poison_book VALUES (?,?,?,?,?,?,?,?,?);)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		std::tuple <
			pof::base::data::duuid_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			pof::base::data::text_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			std::uint64_t,
			pof::base::data::datetime_t
		> tup;
		pof::base::build(tup, *iter);
		bool status = mLocalDatabase->bind(*stmt, tup);
		assert(status);
		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		gPoisonBookChanged(ADDED); //signal added
		return status;
	}
	return false;
}

//DELETES THE ENTIRE BOOK FOR A PARTICULAR PRODUCT
bool pof::PoisonBookManager::OnRemoveRecord(pof::base::data::const_iterator iter)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(DELETE FROM poison_book WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);

		auto& puid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[PUID]);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);


		status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

bool pof::PoisonBookManager::OnUpdateRecord(pof::base::data::const_iterator iter)
{
	if(mLocalDatabase){
		constexpr static std::array<std::string_view, MAX> colName = {
				"puid", "patient_name", "patient_addy", "pharmacist_name", "is_verified" ,"quantity", "start_stock", "running_balance", "date"
		};
		std::ostringstream os;
		std::vector<size_t> upIdx;
		os << "UPDATE poison_book SET ";
		auto& updateFlag = iter->second.second;
		for (int i = 0; i < MAX; i++) {
			if (updateFlag.test(i)) {
				if (upIdx.size() != 0) {
					os << ", ";
				}
				upIdx.push_back(i);
				os << colName[i] << "= ?";
			}
		}
		os << " WHERE puid = ?;";
		upIdx.push_back(PUID);

		auto stmt = mLocalDatabase->prepare(os.str());
		if (!stmt.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			return false;
		}
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[PUID]);
		size_t i = 1;
		auto& meta = mPoisonBook->GetDatastore().get_metadata();
		auto& v = iter->first;
		for (size_t d : upIdx) {
			auto kind = meta[d];
			switch (kind)
			{
			case pof::base::data::kind::uint64:
				mLocalDatabase->bind(*stmt, boost::variant2::get<std::uint64_t>(v[d]), i);
				break;
			case pof::base::data::kind::datetime:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::datetime_t>(v[d]), i);
				break;
			case pof::base::data::kind::text:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::text_t>(v[d]), i);
				break;
			case pof::base::data::kind::uuid:
				mLocalDatabase->bind(*stmt, boost::variant2::get<pof::base::data::duuid_t>(v[d]), i);
				break;
			default:
				break;
			}
			i++;
		}
		bool status = mLocalDatabase->execute(*stmt);
		if (!status) {
			spdlog::error(mLocalDatabase->err_msg());
		}
		mLocalDatabase->finalise(*stmt);
		return status;
	}
	return false;
}

std::optional<pof::base::relation<pof::base::data::duuid_t,std::string,std::string, std::string>> pof::PoisonBookManager::GetBooks()
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT DISTINCT pb.puid, p.name, p.strength, p.formulation  FROM products p, poison_book pb WHERE p.uuid = pb.puid;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		auto rel = mLocalDatabase->retrive<
			pof::base::data::duuid_t,
			std::string,
			std::string,
			std::string
		>(*stmt);
		mLocalDatabase->finalise(*stmt);
		return rel;
	}
	return std::nullopt;
}

bool pof::PoisonBookManager::IsBookCreated(const pof::base::data::duuid_t& puid)
{
	if (mLocalDatabase)
	{
		constexpr const std::string_view sql = R"(SELECT 1 FROM poison_book WHERE puid = ?;)";
		auto stmt = mLocalDatabase->prepare(sql);
		assert(stmt);
		bool status = mLocalDatabase->bind(*stmt, std::make_tuple(puid));
		assert(status);

		auto rel = mLocalDatabase->retrive<std::uint32_t>(*stmt);
		if (!rel.has_value()) {
			spdlog::error(mLocalDatabase->err_msg());
			mLocalDatabase->finalise(*stmt);
			return false;
		}
		mLocalDatabase->finalise(*stmt);
		return (!rel->empty());
	}
	return false;
}

void pof::PoisonBookManager::ReloadPoisonBook()
{

}
