/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/common-type.hpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/libcycliccommon is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * cyclicdb is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the COPYING file at the root of the source distribution for more details.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _CYCLIC_COMMON_TYPE_HPP_
#define _CYCLIC_COMMON_TYPE_HPP_

#include <stdexcept>
#include <variant>

namespace cyclic
{
    /**
     * Enumeration for specifying storable data type.
     */
    enum data_type : int16_t
    {
        CDB_DT_UNSPECIFIED = -1, ///< Unspecified data type
        CDB_DT_VOID = 0,         ///< Void data type (no data)
        CDB_DT_BOOLEAN,          ///< Boolean
        CDB_DT_SIGNED_8,         ///< Signed 8-bits/1-byte integer
        CDB_DT_UNSIGNED_8,       ///< Unsigned 8-bits/1-byte integer
        CDB_DT_SIGNED_16,        ///< Signed 16-bits/2-bytes integer
        CDB_DT_UNSIGNED_16,      ///< Unsigned 16-bits/2-bytes integer
        CDB_DT_SIGNED_32,        ///< Signed 32-bits/4-bytes integer
        CDB_DT_UNSIGNED_32,      ///< Unsigned 32-bits/4-bytes integer
        CDB_DT_SIGNED_64,        ///< Signed 64-bits/8-bytes integer
        CDB_DT_UNSIGNED_64,      ///< Unsigned 64-bits/8-bytes integer
        CDB_DT_FLOAT_4,          ///< 32-bits/4-bytes floating point number
        CDB_DT_FLOAT_8,          ///< 64-bits/8-bytes floating point number

        CDB_DT_MAX_TYPE,         ///< Number of specified type enumeration values.

        CDB_DT_BYTE = CDB_DT_UNSIGNED_8,                ///< Alias for CDB_DT_UNSIGNED_8
        CDB_DT_SHORT = CDB_DT_SIGNED_16,                ///< Alias for CDB_DT_SIGNED_16
        CDB_DT_UNSIGNED_SHORT = CDB_DT_UNSIGNED_16,     ///< Alias for CDB_DT_UNSIGNED_16
        CDB_DT_INTEGER = CDB_DT_SIGNED_32,              ///< Alias for CDB_DT_SIGNED_32
        CDB_DT_UNSIGNED_INTEGER = CDB_DT_UNSIGNED_32,   ///< Alias for CDB_DT_UNSIGNED_32
        CDB_DT_LONG = CDB_DT_SIGNED_32,                 ///< Alias for CDB_DT_SIGNED_32
        CDB_DT_UNSIGNED_LONG = CDB_DT_UNSIGNED_32,      ///< Alias for CDB_DT_UNSIGNED_32
        CDB_DT_LONGLONG = CDB_DT_SIGNED_64,             ///< Alias for CDB_DT_SIGNED_64
        CDB_DT_UNSIGNED_LONGLONG = CDB_DT_UNSIGNED_64,  ///< Alias for CDB_DT_UNSIGNED_64

        CDB_DT_FLOAT = CDB_DT_FLOAT_4,      ///< Alias for CDB_DT_FLOAT_4
        CDB_DT_DOUBLE = CDB_DT_FLOAT_8,     ///< Alias for CDB_DT_FLOAT_8

        CDB_DT_NULL = CDB_DT_VOID      ///< Alias for CDB_DT_VOID
    };

    /**
     * Report type mismatch error.
     */
    class type_exception : public std::runtime_error
    {
    public:
        type_exception():std::runtime_error(nullptr){}
        explicit type_exception(const std::string& what_arg):std::runtime_error(what_arg){}
        explicit type_exception(const char* what_arg):std::runtime_error(what_arg){}
        ~type_exception(){}
    };

    /**
     * Report no assigned value error.
     */
    class no_value_exception : public std::runtime_error
    {
    public:
        no_value_exception():std::runtime_error(nullptr){}
        explicit no_value_exception(const std::string& what_arg):std::runtime_error(what_arg){}
        explicit no_value_exception(const char* what_arg):std::runtime_error(what_arg){}
        ~no_value_exception(){}
    };

    /**
     * Define a null type.
     * Usefull to set null values.
     */
    struct null_t {
        null_t() = default;
        null_t(const null_t&) = default;
        null_t(null_t&&) = default;
        null_t& operator=(const null_t&) = default;
        null_t& operator=(null_t&&) = default;
    };

    /**
     * Null special value, to use to initiate a value to no content.
     */
    constexpr null_t null;

    /**
     * Type-union definition for all acceptable value types.
     * null_t means explicit null value.
     */
    typedef std::variant<null_t, bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double> var_value_t;

    template<typename T>
    struct get_visitor
    {
        T operator()(null_t)const{throw no_value_exception();}
        T operator()(bool val)const{return val ? 1 : 0;}
        T operator()(int8_t val)const{return (T)val;}
        T operator()(uint8_t val)const{return (T)val;}
        T operator()(int16_t val)const{return (T)val;}
        T operator()(uint16_t val)const{return (T)val;}
        T operator()(int32_t val)const{return (T)val;}
        T operator()(uint32_t val)const{return (T)val;}
        T operator()(int64_t val)const{return (T)val;}
        T operator()(uint64_t val)const{return (T)val;}
        T operator()(float val)const{return (T)val;}
        T operator()(double val)const{return (T)val;}
    };

