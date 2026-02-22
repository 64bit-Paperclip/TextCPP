// ============================================================================
// MemoryCPP - High Performance Arena Allocator & Memory Utility Library
// ----------------------------------------------------------------------------
// File:        fixed_string.h
// Author:      Jason Penick
// Website:     630Studios.com
// Created:     2026
//
// Copyright (c) 2026 Jason Penick. All rights reserved.
//
// This software is provided under the terms outlined in LICENSE.txt
// See README.md for full documentation and usage examples
// ============================================================================

#pragma once


#ifndef __FIXED_STRING_H_GUARD
#define __FIXED_STRING_H_GUARD

#include <iostream>
#include <cassert>
#include <string>
#include <string_view>
#include <cstring>
#include <algorithm>


/// <summary>
/// A fixed-size string with a compile-time capacity stored inline within the object.
/// Provides allocation-free string storage by avoiding internal heap requests.
/// The data's location (stack, heap, or static) matches the location of the object itself.
/// </summary>
/// <typeparam name="N">The total buffer size in bytes, including the null terminator.</typeparam>
template<size_t N>
class FixedString
{

    public:
        /// <summary>
        /// The raw character buffer. Public to allow POD-style aggregate initialization.
        /// Always null-terminated after any Assign operation.
        /// </summary>
        char Data[N];

        /// <summary>
        /// Default constructor. Zero-initializes the entire buffer.
        /// </summary>
        FixedString() { std::memset(Data, 0, N); }

        /// <summary>
        /// Copy constructor. Copies the full buffer from another FixedString of the same capacity.
        /// </summary>
        FixedString(const FixedString&) = default;

        /// <summary>
        /// Copy assignment operator. Copies the full buffer from another FixedString of the same capacity.
        /// </summary>
        FixedString& operator=(const FixedString&) = default;

        /// <summary>
        /// Constructs a FixedString from a null-terminated C string. Null pointer is treated as empty string.
        /// </summary>
        /// <param name="str">The source C string. May be null.</param>
        FixedString(const char* str) { Assign(str); }

        /// <summary>
        /// Constructs a FixedString from a std::string.
        /// </summary>
        /// <param name="str">The source string.</param>
        FixedString(const std::string& str) { Assign(str.c_str()); }

        /// <summary>
        /// Constructs a FixedString from a std::string_view.
        /// </summary>
        /// <param name="sv">The source string view.</param>
        FixedString(std::string_view sv) { Assign(sv); }

        /// <summary>
        /// Assigns from a null-terminated C string. Null pointer is treated as empty string.
        /// </summary>
        /// <param name="str">The source C string. May be null.</param>
        /// <returns>Reference to this instance.</returns>
        FixedString& operator=(const char* str) { Assign(str); return *this; }

        /// <summary>
        /// Assigns from a std::string.
        /// </summary>
        /// <param name="str">The source string.</param>
        /// <returns>Reference to this instance.</returns>
        FixedString& operator=(const std::string& str) { Assign(str.c_str()); return *this; }

        /// <summary>
        /// Assigns from a std::string_view.
        /// </summary>
        /// <param name="sv">The source string view.</param>
        /// <returns>Reference to this instance.</returns>
        FixedString& operator=(std::string_view sv) { Assign(sv); return *this; }

        /// <summary>
        /// Equality comparison against another FixedString of potentially different capacity.
        /// Compares string contents, not buffer sizes.
        /// </summary>
        /// <param name="other">The FixedString to compare against.</param>
        /// <returns>True if string contents are identical.</returns>
        template<size_t M>
        bool operator==(const FixedString<M>& other) const { return std::strcmp(Data, other.Data) == 0; }

        /// <summary>
        /// Equality comparison against a null-terminated C string.
        /// A null pointer is never considered equal.
        /// </summary>
        /// <param name="other">The C string to compare against. May be null.</param>
        /// <returns>True if string contents are identical. False if other is null.</returns>
        bool operator==(const char* other) const
        {
            if (!other) return false;
            return std::strcmp(Data, other) == 0;
        }

        /// <summary>
        /// Equality comparison against a std::string_view.
        /// </summary>
        /// <param name="other">The string view to compare against.</param>
        /// <returns>True if string contents are identical.</returns>
        bool operator==(std::string_view other) const { return std::string_view(Data) == other; }

        /// <summary>
        /// Inequality comparison against a null-terminated C string.
        /// </summary>
        /// <param name="other">The C string to compare against. May be null.</param>
        /// <returns>True if string contents differ, or if other is null.</returns>
        bool operator!=(const char* other) const { return !(*this == other); }

        /// <summary>
        /// Inequality comparison against a std::string_view.
        /// </summary>
        /// <param name="other">The string view to compare against.</param>
        /// <returns>True if string contents differ.</returns>
        bool operator!=(std::string_view other) const { return !(*this == other); }

        /// <summary>
        /// Inequality comparison against another FixedString of potentially different capacity.
        /// Compares string contents, not buffer sizes.
        /// </summary>
        /// <param name="other">The FixedString to compare against.</param>
        /// <returns>True if string contents differ.</returns>
        template<size_t M>
        bool operator!=(const FixedString<M>& other) const { return !(*this == other); }



