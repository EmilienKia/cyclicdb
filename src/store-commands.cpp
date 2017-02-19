/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/store-commands.cpp
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
 *
 * cyclicdb/cyclicstore is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or (at your
 * option) any later version.
 *
 * cyclicdb is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the COPYING file at the root of the source distribution for more details.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "store-commands.hpp"
#include "store-parser-commands.hpp"

namespace cyclicstore
{

std::string val_to_str(cyclic::value_t val)
{
    switch(val.type())
    {
    case cyclic::CDB_DT_BOOLEAN: return val.value<bool>() ? "true" : "false";
    case cyclic::CDB_DT_SIGNED_8:
    case cyclic::CDB_DT_SIGNED_16:
    case cyclic::CDB_DT_SIGNED_32:
        return std::to_string(val.value<int32_t>());
    case cyclic::CDB_DT_UNSIGNED_8:
    case cyclic::CDB_DT_UNSIGNED_16:
    case cyclic::CDB_DT_UNSIGNED_32:
        return std::to_string(val.value<uint32_t>());
    case cyclic::CDB_DT_SIGNED_64:
        return std::to_string(val.value<int64_t>());
    case cyclic::CDB_DT_UNSIGNED_64:
        return std::to_string(val.value<uint64_t>());
    case cyclic::CDB_DT_FLOAT_4:
        return std::to_string(val.value<float>());
    case cyclic::CDB_DT_FLOAT_8:
        return std::to_string(val.value<double>());
    case cyclic::CDB_DT_UNSPECIFIED:
        return "<null>";
    case cyclic::CDB_DT_VOID:
        return "<void>";
    default:
        return "<>";
    }
}


const std::vector<std::string> field_type_canonical_names
{
    "void", // CDB_DT_VOID = 0,
    "bool", // CDB_DT_BOOLEAN,
    "signed8", // CDB_DT_SIGNED_8,
    "unsigned8", // CDB_DT_UNSIGNED_8,
    "signed16", // CDB_DT_SIGNED_16,
    "unsigned16", // CDB_DT_UNSIGNED_16,
    "signed32", // CDB_DT_SIGNED_32,
    "unsigned32", // CDB_DT_UNSIGNED_32,
    "signed64", // CDB_DT_SIGNED_64,
    "unsigned64", // CDB_DT_UNSIGNED_64,
    "float4", // CDB_DT_FLOAT_4,
    "float8", // CDB_DT_FLOAT_8,

// CDB_DT_MAX_TYPE,
};


namespace commands
{
    //
    // query_with_colnames
    //

    query_with_colnames::query_with_colnames(const boost::optional<std::vector<std::string>>& colnames)
    {
        if(colnames)
        {
            this->_colnames = *colnames;
        }
    }

    query_with_colnames::query_with_colnames(const std::vector<std::string>& colnames):
    _colnames(colnames)
    {
    }


    bool query_with_colnames::deduce_columns(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        _columns.clear();
        if(_colnames.empty())
        {
            _columns.reserve(table->field_count());
            for(cyclic::field_index_t f=0; f<table->field_count(); ++f)
            {
                _columns.push_back(f);
            }
            return true;
        }
        else
        {
            _columns.reserve(_colnames.size());
            for(size_t n=0; n<_colnames.size(); ++n)
            {
                bool found = false;
                for(cyclic::field_index_t f=0; f<table->field_count(); ++f)
                {
                    if(table->field(f).name()==_colnames[n])
                    {
                        _columns.push_back(f);
                        found = true;
                        break;
                    }
                }
                if(!found)
                {
                    std::cerr << "Cannot find field '" << _colnames[n] << "'." << std::endl;
                    return false;
                }
            }
            return true;
        }
    }

    //
    // query_with_colnames_and_position
    //
    query_with_colnames_and_position::query_with_colnames_and_position(
            const boost::optional<std::vector<std::string>>& colnames,
            const helpers::position& pos):
    query_with_colnames(colnames),
    _position(pos)
    {
    }

    
    //
    // query_with_colnames_values_and_position
    //
    query_with_colnames_values_and_position::query_with_colnames_values_and_position(
        const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos):
        query_with_colnames_and_position(colnames, pos),
        _values(values)
    {
    }

