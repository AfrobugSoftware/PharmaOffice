#pragma once
#include <wx/dataview.h>

#include <tuple>

#include <fmt/chrono.h>

#include <Data.h>
#include <data_tuple.h>
#include <packages.h>

#include <boost/signals2.hpp>
#include <unordered_map>
#include <functional>

namespace std {
	template<>
	struct hash<wxDataViewItem>
	{
		constexpr size_t operator()(const wxDataViewItem& item) const noexcept
		{
			const size_t i = reinterpret_cast<size_t>(item.GetID());
			return std::hash<size_t>{}(i);
		}
	};
};

namespace pof {

	template<typename... args>
	class DataModel : public wxDataViewModel
	{
	public:
		using type_list = std::tuple<args...>;
		using iterator = pof::base::data::iterator;
		using const_iterator = pof::base::data::const_iterator;
		using row_type = pof::base::data::value_type;
		using item_attr_map = std::unordered_map<wxDataViewItem, std::shared_ptr<wxDataViewItemAttr>>;
		
		typedef std::function<wxVariant(size_t, size_t)> get_function_t;
		typedef std::function<bool(size_t, size_t, const wxVariant&)> set_function_t;
		typedef std::pair<get_function_t, set_function_t> SpeicalColHandler_t;

		using special_col_map = std::map<size_t, SpeicalColHandler_t>;

		enum class Signals {
				ADDED,
				REMOVED,
				CHANGED,
				LOADED,
				MAX
		};

		using signal_t = boost::signals2::signal<void(iterator, Signals)>;

		DataModel() : pack(datastore), unpack(datastore) {
			pof::base::adapt<args...>(datastore);
		}
		
		virtual ~DataModel() {}

		pof::base::pack_t Pack() {
			return pack();
		}
		void Unpack(const pof::base::pack_t& package) {
			unpack(package);
		}

		virtual bool HasContainerColumns(const wxDataViewItem& item) const
		{
			return false;
		}
		virtual bool HasDefaultCompare() const
		{
			return true;
		}

		virtual bool IsEnabled(const wxDataViewItem& item, unsigned int col) const
		{
			size_t index = wxPtrToUInt(item.GetID());
			index--;
			return (index < mData.size());
		}

		static size_t GetIdxFromItem(const wxDataViewItem& item) {
			size_t i = reinterpret_cast<size_t>(item.GetID());
			return (--i);
		}

		virtual bool IsContainer(const wxDataViewItem& item) const
		{
			return false;
		}
		virtual bool IsVirtualListModel() const
		{
			return false;
		}

		bool AddAttr(const wxDataViewItem& item, std::shared_ptr<wxDataViewItemAttr> attr){
			auto [iter, inserted] = attributes.insert({ item, attr });
			return inserted;
		}

		void RemoveAttr(const wxDataViewItem& item)
		{
			attributes.erase(item);
		}

		virtual unsigned int GetColumnCount() const
		{
			return datastore.get_metadata().size();
		}


		void SetSpecialColumnHandler(size_t column, get_function_t&& function)
		{
			auto [iter, inserted] = mSpecialColHandlers.insert({ column, {function, nullptr} });
			//if insertion fails assume replacement of handlers for the column
			if (!inserted) {
				iter->second.first = function;
			}
		}

		void SetSpecialColumnHandler(size_t column, get_function_t&& get_function, set_function_t&& set_function)
		{
			auto [iter, inserted] = mSpecialColHandlers.insert({ column, {get_function, set_function} });
			if (!inserted) {
				//column already has either a get or a set operation, assumn that iter wants to change them
				(*iter) = { get_function, set_function };
			}
		}


		void RemoveSpecialColumnHandler(size_t column)
		{
			mSpecialColHandlers.erase(column);
		}

		void SetSpecialSetColumnHandler(size_t column, set_function_t&& function)
		{
			auto [iter, inserted] = mSpecialColHandlers.insert({ column, {nullptr, function} });
			if (!inserted) {
				iter->second.second = function;
			}
		}

		virtual int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const
		{
			if (column >= GetColumnCount()) return 0;
			size_t i = GetIdxFromItem(item1);
			size_t i2 = GetIdxFromItem(item2);
			auto& [r, s] = datastore[i];
			auto& [r2, s] = datastore[i2];
			auto& val1 = r[column];
			auto& val2 = r2[column];

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

		virtual bool HasValue(const wxDataViewItem& item, unsigned col) const
		{
			const size_t i = GetIdxFromItem(item);
			return (i >= datastore.size());
		}

		virtual void GetValue(wxVariant& v, const wxDataViewItem& item, unsigned int col) const
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
				v = fmt::format("{:%y-%m-%d}", std::localtime(t));
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



	private:
		pof::base::data datastore;
		pof::base::packer pack;
		pof::base::unpacker unpack;

		item_attr_map attributes;
		special_col_map mSpecialColHandlers;

	};
};