    template<>
    struct get_visitor<bool>
    {
        bool operator()(null_t)const{throw no_value_exception();}
        bool operator()(bool val)const{return val;}
        bool operator()(int8_t val)const{return val!=0;}
        bool operator()(uint8_t val)const{return val!=0;}
        bool operator()(int16_t val)const{return val!=0;}
        bool operator()(uint16_t val)const{return val!=0;}
        bool operator()(int32_t val)const{return val!=0;}
        bool operator()(uint32_t val)const{return val!=0;}
        bool operator()(int64_t val)const{return val!=0;}
        bool operator()(uint64_t val)const{return val!=0;}
        bool operator()(float val)const{return val!=0.0;}
        bool operator()(double val)const{return val!=0.0;}
    };




    /**
     * Dataholder for storable values.
     * This class acts like a std::variant (or std::any) with conversion rules.
     * It is used to set and retrieve data from storage and datasets.
     */
    class value_t : public var_value_t
    {
    public:
        /** Default construct a value without any stored data.*/
        constexpr value_t() noexcept : var_value_t(null_t{}) {}

        /** Default construct a value with nullptr.*/
        constexpr value_t(std::nullptr_t&&) noexcept : var_value_t(null_t{}) {}


        /**
         * Move-construct a value.
         * @tparam T Type of value to set.
         * @param value Value to set.
         */
        template<typename T>
        constexpr value_t(const T& val):var_value_t(val){}

        /**
         * Move-construct a value.
         * @tparam T Type of value to set.
         * @param value Value to set.
         */
        template<typename T>
        constexpr value_t(T&& val) noexcept : var_value_t(std::forward<T>(val)){}

        /**
         * Assing a value to this datastorage.
         * @param other Value to assign.
         * @return This.
         */
        value_t& operator=(const var_value_t& other) {var_value_t::operator=(other); return *this;}

        /**
         * Assing a value to this datastorage.
         * @param other Value to assign.
         * @return This.
         */
        value_t& operator=(var_value_t&& other) noexcept {var_value_t::operator=(std::move(other)); return *this;}

        /**
         * Assing a value to this datastorage.
         * @param other Value to assign.
         * @return This.
         */
        template<typename T>
        constexpr value_t& operator=(const T& val) { var_value_t::operator=(val); return *this;}

        /**
         * Reset the value datastore.
         * Erase stored value if any.
         * Reset stored data type.
         */
        void reset() { *this = var_value_t{}; }

        /**
         * Retrieve the type of the stored data.
         * @return Type of stored data.
         */
        // NOTE there is a shift due to null == 0 (not unspec)
        data_type type()const { return data_type(((int16_t)index())); }

        /**
         * Test if a value is stored.
         * @return True if a value is stored, false otherwise.
         */
        bool has_value()const{return !std::holds_alternative<null_t>(*this);}

        /**
         * Test if a value is stored.
         * @return True if a value is stored, false otherwise.
         */
        operator bool()const{return has_value();}

        /**
         * Test if the assigned type is the specified one.
         */
        template<typename T>
        bool is()const noexcept{return std::holds_alternative<T>(*this);}

        /**
         * Retrieve the value, forcing type conversion.
         * @tparam T requested type to which cast stored value (if possible).
         * @return Stored value, casted if needed.
         */
        template<typename T> inline T value()const {
            const var_value_t& val = *this;
            return std::visit(get_visitor<T>{}, val);
        }

        /**
         * Retrieve the value, forcing type conversion and a default value if no data is stored.
         * @tparam T requested type to which cast stored value (if possible).
         * @param default_value Default value to return if nothing is stored.
         * @return Stored value, casted if needed, or default value if nothing is stored.
         */
        template<typename T> inline T value_or(T default_value)const {
            try {
                return value<T>();
            } catch(no_value_exception&) {
                return default_value;
            }
        }

        /**
         * Retrieve the value if, and only if, its type strictly.corresponds to the requested one.
         * @tparam T requested type.
         * @return Stored value.
         * @throws std::bad_variant_access if type does not match
         */
        template<typename T> inline T value_strict()const {
            return std::get<T>(*this);
        }

        /**
         * Retrieve the value if, and only if, its type strictly.corresponds to the requested one.
         * If no value is stored, return the default value passed as parameter.
         * @tparam T requested type.
         * @param default_value Default value to return if nothing is stored.
         * @return Stored value.
         * @throws std::bad_variant_access if type does not match
         */
        template<typename T> inline T value_strict_or(T default_value)const {
            if(std::holds_alternative<std::monostate>(*this))
                return default_value;
            else
                return std::get<T>(*this);
        }

        /**
         * Assign a value to this dataholder.
         * @tparam T Type of value to assign.
         * @param value New value to assign.
         * @return This
         */
        template<typename T> inline value_t& value(T value) {
            var_value_t::emplace(value);
            return *this;
        }

    };

} // namespace cyclic
#endif // _CYCLIC_COMMON_TYPE_HPP_