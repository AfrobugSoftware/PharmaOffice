#pragma once
#include <wx/dataobj.h>
#include <Data.h>
#include <functional>
#include <boost/iostreams/device/array.hpp> //use boost iostreams
#include <boost/iostreams/stream.hpp>
#include <optional>

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
};