#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace types
{

/**
 * Struct that represents a value with corresponding flag,
 * that indicates value's validity
 */
template <typename T>
struct ValueWithFlag
{
    // Construct invalid object
    ValueWithFlag() :
        is_valid{false},
        value{}
    {
    }

    // Construct valid object with provided value
    ValueWithFlag(T value) :
        is_valid{true},
        value{value}
    {
    }

    bool is_valid;
    T value;
};

using Temperature = ValueWithFlag<float>;

}

#endif // TYPES_H
