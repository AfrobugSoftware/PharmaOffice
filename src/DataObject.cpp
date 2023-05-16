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
		if (datum.index() != static_cast<std::underlying_type_t<pof::base::data::kind>>(kind)) continue; //only not nulls
		count += sizeof(std::uint32_t); //index into meta
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
			count += boost::variant2::get<pof::base::data::uuid_t>(datum).size();
			break;
		case pof::base::data::kind::currency:
			count += boost::variant2::get<pof::base::data::currency_t>(datum).data().size();
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
	//boost::iostreams::stream<boost::iostreams::array_sink> stream{sink};

	boost::iostreams::filtering_ostreambuf streambuf;
	streambuf.push(sink);
	
	std::ostream stream(&streambuf);

	for (size_t i = 0; i < mMeta.get().size(); i++) {
		auto& datum = mValue.get().first[i];
		auto kind = mMeta.get()[i];
		if (datum.index() != static_cast<std::underlying_type_t<pof::base::data::kind>>(kind))
		{
			continue;
		}
		stream.write(reinterpret_cast<const char*>(&i), sizeof(std::uint32_t));
		switch (kind)
		{
		case pof::base::data::kind::int32:
			stream.write(reinterpret_cast<const char*>(&boost::variant2::get<std::int32_t>(datum)), sizeof(std::int32_t));
			break;
		case pof::base::data::kind::int64:
			stream.write(reinterpret_cast<const char*>(&boost::variant2::get<std::int64_t>(datum)), sizeof(std::int64_t));
			break;
		case pof::base::data::kind::uint32:
			stream.write(reinterpret_cast<const char*>(&boost::variant2::get<std::uint32_t>(datum)), sizeof(std::uint32_t));
			break;
		case pof::base::data::kind::uint64:
			stream.write(reinterpret_cast<const char*>(&boost::variant2::get<std::uint64_t>(datum)), sizeof(std::uint64_t));
			break;
		case pof::base::data::kind::float32:
			stream.write(reinterpret_cast<const char*>(&boost::variant2::get<float>(datum)), sizeof(float));
			break;
		case pof::base::data::kind::float64:
			stream.write(reinterpret_cast<const char*>(&boost::variant2::get<double>(datum)), sizeof(double));
			break;
		case pof::base::data::kind::datetime:
		{
			auto rep = boost::variant2::get<pof::base::data::datetime_t>(datum).time_since_epoch().count();
			stream.write(reinterpret_cast<const char*>(&rep), sizeof(rep));
			break;
		}
		case pof::base::data::kind::text:
		{
			auto& text = boost::variant2::get < pof::base::data::text_t>(datum);
			const auto size = text.size();
			stream.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
			stream.write(text.data(), text.size());
			break;
		}
		case pof::base::data::kind::blob:
		{
			auto& blob = boost::variant2::get < pof::base::data::blob_t>(datum);
			const auto size = static_cast<std::uint32_t>(blob.size());
			stream.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
			stream.write(reinterpret_cast<const char*>(blob.data()), blob.size());
			break;
		}
		case pof::base::data::kind::uuid:
		{
			auto& uuid = boost::variant2::get < pof::base::data::uuid_t>(datum);
			stream.write(reinterpret_cast<const char*>(uuid.data), uuid.size());
			break;
		}
		case pof::base::data::kind::currency:
		{
			auto& cur = boost::variant2::get<pof::base::data::currency_t>(datum);
			stream.write(reinterpret_cast<const char*>(cur.data().data()), cur.data().size());
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
	//boost::iostreams::stream<boost::iostreams::array_source> stream{ source };

	boost::iostreams::filtering_istreambuf streambuf;
	streambuf.push(source);
	std::istream stream(&streambuf);

	//what is that thing we are copying into
	mSetData.emplace(); //construct a new value
	auto& v = mSetData.value();
	v.first.resize(mMeta.get().size());

	std::uint32_t idx = 0;
	while(!stream.eof()) {
		stream.read(reinterpret_cast<char*>(&idx), sizeof(std::uint32_t));
		auto kind = mMeta.get()[idx];

		auto& datum = v.first[idx];
		switch (kind)
		{
		case pof::base::data::kind::int32:
		{
			std::int32_t value = 0;
			stream.read(reinterpret_cast<char*>(&value), sizeof(std::int32_t));
			datum = value;
			break;	
		}
		case pof::base::data::kind::int64:
		{
			std::int64_t value = 0;
			stream.read(reinterpret_cast<char*>(&value), sizeof(std::int64_t));
			datum = value;
			break;
		}
		case pof::base::data::kind::uint32:
		{
			std::uint32_t value = 0;
			stream.read(reinterpret_cast<char*>(&value), sizeof(std::uint32_t));
			datum = value;
			break;
		}
		case pof::base::data::kind::uint64:
		{
			std::uint64_t value = 0;
			stream.read(reinterpret_cast<char*>(&value), sizeof(std::uint64_t));
			datum = value;
			break;
		}
		case pof::base::data::kind::float32:
		{
			float value = 0.0f;
			stream.read(reinterpret_cast<char*>(&value), sizeof(float));
			datum = value;
			break;
		}
		case pof::base::data::kind::float64:
		{
			double value = 0.0;
			stream.read(reinterpret_cast<char*>(&value), sizeof(double));
			datum = value;
			break;
		}
		case pof::base::data::kind::datetime:
		{
			pof::base::data::clock_t::rep rep = 0;
			stream.read(reinterpret_cast<char*>(&rep), sizeof(rep));
			datum = pof::base::data::datetime_t(pof::base::data::clock_t::duration(rep));
			break;
		}
		case pof::base::data::kind::text:
		{
			std::uint32_t size = 0; 
			stream.read(reinterpret_cast<char*>(&size), sizeof(std::uint32_t));
			pof::base::data::text_t text;
			text.resize(size);
			stream.read(text.data(), size);
			datum = std::move(text);
			break;
		}
		case pof::base::data::kind::blob:
		{
			std::uint32_t size = 0;
			stream.read(reinterpret_cast<char*>(&size), sizeof(std::uint32_t));
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
		case pof::base::data::kind::currency:
		{
			pof::base::data::currency_t cur{};
			stream.read(reinterpret_cast<char*>(cur.data().data()), cur.data().size());
			datum = std::move(cur);
			break;

		}
		default:
			return false; //invalid metadata type
		}
	}
	return true;
}
