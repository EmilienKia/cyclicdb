/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/libstore-file-impl.cpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/libcyclicstore is free software: you can redistribute it and/or
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

#include "libstore-file-impl.hpp"

#include <iostream>
#include <sstream>

#include "common-file.hpp"

/**
 * @page cydbfile .cydb file format
 *
 * This page describe the format of .cydb table storage files.
 * As it is a binary format, it is described in byte-based blocks and some parts
 * are leaved reserved or padded for type alignement.
 *
 * Explicit characters or groups of characters are explains arrounded by quotes (' or ").
 * The must be treated as contiguous bytes strings.
 *
 * CYDB file structure
 * ===================
 *
 * CYDB storage files are composed of a header and a data storage parts.
 * In basic datastore these two parts are juxtaposed.
 *
 * CYDB Header structure
 * ---------------------
 *
 * The CYDB storage file header is composed of 4 sections which are juxtaposed:
 * * File header
 * * Storage structure
 * * Storage content index
 * * Field descriptions
 * * Additionnal data (not used yet)
 *
 * ### File header
 *
 * The file header is the common 8-bytes chunk that identifies CYDB files.
 * <table>
 *   <caption>CYDB file header</caption>
 *   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
 *   <tr>
 *     <th>File header (8 bytes)
 *     <td colspan="4">File marker "CYDB"</td>
 *     <td colspan="2">File format version "01"</td>
 *     <td colspan="2">Global file options</td>
 * </table>
 *
 * Where:
 * * File marker is a four chars value of "CYDB". 4 Bytes.
 * * File format version is a couple of char defining the version of file structure.
 *   Currently of value "01". 2 Bytes (version 0.1)
 * * Global file options. Flags characterizing content of file. Currently ignored. 2 bytes.
 *
 * ### Storage structure
 *
 * The storage structure block contains global definitions of data stored in this table store.
 *
 * <table>
 * <caption>CYDB 1.0 storage structure</caption>
 *   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
 *   <tr>
 *     <th rowspan="5">Storage structure (40 bytes)
 *     <td colspan="4">Header size</td>
 *     <td colspan="4">Record options</td>
 *   </tr>
 *   <tr>
 *     <td colspan="4">Record capacity</td>
 *     <td colspan="2">Field count</td>
 *     <td colspan="2">Reserved</td>
 *   </tr>
 *   <tr><td colspan="8">Record origin</td></tr>
 *   <tr><td colspan="8">Record duration time</td></tr>
 *   <tr>
 *     <td colspan="4">Record header size</td>
 *     <td colspan="4">Record size</td>
 *   </tr>
 * </table>
 *
 * Where:
 * * Header size: size of file header, including file marker, in bytes (4 bytes)
 * * Record options: specific record option flags, not used yet (4 bytes)
 * * Record capacity: number of record the table is able to store (4 bytes)
 * * Field count: number of fields (per record) (2 bytes)
 * * Record origin: Time of record origin (8 bytes)
 * * Record duration: duration of a record, 0 if record time is not set (8 bytes)
 * * Record header size: size of each record header, in bytes (4 bytes
 * * Record size: size of each record including their headers, in bytes (4 bytes)
 *
 * The storage structure is positionned at byte 8 in the file (size of file header block).
 *
 * ### Storage content index
 *
 * The storage content index is the table which have record index and position.
 * It describes the structure of data part.
 *
 * <table>
 * <caption>CYDB 1.0 Storage content index format</caption>
 *   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
 *   <tr>
 *     <th rowspan="4">Storage content index (32 bytes)</th>
 *     <td colspan="4">First index</td>
 *     <td colspan="4">unused</td>
 *   </tr>
 *   <tr>
 *     <td colspan="4">Min index</td>
 *     <td colspan="4">Min position</td>
 *   </tr>
 *   <tr>
 *     <td colspan="4">Max index</td>
 *     <td colspan="4">Max position</td>
 *   </tr>
 *   <tr><td colspan="8">Reserved</td></tr>
 * </table>
 *
 * Where:
 * * First index: index of the first record slot (position 0), if used, 0-based, -1 if not used (4 bytes)
 * * Min index: index of the first record, 0-based, -1 if no record (4 bytes)
 * * Min position: position of the first record, 0-based, -1 if no record (4 bytes)
 * * Max index: index of the last record, 0-based, min==max if one record, -1 if no record (4 bytes)
 * * Max position: position of the last record, 0-based, min==max if one record, -1 if no record (4 bytes)
 *
 * The storage content index is positionned at byte 48 in the file (size of file header and storage structure blocks).
 *
 * ### Field descriptions
 *
 * The field description block is the juxtaposition of field descriptors, one per field (see field count).
 *
 * <table>
 * <caption>CYDB 1.0 field description</caption>
 *   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
 *   <tr>
 *     <th rowspan="2">Field descriptor (8+1+xxx bytes)</th>
 *     <td colspan="2">Field type</td>
 *     <td colspan="2">Field size</td>
 *     <td colspan="2">Field offset</td>
 *     <td colspan="2">Field options</td>
 *   </tr>
 *   <tr>
 *     <td>Field name size</td>
 *     <td colspan="7">Field name ... (up to 255 bytes, can be empty)</td>
 *   </tr>
 * </table>
 *
 * Where:
 * * Field type: type of binary value stored for this field (2 bytes)
 * * Field size: size of binary value, in bytes (2 bytes)
 * * Field offset: field offset in the record (2 bytes)
 *   Begining of field data regarding to begining of all data of the field.
 *   Typically the first field has an offset of 0.
 * * Field options: Additionnal option flags for this field, unused yet (2 bytes)
 * * Field name size: Size of the field name, in bytes (1 byte)
 * * Field name: string representing the field name.
 *   Name should be composed of ASCII printable characters.
 *   Its size is specified in previous variable and can be 0 (empty name).
 *
 * ### Additionnal header content
 *
 * This section will add some other table-related data.
 * Not used yet.
 * Section is 0 byte length.
 *
 *
 * CYDB Data storage
 * -----------------
 *
 * The data part is a juxtaposition of record storage.
 * It is located at 'Header size' of the file.
 * Each record storage has the exact size specified in 'Record size'.
 * Each record is composed of
 * * a record header. Its size is defined in 'Record header size'.
 *   The record header is a bitmap to specify if a record field is set or not (empty/null or not).
 * * a suite of record field data.
 *   Each field data is located at 'field offset' after the begining of record field data part and have the 'Field size' size.
 *
 * The data storage can be viewed as a contiguous array of record.
 * Each record has a position (0-based) from the first (0) to the last (table storage capacity).
 *
 * For exemple:
 * <table>
 *   <caption>Example of data storage</caption>
 *   <tr><th>           <th>index   <th>pos<th>rec header<th>field 0<th>field 1<th>field 2<th>...<th>field r
 *   <tr><td>first index<td>2       <td>0
 *   <tr><td>           <td>3       <td>1
 *   <tr><td>max index  <td>4       <td>2
 *   <tr><td>           <td>not used<td>3
 *   <tr><td>           <td>not used<td>4
 *   <tr><td>           <td>...     <td>...
 *   <tr><td>           <td>not used<td>n-3
 *   <tr><td>           <td>not used<td>n-2
 *   <tr><td>min index  <td>0       <td>n-1
 *   <tr><td>           <td>1       <td>n
 * </table>
 *
 * With:
 * * field count: the number of field in a record.
 *   A field f is 0 <= f <= r, r = field_count-1
 * * record capacity: the maxmum number of records in the table.
 *   The position of a record is the 0-based indexed position of the record in the table.
 *   A record at position pos is 0 <= pos <= n, n = record_capacity-1.
 * * first index: the index of the record at position 0
 * * min index: the current lowest valid index of the table
 * * min position: the position of the index_min record
 * * max index: the current highest valid index of the table
 * * max position: the position of the index_max record
 *
 **/

