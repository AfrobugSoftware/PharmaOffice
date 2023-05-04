#include "DataObject.h"

void pof::DataObject::GetAllFormats(wxDataFormat* formats, Direction dir) const
{
	//this fucntions is a source of bugs and it is unsafe, wxWidget code uses it
	formats[0] = mFormat;
}

size_t pof::DataObject::GetDataSize(const wxDataFormat& format) const
{
	assert(mMeta.get().size() == mValue.get().first.size() && "Meta does not match data");
	size_t count = 0;
	for (size_t i = 0; i < mValue.get().first.size(); i++) {
		auto& datum = mValue.get().first[i];
		auto kind = mMeta.get()[i];
		switch (kind)
		{
		case pof::base::data::kind::int32:
			count += sizeof(std::int32_t);
			break;
		case pof::base::data::kind::int64:
			count += sizeof(std::int64_t);
			break;
		case pof::base::data::kind::uint32:
			count += sizeof(std::uint32_t);
			break;
		case pof::base::data::kind::uint64:
			count += sizeof(std::uint64_t);
			break;
		case pof::base::data::kind::float32:
			count += sizeof(float);
			break;
		case pof::base::data::kind::float64:
			count += sizeof(double);
			break;
		case pof::base::data::kind::datetime:
			count += sizeof(pof::base::data::clock_t::duration::rep);
			break;
		case pof::base::data::kind::text:
			count += sizeof(std::uint32_t);
			count += boost::variant2::get<pof::base::data::text_t>(datum).size();
			break;
		case pof::base::data::kind::blob:
			count += sizeof(std::uint32_t);
			count += boost::variant2::get<pof::base::data::blob_t>(datum).size();
			break;
		case pof::base::data::kind::uuid:
			count += sizeof(std::uint32_t);
			count += boost::variant2::get<pof::base::data::uuid_t>(datum).size();
			break;
		default:
			break;
		}
	}
	return count;
}

wxDataFormat pof::DataObject::GetPreferredFormat(Direction WXUNUSED(dir)) const
{
	return mFormat;
}

bool pof::DataObject::GetDataHere(const wxDataFormat& format, void* buffer) const
{
	assert(mMeta.get().size() == mValue.get().first.size() && "Meta does not match data");
	boost::iostreams::array_sink sink{static_cast<char*>(buffer), GetDataSize(format)};
	boost::iostreams::stream<boost::iostreams::array_sink> stream{ sink };
	for (size_t i = 0; i < mMeta.get().size(); i++) {
		auto& datum = mValue.get().first[i];
		auto kind = mMeta.get()[i];
		switch (kind)
		{
		case pof::base::data::kind::int32:
			stream << boost::variant2::get<std::int32_t>(datum);
			break;
		case pof::base::data::kind::int64:
			stream << boost::variant2::get<std::int64_t>(datum);
			break;
		case pof::base::data::kind::uint32:
			stream << boost::variant2::get<std::uint32_t>(datum);
			break;
		case pof::base::data::kind::uint64:
			stream << boost::variant2::get<std::uint64_t>(datum);
			break;
		case pof::base::data::kind::float32:
			stream << boost::variant2::get<float>(datum);
			break;
		case pof::base::data::kind::float64:
			stream << boost::variant2::get<double>(datum);
			break;
		case pof::base::data::kind::datetime:
		{
			auto rep = boost::variant2::get<pof::base::data::datetime_t>(datum).time_since_epoch().count();
			stream << rep;
			break;
		}
		case pof::base::data::kind::text:
		{
			auto& text = boost::variant2::get < pof::base::data::text_t>(datum);
			stream << text.size();
			stream.write(text.data(), text.size());
			break;
		}
		case pof::base::data::kind::blob:
		{
			auto& blob = boost::variant2::get < pof::base::data::blob_t>(datum);
			stream << blob.size();
			stream.write(reinterpret_cast<const char*>(blob.data()), blob.size());
			break;
		}
		case pof::base::data::kind::uuid:
		{
			auto& uuid = boost::variant2::get < pof::base::data::uuid_t>(datum);
			stream.write(reinterpret_cast<const char*>(uuid.data), uuid.size());
			break;
		}
		default:
			return false;
		}
	}
	stream << std::flush;
	return true;
}

bool pof::DataObject::SetData(const wxDataFormat& format, size_t len, const void* buf)
{
	if (format != mFormat) return false;

	boost::iostreams::array_source source{ reinterpret_cast<const char*>(buf), len };
	boost::iostreams::stream<boost::iostreams::array_source> stream{ source };

	//what is that thing we are copying into
	mSetData.emplace(); //construct a new value
	auto& v = mSetData.value();
	v.first.resize(mMeta.get().size());

	for (size_t i = 0; i < mMeta.get().size(); i++) {
		auto kind = mMeta.get()[i];
		auto& datum = v.first[i];
		switch (kind)
		{
		case pof::base::data::kind::int32:
			stream >> boost::variant2::get<std::int32_t>(datum);
			break;
		case pof::base::data::kind::int64:
			stream >> boost::variant2::get<std::int64_t>(datum);
			break;
		case pof::base::data::kind::uint32:
			stream >> boost::variant2::get<std::uint32_t>(datum);
			break;
		case pof::base::data::kind::uint64:
			stream >> boost::variant2::get<std::uint64_t>(datum);
			break;
		case pof::base::data::kind::float32:
			stream >> boost::variant2::get<float>(datum);
			break;
		case pof::base::data::kind::float64:
			stream >> boost::variant2::get<double>(datum);
			break;
		case pof::base::data::kind::datetime:
		{
			pof::base::data::clock_t::rep rep = {};
			stream >> rep;
			datum = pof::base::data::datetime_t(pof::base::data::clock_t::duration(rep));
			break;
		}
		case pof::base::data::kind::text:
		{
			std::uint32_t size = 0; 
			stream >> size;
			pof::base::data::text_t text;
			text.resize(size);
			stream.read(text.data(), size);
			datum = std::move(text);
			break;
		}
		case pof::base::data::kind::blob:
		{
			std::uint32_t size = 0;
			stream >> size;
			pof::base::data::blob_t blob;
			blob.resize(size);
			stream.read(reinterpret_cast<char*>(blob.data()), size);
			datum = std::move(blob);
			break;
		}
		case pof::base::data::kind::uuid:
		{
			pof::base::data::uuid_t uuid = {0};
			stream.read(reinterpret_cast<char*>(uuid.data), uuid.size());
			datum = std::move(uuid);
			break;
		}
		default:
			return false; //invalid metadata type
		}

	}
	return true;
}
