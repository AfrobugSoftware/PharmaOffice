#include "DataModel.h"

pof::base::pack_t pof::DataModel::Pack() const {
	return pack();
}
void pof::DataModel::Unpack(const pof::base::pack_t& package) {
	unpack(package);
}

bool pof::DataModel::HasContainerColumns(const wxDataViewItem& item) const
{
	return false;
}
 bool pof::DataModel::HasDefaultCompare() const
{
	return true;
}

bool pof::DataModel::IsEnabled(const wxDataViewItem& item, unsigned int col) const
{
	const size_t i = GetIdxFromItem(item);
	return (i < datastore.size());
}

size_t pof::DataModel::GetIdxFromItem(const wxDataViewItem& item) {
	size_t i = reinterpret_cast<size_t>(item.GetID());
	return (--i);
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
}

wxString pof::DataModel::GetColumnType(unsigned int col) const
{
	if (col >= GetColumnCount()) return wxString();
	const pof::base::data::kind k = datastore.get_metadata()[col];
	switch (k)
	{
	case pof::base::data::kind::int32:
		return "int";
	case pof::base::data::kind::int64:
		return "long long";
	case pof::base::data::kind::uint32:
		break;
	case pof::base::data::kind::uint64:
		break;
	case pof::base::data::kind::float32:
		break;
	case pof::base::data::kind::float64:
		break;
	case pof::base::data::kind::datetime:
		break;
	case pof::base::data::kind::text:
		return "string";
	case pof::base::data::kind::blob:
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
	return datastore.get_metadata().size();
}


void pof::DataModel::SetSpecialColumnHandler(size_t column, get_function_t&& function)
{
	auto [iter, inserted] = mSpecialColHandlers.insert({ column, {function, nullptr} });
	//if insertion fails assume replacement of handlers for the column
	if (!inserted) {
		iter->second.first = function;
	}
}

void pof::DataModel::SetSpecialColumnHandler(size_t column, get_function_t&& get_function, set_function_t&& set_function)
{
	auto [iter, inserted] = mSpecialColHandlers.insert({ column, {get_function, set_function} });
	if (!inserted) {
		//column already has either a get or a set operation, assumn that iter wants to change them
		(*iter).second = { get_function, set_function };
	}
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
	const auto& [r, s] = datastore[i];
	const auto& [r2, s2] = datastore[i2];
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
	return (i >= datastore.size());
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

	const auto& [r, s] = datastore[i];
	const pof::base::data::kind k = datastore.get_metadata()[col];
	const auto& d = r[col];

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
		v = fmt::format("{:%y-%m-%d}", fmt::localtime(t));
		break;
	}
	case pof::base::data::kind::text:
		v = boost::variant2::get<pof::base::data::text_t>(d);
		break;
	case pof::base::data::kind::blob:
		//cannot display a blob type
		break;
	default:
		break;
	}
}

bool pof::DataModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
	if (!item.IsOk() || col >= GetColumnCount()) return false;
	const size_t i = GetIdxFromItem(item);
	const auto iter = mSpecialColHandlers.find(col);
	if (iter != mSpecialColHandlers.end()) {
		auto& [get, set] = iter->second;
		if (set) {
			return set(i, col, variant);
		}
	}
	pof::base::data::kind k = datastore.get_metadata()[col];
	auto& [r, s] = datastore[i];
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
	default:
		return false;
	}
	datastore.tsModified(pof::base::data::clock_t::now());
	s.set(static_cast<std::underlying_type_t<pof::base::data::state>>(pof::base::data::state::MODIFIED));
	ValueChanged(item, col);
	sig(std::next(datastore.begin(), i), Signals::UPDATE);
	return true;
}

void pof::DataModel::Reload()
{
	Cleared();
	for (size_t i = 0; i < datastore.size(); i++) {
		ItemAdded(wxDataViewItem(0), wxDataViewItem(wxUIntToPtr(i + 1)));
	}
}

unsigned int pof::DataModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
	return -1;
}

wxDataViewItem pof::DataModel::GetParent(const wxDataViewItem& item) const
{
	return wxDataViewItem(0);
}