    //
    // select
    //
    select::select(const boost::optional<std::vector<std::string>>& colnames,
            const boost::optional<unsigned long>& start,
            const boost::optional<unsigned long>& end):
    query_with_colnames(colnames)
    {
        if(start)
        {
            this->_start = *start;
        }
        if(end)
        {
            this->_end = *end;
        }
    }

    bool select::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        if(!deduce_columns(table))
            return false;

        // Print headers
        std::cout << "index";
        for(size_t n=0; n<_columns.size(); ++n)
        {
                std::cout << "\t" << n;
        }
        std::cout << std::endl;
        for(size_t n=0; n<_columns.size(); ++n)
        {
                std::cout << "\t" << table->field(_columns[n]).name();
        }
        std::cout << std::endl;

        if(table->record_count()>0)
        {
            for(cyclic::record_index_t r = std::max(_start, table->min_index());
                    r <= std::min(_end, table->max_index());
                    ++r)
            {
                std::cout << r;
                auto rec = table->get_record(r);
                for(size_t n=0; n<_columns.size(); ++n)
                {
                    std::cout << "\t" << val_to_str((*rec)[_columns[n]]);
                }
                std::cout << std::endl;
            }
        }
        return true;
    }

    //
    // insert
    //
    insert::insert(const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos):
    query_with_colnames_values_and_position(colnames, values, pos)
    {
    }

    bool insert::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        if(!deduce_columns(table))
            return false;

        cyclic::raw_record rec;
        for(size_t n=0; n<_columns.size() && n<values().size(); ++n)
        {
            rec.set(_columns[n], values()[n]);
        }

        if(position().state()==helpers::position::INDEX)
        {
            // Specified index
            table->insert_record(position().index(), rec);
        }
        else if(table->record_count()>0)
        {
            // Last index + 1
            table->insert_record(table->max_index()+1, rec);
        }
        else
        {
            // First index
            table->insert_record(cyclic::record_index_t{0}, rec);
        }
        return true;
    }

    //
    // set
    //
    set::set(const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos):
    query_with_colnames_values_and_position(colnames, values, pos)
    {
    }

    bool set::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        if(!deduce_columns(table))
            return false;

        cyclic::raw_record rec;
        for(size_t n=0; n<_columns.size() && n<values().size(); ++n)
        {
            rec.set(_columns[n], values()[n]);
        }

        if(table->record_count()>0)
        {
            if(position().state()==helpers::position::INDEX)
            {
                // Specified index
                table->set_record(position().index(), rec);
            }
            else
            {
                // Last index + 1
                table->set_record(table->max_index(), rec);
            }
        }
        else
        {
            // TODO ERROR Cannot set a record when table is empty
        }
        return true;
    }

    //
    // append
    //
    append::append(const boost::optional<std::vector<std::string>>& colnames,
        const std::vector<cyclic::value_t>& values,
        const helpers::position& pos):
    query_with_colnames_values_and_position(colnames, values, pos)
    {
    }

    bool append::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        if(!deduce_columns(table))
            return false;

        cyclic::raw_record rec;
        for(size_t n=0; n<_columns.size() && n<values().size(); ++n)
        {
            rec.set(_columns[n], values()[n]);
        }

        if(position().state()==helpers::position::INDEX)
        {
            // Specified index
            table->append_record(position().index(), rec);
        }
        else
        {
            // Last index + 1
            table->append_record(cyclic::record::invalid_index(), rec);
        }
        return true;
    }

    //
    // reset
    //
    reset::reset(const boost::optional<std::vector<std::string>>& colnames,
        const helpers::position& pos):
    query_with_colnames_and_position(colnames, pos)
    {
    }

    bool reset::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        if(!deduce_columns(table))
            return false;

        // TODO
        if(table->record_count()>0)
        {
            cyclic::record_index_t index;
            if(position().state()==helpers::position::INDEX)
            {
                // Reset specified record.
                index = position().index();
            }
            else
            {
                // Reset last record.
                index = table->max_index();
            }

            std::shared_ptr<cyclic::record> r = table->get_record(index);
            std::shared_ptr<cyclic::mutable_record> rec = std::dynamic_pointer_cast<cyclic::mutable_record>(r);
            if(rec)
            {
                for(size_t n=0; n<_columns.size(); ++n)
                {
                    rec->reset(_columns[n]);
                }
                table->set_record(index, *rec);
            }
            else
            {
                std::cerr << "Internal error : record is not mutable, cannot reset some of its fields." << std::endl;
            }
        }
        else
        {
            // Cannot reset an  empty table.
            std::cerr << "Cannot reset a record in an empty table." << std::endl;
        }
        
        return false;
    }

    //
    // dump
    //

    bool dump::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        return select::execute(table);
    }

    //
    // status
    //
    bool status::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
        if(table->record_count()==0)
        {
            std::cout << "Empty table with a capacity of " << table->record_capacity() << " records" << std::endl;
        }
        else
        {
            std::cout << table->record_count() << " used record(s) on a table capacity of " << table->record_capacity() << std::endl;
            std::cout << "Lower index : " << table->min_index() << std::endl;
            std::cout << "Upper index : " << table->max_index() << " (current index)" << std::endl;
        }
        return true;
    }

    //
    // details
    //

    bool details::execute(std::shared_ptr<cyclic::store::impl::file_table_impl> table)
    {
            // TODO add table store global details like storage version, options ...

            std::cout
                            << "Definition" << std::endl
                            << "  Fields      : " << table->field_count() << std::endl;

            for(cyclic::field_index_t f=0; f<table->field_count(); ++f)
            {
                    const cyclic::store::impl::field_impl& field = table->field(f);
                    std::cout << "     " << f << " - " << field.name() << " : " << field_type_canonical_names[field.type()] << " size=" << field.size() << " offset=" << field.offset() << std::endl;
            }

            std::cout
                            << std::endl
                            << "Usage" << std::endl
                            << "  Capacity    : " << table->record_capacity() << " records" << std::endl
                            << "  Used        : " << table->record_count() << " record(s)" << std::endl;
            if(table->min_index()!=cyclic::record::invalid_index())
                std::cout   << "  Lower index : " << table->min_index() << std::endl;
            else
                std::cout   << "  Lower index : <none> " << std::endl;
            if(table->max_index()!=cyclic::record::invalid_index())
                std::cout   << "  Upper index : " << table->max_index() << std::endl;
            else
                std::cout   << "  Upper index : <none> " << std::endl;

        // TODO Add more details here
        return true;
    }

    //
    // create
    //

    create::create(const std::vector<cyclic::field_st>& fields, cyclic::record_index_t capacity):
    fields(fields),
    capacity(capacity)
    {
    }

    std::shared_ptr<cyclic::store::impl::file_table_impl> create::execute(const std::string& filename)
    {
        for(cyclic::field_st desc : fields)
        {
            std::cout << " * " << desc.name << " " << field_type_canonical_names[desc.type] << std::endl;
        }

        std::shared_ptr<cyclic::recordset> table = cyclic::store::file::create(filename, cyclic::store::file::COMPACT, fields, capacity);
        return std::dynamic_pointer_cast<cyclic::store::impl::file_table_impl>(table);
    }

} // namespace commands




