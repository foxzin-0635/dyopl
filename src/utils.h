#pragma once
#include <string>
#include <cstdint>
#include <algorithm>

// a utils file from my previous internal works.

using namespace std;

// My most used function ever.
template<typename T>
static constexpr T extract_bits(T data, unsigned char size, unsigned char offset) {
    if (size == 0) return 0;
    
    return (data >> offset) & ((1ULL << size) - 1);
}

// GCC/Clang function to swap bits.
template<typename T>
static T flip_endian(T data) {
    return _builtin_bswap32(data); // clang
}

// That works.
template<typename numt>
static string to_str_hex(numt num, uint64_t fixedSize = 0, int8_t endian = -1) {
    bool negative = num < 0;

    using Unsigned = std::make_unsigned_t<numt>;
    Unsigned unum = negative ? -static_cast<Unsigned>(num)
                             : static_cast<Unsigned>(num);
    
    string str =  ((num == 0) ? "0" : "");
    string ltable = "0123456789ABCDEF";
    
    while (unum > 0) {
        str += ltable[unum & 0xF];
        unum >>= 4;
    }
    
    reverse(str.begin(), str.end());
    
    if (fixedSize != 0 && endian != -1) {
        if (endian == 0) {
            size_t strs = str.size();
            for (int i = strs; i < fixedSize; i++) {
                str = ((num < 0) ? "F" : "0") + str;
            }
        } else if (endian == 1) {
            size_t strs = str.size();
            for (int i = strs; i < fixedSize; i++) {
                str += ((num < 0) ? "F" : "0");
            }
        }
    }
    
    return str;
}

// not sure why i've did this though.
struct BaseHolder {
public:
    virtual ~BaseHolder() = default;
};

// and this too.
template<typename VT>
struct ValueHolder: BaseHolder {
protected:
    VT value;
public:
    ValueHolder(): value(VT()) {}
    ValueHolder(const VT& value): value(value) {}
    
    virtual VT& get() { return value; }
    virtual const VT& get() const { return value; }
    virtual void set(const VT& value) { this->value = value; }
};

// a bit deprecated with the new enum method.
template<typename T>
struct InheritableEnum {
private:
    T v;
public:
    constexpr InheritableEnum() : v(0) {}
    constexpr InheritableEnum(T v) : v(v) {}
    constexpr T& get() const { return v; }
    constexpr operator T() const { return v; }
    
    InheritableEnum& operator=(const T& value) {
        v = value;
        return *this;
    }
    
    InheritableEnum& operator=(InheritableEnum& other) {
        v = other.v;
        return *this;
    }
    
    InheritableEnum& operator=(const InheritableEnum& other) {
        v = other.v;
        return *this;
    }
    
    bool operator==(const T& value) {
        return v == value;
    }
    
    bool operator==(InheritableEnum& other) {
        return v == other.v;
    }
    
    bool operator==(const InheritableEnum& other) {
        return v == other.v;
    }
};