#include <boost/variant2/variant.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include <ranges>
#include <chrono>
#include <vector>
#include <bitset>


namespace pof
{
    //represents data in the application
    using namespace std::literals::string_literals;
    using namespace std::literals::chrono_literals;
    namespace ch = std::chrono;
    namespace v2 = boost::variant2;

    namespace base{
        class Data{
        public:
            enum class state {
                MODIFIED,
                HIDDEN,
                MAX_STATE
            };

            using datetime_t = std::chrono::steady_clock::time_point;
            using text_t = std::string;
            using blob_t = std::vector<std::uint8_t>;

            using data_t = v2::variant<
                std::int32_t,
                std::int64_t,
                std::uint32_t,
                std::uint64_t,
                float,
                double,
                datetime_t,
                text_t,
                blob_t
            >;
            
            using state_t = std::bitset<static_cast<size_t>(std::underlying_type_t<state>(state::MAX_STATE))>;
            using row_t = std::pair<std::vector<data_t>, state_t>;
            using table_t = std::vector<row_t>;
            
            using iterator = table_t::iterator;
            using const_iterator = table_t::const_iterator;
            using value_type = table_t::value_type;

            //ctors
            Data();
            Data(size_t count);
            ~Data();

            inline Data(const Data& rhs) : 
                value(rhs.value),
                bModified(false),
                created(ch::steady_clock::now()),
                modified(ch::steady_clock::now())
            {}
            inline Data(Data&& rhs) noexcept :
                value(std::move(rhs.value)), 
                bModified(false),
                created(ch::steady_clock::now()),
                modified(ch::steady_clock::now())
            {}

            inline Data& operator=(const Data& rhs) { value = rhs.value; }
            inline Data& operator=(Data&& rhs) noexcept { value = std::move(rhs.value); }

            void insert(row_t&& row);
            void insert(const typename row_t::first_type& vals);
            void insert(const typename row_t::first_type& vals, const typename row_t::second_type& st);


            const row_t& at(size_t i ) const; //throws std::out_of_range if out of bands


            constexpr size_t size() const { return value.size(); }
            inline void clear() { value.clear(); }
            inline void reserve(size_t size) { value.reserve(size); }
            inline void resize(size_t size) { value.resize(size); }

            const row_t& operator[](size_t i) const;
            bool operator==(const Data& rhs) const = default;

            //get the underlying table
            inline const table_t& tab() const { return value; }

            inline iterator begin() { return value.begin(); }
            inline const_iterator begin() const { return value.begin(); }
            inline iterator end() { return value.end(); }
            inline const_iterator end() const { return value.end();}

            //modifying the wors


            template<typename Archive>
            void serialise(Archive& ar, const unsigned int version)  {
                for (auto& row : value) {
                    for (auto& v : row.first) {
                        if (bModified) {
                            
                        }
                    }
                }
            }


            inline constexpr datetime_t tsCreated() const { return created; }
            inline constexpr datetime_t tsModified() const { return modified; }
        private:
            //serialise flags
            bool bModified; //serialise only modified rows


            datetime_t created;
            datetime_t modified;

            table_t value;

        };

        class DataView : public std::ranges::view_interface<DataView>
        {
        public:
            DataView(const Data& data);
            DataView(Data::const_iterator beg, Data::const_iterator en);



        private:
            Data::const_iterator mEnd;
            Data::const_iterator mBegin;
        };

    };
};