/*
A table can be in many states (12):

 0   1   2   3   4   5   6   7   8   9   10  11

miax
                                                          +-
    miax         min min         #   #   #   #        0   |
                 #   #           #   #   #   #            +
                 #   #           max max max #        1   |
                 #   #                       #            +
                 #   #   min min         min #        2   |
                 #   #   #   #           #   #            +
        miax     max #   #   #   min     #   #        3   |
                     #   #   #   #       #   #            +
                     #   max #   #       #   #        4   |
                     #       #   #       #   #            +
                     #       #   #       #   #        ... |
                     #       #   #       #   #            +
                     #       #   #       #   max      n-1 |
                     #       #   #       #                +
            miax     max     max #   min #   min      n   |
                                                          +--

(with positions: n last possible position and xand y some intermediate positions with x<y)

No record:
 0 - there is no record
Only one record:
 1 - min=max at pos 0
 2 - min=max at pos x
 3 - min=max at pos n
Contiguous records:
 4 - min<max with min=0 and max=x
 5 - min<max with min=0 and max=n
 6 - min<max with min=x and max=y
 7 - min<max with min=x and max=n
Split records (after a loop):
 8 - min>max with min=x and max=y
 9 - min>max with min=n and max=x
10 - min>max with min=y and max=x and min=max+1
11 - min>max with min=n and max=n-1 and min=max+1
 */


