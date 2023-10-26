#include "DataModel.h"

pof::DataModel::DataModel()
{
	datastore = std::make_shared<pof::base::data>();
	pack = std::make_shared<pof::base::packer>(*datastore);
	unpack = std::make_shared<pof::base::unpacker>(*datastore);
}

pof::DataModel::DataModel(std::shared_ptr<pof::base::data> datastore_ptr)
{
	datastore = datastore_ptr;
	pack = std::make_shared<pof::base::packer>(*datastore);
	unpack = std::make_shared<pof::base::unpacker>(*datastore);
}

//shallow copy, use move for deep copy
pof::DataModel::DataModel(const DataModel& model)
{
	datastore = model.datastore;
	pack = model.pack;
	unpack = model.unpack;

}

pof::DataModel::DataModel(DataModel&& model) noexcept
 {
	datastore = std::move(model.datastore);
	pack = std::move(model.pack);
	unpack = std::move(model.unpack);
	sig = std::move(model.sig);
	attributes = std::move(model.attributes);
	mSpecialColHandlers = std::move(model.mSpecialColHandlers);
	mItems = std::move(model.mItems);
}
/**
* Cloning is making a deep copy of the underlying datastore
* Signals cannot be cloned, new clones should net up its own signals
* attributes and special column handlers can be as well
*/
pof::DataModel pof::DataModel::Clone()
{
	DataModel model;
	model.attributes = attributes;
	model.mSpecialColHandlers = mSpecialColHandlers;
	model.mItems = mItems;
	*(model.datastore) = *datastore;
	return model;
}

//signals cannot be copied, set up signals if needed 
pof::DataModel& pof::DataModel::operator=(const DataModel& model)
{
	datastore = model.datastore;
	pack = model.pack;
	unpack = model.unpack;

	return (*this);
}

pof::DataModel& pof::DataModel::operator=(DataModel&& model) noexcept
{
	datastore = std::move(model.datastore);
	pack = std::move(model.pack);
	unpack = std::move(model.unpack);
	sig = std::move(model.sig);
	attributes = std::move(model.attributes);
	mSpecialColHandlers = std::move(model.mSpecialColHandlers);
	mItems = std::move(mItems);
	return (*this);
}

pof::base::pack_t pof::DataModel::Pack() const {
	return (*pack)();
}
void pof::DataModel::Unpack(const pof::base::pack_t& package) {
	if (!datastore->empty()) {
		datastore->clear();
		attributes.clear();
		mItems.Clear();
		Cleared();
	}

	(*unpack)(package);

	mItems.resize(datastore->size());
	size_t i = 0;
	std::generate(mItems.begin(), mItems.end(),
		[&]() { return wxDataViewItem(reinterpret_cast<void*>(++i)); });
	ItemsAdded(wxDataViewItem{ 0 }, mItems);
	mSignals[static_cast<size_t>(Signals::LOADED)](datastore->begin());

}

void pof::DataModel::Emplace(pof::base::data&& d)
{
	Cleared();
	mItems.clear();
	attributes.clear();
	//problem with this emplace
	*datastore = (std::forward<pof::base::data>(d));
	mItems.resize(datastore->size());
	size_t i = 0;
	std::generate(mItems.begin(), mItems.end(), 
		[&]() { return wxDataViewItem(reinterpret_cast<void*>(++i)); });
	ItemsAdded(wxDataViewItem{ 0 }, mItems);
	mSignals[static_cast<size_t>(Signals::LOADED)](datastore->begin());
}

void pof::DataModel::EmplaceData(pof::base::data::row_t&& r)
{
	datastore->insert(std::forward<pof::base::data::row_t>(r));
	const size_t count = datastore->size();
	mItems.push_back(wxDataViewItem{ reinterpret_cast<void*>(count) });
	ItemAdded(wxDataViewItem{ 0 }, mItems.back());
	mSignals[static_cast<size_t>(Signals::ADDED)](datastore->end() - 1);

}

