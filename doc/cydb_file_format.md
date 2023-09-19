# cydbfile .cydb file format

This page describe the format of .cydb table storage files.
As it is a binary format, it is described in byte-based blocks and some parts
are leaved reserved or padded for type alignment.

Explicit characters or groups of characters are explains surrounded by quotes (' or ").
The must be treated as contiguous bytes strings.

CYDB file structure
===================

CYDB storage files are composed of a header and a data storage parts.
In basic datastore these two parts are juxtaposed.

CYDB Header structure
---------------------

The CYDB storage file header is composed of 4 sections which are juxtaposed:
* File header
* Storage structure
* Storage content index
* Field descriptions
* Additional data (not used yet)

### File header

The file header is the common 8-bytes chunk that identifies CYDB files.


 <table>
   <caption>CYDB file header</caption>
   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
   <tr>
     <th>File header<br/>(offset: 0, size: 8)</th>
     <td colspan="4">File marker "CYDB"</td>
     <td colspan="2">File format version "01"</td>
     <td colspan="2">Global file options</td>
 </table>

Where:
* File marker is a four chars value of "CYDB". 4 Bytes.
* File format version is a couple of char defining the version of file structure.
  Currently of value "01". 2 Bytes (version 0.1)
* Global file options. Flags characterizing content of file. Currently ignored. 2 bytes.

### Storage structure

The storage structure block contains global definitions of data stored in this table store.

 <table>
 <caption>CYDB 1.0 storage structure</caption>
   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
   <tr>
     <th rowspan="5">Storage structure<br/>(offset: 8, size: 40)</th>
     <td colspan="4">Header size</td>
     <td colspan="4">Record options</td>
   </tr>
   <tr>
     <td colspan="4">Record capacity</td>
     <td colspan="2">Field count</td>
     <td colspan="2">Reserved</td>
   </tr>
   <tr><td colspan="8">Record origin</td></tr>
   <tr><td colspan="8">Record duration time</td></tr>
   <tr>
     <td colspan="4">Record header size</td>
     <td colspan="4">Record size</td>
   </tr>
 </table>

Where:
* Header size: size of file header, including file marker, in bytes (4 bytes)
* Record options: specific record option flags, not used yet (4 bytes)
* Record capacity: number of record the table is able to store (4 bytes)
* Field count: number of fields (per record) (2 bytes)
* Record origin: Time of record origin (8 bytes)
* Record duration: duration of a record, 0 if record time is not set (8 bytes)
* Record header size: size of each record header, in bytes (4 bytes)
* Record size: size of each record including their headers, in bytes (4 bytes)

The storage structure is positionned at byte 8 in the file (size of file header block).

### Storage content index

The storage content index is the table which have record index and position.
It describes the structure of data part.

 <table>
 <caption>CYDB 1.0 Storage content index format</caption>
   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
   <tr>
     <th rowspan="4">Storage content index<br/>(offset: 48, size: 32)</th>
     <td colspan="4">First index</td>
     <td colspan="4">unused</td>
   </tr>
   <tr>
     <td colspan="4">Min index</td>
     <td colspan="4">Min position</td>
   </tr>
   <tr>
     <td colspan="4">Max index</td>
     <td colspan="4">Max position</td>
   </tr>
   <tr><td colspan="8">Reserved</td></tr>
 </table>

Where:
* First index: index of the first record slot (position 0), if used, 0-based, -1 if not used (4 bytes)
* Min index: index of the first record, 0-based, -1 if no record (4 bytes)
* Min position: position of the first record, 0-based, -1 if no record (4 bytes)
* Max index: index of the last record, 0-based, min==max if one record, -1 if no record (4 bytes)
* Max position: position of the last record, 0-based, min==max if one record, -1 if no record (4 bytes)

The storage content index is positioned at byte 48 in the file (size of file header and storage structure blocks).

### Field descriptions

The field description block is the juxtaposition of field descriptors, one per field (see field count).

 <table>
 <caption>CYDB 1.0 field description</caption>
   <tr><th> <th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
   <tr>
     <th rowspan="2">Field descriptor<br>size: 8+1+xxx bytes</th>
     <td colspan="2">Field type</td>
     <td colspan="2">Field size</td>
     <td colspan="2">Field offset</td>
     <td colspan="2">Field options</td>
   </tr>
   <tr>
     <td>Field name size</td>
     <td colspan="7">Field name ... (up to 255 bytes, can be empty)</td>
   </tr>
 </table>

Where:
* Field type: type of binary value stored for this field (2 bytes)
* Field size: size of binary value, in bytes (2 bytes)
* Field offset: field offset in the record (2 bytes)
  Begining of field data regarding to begining of all data of the field.
  Typically the first field has an offset of 0.
* Field options: Additionnal option flags for this field, unused yet (2 bytes)
* Field name size: Size of the field name, in bytes (1 byte)
* Field name: string representing the field name.
  Name should be composed of ASCII printable characters.
  Its size is specified in previous variable and can be 0 (empty name).

### Additionnal header content

This section will add some other table-related data.
Not used yet.
Section is 0 byte length.


CYDB Data storage
-----------------

The data part is a juxtaposition of record storage.
It is located at 'Header size' of the file.
Each record storage has the exact size specified in 'Record size'.
Each record is composed of
* a record header. Its size is defined in 'Record header size'.
  The record header is a bitmap to specify if a record field is set or not (empty/null or not).
* a suite of record field data.
  Each field data is located at 'field offset' after the begining of record field data part and have the 'Field size' size.

The data storage can be viewed as a contiguous array of record.
Each record has a position (0-based) from the first (0) to the last (table storage capacity).

For exemple:
 <table>
   <caption>Example of data storage</caption>
   <tr><th>           <th>index   <th>pos<th>rec header<th>field 0<th>field 1<th>field 2<th>...<th>field r
   <tr><td>first index<td>2       <td>0
   <tr><td>           <td>3       <td>1
   <tr><td>max index  <td>4       <td>2
   <tr><td>           <td>not used<td>3
   <tr><td>           <td>not used<td>4
   <tr><td>           <td>...     <td>...
   <tr><td>           <td>not used<td>n-3
   <tr><td>           <td>not used<td>n-2
   <tr><td>min index  <td>0       <td>n-1
   <tr><td>           <td>1       <td>n
 </table>

With:
* field count: the number of field in a record.
  A field f is 0 <= f <= r, r = field_count-1
* record capacity: the maxmum number of records in the table.
  The position of a record is the 0-based indexed position of the record in the table.
  A record at position pos is 0 <= pos <= n, n = record_capacity-1.
* first index: the index of the record at position 0
* min index: the current lowest valid index of the table
* min position: the position of the index_min record
* max index: the current highest valid index of the table
* max position: the position of the index_max record


CYDB storage internal states
----------------------------

A table can be in one of the following 12 states:

| pos |   0  |   1  |   2  |   3  |  4  |  5  |  6  |  7  |  8  |  9  |  10 |  11 |
|:---:|:----:|:----:|:----:|:----:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|  0  | miax | miax |      |      | min | min |     |     |  #  |  #  |  #  |  #  |
|  1  |      |      |      |      |  #  |  #  |     |     | max | max | max |  #  |
|  2  |      |      |      |      |  #  |  #  | min | min |     |     | min |  #  |
|  3  |      |      | miax |      | max |  #  |  #  |  #  | min |     |  #  |  #  |
|  4  |      |      |      |      |     |  #  | max |  #  |  #  |     |  #  |  #  |
|  …  |      |      |      |      |     |  #  |     |  #  |  #  |     |  #  |  #  |
| n-1 |      |      |      |      |     |  #  |     |  #  |  #  |     |  #  | max |
|  n  |      |      |      | miax |     | max |     | max |  #  | min |  #  | min |


(with positions: n last possible position; and x and y some intermediate positions with x<y)

* No record:
  * 0 - there is no record
* Only one record (min=max):
  * 1 - min=max at pos 0
  * 2 - min=max at pos x
  * 3 - min=max at pos n
* Contiguous records (min<max):
  * 4 - min<max with min=0 and max=x
  * 5 - min<max with min=0 and max=n
  * 6 - min<max with min=x and max=y
  * 7 - min<max with min=x and max=n
* Split records (after a loop, min>max):
  * 8 - min>max with min=x and max=y
  * 9 - min>max with min=n and max=x
  * 10 - min>max with min=y and max=x and min=max+1
  * 11 - min>max with min=n and max=n-1 and min=max+1
