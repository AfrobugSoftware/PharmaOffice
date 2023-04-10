#pragma once
#include <boost/variant2/variant.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/uuid/uuid.hpp>



#include <ranges>
#include <chrono>
#include <vector>
#include <bitset>
#include <cassert>


namespace pof
{
    //represents data in the application
    using namespace std::literals::string_literals;
    using namespace std::literals::chrono_literals;
    namespace ch = std::chrono;
    namespace v2 = boost::variant2;

    namespace base{
        class data{
        public:
            enum class state {
                CREATED,
                MODIFIED,
                HIDDEN,
                MAX_STATE
            };
            
            enum class kind : std::uint8_t {
                int32,
                int64,
                uint32,
                uint64,
                float32,
                float64,
                datetime,
                text,
                blob,
                uuid
            };

            using clock_t = std::chrono::system_clock;
            using datetime_t = clock_t::time_point;
            using text_t = std::string;
            using blob_t = std::vector<std::uint8_t>;
            using metadata_t = std::vector<kind>;
            using uuid_t = boost::uuids::uuid;

            using data_t = v2::variant<
                std::int32_t,
                std::int64_t,
                std::uint32_t,
                std::uint64_t,
                float,
                double,
                datetime_t,
                text_t,
                blob_t,
                uuid_t
            >;
            
            using state_t = std::bitset<static_cast<size_t>(std::underlying_type_t<state>(state::MAX_STATE))>;
            using row_t = std::pair<std::vector<data_t>, state_t>;
            using table_t = std::vector<row_t>;
            
            using iterator = table_t::iterator;
            using const_iterator = table_t::const_iterator;
            using value_type = table_t::value_type;

            //ctors
            data();
            data(size_t count);
            ~data();

            inline data(const data& rhs) : 
                metadata(rhs.metadata),
                value(rhs.value),
                bModified(rhs.bModified),
                created(rhs.created),
                modified(rhs.modified)
            {}
            inline data(data&& rhs) noexcept :
                metadata(std::move(rhs.metadata)),
                value(std::move(rhs.value)), 
                bModified(rhs.bModified),
                created(rhs.created),
                modified(rhs.modified)
            {}

            inline data& operator=(const data& rhs);
            inline data& operator=(data&& rhs) noexcept;

            //metadata functions
            inline void set_metadata(const metadata_t& md) { metadata = md; }
            constexpr const metadata_t& get_metadata() const { return metadata; }
            constexpr metadata_t& get_metadata() { return metadata; }


            void insert(row_t&& row);
            void insert(const typename row_t::first_type& vals);
            void insert(const typename row_t::first_type& vals, const typename row_t::second_type& st);
            void update(const typename row_t::first_type::value_type& d, size_t idx, size_t idy);


            void emplace(typename row_t::first_type&& vals);
            const row_t& at(size_t i ) const; //throws std::out_of_range if out of bands


            constexpr size_t size() const { return value.size(); }
            inline void clear() { value.clear(); }
            void clear_state(state s);
            void clear_state(size_t x, state s);

            void set_state(state s);
            void set_state(size_t x, state s);

            bool test_state(size_t x, state s);

            inline void reserve(size_t size) { value.reserve(size); }
            inline void resize(size_t size) { value.resize(size); }

            const row_t& operator[](size_t i) const;
            row_t& operator[](size_t i);
            bool operator==(const data& rhs) const = default;

            //get the underlying table
            inline const table_t& tab() const { return value; }

            inline iterator begin() { return value.begin(); }
            inline const_iterator begin() const { return value.begin(); }
            inline iterator end() { return value.end(); }
            inline const_iterator end() const { return value.end();}

            //modifying the wors

            //TODO: test for the arch of the system, should not be built on a 32 bit machine?
            //or change the datatype on a 32 bit machine ?
            //how would the server handle different machine architecture?
            template<typename Archive>
            void save(Archive& ar, const unsigned int version = 1) const {
                assert(!metadata.empty());
                if (value.empty()) return; // nothing to serialise

                //forms the header of the data:
                //write time created and time last modified
                ar & created.time_since_epoch().count();
                ar & modified.time_since_epoch().count();
                ar & static_cast<std::uint32_t>(value.size()); //size of rows
                ar & static_cast<std::uint32_t>(value[0].first.size()); //write the size of the columns
                ar & static_cast<std::uint32_t>(metadata.size());
                for (auto& k : metadata) {
                    ar & static_cast<std::underlying_type_t<kind>>(k);
                }

;                for (auto& row : value) 
                 {
                        auto& [r, s] = row;
                        const size_t size = r.size();
                        if (!s.test(static_cast<std::underlying_type_t<state>>(state::CREATED)) && !s.test(static_cast<std::underlying_type_t<state>>(state::MODIFIED))) {
                            continue;
                        }

                        for (int i = 0; i < size; i++) {
                            auto k = metadata[i];
                            auto& d = r[i];
                            switch (k)
                            {
                            case pof::base::data::kind::int32:
                                 ar & boost::variant2::get<std::int32_t>(d);
                                break;
                            case pof::base::data::kind::int64:
                                ar & boost::variant2::get<std::int64_t>(d);
                                break;
                            case pof::base::data::kind::uint32:
                                ar & boost::variant2::get<std::uint32_t>(d);
                                break;
                            case pof::base::data::kind::uint64:
                                ar & boost::variant2::get<std::uint64_t>(d);
                                break;
                            case pof::base::data::kind::float32:
                                ar & boost::variant2::get<float>(d);
                                break;
                            case pof::base::data::kind::float64:
                                ar & boost::variant2::get<double>(d);
                                break;
                            case pof::base::data::kind::datetime:
                            {
                                auto& t = boost::variant2::get<datetime_t>(d);
                                ar & t.time_since_epoch().count();
                                break;
                            }
                            case pof::base::data::kind::text:
                            {
                                ar & boost::variant2::get<text_t>(d);
                                break;
                            }
                            case pof::base::data::kind::blob:
                                ar & boost::variant2::get<blob_t>(d);
                                break;
                            case pof::base::data::kind::uuid:
                            {
                                ar & boost::variant2::get<uuid_t>(d).data;
                                break;
                            }
                            default:
                                break;
                            }
                        }
                }
            }

