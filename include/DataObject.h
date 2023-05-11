#pragma once
#include <wx/dataobj.h>
#include <Data.h>
#include <functional>
#include <boost/iostreams/device/array.hpp> //use boost iostreams
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <optional>
#include <utility>
#include <tuple>

#include <wx/treebase.h>

namespace pof {
	class DataObject : public wxDataObject
	{
	public:
		using row_t = pof::base::data::row_t;
		using meta_t = pof::base::data::metadata_t;

		//makes sure the types are copy constructable
		//holds a of the data
		//the data should 
		DataObject(const std::string& format, row_t& value, meta_t& meta) : mValue(value), mMeta(meta) {
			mFormat.SetId(format);
		}

		virtual ~DataObject() { }

		virtual size_t GetFormatCount(Direction dir) const override {
			//only supports the row_t format. 
			return 1;
		}

		virtual void GetAllFormats(wxDataFormat* formats, Direction dir) const override;
		virtual size_t GetDataSize(const wxDataFormat& format) const;
		virtual wxDataFormat GetPreferredFormat(Direction WXUNUSED(dir)) const override;

		virtual bool GetDataHere(const wxDataFormat& format, void* buffer) const override;
		virtual bool SetData(const wxDataFormat& format, size_t len, const void* buf) override;

		inline const row_t& GetValue() const { return mValue; }
		inline const meta_t& GetMeta() const { return mMeta; }
		inline constexpr const std::optional<row_t>& GetSetData() const { return mSetData; }
	private:
		//represents the value
		//make referece ? 
		std::optional<row_t> mSetData; //data we are copying into
		std::reference_wrapper<row_t> mValue;
		std::reference_wrapper<meta_t> mMeta;
		wxDataFormat mFormat;

	};

	class TreeItemDataObject : public wxDataObject
	{
	public:
		using data_t = std::tuple<wxTreeItemId, wxWindow*, std::string, int>;
		TreeItemDataObject(const data_t& item) {
			mItem = item;
			mFormat.SetId("TREEITEMID");
		}

		virtual size_t GetFormatCount(Direction dir) const override {
			//only supports the row_t format. 
			return 1;
		}

		virtual void GetAllFormats(wxDataFormat* formats, Direction dir) const override
		{
			formats[0] = mFormat;
		}
		virtual size_t GetDataSize(const wxDataFormat& format) const {
			size_t count = sizeof(std::get<0>(mItem).GetID()) + 
				sizeof(std::get<1>(mItem)) + sizeof(size_t) + std::get<2>(mItem).size() + sizeof(int);
			return count;
		}
		virtual wxDataFormat GetPreferredFormat(Direction WXUNUSED(dir)) const override
		{
			return mFormat;
		}

		virtual bool GetDataHere(const wxDataFormat& format, void* buffer) const override
		{
			if (!std::get<0>(mItem).IsOk()) return false;
			void* id = std::get<0>(mItem).GetID();
			std::memcpy(static_cast<char*>(buffer), &id, sizeof(void*));
			std::memcpy(static_cast<char*>(buffer) + sizeof(void*), &std::get<1>(mItem), sizeof(wxWindow*));
			size_t s = std::get<2>(mItem).size();
			std::memcpy(static_cast<char*>(buffer) + sizeof(void*) + sizeof(wxWindow*), &s, sizeof(size_t));
			std::memcpy(static_cast<char*>(buffer) + sizeof(void*) + 
					sizeof(wxWindow*) + sizeof(size_t), std::get<2>(mItem).data(), std::get<2>(mItem).size());
			size_t offset = sizeof(void*) +
				sizeof(wxWindow*) + sizeof(size_t) + std::get<2>(mItem).size();
			std::memcpy(static_cast<char*>(buffer) + offset, &std::get<3>(mItem), sizeof(int));
			return true;
		}
		virtual bool SetData(const wxDataFormat& format, size_t len, const void* buf) override
		{
			if (mFormat != format) return false;
			void* id = 0;
			wxWindow* win = 0;
			size_t size = 0;
			std::string text;
			int image = 0;
			std::memcpy(&id, static_cast<const char*>(buf), sizeof(void*));
			size_t offset = sizeof(void*);
			std::memcpy(&win, static_cast<const char*>(buf) + offset, sizeof(wxWindow*));
			offset += sizeof(wxWindow*);
			std::memcpy(&size, static_cast<const char*>(buf) + offset, sizeof(size_t));
			text.resize(size);
			offset += sizeof(size_t);
			std::memcpy(text.data(), static_cast<const char*>(buf) + offset, size);
			offset += size;
			std::memcpy(&image, static_cast<const char*>(buf) + offset, sizeof(int));

			mItem = std::make_tuple(wxTreeItemId(id), win, std::move(text), image);
			return true;
		}

		inline const data_t& GetItem() const { return mItem; }
	
	private:
		wxDataFormat mFormat;
		data_t mItem;
	};

};