void pof::DataModel::Clear()
{
	Cleared();
	mItems.clear();
	attributes.clear();
	datastore->clear();

}

void pof::DataModel::Store(pof::base::data&& d)
{
	Cleared();
	mItems.clear();
	attributes.clear();
	//problem with this emplace
	*datastore = (std::forward<pof::base::data>(d));
	mItems.resize(datastore->size());
	size_t i = 0;
	std::generate(mItems.begin(), mItems.end(),
		[&]() { return wxDataViewItem(reinterpret_cast<void*>(++i)); });
	ItemsAdded(wxDataViewItem{ 0 }, mItems);
	mSignals[static_cast<size_t>(Signals::STORE_LOAD)](datastore->begin());
}

void pof::DataModel::StoreData(pof::base::data::row_t&& r)
{
	datastore->insert(std::forward<pof::base::data::row_t>(r));
	const size_t count = datastore->size();
	mItems.push_back(wxDataViewItem{ reinterpret_cast<void*>(count) });
	ItemAdded(wxDataViewItem{ 0 }, mItems.back());
	mSignals[static_cast<size_t>(Signals::STORE)](std::next(datastore->begin(), count - 1));
}

void pof::DataModel::Reload(const std::vector<wxDataViewItem>& items)
{
	if (items.empty()) return;
	Cleared();
	mItems.Clear();
	attributes.clear();

	mItems.resize(items.size());
	std::ranges::copy(items, mItems.begin());
	ItemsAdded(wxDataViewItem(0), mItems);
	mSignals[static_cast<size_t>(Signals::LOADED)](datastore->begin());
}

void pof::DataModel::StringSearchAndReload(size_t col, const std::string& search_for)
{
	if (datastore->empty() || col > datastore->get_metadata().size() 
			|| datastore->get_metadata()[col] != pof::base::data::kind::text) return;
	std::string reg;
	reg.reserve(search_for.size() * 2);
	for (auto& c : search_for)
	{
		if (!std::isalnum(c)) {
			//what to do
			return;
		}
		reg += fmt::format("[{:c}|{:c}]", (char)std::tolower(c), (char)std::toupper(c));
	}
	reg += "(?:.*)?";
	std::regex searchreg(std::move(reg));
	Cleared();
	mItems.clear();
	attributes.clear(); //dont know if i should clear the attributes here?
	for (size_t i = 0; i < datastore->size(); i++) {
		auto& datum = (*datastore)[i].first[col];
		auto& text = boost::variant2::get<pof::base::data::text_t>(datum);
		if (std::regex_match(text, searchreg)) {
			mItems.push_back(wxDataViewItem{ reinterpret_cast<void*>(i + 1) });
		}
	}
	ItemsAdded(wxDataViewItem(0), mItems);
	mSignals[static_cast<size_t>(Signals::SEARCHED)](datastore->begin());
}

void pof::DataModel::StringSearchAndReloadSet(size_t col, const std::string& searchFor)
{
	if (mItems.empty() || col > datastore->get_metadata().size()
		|| datastore->get_metadata()[col] != pof::base::data::kind::text) return;

	std::string reg;
	reg.reserve(searchFor.size() * 2);
	for (auto& c : searchFor)
	{
		if (!std::isalnum(c)) {
			//what to do
			return;
		}
		reg += fmt::format("[{:c}|{:c}]", (char)std::tolower(c), (char)std::toupper(c));
	}
	reg += "(?:.*)?";
	std::regex searchreg(std::move(reg));

	Cleared();
	attributes.clear();
	wxDataViewItemArray ars;
	ars.reserve(mItems.size());
	for (auto& item : mItems) {
		size_t idx = GetIdxFromItem(item);

		auto& datum = (*datastore)[idx].first[col];
		auto& text = boost::variant2::get<pof::base::data::text_t>(datum);
		if (std::regex_match(text, searchreg)) {
			ars.push_back(item);
		}
	}
	ItemsAdded(wxDataViewItem(0), ars);
}