            template<typename Archiver>
            void load(Archiver& ar, const unsigned int version = 1){
                //read the header
                clock_t::duration::rep rep = 0; 
                std::uint32_t rowsize = 0, size = 0, colsize = 0;
                ar >> rep;
                created = datetime_t(clock_t::duration(rep));
                ar >> rep;
                modified = datetime_t(clock_t::duration(rep));

                ar >> rowsize;
                value.reserve(size);

                ar >> colsize; //save for later
                
                //metadata size
                ar >> size;
                metadata.resize(size);
                std::uint8_t k;
                for (int i = 0; i < metadata.size(); i++) {
                    ar >> k;
                    metadata[i] = static_cast<kind>(k);
                }
                
                for (int j = 0; j < rowsize; j++) {

                    value.emplace_back(row_t{});
                    auto& r = value.back().first;
                    r.reserve(colsize);

                    for (int i = 0; i < colsize; i++)
                    {
                        auto k = metadata[i];
                        switch (k)
                        {
                        case pof::base::data::kind::int32:
                        {
                            std::int32_t temp = 0;
                            ar >> temp;
                            r.emplace_back(temp);
                            break;
                        }
                        case pof::base::data::kind::int64:
                        {
                            std::int64_t temp = 0;
                            ar >> temp;
                            r.emplace_back(temp);
                            break;
                        }
                        case pof::base::data::kind::uint32:
                        {
                            std::uint32_t temp = 0;
                            ar >> temp;
                            r.emplace_back(temp);
                            break;
                        }
                        case pof::base::data::kind::uint64:
                        {
                            std::uint64_t temp = 0;
                            ar >> temp;
                            r.emplace_back(temp);
                            break;
                        }
                        case pof::base::data::kind::float32:
                        {
                            float temp = 0.0f;
                            ar >> temp;
                            r.emplace_back(temp);
                            break;
                        }
                        case pof::base::data::kind::float64:
                        {
                            double temp = 0.0;
                            ar >> temp;
                            r.emplace_back(temp);
                            break;
                        }
                        case pof::base::data::kind::datetime:
                        {
                            clock_t::duration::rep rep = 0;
                            ar >> rep;
                            r.emplace_back(datetime_t(clock_t::duration(rep)));
                            break;
                        }
                        case pof::base::data::kind::text:
                        {
                            text_t temp{};
                            ar >> temp;
                            r.emplace_back(std::move(temp));
                            break;
                        }
                        case pof::base::data::kind::blob:
                        {
                            blob_t temp{};
                            ar >> temp;
                            r.emplace_back(std::move(temp));
                            break;
                        }
                        case pof::base::data::kind::uuid:
                        {
                            uuid_t uuid;
                            ar >> uuid.data;
                            r.emplace_back(std::move(uuid));
                        }
                        default:
                            break;
                        }
                    }
                }
            }


            inline constexpr datetime_t tsCreated() const { return created; }
            inline constexpr datetime_t tsModified() const { return modified; }

            inline constexpr void tsCreated(const datetime_t& dt) { created = dt; }
            inline constexpr void tsModified(const datetime_t& dt) { modified = dt; }
        private:
            friend class boost::serialization::access;
            BOOST_SERIALIZATION_SPLIT_MEMBER()
            //serialise flags
            bool bModified; //serialise only modified rows


            datetime_t created;
            datetime_t modified;

            metadata_t metadata; //holds type information of the varaints
            table_t value;

        };

        class dataView : public std::ranges::view_interface<dataView>
        {
        public:
            dataView(const pof::base::data& data_) : mBegin(data_.begin()),
                mEnd(data_.end())
            {};
            dataView(data::const_iterator beg, data::const_iterator en) : 
                mBegin(beg),
                mEnd(en)
            {}

        private:
            data::const_iterator mEnd;
            data::const_iterator mBegin;
        };

    };
};