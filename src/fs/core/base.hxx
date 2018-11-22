
#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace fs0 {

enum class type_id : uint16_t {
	invalid_t = 0,
	object_t,
	bool_t,
// 	ushort_t,
// 	short_t,
// 	uint_t,
	int_t,
// 	ufloat_t,
	float_t,

	set_t,
	interval_t
	// ...
};

std::string to_string(const type_id& t);
type_id from_string(const std::string& t);
std::ostream& operator<<(std::ostream &os, const type_id& t);

/**
 * First 8 bits for type, rest for value.
 */
/*
class object_id {
public:
    using data_t = uint64_t;

    template <typename T>
    object_id(type_id t, T value)
        : _data(to_data(t, value))
    {}


    template <typename T>
    static data_t to_data(type_id t, T value) {
        static_assert(sizeof(T) <= 7, "Unsupported object_id value type");
        return ((data_t) t << 56) | value;
    }

private:
    data_t _data;
};

type_id type(const object_id& o) {
	return (type_id) ((o & 0xFFFF000000000000) >> 48);
}
*/

class object_id {
public:
	const static object_id INVALID;
	// Not sure we need these two at the moment:
// 	const static object_id FALSE; // For the sake of performance?
// 	const static object_id TRUE;

    using value_t = uint32_t;

	explicit object_id() : _type(type_id::invalid_t), _value(0) {}

private:

    template <typename T>
    explicit object_id(type_id t, T value)
        : _type(t), _value(value)
    { static_assert(sizeof(T) <= sizeof(value_t), "Unsupported object_id value type"); }

    template <typename T> friend object_id make_object(type_id t, T value);
	template <typename T> friend object_id make_object(const T& value);


public:
    //! TODO Might want to remove this operators in the future?
    explicit operator bool() const { return (bool) _value; }
    explicit operator int() const { return (int) _value; }


	~object_id()                           = default;
	object_id(const object_id&)            = default;
	object_id(object_id&&)                 = default;
	object_id& operator=(const object_id&) = default;
	object_id& operator=(object_id&&)      = default;

    // TODO - MAKE THESE TWO PRIVATE SO THAT THEY CAN ONLY BE ACCESSED
	//        THROUGH THE APPROPRIATE friend METHODS value() and o_type()
	inline type_id type() const { return _type; }
	inline value_t value() const { return _value; }

	// Required by Boost.serialization
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & _type;
		ar & _value;
	}

	//! Prints a representation of the state to the given stream.
	friend std::ostream& operator<<(std::ostream &os, const object_id& o) { return o.print(os); }
	std::ostream& print(std::ostream& os) const;

private:
    type_id _type;
	value_t _value;
};
static_assert(sizeof(object_id) == 8, "object_id should have overall size equal to 64 bits");

//! Compute a hash of the object ID
//! The method name is important, as it is the one used by boost:hash.
std::size_t hash_value(const object_id& o);

inline bool operator==(const object_id& lhs, const object_id& rhs) { return lhs.type() == rhs.type() && lhs.value() == rhs.value(); }
inline bool operator!=(const object_id& lhs, const object_id& rhs) { return !operator==(lhs,rhs); }
inline bool operator< (const object_id& lhs, const object_id& rhs) { return lhs.type() < rhs.type() || (lhs.type() == rhs.type() && lhs.value() < rhs.value()); }
inline bool operator> (const object_id& lhs, const object_id& rhs) { return  operator< (rhs,lhs); }
inline bool operator<=(const object_id& lhs, const object_id& rhs) { return !operator> (lhs,rhs); }
inline bool operator>=(const object_id& lhs, const object_id& rhs) { return !operator< (lhs,rhs); }


//! A range is made up of a minimum and a maximum value
//! both object_id types should coincide.
using type_range = std::pair<object_id, object_id>;
const type_range INVALID_TYPE_RANGE = std::make_pair(object_id::INVALID, object_id::INVALID);

class ObjectTable {
public:
	const static ObjectTable EMPTY_TABLE;

};

class set_t {
public:

};


class type_mismatch_error : public std::runtime_error {
public:
	type_mismatch_error() : std::runtime_error("Type Mismatch Error") {}
};

class value_mismatch_error : public std::runtime_error {
public:
	value_mismatch_error() : std::runtime_error("Value Mismatch Error") {}
};


inline type_id
o_type(const object_id& o) { return o.type(); }

template< typename T>
object_id::value_t raw_value( const object_id& o) { return (T)o.value(); }

template <typename T>
T value(const object_id& o, const ObjectTable& itp);
template <typename T>
T value(const object_id& o);


template <>
bool value(const object_id& o);
template <>
inline bool value(const object_id& o, const ObjectTable& itp) { return value<bool>(o); }



template <>
int32_t value(const object_id& o);
template <>
inline int32_t value(const object_id& o, const ObjectTable& itp) { return value<int32_t>(o); }


template <>
float value(const object_id& o);

template <>
inline float value(const object_id& o, const ObjectTable& itp) { return value<float>(o); }

template <typename T>
std::vector<T> values(const std::vector<object_id>& o, const ObjectTable& itp);


template <typename T>
inline object_id make_object(type_id t, T value) { return object_id(t, value); }


template <typename T>
object_id make_object(const T& value);

template <>
object_id make_object(const object_id& value);

template <>
object_id make_object(const bool& value);

template <>
object_id make_object(const int32_t& value);

template <>
object_id make_object(const float& value);

template <>
object_id make_object(const double& value);


template <typename T>
type_range make_range(const T& lower, const T& upper) {
	return std::make_pair(make_object<T>(lower), make_object<T>(upper));
}


} // namespaces


//! Define std::hashing for object_ids
namespace std {
  template <> struct hash<fs0::object_id> {
    size_t operator()(const fs0::object_id& o) const { return fs0::hash_value(o); }
  };
}