        /// <summary>
        /// Core assignment implementation. Copies up to N-1 characters from the source
        /// and null-terminates the result. Does not zero the remainder of the buffer.
        /// </summary>
        /// <param name="sv">The source string view.</param>
        /// <remarks>
        /// Asserts in debug builds that N is greater than zero and that the source
        /// does not exceed the buffer capacity (which would cause truncation).
        /// </remarks>
        void Assign(std::string_view sv)
        {
            assert(N > 0 && "FixedString capacity must be > 0");
            assert(sv.size() < N && "FixedString: input will be truncated");

            size_t copyLen = std::min(sv.size(), N - 1);        // Leave room for the null terminator

            if (copyLen > 0) {
                std::memcpy(Data, sv.data(), copyLen);
            }
            
            Data[copyLen] = '\0';                               // Null terminate exactly at the end of the content
        }

        /// <summary>
        /// Assignment from a null-terminated C string.
        /// Guards against null pointer before delegating to the string_view overload.
        /// </summary>
        /// <param name="str">The source C string. If null, the buffer is set to empty.</param>
        void Assign(const char* str)
        {
            if (!str)
            {
                Data[0] = '\0'; return;
            }

            Assign(std::string_view(str));
        }

        /// <summary>
        /// Returns a null-terminated pointer to the internal buffer.
        /// </summary>
        const char* c_str() const { return Data; }
        /// <summary>
        /// Returns true if the string is empty (first byte is null terminator).
        /// </summary>
        bool empty() const { return Data[0] == '\0'; }

        /// <summary>
        /// Returns the length of the string in characters, excluding the null terminator. O(n).
        /// </summary>
        size_t length() const { return std::strlen(Data); }

        /// <summary>
        /// The total buffer capacity in bytes, including space for the null terminator.
        /// Equivalent to the template parameter N. Available at compile time.
        /// </summary>
        static constexpr size_t Capacity = N;

        /// <summary>
        /// Implicit conversion to std::string_view. Does not allocate.
        /// </summary>
        operator std::string_view() const { return std::string_view(Data); }

        /// <summary>
        /// Implicit conversion to const char*. Returns a pointer to the internal buffer.
        /// </summary>
        operator const char* () const { return Data; }

        /// <summary>
        /// Explicitly converts the FixedString to a std::string, allocating a new string
        /// from the internal buffer. Prefer c_str() or implicit string_view conversion
        /// where possible to avoid unnecessary allocation.
        /// </summary>
        /// <returns>A new std::string containing the string contents.</returns>
        std::string ToString() const { return std::string(Data); }


        /// <summary>
        /// Stream output operator. Writes the string contents to the output stream.
        /// </summary>
        /// <param name="os">The output stream.</param>
        /// <param name="fs">The FixedString to write.</param>
        /// <returns>Reference to the output stream.</returns>
        friend std::ostream& operator<<(std::ostream& os, const FixedString& fs) { return os << fs.Data; }

        /// <summary>
        /// Concatenates a std::string_view with a FixedString.
        /// Returns a std::string since the resulting length is not known at compile time.
        /// Intended primarily for logging and diagnostic string building.
        /// </summary>
        /// <param name="lhs">The left-hand string view.</param>
        /// <param name="rhs">The right-hand FixedString.</param>
        /// <returns>A new std::string containing the concatenated result.</returns>
        friend std::string operator+(std::string_view lhs, const FixedString& rhs) { return std::string(lhs) + rhs.Data; }

        /// <summary>
        /// Concatenates a FixedString with a std::string_view.
        /// Returns a std::string since the resulting length is not known at compile time.
        /// Intended primarily for logging and diagnostic string building.
        /// </summary>
        /// <param name="lhs">The left-hand FixedString.</param>
        /// <param name="rhs">The right-hand string view.</param>
        /// <returns>A new std::string containing the concatenated result.</returns>
        friend std::string operator+(const FixedString& lhs, std::string_view rhs) { return std::string(lhs.Data) + std::string(rhs); }

        /// <summary>
        /// Concatenates a FixedString with a null-terminated C string.
        /// Returns a std::string since the resulting length is not known at compile time.
        /// A null pointer on the right-hand side is treated as an empty string.
        /// </summary>
        /// <param name="lhs">The left-hand FixedString.</param>
        /// <param name="rhs">The right-hand C string. May be null.</param>
        /// <returns>A new std::string containing the concatenated result.</returns>
        friend std::string operator+(const FixedString& lhs, const char* rhs) { return std::string(lhs.Data) + (rhs ? rhs : ""); }

        /// <summary>
        /// Concatenates a null-terminated C string with a FixedString.
        /// Returns a std::string since the resulting length is not known at compile time.
        /// A null pointer on the left-hand side is treated as an empty string.
        /// </summary>
        /// <param name="lhs">The left-hand C string. May be null.</param>
        /// <param name="rhs">The right-hand FixedString.</param>
        /// <returns>A new std::string containing the concatenated result.</returns>
        friend std::string operator+(const char* lhs, const FixedString& rhs) { return std::string(lhs ? lhs : "") + rhs.Data; }

        /// <summary>
        /// Concatenates two FixedStrings of potentially different capacities.
        /// Returns a std::string since the resulting length is not known at compile time.
        /// Intended primarily for logging and diagnostic string building.
        /// </summary>
        /// <param name="lhs">The left-hand FixedString.</param>
        /// <param name="rhs">The right-hand FixedString of potentially different capacity.</param>
        /// <returns>A new std::string containing the concatenated result.</returns>
        template<size_t M>
        friend std::string operator+(const FixedString& lhs, const FixedString<M>& rhs) { return std::string(lhs.Data) + rhs.Data; }
};



#endif