bool pof::DataModel::HasContainerColumns(const wxDataViewItem& item) const
{
	return false;
}
 bool pof::DataModel::HasDefaultCompare() const
{
	return false;
}

bool pof::DataModel::IsEnabled(const wxDataViewItem& item, unsigned int col) const
{
	const size_t i = GetIdxFromItem(item);
	return (i < datastore->size());
}

size_t pof::DataModel::GetIdxFromItem(const wxDataViewItem& item) {
	size_t i = reinterpret_cast<size_t>(item.GetID());
	return (--i);
}

wxDataViewItem pof::DataModel::GetItemFromIdx(size_t idx)
{
	return wxDataViewItem{reinterpret_cast<void*>(idx + 1)};
}

bool pof::DataModel::IsContainer(const wxDataViewItem& item) const
{
	return false;
}

bool pof::DataModel::IsVirtualListModel() const
{
	return false;
}

bool pof::DataModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
{
	if (col >= GetColumnCount()) return false;
	if (!item.IsOk()) return false;
	auto attrIter = attributes.find(item);
	if (attrIter != attributes.end()) {
		attr = *(attrIter->second);
		return true;
	}
	return false;
}

wxString pof::DataModel::GetColumnType(unsigned int col) const
{
	if (col >= GetColumnCount()) return wxString();
	const pof::base::data::kind k = datastore->get_metadata()[col];
	switch (k)
	{
	case pof::base::data::kind::int32:
		return "int";
	case pof::base::data::kind::int64:
		return "long long";
	case pof::base::data::kind::uint32:
		return "unsigned int";
		break;
	case pof::base::data::kind::uint64:
		return "unsigned long long";
		break;
	case pof::base::data::kind::float32:
		return "float";
		break;
	case pof::base::data::kind::float64:
		return "double";
		break;
	case pof::base::data::kind::datetime:
		return "datetime";
	case pof::base::data::kind::text:
		return "string";
	case pof::base::data::kind::blob:
		return "blob";
		break;
	default:
		return wxString();
	}
}

bool pof::DataModel::AddAttr(const wxDataViewItem& item, std::shared_ptr<wxDataViewItemAttr> attr) {
	auto [iter, inserted] = attributes.insert({ item, attr });
	return inserted;
}

void pof::DataModel::RemoveAttr(const wxDataViewItem& item)
{
	attributes.erase(item);
}

unsigned int pof::DataModel::GetColumnCount() const
{
	return datastore->get_metadata().size();
}


void pof::DataModel::SetSpecialColumnHandler(size_t column, SpeicalColHandler_t&& handler)
{
	auto [iter, inserted] = mSpecialColHandlers.insert({ column, std::forward<SpeicalColHandler_t>(handler) });
}

void pof::DataModel::SetSpecialColumnHandler(size_t column, get_function_t&& function)
{
	auto [iter, inserted] = mSpecialColHandlers.insert({ column, {std::forward<get_function_t>(function), nullptr} });
}

void pof::DataModel::SetSpecialColumnHandler(size_t column, get_function_t&& get_function, set_function_t&& set_function)
{
	auto [iter, inserted] = mSpecialColHandlers.insert({ column, {std::forward<get_function_t>(get_function), 
			std::forward<set_function_t>(set_function)} });
}


void pof::DataModel::RemoveSpecialColumnHandler(size_t column)
{
	mSpecialColHandlers.erase(column);
}

void pof::DataModel::SetSpecialSetColumnHandler(size_t column, set_function_t&& function)
{
	auto [iter, inserted] = mSpecialColHandlers.insert({ column, {nullptr, function} });
	if (!inserted) {
		iter->second.second = function;
	}
}

