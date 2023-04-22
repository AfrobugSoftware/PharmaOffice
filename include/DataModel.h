#pragma once
#include <wx/dataview.h>

#include <tuple>

#include <fmt/chrono.h>

#include <Data.h>
#include <data_tuple.h>
#include <packages.h>
#include <datareq.h>

#include <boost/signals2.hpp>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <shared_mutex>

namespace std {
	template<>
	struct hash<wxDataViewItem>
	{
		size_t operator()(const wxDataViewItem& item) const noexcept
		{
			const size_t i = reinterpret_cast<size_t>(item.GetID());
			return std::hash<size_t>{}(i);
		}
	};
};

namespace pof {
	class DataModel : public wxDataViewModel
	{
	public:
		using iterator = pof::base::data::iterator;
		using const_iterator = pof::base::data::const_iterator;
		using row_type = pof::base::data::value_type;
		using item_attr_map = std::unordered_map<wxDataViewItem, std::shared_ptr<wxDataViewItemAttr>>;
		using get_data_req_t = pof::base::datareq<boost::beast::http::verb::get>;
		using set_data_req_t = pof::base::datareq<boost::beast::http::verb::post>;


		typedef std::function<wxVariant(size_t, size_t)> get_function_t;
		typedef std::function<bool(size_t, size_t, const wxVariant&)> set_function_t;
		typedef std::pair<get_function_t, set_function_t> SpeicalColHandler_t;

		using special_col_map = std::map<size_t, SpeicalColHandler_t>;

		enum class Signals {
				ADDED,
				REMOVED,
				UPDATE,
				LOADED,
				MAX
		};

		using signal_t = boost::signals2::signal<void(iterator, Signals)>;

		DataModel() : pack(datastore), unpack(datastore) {
		}
		
		DataModel(const DataModel& model);
		DataModel(DataModel&& model) noexcept;


		virtual ~DataModel() {}

		DataModel& operator=(const DataModel& model);
		DataModel& operator=(DataModel&& model) noexcept;



		pof::base::pack_t Pack() const;
		void Unpack(const pof::base::pack_t& package);
		void Emplace(pof::base::data&& d);
	

		static size_t GetIdxFromItem(const wxDataViewItem& item);


		bool AddAttr(const wxDataViewItem& item, std::shared_ptr<wxDataViewItemAttr> attr);
		void RemoveAttr(const wxDataViewItem& item);

		void SetSpecialColumnHandler(size_t column, get_function_t&& function);
		void SetSpecialColumnHandler(size_t column, get_function_t&& get_function, set_function_t&& set_function);
		void RemoveSpecialColumnHandler(size_t column);
		void SetSpecialSetColumnHandler(size_t column, set_function_t&& function);


		virtual bool HasContainerColumns(const wxDataViewItem& item) const override;
		virtual bool HasDefaultCompare() const override;
		virtual bool IsEnabled(const wxDataViewItem& item, unsigned int col) const override;
		virtual bool IsContainer(const wxDataViewItem& item) const override;
		virtual bool IsVirtualListModel() const override;
		virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const override;
		virtual wxString GetColumnType(unsigned int col) const override;
		virtual unsigned int GetColumnCount() const override;
		virtual int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const override;
		virtual bool HasValue(const wxDataViewItem& item, unsigned col) const override;
		virtual void GetValue(wxVariant& v, const wxDataViewItem& item, unsigned int col) const override;
		virtual bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col) override;
		virtual unsigned int GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const override;
		virtual wxDataViewItem GetParent(const wxDataViewItem& item) const;

		template<typename...args>
		void Adapt() {
			pof::base::adapt<args...>(datastore);
		}

		//inline std::shared_mutex& GetDatastoreMutex() { return datastoremutex; }
		inline pof::base::data& GetDatastore() { return datastore; }

		void Reload();
	private:
		//std::shared_mutex datastoremutex;
		pof::base::data datastore;
		pof::base::packer pack;
		pof::base::unpacker unpack;


		signal_t sig;
		item_attr_map attributes;
		special_col_map mSpecialColHandlers;

	};
};