/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/libstore.hpp
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

#ifndef _CYCLIC_LIBSTORE_HPP_
#define _CYCLIC_LIBSTORE_HPP_

#include <memory>
#include <string>
#include <vector>

#include "common-base.hpp"

namespace cyclic
{
/**
 * CyclicDB storage layer API.
 */
namespace store
{
    /**
     * Interface for volatile table storage in memory.
     */
    /* abstract */ class memory
    {
    public:
        /**
         * Create a cyclic table stored in memory.
         * This table is accessible for the current process only.
         * It will be alive until no client access it.
         * @param fields Field descriptors for create table.
         * @param record_capacity Table capacity in record number.
         * @param origin Table time origin.
         * @param duration Table time duration.
         * @return Created memory table.
         * @throw std::invalid_argument Fields list is empty.
         * This is a non-sense to create a table without fields.
         * @throw std::invalid_argument Record capacity of 0.
         * This is a non-sense to create a table without storage capacity.
         * @throw std::invalid_argument Invalid record capacity.
         **/
        static std::unique_ptr<cyclic::table> create(const std::vector<field_st>& fields, record_index_t record_capacity,
                record_time_t origin = 0, record_time_t duration = 0);
    };


    /**
     * Interface for table storage in files.
     */
    /* abstract */ class file
    {
    public:
        /**
         * Type of file storage.
         */
        enum table_type {
                COMPACT = 0 ///< Compact file (one file)
        };

        /**
         * Create a table stored in a file.
         * @param filename Name of file to create to store table.
         * @param type Type of table to create.
         * @param fields Field descriptors for create table.
         * @param record_capacity Table capacity in record number.
         * @param origin Table time origin.
         * @param duration Table time duration.
         * @return Created file table.
         * @throw std::invalid_argument Fields list is empty.
         * This is a non-sense to create a table without fields.
         * @throw std::invalid_argument Record capacity of 0.
         * This is a non-sense to create a table without storage capacity.
         * @throw std::invalid_argument Invalid record capacity.
         */
        static std::unique_ptr<cyclic::table> create(const std::string& filename, table_type type,
            const std::vector<field_st>& fields, record_index_t record_capacity,
            record_time_t origin = 0, record_time_t duration = 0);
        /**
         * Open a table from a file.
         * @param filename Name of table file to open.
         * @return Opened file table.
         * @throw std::invalid_argument Filename shall be specified.
         * @throw cyclic::io::io_exception An I/O exception occurs.
         */
        static std::unique_ptr<cyclic::table> open(const std::string& filename);
    };

}} // namespace cyclic::store
#endif // _CYCLIC_LIBSTORE_HPP_