int pof::DataModel::Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const
{
	if (column >= GetColumnCount()) return 0;
	const size_t i = GetIdxFromItem(item1);
	const size_t i2 = GetIdxFromItem(item2);
	const auto& [r, s] = (*datastore)[i];
	const auto& [r2, s2] = (*datastore)[i2];
	const auto& val1 = r[column];
	const auto& val2 = r2[column];

	if (ascending) {
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;
	}
	else {
		if (val1 > val2) return -1;
		else if (val1 < val2) return 1;
		else return 0;
	}
	return 0;
}

bool pof::DataModel::HasValue(const wxDataViewItem& item, unsigned col) const
{
	const size_t i = GetIdxFromItem(item);
	return (i < datastore->size());
}

void pof::DataModel::GetValue(wxVariant& v, const wxDataViewItem& item, unsigned int col) const
{
	if (!item.IsOk()) return;
	const size_t i = GetIdxFromItem(item);

	//handle if it is a special column
	const auto iter = mSpecialColHandlers.find(col);
	if (iter != mSpecialColHandlers.end()) {
		const auto& [get, set] = iter->second;
		if (get) {
			v = get(i, col);
			return;
		}
	}
	if (col >= GetColumnCount()) return; //col not a specail col and not in range

	const auto& [r, s] = (*datastore)[i];
	const pof::base::data::kind k = datastore->get_metadata()[col];
	const auto& d = r[col];

	try {
		switch (k)
		{
		case pof::base::data::kind::int32:
			v = std::to_string(boost::variant2::get<std::int32_t>(d));
			break;
		case pof::base::data::kind::int64:
			v = std::to_string(boost::variant2::get<std::int64_t>(d));
			break;
		case pof::base::data::kind::uint32:
			v = std::to_string(boost::variant2::get<std::uint32_t>(d));
			break;
		case pof::base::data::kind::uint64:
			v = std::to_string(boost::variant2::get<std::uint64_t>(d));
			break;
		case pof::base::data::kind::float32:
			v = std::to_string(boost::variant2::get<float>(d));
			break;
		case pof::base::data::kind::float64:
			v = std::to_string(boost::variant2::get<double>(d));
			break;
		case pof::base::data::kind::datetime:
		{
			auto t = pof::base::data::clock_t::to_time_t(boost::variant2::get<pof::base::data::datetime_t>(d));
			v = fmt::format("{:%d/%m/%Y}", boost::variant2::get<pof::base::data::datetime_t>(d));
			break;
		}
		case pof::base::data::kind::text:
			v = boost::variant2::get<pof::base::data::text_t>(d);
			break;
		case pof::base::data::kind::blob:
			//cannot display a blob type
			break;
		case pof::base::data::kind::uuid:
			v = boost::uuids::to_string(boost::variant2::get<pof::base::data::duuid_t>(d));
			break;
		case pof::base::data::kind::currency:
			v = fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(d));
			break;
		default:
			break;
		}
	}
	catch (const std::exception& exp) {
		spdlog::critical(exp.what());
	}
}

bool pof::DataModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
	if (!item.IsOk()) return false;
	const size_t i = GetIdxFromItem(item);
	const auto iter = mSpecialColHandlers.find(col);
	if (iter != mSpecialColHandlers.end()) {
		auto& [get, set] = iter->second;
		if (set) {
			return set(i, col, variant);
		}
	}

	if (col >= GetColumnCount()) return false; //col not a specail col and not in col range

	pof::base::data::kind k = datastore->get_metadata()[col];
	auto& [r, s] = (*datastore)[i];
	auto& dat = r[col];
	switch (k)
	{
	case pof::base::data::kind::int32:
		dat = static_cast<std::int32_t>(variant.GetLong());
		break;
	case pof::base::data::kind::int64:
		dat = static_cast<std::int64_t>(variant.GetLongLong().GetValue());
		break;
	case pof::base::data::kind::uint32:
		dat = static_cast<std::uint32_t>(variant.GetLong());
		break;
	case pof::base::data::kind::uint64:
		dat = static_cast<std::uint64_t>(variant.GetULongLong().GetValue());
		break;
	case pof::base::data::kind::float32:
		dat = static_cast<float>(variant.GetDouble());
		break;
	case pof::base::data::kind::float64:
		dat = variant.GetDouble();
		break;
	case pof::base::data::kind::datetime:
	{
		auto dt = variant.GetDateTime();
		dat = pof::base::data::clock_t::from_time_t(dt.GetTicks());
		break;
	}
	case pof::base::data::kind::text:
		dat = variant.GetString().ToStdString();
		break;
	case pof::base::data::kind::blob:
	{
		return false; //should not be able to set a blob data from the view.
	}
	case pof::base::data::kind::uuid:
		return false;
	case pof::base::data::kind::currency:
		dat = pof::base::data::currency_t{ variant.GetString().ToStdString() };
		break;
	default:
		return false;
	}
	datastore->tsModified(pof::base::data::clock_t::now());
	datastore->set_state(i, pof::base::data::state::MODIFIED);
	ValueChanged(item, col);
	mSignals[static_cast<size_t>(Signals::UPDATE)](std::next(datastore->begin(), i));
	return true;
}

