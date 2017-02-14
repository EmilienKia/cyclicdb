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
        CDB_DT_DOUBLE = CDB_DT_FLOAT_8      ///< Alias for CDB_DT_FLOAT_8
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
    typedef struct {} null_t;

    /**
     * Null special value, to use to initiate a value to no content.
     */
    constexpr null_t null;

    /**
     * Dataholder for storable values.
     * This class acts like a std::variant (or std::any) with conversion rules.
     * It is used to set and retrieve data from storage and datasets.
     */
    class value_t
    {
    protected:
        data_type _type = CDB_DT_UNSPECIFIED;
        union
        {
            bool b;
            uint8_t u8;
            int8_t i8;
            uint16_t u16;
            int16_t i16;
            uint32_t u32;
            int32_t i32;
            uint64_t u64;
            int64_t i64;
            float f4;
            double f8;
        } _value;
    public:
        /** Default construct a value without any stored data.*/
        value_t() = default;

        /** Copy a value.*/
        value_t(const value_t& other):_type(other._type),_value(other._value){}

        /**
         * Construct a value directly set a value.
         * @tparam T Type of value to set.
         * @param value Value to set.
         */
        template<typename T> inline value_t(T value){}

        /**
         * Construct a value directly set a value and force the underlying type.
         * @tparam T Type of value passed in parameter.
         * @param val Value to set.
         * @param type Type to force to underlying storage.
         */
        template<typename T> inline value_t(T val, data_type type){value(val, type);}

        /**
         * Assin a value to this datastorage.
         * @param other Value to assign.
         * @return This.
         */
        value_t& operator=(const value_t& other) {_type = other._type; _value = other._value; return *this;}

        /**
         * Reset the value datastore.
         * Erase stored value if any.
         * Reset stored data type.
         */
        void reset() {_type = CDB_DT_UNSPECIFIED;}

        /**
         * Retrieve the type of the stored data.
         * @return Type of stored data.
         */
        data_type type()const{return _type;}

        /**
         * Test if a value is stored.
         * @return True if a value is stored, false otherwise.
         */
        bool has_value()const{return _type!=CDB_DT_UNSPECIFIED;}

        /**
         * Test if a value is stored.
         * @return True if a value is stored, false otherwise.
         */
        operator bool()const{return _type!=CDB_DT_UNSPECIFIED;}

        /**
         * Retrieve the value, forcing type conversion.
         * @tparam T requested type to which cast stored value (if possible).
         * @return Stored value, casted if needed.
         */
        template<typename T> inline T value()const;

        /**
         * Retrieve the value, forcing type conversion and a default value if no data is stored.
         * @tparam T requested type to which cast stored value (if possible).
         * @param default_value Default value to return if nothing is stored.
         * @return Stored value, casted if needed, or default value if nothing is stored.
         */
        template<typename T> inline T value_or(T default_value)const;

        /**
         * Retrieve the value if, and only if, its type strictly.corresponds to the requested one.
         * @tparam T requested type.
         * @return Stored value.
         */
        template<typename T> inline T value_strict()const;

        /**
         * Retrieve the value if, and only if, its type strictly.corresponds to the requested one.
         * If no value is stored, return the default value passed as parameter.
         * @tparam T requested type.
         * @param default_value Default value to return if nothing is stored.
         * @return Stored value.
         */
        template<typename T> inline T value_strict_or(T default_value)const;

        /**
         * Assign a value to this dataholder.
         * @tparam T Type of value to assign.
         * @param value New value to assign.
         * @return This
         */
        template<typename T> inline value_t& value(T value);

        /**
         * Assign a value to this dataholder, forcing cast to a specified underlying type.
         * @tparam T Type of value to assign.
         * @param value New value to assign.
         * @param type Type to force for underlying store.
         * @return This
         */
        template<typename T> inline value_t& value(T value, data_type type);

        /**
         * Assign a value to this dataholder.
         * @tparam T Type of value to assign.
         * @param value New value to assign.
         * @return This
         */
        template<typename T> inline value_t& operator=(T val){return value(val);}
    };

    template<> inline value_t::value_t<std::nullptr_t>(std::nullptr_t):_type(CDB_DT_UNSPECIFIED){}
    template<> inline value_t::value_t<cyclic::null_t>(cyclic::null_t):_type(CDB_DT_UNSPECIFIED){}
    template<> inline value_t::value_t<bool>(bool value):_type(CDB_DT_BOOLEAN){_value.b=value;}
    template<> inline value_t::value_t<int8_t>(int8_t value):_type(CDB_DT_SIGNED_8){_value.i8=value;}
    template<> inline value_t::value_t<uint8_t>(uint8_t value):_type(CDB_DT_UNSIGNED_8){_value.u8=value;}
    template<> inline value_t::value_t<int16_t>(int16_t value):_type(CDB_DT_SIGNED_16){_value.i16=value;}
    template<> inline value_t::value_t<uint16_t>(uint16_t value):_type(CDB_DT_UNSIGNED_16){_value.u16=value;}
    template<> inline value_t::value_t<int32_t>(int32_t value):_type(CDB_DT_SIGNED_32){_value.i32=value;}
    template<> inline value_t::value_t<uint32_t>(uint32_t value):_type(CDB_DT_UNSIGNED_32){_value.u32=value;}
    template<> inline value_t::value_t<int64_t>(int64_t value):_type(CDB_DT_SIGNED_64){_value.i64=value;}
    template<> inline value_t::value_t<uint64_t>(uint64_t value):_type(CDB_DT_UNSIGNED_64){_value.u64=value;}
    template<> inline value_t::value_t<float>(float value):_type(CDB_DT_FLOAT_4){_value.f4=value;}
    template<> inline value_t::value_t<double>(double value):_type(CDB_DT_FLOAT_8){_value.f8=value;}

    template<> inline bool value_t::value()const
    {
        switch(_type)
        {
        case CDB_DT_BOOLEAN: return _value.b;
        case CDB_DT_SIGNED_8: return _value.i8 != 0;
        case CDB_DT_UNSIGNED_8: return _value.u8 != 0;
        case CDB_DT_SIGNED_16: return _value.i16 != 0;
        case CDB_DT_UNSIGNED_16: return _value.u16 != 0;
        case CDB_DT_SIGNED_32: return _value.i32 != 0;
        case CDB_DT_UNSIGNED_32: return _value.u32 != 0;
        case CDB_DT_SIGNED_64: return _value.i64 != 0;
        case CDB_DT_UNSIGNED_64: return _value.u64 != 0;
        case CDB_DT_FLOAT_4: return _value.f4 != 0;
        case CDB_DT_FLOAT_8: return _value.f8 != 0;
        case CDB_DT_UNSPECIFIED: throw no_value_exception();
        default: throw type_exception("Type not supported");
        }
    }

    template<typename T> inline T value_t::value()const
    {
        switch(_type)
        {
        case CDB_DT_BOOLEAN: return _value.b ? 1 : 0;
        case CDB_DT_SIGNED_8: return _value.i8;
        case CDB_DT_UNSIGNED_8: return _value.u8;
        case CDB_DT_SIGNED_16: return _value.i16;
        case CDB_DT_UNSIGNED_16: return _value.u16;
        case CDB_DT_SIGNED_32: return _value.i32;
        case CDB_DT_UNSIGNED_32: return _value.u32;
        case CDB_DT_SIGNED_64: return _value.i64;
        case CDB_DT_UNSIGNED_64: return _value.u64;
        case CDB_DT_FLOAT_4: return _value.f4;
        case CDB_DT_FLOAT_8: return _value.f8;
        case CDB_DT_UNSPECIFIED: throw no_value_exception();
        default: throw type_exception("Type not supported");
        }
    }

    template<> inline bool value_t::value_or(bool default_value)const
    {
        switch(_type)
        {
        case CDB_DT_BOOLEAN: return _value.b;
        case CDB_DT_SIGNED_8: return _value.i8 != 0;
        case CDB_DT_UNSIGNED_8: return _value.u8 != 0;
        case CDB_DT_SIGNED_16: return _value.i16 != 0;
        case CDB_DT_UNSIGNED_16: return _value.u16 != 0;
        case CDB_DT_SIGNED_32: return _value.i32 != 0;
        case CDB_DT_UNSIGNED_32: return _value.u32 != 0;
        case CDB_DT_SIGNED_64: return _value.i64 != 0;
        case CDB_DT_UNSIGNED_64: return _value.u64 != 0;
        case CDB_DT_FLOAT_4: return _value.f4 != 0;
        case CDB_DT_FLOAT_8: return _value.f8 != 0;
        case CDB_DT_UNSPECIFIED: return default_value;
        default: throw type_exception("Type not supported");
        }
    }

    template<typename T> inline T value_t::value_or(T default_value)const
    {
        switch(_type)
        {
        case CDB_DT_BOOLEAN: return _value.b ? 1 : 0;
        case CDB_DT_SIGNED_8: return _value.i8;
        case CDB_DT_UNSIGNED_8: return _value.u8;
        case CDB_DT_SIGNED_16: return _value.i16;
        case CDB_DT_UNSIGNED_16: return _value.u16;
        case CDB_DT_SIGNED_32: return _value.i32;
        case CDB_DT_UNSIGNED_32: return _value.u32;
        case CDB_DT_SIGNED_64: return _value.i64;
        case CDB_DT_UNSIGNED_64: return _value.u64;
        case CDB_DT_FLOAT_4: return _value.f4;
        case CDB_DT_FLOAT_8: return _value.f8;
        case CDB_DT_UNSPECIFIED: return default_value;
        default: throw type_exception("Type not supported");
        }
    }

    template<> inline bool value_t::value_strict<bool>()const {if(_type==CDB_DT_BOOLEAN) return _value.b; else throw type_exception("bool type strictly required"); }
    template<> inline int8_t value_t::value_strict<int8_t>()const {if(_type==CDB_DT_SIGNED_8) return _value.i8; else throw type_exception("signed8 type strictly required"); }
    template<> inline uint8_t value_t::value_strict<uint8_t>()const {if(_type==CDB_DT_UNSIGNED_8) return _value.u8; else throw type_exception("unsigned8 type strictly required"); }
    template<> inline int16_t value_t::value_strict<int16_t>()const {if(_type==CDB_DT_SIGNED_16) return _value.i16; else throw type_exception("signed16 type strictly required"); }
    template<> inline uint16_t value_t::value_strict<uint16_t>()const {if(_type==CDB_DT_UNSIGNED_16) return _value.u16; else throw type_exception("unsigned16 type strictly required"); }
    template<> inline int32_t value_t::value_strict<int32_t>()const {if(_type==CDB_DT_SIGNED_32) return _value.i32; else throw type_exception("signed32 type strictly required"); }
    template<> inline uint32_t value_t::value_strict<uint32_t>()const {if(_type==CDB_DT_UNSIGNED_32) return _value.u32; else throw type_exception("unsigned32 type strictly required"); }
    template<> inline int64_t value_t::value_strict<int64_t>()const {if(_type==CDB_DT_SIGNED_64) return _value.i64; else throw type_exception("signed64 type strictly required"); }
    template<> inline uint64_t value_t::value_strict<uint64_t>()const {if(_type==CDB_DT_UNSIGNED_64) return _value.u64; else throw type_exception("unsigned64 type strictly required"); }
    template<> inline float value_t::value_strict<float>()const {if(_type==CDB_DT_FLOAT_4) return _value.f4; else throw type_exception("float4 type strictly required"); }
    template<> inline double value_t::value_strict<double>()const {if(_type==CDB_DT_FLOAT_8) return _value.f8; else throw type_exception("float8 type strictly required"); }

    template<> inline bool value_t::value_strict_or<bool>(bool default_value)const {if(_type==CDB_DT_BOOLEAN) return _value.b; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("bool type strictly required"); }
    template<> inline int8_t value_t::value_strict_or<int8_t>(int8_t default_value)const {if(_type==CDB_DT_SIGNED_8) return _value.i8; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("signed8 type strictly required"); }
    template<> inline uint8_t value_t::value_strict_or<uint8_t>(uint8_t default_value)const {if(_type==CDB_DT_UNSIGNED_8) return _value.u8; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("unsigned8 type strictly required"); }
    template<> inline int16_t value_t::value_strict_or<int16_t>(int16_t default_value)const {if(_type==CDB_DT_SIGNED_16) return _value.i16; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("signed16 type strictly required"); }
    template<> inline uint16_t value_t::value_strict_or<uint16_t>(uint16_t default_value)const {if(_type==CDB_DT_UNSIGNED_16) return _value.u16; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("unsigned16 type strictly required"); }
    template<> inline int32_t value_t::value_strict_or<int32_t>(int32_t default_value)const {if(_type==CDB_DT_SIGNED_32) return _value.i32; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("signed32 type strictly required"); }
    template<> inline uint32_t value_t::value_strict_or<uint32_t>(uint32_t default_value)const {if(_type==CDB_DT_UNSIGNED_32) return _value.u32; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("unsigned32 type strictly required"); }
    template<> inline int64_t value_t::value_strict_or<int64_t>(int64_t default_value)const {if(_type==CDB_DT_SIGNED_64) return _value.i64; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("signed64 type strictly required"); }
    template<> inline uint64_t value_t::value_strict_or<uint64_t>(uint64_t default_value)const {if(_type==CDB_DT_UNSIGNED_64) return _value.u64; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("unsigned64 type strictly required"); }
    template<> inline float value_t::value_strict_or<float>(float default_value)const {if(_type==CDB_DT_FLOAT_4) return _value.f4; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("float4 type strictly required"); }
    template<> inline double value_t::value_strict_or<double>(double default_value)const {if(_type==CDB_DT_FLOAT_8) return _value.f8; else if(_type==CDB_DT_UNSPECIFIED) return default_value; else throw type_exception("float8 type strictly required"); }

    template<> inline value_t& value_t::value<bool>(bool val){_type=CDB_DT_BOOLEAN; _value.b=val;}
    template<> inline value_t& value_t::value<int8_t>(int8_t val){_type=CDB_DT_SIGNED_8; _value.i8=val;}
    template<> inline value_t& value_t::value<uint8_t>(uint8_t val){_type=CDB_DT_UNSIGNED_8; _value.u8=val;}
    template<> inline value_t& value_t::value<int16_t>(int16_t val){_type=CDB_DT_SIGNED_16; _value.i16=val;}
    template<> inline value_t& value_t::value<uint16_t>(uint16_t val){_type=CDB_DT_UNSIGNED_16; _value.u16=val;}
    template<> inline value_t& value_t::value<int32_t>(int32_t val){_type=CDB_DT_SIGNED_32; _value.i32=val;}
    template<> inline value_t& value_t::value<uint32_t>(uint32_t val){_type=CDB_DT_UNSIGNED_32; _value.u32=val;}
    template<> inline value_t& value_t::value<int64_t>(int64_t val){_type=CDB_DT_SIGNED_64; _value.i64=val;}
    template<> inline value_t& value_t::value<uint64_t>(uint64_t val){_type=CDB_DT_UNSIGNED_64; _value.u64=val;}
    template<> inline value_t& value_t::value<float>(float val){_type=CDB_DT_FLOAT_4; _value.f4=val;}
    template<> inline value_t& value_t::value<double>(double val){_type=CDB_DT_FLOAT_8; _value.f8=val;}

    template<> inline value_t& value_t::value<bool>(bool value, data_type type)
    {
        _type = type;
        switch(type)
        {
        case CDB_DT_BOOLEAN: _value.b = value; break;
        case CDB_DT_SIGNED_8: _value.i8 = value?1:0; break;
        case CDB_DT_UNSIGNED_8: _value.u8 = value?1:0; break;
        case CDB_DT_SIGNED_16: _value.i16 = value?1:0; break;
        case CDB_DT_UNSIGNED_16: _value.u16 = value?1:0; break;
        case CDB_DT_SIGNED_32: _value.i32 = value?1:0; break;
        case CDB_DT_UNSIGNED_32: _value.u32 = value?1:0; break;
        case CDB_DT_SIGNED_64: _value.i64 = value?1:0; break;
        case CDB_DT_UNSIGNED_64: _value.u64 = value?1:0; break;
        case CDB_DT_FLOAT_4: _value.f4 = value?1:0; break;
        case CDB_DT_FLOAT_8: _value.f8 = value?1:0; break;
        case CDB_DT_UNSPECIFIED: throw no_value_exception();
        default: throw type_exception("Type not supported");
        }
        return *this;
    }

    template<typename T> inline value_t& value_t::value(T value, data_type type)
    {
        _type = type;
        switch(type)
        {
        case CDB_DT_BOOLEAN: _value.b = value?1:0; break;
        case CDB_DT_SIGNED_8: _value.i8 = value; break;
        case CDB_DT_UNSIGNED_8: _value.u8 = value; break;
        case CDB_DT_SIGNED_16: _value.i16 = value; break;
        case CDB_DT_UNSIGNED_16: _value.u16 = value; break;
        case CDB_DT_SIGNED_32: _value.i32 = value; break;
        case CDB_DT_UNSIGNED_32: _value.u32 = value; break;
        case CDB_DT_SIGNED_64: _value.i64 = value; break;
        case CDB_DT_UNSIGNED_64: _value.u64 = value; break;
        case CDB_DT_FLOAT_4: _value.f4 = value; break;
        case CDB_DT_FLOAT_8: _value.f8 = value; break;
        case CDB_DT_UNSPECIFIED: throw no_value_exception();
        default: throw type_exception("Type not supported");
        }
        return *this;
    }

} // namespace cyclic
#endif // _CYCLIC_COMMON_TYPE_HPP_