//
// command_executor
//

bool command_executor::parse_and_execute(const std::string& command)
{
    commands::command* cmd;

    std::string::const_iterator begin = command.cbegin(), end = command.cend();

    parser::query_parser<std::string::const_iterator> grammar;

    if(phrase_parse(begin, end, grammar, boost::spirit::ascii::space, cmd))
    {
        return execute(cmd);
    }
    else if(begin != end)
    {
        std::cerr << "Not all requested expression have been parsed:" << std::endl;
        std::cerr << command << std::endl;

        for(auto it = command.cbegin(); it != begin; ++it)
            std::cerr << '-';

        std::cerr << "|" << std::endl;
    }
    return false;
}


bool command_executor::execute(commands::command* command)
{
    if(commands::query* query = dynamic_cast<commands::query*>(command))
    {
        // Open table if not already done
        if(!table)
        {
            std::shared_ptr<cyclic::recordset> tbl = cyclic::store::file::open(filename);
            if(!tbl)
            {
                std::cerr << "Cannot open file '" << filename << "'" << std::endl;
                return -2;
            }
            table = std::dynamic_pointer_cast<cyclic::store::impl::file_table_impl>(tbl);
        }
        return query->execute(table);
    }
    else if(commands::create* query = dynamic_cast<commands::create*>(command))
    {
        std::shared_ptr<cyclic::store::impl::file_table_impl> res = query->execute(filename);
        if(res)
        {
            table = res;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        std::cerr << "Error in query parsing" << std::endl;
        return false;
    }
}


} // namespace cyclicstore