namespace cyclic
{
namespace store
{
namespace impl
{

//
// table_impl
//


std::string file_table_impl::FILE_MARKER = "CYDB";

file_table_impl::~file_table_impl()
{
    if(_file)
    {
        _file.sync();
        _file.close();
    }
}

void file_table_impl::create(const std::string& filename, const std::vector<field_st>& fields,
        record_index_t record_capacity, record_time_t origin, record_time_t duration)
{
    base_table_impl::create(fields, record_capacity, origin, duration);
    _filename = filename;
    initialize_on_creation(fields);
}

void file_table_impl::initialize_on_creation(const std::vector<field_st>& fields)
{
    // Compute record header size (bitset)
    // Enough space to save one bit per field.
    _record_header_size = _field_count > 0 ? (_field_count - 1) / 8 + 1 : 0;

    // Compute record size
    // Enought space to save the record header and all fields.
    _record_size = _field_count * sizeof(uint64_t) + _record_header_size;

    // Compute table header size:
    _table_header_size = 8 + 40 + 32; // See file spec (File header + Storage structure + Storage content index)
    for(field_index_t f = 0; f < _field_count; ++f)
    {
        _table_header_size += 2 /*type*/ + 2 /*options*/
                + 2 /*size*/ + 2 /*offset*/
                + 1 /*name size*/ + fields[f].name.size();
    }

    // Compute table complete size
    _table_size = _table_header_size + _record_size * _record_capacity;

    // Initialize fields internal descriptors
    _fields.clear();
    _fields.reserve(fields.size());
    uint16_t offset = 0;
    for(field_index_t idx = 0; idx < fields.size(); ++idx)
    {
        const field_st& field = fields[idx];
        std::string name = field.name;
        if(name.size() > 255)
        {
            name = name.substr(0, 255);
        }
        uint16_t size = field_size(field.type);
        _fields.push_back(field_impl(field.type, idx, name, size, offset));
        offset += size;
    }

    // Really create the table file.
    create_table_file();
}

void file_table_impl::create_table_file()
{
    _file.create(_filename);
    if(!_file)
    {
        std::ostringstream stm;
        stm << "Error while creating table file " << _filename << std::endl;
        throw cyclic::io::io_exception(0, stm.str());
    }

    //
    // Table header
    //
    long zero = 0;
    // File header
    _file.write(FILE_MARKER.data(), 4); // File marker
    _file.write(_version_marker[0]); // File version marker
    _file.write(_version_marker[1]); // File version marker
    _file.write(_global_options); // Global options
    // Storage structure
    _file.write(_table_header_size); // Table header size
    _file.write(_record_options); // Record options
    _file.write(_record_capacity); // Record capacity (in slot count)
    _file.write(_field_count); // Field count
    _file.write<uint16_t>(0); // Reserved
    _file.write(_origin); // Record origin
    _file.write(_duration); // Record duration
    _file.write(_record_header_size); // Record header size
    _file.write(_record_size); // Record size
    // Storage content index
    _file.write(_first_index); // first index
    _file.write<uint32_t>(0); // Unused
    _file.write(_min_index); // min index
    _file.write(_min_position); // min position
    _file.write(_max_index); // max index
    _file.write(_max_position); // max position
    _file.write<uint64_t>(0); // Unused

    // Field descriptors
    for(field_index_t f = 0; f < _field_count; ++f)
    {
        const field_impl& field = _fields[f];
        _file.write(field._type);
        _file.write(field._size);
        _file.write(field._offset);
        _file.write<uint16_t>(0); // No option for now
        _file.write<uint8_t>((uint8_t) field._name.size());
        _file.write(field._name.data(), field._name.size());
    }

    //
    // Table content
    //
    uint8_t* record = new uint8_t[_record_size];
    for(size_t n = 0; n < _record_size; ++n)
    {
        record[n] = 0;
    }
    for(size_t n = 0; n < _record_capacity; ++n)
    {
        _file.write(record, _record_size);
    }

    //
    // Flush
    //
    _file.sync();
}

void file_table_impl::open(const std::string& filename, const io::file& file, const std::string& /*version*/)
{
    if(filename.empty())
    {
        throw std::invalid_argument{"A file name must be provided to open a table storage "};
    }
    if(!file)
    {
        std::ostringstream stm;
        stm << "Error while opening table file " << _filename << std::endl;
        throw cyclic::io::io_exception(0, stm.str());
    }
    _filename = filename;
    _file = file;

    // End of file header (1 byte)
    _file.read(_global_options); // Global options

    // Storage structure (40 bytes)
    _file.read(_table_header_size); // Table header size
    _file.read(_record_options); // Record options
    _file.read(_record_capacity); // Record capacity (in slot count)
    _file.read(_field_count); // Field count
    _file.skip<2>(); // Reserved for table global variables, unused yet
    _file.read(_origin); // Record origin
    _file.read(_duration); // Record duration
    _file.read(_record_header_size); // Record header size
    _file.read(_record_size); // Record size

    // Storage content index (32 bytes)
    _file.read(_first_index); // first index
    _file.skip<4>(); // Unused
    _file.read(_min_index); // min index
    _file.read(_min_position); // min position
    _file.read(_max_index); // max index
    _file.read(_max_position); // max position
    _file.skip<8>(); // Unused

    // Field descriptions
    _fields.reserve(_field_count);
    for(field_index_t f = 0; f < _field_count; ++f)
    {
        int16_t type;
        uint16_t size, offset;
        uint16_t options;
        _file.read(type);
        _file.read(size);
        _file.read(offset);
        _file.read(options);

        std::string name;
        std::array<char, 256> buffer;
        uint8_t name_size;
        _file.read(name_size);
        if(name_size > 0)
        {
            _file.read(buffer.data(), name_size);
            name = std::string(buffer.data(), name_size);
        }

        _fields.push_back(field_impl((data_type) type, f, name, size, offset));
    }

    // TODO Additionnal header content
}

void file_table_impl::read_table_index_descriptor()
{
    _file.seek(_table_index_descriptor_position)
            .read(_first_index) // first index
            .skip<4>() // Unused
            .read(_min_index) // min index
            .read(_min_position) // min position
            .read(_max_index) // max index
            .read(_max_position) // max position
            ;
}

void file_table_impl::write_table_index_descriptor()
{
    _file.seek(_table_index_descriptor_position)
            .write(_first_index) // first index
            .write((uint32_t)0) // Unused
            .write(_min_index) // min index
            .write(_min_position) // min position
            .write(_max_index) // max index
            .write(_max_position) // max position
            ;
}

uint16_t file_table_impl::field_size(data_type type)
{
    switch(type)
    {
    case CDB_DT_BOOLEAN:
    case CDB_DT_SIGNED_8:
    case CDB_DT_UNSIGNED_8:
        return 1;
    case CDB_DT_SIGNED_16:
    case CDB_DT_UNSIGNED_16:
        return 2;
    case CDB_DT_SIGNED_32:
    case CDB_DT_UNSIGNED_32:
        return 4;
    case CDB_DT_SIGNED_64:
    case CDB_DT_UNSIGNED_64:
        return 8;
    case CDB_DT_FLOAT_4:
        return 4;
    case CDB_DT_FLOAT_8:
        return 8;
    case CDB_DT_VOID:
    case CDB_DT_UNSPECIFIED:
    default:
        return 0;
    }
}

raw_record file_table_impl::get_record_at_position(record_index_t pos) const
{
    if(pos < _record_capacity)
    {
        std::vector<uint8_t> buff(_record_size);
        _file.read_at(buff.data(), _record_size, _table_header_size + _record_size * pos);
        const uint8_t* data = buff.data();

        raw_record rec {this, position_to_index(pos)};

        for(field_index_t f = 0; f < field_count(); ++f)
        {
            const field_impl& fld = field(f);
            bool has = (*(data + (f / 8)) & (1 << (f % 8))) != 0;
            if(!has)
            {
                rec[f].reset();
            }
            else
            {
                const uint8_t* ptr = data + _record_header_size + fld._offset;
                switch(fld.type())
                {
                case CDB_DT_BOOLEAN: rec[f] = (bool) (*(uint8_t*) ptr) != 0;
                    break;
                case CDB_DT_SIGNED_8: rec[f] = (int8_t) (*(int8_t*) ptr);
                    break;
                case CDB_DT_UNSIGNED_8: rec[f] = (uint8_t) (*(uint8_t*) ptr);
                    break;
                case CDB_DT_SIGNED_16: rec[f] = (int16_t) (*(int16_t*) ptr);
                    break;
                case CDB_DT_UNSIGNED_16: rec[f] = (uint16_t) (*(uint16_t*) ptr);
                    break;
                case CDB_DT_SIGNED_32: rec[f] = (int32_t) (*(int32_t*) ptr);
                    break;
                case CDB_DT_UNSIGNED_32: rec[f] = (uint32_t) (*(uint32_t*) ptr);
                    break;
                case CDB_DT_SIGNED_64: rec[f] = (int64_t) (*(int64_t*) ptr);
                    break;
                case CDB_DT_UNSIGNED_64: rec[f] = (uint64_t) (*(uint64_t*) ptr);
                    break;
                case CDB_DT_FLOAT_4: rec[f] = (float) (*(float*) ptr);
                    break;
                case CDB_DT_FLOAT_8: rec[f] = (double) (*(double*) ptr);
                    break;
                default: rec[f].reset();
                    break;
                }
            }
        }
        return rec;
    }
    else
    {
        throw std::range_error{"Internal getting record position error"};
    }
}

void file_table_impl::reset_record_at_position(record_index_t pos)
{
    if(pos < _record_capacity)
    {
        _file.write_n_at(0, _record_size, _table_header_size + _record_size * pos);
    }
    else
    {
        throw std::range_error{"Internal reseting record position error"};
    }
}

void file_table_impl::set_record_at_position(record_index_t pos, const record& rec)
{
    if(pos < _record_capacity)
    {
        std::vector<uint8_t> buff(_record_size, 0);
        uint8_t* data = buff.data();

        for(field_index_t f = 0; f < field_count(); ++f)
        {
            const field_impl& fld = field(f);
            if(rec.has(f))
            {
                uint8_t* hdr = data + (f / 8);
                *hdr |= (1 << (f % 8));

                uint8_t* ptr = data + _record_header_size + fld._offset;

                switch(fld.type())
                {
                case CDB_DT_BOOLEAN:
                    *ptr = rec[f].value<bool>() ? 1 : 0;
                    break;
                case CDB_DT_SIGNED_8:
                    (*(int8_t*) ptr) = rec[f].value<int8_t>();
                    break;
                case CDB_DT_UNSIGNED_8:
                    (*(uint8_t*) ptr) = rec[f].value<uint8_t>();
                    break;
                case CDB_DT_SIGNED_16:
                    (*(int16_t*) ptr) = rec[f].value<int16_t>();
                    break;
                case CDB_DT_UNSIGNED_16:
                    (*(uint16_t*) ptr) = rec[f].value<uint16_t>();
                    break;
                case CDB_DT_SIGNED_32:
                    (*(int32_t*) ptr) = rec[f].value<int32_t>();
                    break;
                case CDB_DT_UNSIGNED_32:
                    (*(uint32_t*) ptr) = rec[f].value<uint32_t>();
                    break;
                case CDB_DT_SIGNED_64:
                    (*(int64_t*) ptr) = rec[f].value<int64_t>();
                    break;
                case CDB_DT_UNSIGNED_64:
                    (*(uint64_t*) ptr) = rec[f].value<uint64_t>();
                    break;
                case CDB_DT_FLOAT_4:
                    (*(float*) ptr) = rec[f].value<float>();
                    break;
                case CDB_DT_FLOAT_8:
                    (*(double*) ptr) = rec[f].value<double>();
                    break;
                default:
                    // Unsupported type
                    break;
                }
            }
        }
        _file.write_at(data, _record_size, _table_header_size + _record_size * pos);
    }
    else
    {
        throw std::range_error{"Internal setting record position error"};
    }
}

}
}
} // namespace cyclic::store::impl