bool pof::DataModel::RemoveData(const wxDataViewItem& item)
{
	size_t idx = GetIdxFromItem(item);
	if (idx >= datastore->size()) return false;

	auto iter = std::next(datastore->begin(), idx);
	mSignals[static_cast<size_t>(Signals::REMOVED)](iter); //do what should be done
	datastore->erase(iter);

	//remove last element item
	ItemDeleted(wxDataViewItem(0), mItems.back());
	mItems.pop_back();
	return true;
}

bool pof::DataModel::RemoveData(const wxDataViewItemArray& items)
{
	//figure out how to do remove-erase here
	//items is sorted in acceding order
	if (items.IsEmpty()) return false;
	size_t count = 0;
	auto remv = std::remove_if(datastore->begin(), datastore->end(), [&](auto& row) {
		bool ret = std::any_of(items.begin(), items.end(), [&](auto& item) {
			bool yes =  (count == GetIdxFromItem(item));
			if (yes) {
				auto iter = std::next(datastore->begin(), count);
				mSignals[static_cast<size_t>(Signals::REMOVED)](iter);
			}
			return yes;
		});
		count++;
		return ret;
	});
	auto& tab = datastore->tab();
	tab.erase(remv, std::end(tab));
	count = 0;
	while (count != items.size()){
		ItemDeleted(wxDataViewItem(0), mItems.back());
		mItems.pop_back();
		count++;
	}
	return true;
		
}

void pof::DataModel::Reload()
{
	Cleared();
	attributes.clear();
	mItems.Clear();
	mItems.reserve(datastore->size());
	for (size_t i = 0; i < datastore->size(); i++) {
		mItems.push_back(wxDataViewItem(wxUIntToPtr(i + 1)));
	}
	ItemsAdded(wxDataViewItem(0), mItems);
	mSignals[static_cast<size_t>(Signals::LOADED)](datastore->begin());
}

void pof::DataModel::ReloadSet()
{
	Cleared();
	ItemsAdded(wxDataViewItem(0), mItems);
}

void pof::DataModel::Signal(Signals sig, size_t i) const
{
	auto iter = std::next(datastore->begin(), i);
	if (iter == std::end(*datastore)) return;

	mSignals[static_cast<size_t>(sig)](iter);
}

boost::signals2::connection pof::DataModel::ConnectSlot(signal_t::slot_type&& slot, Signals signal)
{
	return mSignals[static_cast<size_t>(signal)].connect(std::forward<signal_t::slot_type>(slot));
}

//this is very stupid, very very very stupid
unsigned int pof::DataModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
	children = mItems;
	return  static_cast<int>(mItems.size());
}

wxDataViewItem pof::DataModel::GetParent(const wxDataViewItem& item) const
{
	return wxDataViewItem(0);
}

bool pof::DataModel::IsListModel() const
{
	return false;
}

