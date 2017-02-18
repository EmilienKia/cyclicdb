/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * src/common-base.hpp
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
#ifndef _CYCLIC_COMMON_BASE_HPP_
#define _CYCLIC_COMMON_BASE_HPP_

#include <initializer_list>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "common-type.hpp"

/**
 * Base CyclicDB namespace.
 * Used to define common types for all CyclicDB layers.
 */
namespace cyclic
{
    class recordset;
    class const_recordset_iterator;
    class table;

    /**
     * Report the record is not attached to a recordset.
     * Typically thrown when a record manipulation need the record be attached to a recordset.
     */
    class detached_record : public std::runtime_error
    {
    public:
        detached_record():std::runtime_error(nullptr){}
        explicit detached_record(const std::string& what_arg):std::runtime_error(what_arg){}
        explicit detached_record(const char* what_arg):std::runtime_error(what_arg){}
        ~detached_record(){}
    };

    /**
     * Report the request field is not known by the recordset.
     */
    class unknown_field : public std::runtime_error
    {
    public:
        unknown_field():std::runtime_error(nullptr){}
        explicit unknown_field(const std::string& what_arg):std::runtime_error(what_arg){}
        explicit unknown_field(const char* what_arg):std::runtime_error(what_arg){}
        ~unknown_field(){}
    };

    /**
     * Report time is not supported by the table..
     */
    class time_not_supported : public std::runtime_error
    {
    public:
        time_not_supported():std::runtime_error(nullptr){}
        explicit time_not_supported(const std::string& what_arg):std::runtime_error(what_arg){}
        explicit time_not_supported(const char* what_arg):std::runtime_error(what_arg){}
        ~time_not_supported(){}
    };

    /**
     * Report table is full.
     * No new record can be append.
     */
    class table_is_full : public std::overflow_error
    {
    public:
        table_is_full():std::overflow_error(nullptr){}
        explicit table_is_full(const std::string& what_arg):std::overflow_error(what_arg){}
        explicit table_is_full(const char* what_arg):std::overflow_error(what_arg){}
        ~table_is_full(){}
    };

    /**
    * Base read-only field descriptor facade.
    * Used to describe field.
    */
    /* abstract */ class field
    {
    public:
        /**
         * Type of field index.
         */
        typedef uint16_t index_t;

        /**
         * Special field index value designating an invalid field index.
         */
        static constexpr index_t invalid_index() { return std::numeric_limits<index_t>::max(); }

        /**
         * Special field index value designating the absolute greatest valid index (inclusive).
         * Note this maximum index is an absolute one, implementations may support a lower maximum value.
         */
        static constexpr index_t absolute_max_index() { return std::numeric_limits<index_t>::max() - 1; }

        /**
         * Index of the field in the recordset.
         * @return Index of the field, invalid_index() if not set.
         */
        virtual index_t index() const = 0;
        /**
         * Name of the field.
         * @return Name of the field, eventually empty.
         */
        virtual std::string name() const = 0;
        /**
         * Data type of the field.
         * @return Datatype of the field, CDB_DT_UNSPECIFIED if not specified.
         */
        virtual data_type type() const = 0;
    };

    /**
     * Field index type.
     * Valid in the range [0-field_index_max],
     * It describes the index of a field in a record or in a dataset (record, table, etc.).
     */
    typedef field::index_t field_index_t;

    /**
     * Basic field initializer descriptor.
     * Used as facility recordset construction.
     */
    struct field_st
    {
        /** Name of the field, might be empty on some context.*/
        std::string name;
        /** Expected data type of the field, shall not be undefined.*/
        data_type type;
    };

