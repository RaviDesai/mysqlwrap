mysqlwrap
=========

C++ Wrapper for MySQL C API

Features:
- Supports both client and embedded database linkage
- Supports both Prepared Statements (preferred) and AdhocStatements.
- Simplified memory management: Database and Statement objects can be used as automatic variables, which release all required memory when program flow exists scope.
- Stongly typed API - select results can only be extracted into variables of the correct size, and signed-ness.
- Nullable types allow for simplified assigning to and retrieval of NULL values in database fields.
- C++ API hides most MYSQL C API idiosyncracies and make database code more compact
-- Supports streaming operator (<<) to fill out parameters in a query, and cause an execute or fetch to occur.
-- Supports streaming operator (>>) to retrieve columns from the current result row into Nullable<T> variables.
-- Converts "C" return values, errno, and errmsg values into exceptions using DatabaseException class.


Limitations:
- Embedded database linkage currently causes a Segmentation Fault when executing Prepared Statements that produce a result set.  This appears to be a MYSQL bug.


Building:
-There is an included Makefile that has only been tested on OS/X Mountain Lion, but that an enterprising young person like yourself should be able to modify pretty easily to run on most varieties of Unix.
-In the Makefile, you can set certain variables to control the type of build.
-- EMBEDDED: embedded or <blank>       ## set to embedded to compile against the embedded server
-- LIBRARY_TYPE: dynamic, static, or <blank>     ## type of library produced (blank means no library at all)
-- LIBRARY_NAME: <name>      ## name of the library produced.  Called lib<name>.dylib for dynamic library