    /**
     * Read-only record structure.
     * It allow to retrieve data from a recordset (like a table).
     * It should point to the recordset from which it come from.
     * It also should retrieve recordset basic properties relevant for its own context
     * like field count.
     */
    /* abstract */ class record
    {
    protected:
        record() = default;
    public:
        /**
         * Type of record index.
         * Index is the 0-based index to which the record is stored in a table.
         */
        typedef uint32_t index_t;

        /**
         * Special record index value designating an invalid record index.
         */
        static constexpr index_t invalid_index() {return std::numeric_limits<index_t>::max()-1;}

        /**
         * Special record index value designating the greatest valid index (inclusive).
         * Note this maximum index is an absolute one, table implementations may support a lower maximum value.
         */
        static constexpr index_t absolute_max_index() {return std::numeric_limits<index_t>::max()-1;}

        /**
         * Type of record time point and duration.
         */
        typedef int64_t time_t;

        virtual ~record() = default;

        /**
         * Return the recordset to which the record is attached.
         * @return The attached recordset, nullptr if not attached.
         */
        virtual const cyclic::recordset* recordset()const =0;

        /**
         * Retrieve the index of the record in the dataset origin.
         * @return Index of the record, invalid_index() if not provided or known.
         */
        virtual index_t index()const =0;

        /**
         * Retrieve the time of the record (time of begining of record)
         * if time is enabled and available.
         * @return Time of record, if available, 0 otherwise.
         */
        virtual time_t time()const =0;

        /**
         * Test if a record if valid.
         * (typically, not empty and assign to a source dataset)
         * @return true if the record is valid.
         */
        virtual bool ok()const =0;

        /**
         * Test if a record if valid.
         * (typically, not empty and assign to a source dataset)
         * @return true if the record is valid.
         */
        operator bool()const{return ok();}

        /**
         * Retrieve the number of field held by the record.
         * Typically the number of fields of the attached datasource.
         * @return Number of field held by the record.
         */
        virtual field_index_t size()const =0;

        /**
         * Test if the record has a value (not null) for the specified field index.
         * @param field Field index.
         * @return True if the record have a value for the field, false otherwise.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        virtual bool has(field_index_t field)const =0;

        /**
         * Test if the record has a value (not null) for the specified field.
         * @param field_name Name of the field to look for.
         * @return True if the record have a value for the field, false otherwise.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        virtual bool has(const std::string& field_name)const =0;

        /**
         * Get the value of a specified field.
         * @param field Field index to look for.
         * @return Value of the specified field.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        virtual const value_t& get(field_index_t field)const =0;

        /**
         * Get the value of a specified field.
         * @param field_name Field name to look for.
         * @return Value of the specified field.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        virtual const value_t& get(const std::string& field_name)const =0;

        /**
         * Const reference the value for specified field.
         * @param field Field index to look for.
         * @return Value of the specified field.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        inline const value_t& operator[](field_index_t field)const {return get(field);}

        /**
         * Const reference the value for specified field.
         * @param field_name Field name to look for.
         * @return Value of the specified field.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        inline const value_t& operator[](const std::string& field_name)const {return get(field_name);}

        /**
         * Get the real value of a specified field along the specified type.
         * @tparam T Requested type.
         * @param field Field index to look for.
         * @return Value of the specified field.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        template<typename T> inline T get(field_index_t field)const { return get(field).value<T>(); }

        /**
         * Get the real value of a specified field along the specified type.
         * @tparam T Requested type.
         * @param field_name Field name to look for.
         * @return Value of the specified field.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        template<typename T> inline T get(const std::string& field_name)const { return get(field_name).value<T>(); }
    };

    /**
     * Record index type.
     * Valid in the range [0-record_index_max].
     * It describes the index of a record in a table.
     */
    typedef record::index_t record_index_t;

    /**
     * Record time point and duration.
     */
    typedef record::time_t record_time_t;

    /**
     * Record having data which can be modified (and set back in tables).
     */
    /* abstract */ class mutable_record : public record
    {
    protected:
        mutable_record() = default;
    public:
        virtual ~mutable_record() = default;

        /**
         * Attach the record to a recordset.
         * @param recordset Recordset to which attach the record.
         * @return this
         */
        virtual record& attach(const cyclic::recordset* recordset) =0;

        /**
         * Set the index of the record.
         * @param index New index of the record.
         * @return this
         */
        virtual record& index(index_t index) =0;

        /**
         * Set the time point of the record.
         * @param time New time of the record.
         * @return this
         */
        virtual mutable_record& time(time_t time) =0;

        /**
         * Reset the value (set to null) of a field.
         * Not persisted until commited on the source dataset.
         * @param field Field index to reset.
         * @return This
         * @throw std::out_of_range if the field index is out of held field range.
         */
        virtual mutable_record& reset(field_index_t field) =0;

        /**
         * Reset the value (set to null) of a field.
         * Not persisted until commited on the source dataset.
         * @param field_name Field name to reset.
         * @return This
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        virtual mutable_record& reset(const std::string& field_name) =0;

        /**
         * Reset all values of the record.
         * (set values to null for all fields).
         * Not persisted until commited on the source dataset.
         */
        virtual void reset() =0;

        /**
         * Update the present record with values of another record.
         * The other record can be attached to another datasource and should not
         * have more fields than the present one.
         * Only not-null values of the other record are set in the present one.
         * @param rec Other record from which update.
         */
        virtual void update(const record& rec) =0;

        /**
         * Get the value reference of a specified field.
         * This value could be modified.
         * Not persisted until commited on the source dataset.
         * @param field Field index to look for.
         * @return Value of the specified field.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        virtual value_t& get(field_index_t field) =0;

        /**
         * Get the value reference of a specified field.
         * This value could be modified.
         * Not persisted until commited on the source dataset.
         * @param field_name Field name to look for.
         * @return Value of the specified field.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        virtual value_t& get(const std::string& field_name) =0;

        /**
         * Set a new value to the record for specified field.
         * Not persisted until commited on the source dataset.
         * @param field Field to update.
         * @param value Value to set, may be null to reset.
         * @return This
         * @throw std::out_of_range if the field index is out of held field range.
         */
        virtual mutable_record& set(field_index_t field, value_t value) =0;

        /**
         * Set a new value to the record for specified field.
         * Not persisted until commited on the source dataset.
         * @param field_name Name of field to update.
         * @param value Value to set, may be null to reset.
         * @return This
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        virtual mutable_record& set(const std::string& field_name, value_t value) =0;

        /**
         * Reference the value for specified field.
         * This value could be modified.
         * Not persisted until commited on the source dataset.
         * @param field Field index to look for.
         * @return Value of the specified field.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        inline value_t& operator[](field_index_t field) {return get(field);}

        /**
         * Reference the value for specified field.
         * This value could be modified.
         * Not persisted until commited on the source dataset.
         * @param field_name Field name to look for.
         * @return Value of the specified field.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        inline value_t& operator[](const std::string& field_name) {return get(field_name);}

        /**
         * Set a new value to the record for specified field.
         * Not persisted until commited on the source dataset.
         * @tparam T Requested type.
         * @param field Field to update.
         * @param value Value to set, may be null to reset.
         * @return This
         * @throw std::out_of_range if the field index is out of held field range.
         */
        template<typename T> inline mutable_record& set(field_index_t field, T value) { set(field, value_t(value)); return *this;}

        /**
         * Set a new value to the record for specified field.
         * Not persisted until commited on the source dataset.
         * @tparam T Requested type.
         * @param field_name Name of field to update.
         * @param value Value to set, may be null to reset.
         * @return This
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        template<typename T> inline mutable_record& set(const std::string& field_name, T value) { set(field_name, value_t(value)); return *this;}
    };

    /**
     * Helper record which simply store data in an inner vector.
     * Usefull to create a record and insert it in a table.
     */
    class raw_record : public mutable_record
    {
    private:
        const cyclic::recordset* _recordset = nullptr;
        index_t _index = invalid_index();
        time_t _time = 0;
        std::vector<value_t> _values;

        /**
         * Ensure that the record has a number of stored values.
         * @param field_count Field count.
         */
        void ensure(field_index_t field_count);
    public:
        raw_record() = default;
        ~raw_record() = default;

        /**
         * Create a raw record, eventually attached to a recordset.
         * @param recordset Recordset to which attach the record. Can be nullptr.
         * @param record Record index. Invalid by default.
         * @param time Time of the record.
         */
        raw_record(const cyclic::recordset* recordset, index_t record = invalid_index(), time_t time = 0);

        /** Copy a raw record. */
        raw_record(const raw_record& rec);
        /** Copy a record. */
        raw_record(const record& rec);
        /** Move a raw record. */
        raw_record(raw_record&& rec);

        /** Copy a record. */
        raw_record& operator=(const record& rec);
        /** Copy a raw_record. */
        raw_record& operator=(const raw_record& raw);
        /** Move a raw_record. */
        raw_record& operator=(raw_record&& rec);

        /**
         * Create a raw record, not attached to any table, but initialized with values.
         * @param init Values with wich initialize record.
         * @return The record filled with values.
         */
        static raw_record raw(std::initializer_list<cyclic::value_t> init);

        const cyclic::recordset* recordset()const override;
        index_t index()const override;
        time_t time()const override;
        bool ok()const override;
        record& attach(const cyclic::recordset* recordset) override;
        record& index(index_t index) override;
        mutable_record& time(time_t time) override;
        field_index_t size() const override;
        bool has(field_index_t field)const override;
        bool has(const std::string& field_name)const override;
        mutable_record& reset(field_index_t field) override;
        mutable_record& reset(const std::string& field_name) override;
        void reset() override;
        void update(const record& rec) override;
        const value_t& get(field_index_t field)const override;
        const value_t& get(const std::string& field_name)const override;
        value_t& get(field_index_t field) override;
        value_t& get(const std::string& field_name) override;
        mutable_record& set(field_index_t field, value_t value) override;
        mutable_record& set(const std::string& field_name, value_t value) override;

        /**
         * Fill the record with some values.
         * @param init Values with wich initialize record.
         */
        void fill(std::initializer_list<cyclic::value_t> init);

    protected:
        /**
         * Internal helper to retrieve attached recordset field descriptor.
         * @param field Field index to look for.
         * @return The field descriptor.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        const cyclic::field& field(field_index_t field)const;
        /**
         * Internal helper to retrieve attached recordset field descriptor.
         * @param field_name Field name to look for.
         * @return The field descriptor.
         * @throw cyclic::detached_record if the record is not attached to a recordset.
         * @throw cyclic::unknown_field if the field is not present in the attached recordset.
         */
        const cyclic::field& field(const std::string& field_name)const;
    };

    /**
     * Inteface for recordset.
     * A recordset is a group of records.
     * Can be from a table or a query.
     */
    /* abstract */ class recordset
    {
    public:
        /**
         * Retrieve the field count of the table.
         * @return Field count.
         */
        virtual field_index_t field_count() const =0;

        /**
         * Return a descriptor of the specified field.
         * @param field Index of field to look for.
         * @return Field descriptor.
         * @throw std::out_of_range if the field index is out of held field range.
         */
        virtual const cyclic::field& field(field_index_t field)const =0;

        /**
         * Return a descriptor of the specified field.
         * @param field_name Name of field to look for.
         * @return Field descriptor.
         * @throw cyclic::unknown_field if the field is not present in the recordset.
         */
        virtual const cyclic::field& field(const std::string& field_name)const =0;

        /**
         * Return the number of records currently stored in the recordset.
         * @return The number of records stored in the recordset.
         */
        virtual record_index_t record_count() const =0;

        /**
         * Return the lowest index of records currently stored in the table.
         * @return The index of the first stored record, record_index_invalid if table is empty.
         */
        virtual record_index_t min_index()const =0;

        /**
         * Return the highest index of records currently stored in the table.
         * @return The index of the last stored record, record_index_invalid if table is empty.
         */
        virtual record_index_t max_index()const =0;

        /**
         * Get record filled with values stored at specified index.
         * @param index Record index to look for. Must be valid.
         * @return Filled record.
         * @throw std::out_of_range if the record index is out of held record range.
         */
        virtual std::unique_ptr<record> get_record(record_index_t index)const =0;

        /**
         * Returns a const iterator to the first record of the recordset.
         * If the recordset is empty, the returned iterator will be equal to cend().
         * @return Const iterator to the first record.
         */
        virtual const_recordset_iterator begin()const =0;

        /**
         * Returns an iterator to the record following the last record of the recordset.
         * This element acts as a placeholder; attempting to access it results in undefined behavior.
         * @return Iterator to the record following the last record.
         */
        virtual const_recordset_iterator end()const =0;

    protected:
        friend class cyclic::const_recordset_iterator;

        /**
         * Interface for virtual const iterators implementations
         */
        struct const_iterator_interface
        {
            /** Default constructor. */
            const_iterator_interface() = default;
            /** Destructor. */
            virtual ~const_iterator_interface() = default;
            /**
             * Test if the iterator is valid.
             * @return True if iterator is valid, false otherwise.
             */
            virtual bool ok()const =0;
            /**
             * Increment the iterator.
             * I.e. point to the next record.
             */
            virtual void increment() =0;
            /**
             * Test if the iterator is equal to another.
             * @param other Other iterator to compare to.
             * @return True if iterators are equals, false otherwise.
             */
            virtual bool equals(const const_iterator_interface* other)const=0;
            /**
             * Dereference the iterator.
             * @return A reference to the pointed record.
             * @throw std::runtime_exception The iterator point to an invalid record.
             * Typically for a after-the-last or invalidated iterator.
             */
            virtual const record& dereference()=0;
        };
    };

    /**
     * Virtual iterator facade helping to iterate on all records of a recordset.
     */
    class const_recordset_iterator
    {
    protected:
        /** Pointer to the iterator implementation. */
        std::shared_ptr<cyclic::recordset::const_iterator_interface> _ptr;
    public:
        /** Type of value pointed by iterator. */
        typedef cyclic::record value_type;
        /** Type of reference to value pointed by iterator. */
        typedef const value_type& reference;
        /** Type of pointer to value pointed by iterator. */
        typedef const value_type* pointer;
        /** Type of this. */
        typedef const_recordset_iterator self;

        /** Copy constructor. */
        const_recordset_iterator(const const_recordset_iterator& it):_ptr(it._ptr){}
        /** Move iterator. */
        const_recordset_iterator(const_recordset_iterator&& it):_ptr(std::move(it._ptr)){}
        /** Implementation construction. */
        const_recordset_iterator(std::shared_ptr<recordset::const_iterator_interface> ptr):_ptr(ptr){}

        /**
         * Dereference the iterator.
         * @return A reference to the pointed record.
         * @throw std::runtime_exception The iterator point to an invalid record.
         * Typically for a after-the-last or invalidated iterator.
         */
        reference operator*()const{return _ptr->dereference();}
        /**
         * Dereference the iterator.
         * @return A reference to the pointed record.
         * @throw std::runtime_exception The iterator point to an invalid record.
         * Typically for a after-the-last or invalidated iterator.
         */
        pointer operator->()const{return &_ptr->dereference();}

        /**
         * Increment the iterator.
         * I.e. point to the next record.
         */
        self& operator++() {_ptr->increment(); return *this;}
        /**
         * Increment the iterator.
         * I.e. point to the next record.
         */
        self operator++(int) {self it{*this}; _ptr->increment(); return it;}

        /**
         * Test if the iterator is equal to another.
         * @param other Other iterator to compare to.
         * @return True if iterators are equals, false otherwise.
         */
        bool operator==(const const_recordset_iterator& other)const{return _ptr->equals(other._ptr.get());}
        /**
         * Test if the iterator is different from another.
         * @param other Other iterator to compare to.
         * @return True if iterators are different, false otherwise.
         */
        bool operator!=(const const_recordset_iterator& other)const{return !_ptr->equals(other._ptr.get());}
    };

    /**
     * Interface of table.
     * This figure out common property accessors and manipulators for tables.
     */
    /* abstract */ class table : public recordset
    {
    public:
        /**
         * Return the capacity of table, in number of records.
         * The capacity is the maximum number of records a table can store at the same time.
         * When full, all new appended record will erase and replace oldest ones (with smallest index).
         * @return The number of record the table is able to store.
         */
        virtual record_index_t record_capacity() const =0;

        /**
         * Return the origin time point.
         * The origin time point is the time point corresponding to the begining of the first record (rec #0).
         * @return The origin time point.
         */
        virtual record_time_t record_origin()const =0;

        /**
         * Return the duration of records.
         * If 0, the record time is not relevant and cannot be used.
         * @return The duration of each record.
         */
        virtual record_time_t record_duration()const =0;

        /**
         * Return the record index corresponding to the time point.
         * The table must support time points (having a record duration != 0).
         * @param time Time point to compute.
         * @return The corresponding record index.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         * @throw std::out_of_range When input time is before the time origin.
         * @throw std::out_of_range When computed index is out of valid index range.
         */
        virtual record_index_t record_index(record_time_t time)const =0;

        /**
         * Return the time of the begining of the specified record.
         * The table must support time points (having a record duration != 0).
         * @param index Index of record to look for.
         * @return The time point of the begining of the record.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual record_time_t record_time(record_index_t index)const =0;

        /**
         * Return an empty mutable record, linked to the table.
         * Usefull to initialize a record before fulfilling it and insert in the table.
         * @return An empty record linked to the table.
         */
        virtual std::unique_ptr<mutable_record> get_record() const=0;

        // Respecify this declaration (from recordset) to prevent C++ strange compilation behavior to occur
        // See http://stackoverflow.com/questions/1005780/scoping-rules-when-inheriting-c?answertab=active#tab-top
        virtual std::unique_ptr<record> get_record(record_index_t index) const =0;

        /**
         * Get record filled with values stored at specified time point.
         * @param time Time point to which look for, Must be valid.
         * @return Filled record.
         * @throw std::out_of_range if the record index is out of held record range.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual std::unique_ptr<record> get_record(record_time_t time) const =0;

        /**
         * Set record values at record index.
         * The record index must be valid.
         * The record time is ignored.
         * @param rec Record to set, its index shall be set.
         * @throw std::invalid_argument index is invalid.
         * @throw std::logic_error set a record in an empty table.
         * @throw std::out_of_range set a record at an index out of current table range.
         * @throw std::range_error internal index computation error.
         */
        virtual void set_record(const record& rec) =0;

        /**
         * Set record values at specified index.
         * The index and the time of the record object are ignored.
         * Usefull to clone (and modify) a record.
         * @param index Index to which store the record. Must be valid.
         * @param rec Record to store.
         * @throw std::invalid_argument index is invalid.
         * @throw std::logic_error set a record in an empty table.
         * @throw std::out_of_range set a record at an index out of current table range.
         * @throw std::range_error internal index computation error.
         */
        virtual void set_record(record_index_t index, const record& rec) =0;

        /**
         * Set record values at specified time.
         * The index and the time of the record object are ignored.
         * @param time Time point to which store the record. Must be valid.
         * @param rec Record to store.
         * @throw std::invalid_argument index is invalid.
         * @throw std::logic_error set a record in an empty table.
         * @throw std::out_of_range set a record at an index out of current table range.
         * @throw std::range_error internal index computation error.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual void set_record(record_time_t time, const record& rec) =0;

        /**
         * Update an existing record with values of another one.
         * Only non-null record values are applyed.
         * @param rec Record from which update values.
         * Record index must be valid.
         * Time of the record is ignored.
         * @throw std::invalid_argument index is invalid.
         * @throw std::logic_error update a record in an empty table.
         * @throw std::out_of_range update a record at an index out of current table range.
         * @throw std::range_error internal index computation error.
         */
        virtual void update_record(const record& rec) =0;

        /**
         * Update an existing record with values of another one.
         * Only non-null record values are applyed.
         * @param index Index of record to update. Must be valid.
         * @param rec Record from which update values.
         * Record index and time are ignored.
         * @throw std::invalid_argument index is invalid.
         * @throw std::logic_error update a record in an empty table.
         * @throw std::out_of_range update a record at an index out of current table range.
         * @throw std::range_error internal index computation error.
         */
        virtual void update_record(record_index_t index, const record& rec) =0;

        /**
         * Update an existing record with values of another one.
         * Only non-null record values are applyed.
         * @param time Time point to which update the record. Must be valid.
         * @param rec Record from which update values.
         * Record index and time are ignored.
         * @throw std::invalid_argument index is invalid.
         * @throw std::logic_error update a record in an empty table.
         * @throw std::out_of_range update a record at an index out of current table range.
         * @throw std::range_error internal index computation error.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual void update_record(record_time_t time, const record& rec) =0;

        /**
         * Append an empty record at the index just following the highest record.
         * The table must not be full (max_index() == record_index_max).
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         */
        virtual void append_record() =0;

        /**
         * Append an empty record at the specified index.
         * The index must be valid: upper than current highest record index and
         * not upper than highest possible index.
         * All intermediate records are set to empty (all fields to null)
         * if not immediatly after the highest record.
         * @param index Index of record to insert.
         * @throw std::out_of_range Append a record before end of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         */
        virtual void append_record(record_index_t index) =0;

        /**
         * Append an empty record at the specified time.
         * The time must be valid: after than current highest record time slot and
         * not upper than highest possible time.
         * All intermediate records are set to empty (all fields to null)
         * if not immediatly after the highest record.
         * @param time Time of record to append.
         * @throw std::out_of_range Append a record before end of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual void append_record(record_time_t time) =0;

        /**
         * Append a record at the specified index.
         * The index must be valid: upper than current highest record index and
         * not upper than highest possible index.
         * If the index is record_index_invalid, append record just after the
         * last record, if table is not full.
         * All intermediate records are set to empty (all fields to null)
         * if not immediatly after the highest record.
         * @param rec Record to append.
         * The time of the record is ignored.
         * @throw std::out_of_range Append a record before end of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         */
        virtual void append_record(const record& rec) =0;

        /**
         * Append an empty record at the specified index.
         * The index must be valid: upper than current highest record index and
         * not upper than highest possible index.
         * All intermediate records are set to empty (all fields to null)
         * if not immediatly after the highest record.
         * @param rec Record to append.
         * The index and the time of the record are ignored.
         * @param index Index of record to insert.
         * @throw std::out_of_range Append a record before end of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         */
        virtual void append_record(record_index_t index, const record& rec) =0;

        /**
         * Append a record at the specified time point.
         * The time must be valid: after than current highest record time slot and
         * not upper than highest possible time.
         * All intermediate records are set to empty (all fields to null)
         * if not immediatly after the highest record.
         * @param rec Record to append.
         * The index and the time of the record are ignored.
         * @param time Time of record to append.
         * @throw std::out_of_range Append a record before end of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual void append_record(record_time_t time, const record& rec) =0;

        /**
         * Insert an empty record (set or append) to the specified index.
         * The index must be in the range [min_index;record_index_max].
         * @param index Index to which insert an empty record.
         * @throw std::out_of_range Insert a record before begining of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         */
        virtual void insert_record(record_index_t index) =0;

        /**
         * Insert an empty record (set or append) to the specified time point.
         * @param time Time of record to insert.
         * @throw std::out_of_range Insert a record before begining of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual void insert_record(record_time_t time) =0;

        /**
         * Insert a record (set or append) to the specified index.
         * The index must be in the range [min_index;record_index_max].
         * @param rec Record to set. Index must be valid.
         * The time of the record is ignored.
         * @throw std::out_of_range Insert a record before begining of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         */
        virtual void insert_record(const record& rec) =0;

        /**
         * Insert a record (set or append) to the specified index.
         * The index must be in the range [min_index;record_index_max].
         * @param rec Record to set. Its index is ignored.
         * The time of the record is ignored.
         * @param index Index to which set the record. Must be valid.
         * @throw std::out_of_range Insert a record before begining of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         */
        virtual void insert_record(record_index_t index, const record& rec) =0;

        /**
         * Insert a record (set or append) to the specified time point.
         * @param rec Record to set.
         * The index and the time of the record are ignored.
         * @param time Time of record to insert.
         * @throw std::out_of_range Insert a record before begining of table.
         * @throw cyclic::table_is_full Table is full, no more record can be append.
         * @throw cyclic::time_not_supported When time is not supported by the table.
         */
        virtual void insert_record(record_time_t time, const record& rec) =0;
    };

} // namespace cyclic
#endif // _CYCLIC_COMMON_BASE_HPP_