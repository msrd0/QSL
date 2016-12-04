#pragma once

/// @file spisquery.h This file contains the `spis::SPISQuery` class that is internally used
/// by the generated code to describe queries.

#include "spisfilter.h"
#include "spisnamespace.h"
#include "spistable.h"

namespace spis {

namespace driver { class Driver; }

/**
 * This class helps creating an SQL Query for a SPISTable. It provides methods
 * for creating SQL queries for different drivers, but it doesn't execute
 * anything.
 * 
 * You probably don't need to use this class directly. Have a look at `SPISTableQuery`.
 */
class SPIS_PUBLIC SPISQuery
{
public:
	/// Creates a new `SPISQuery` for the given `SPISTable` with the given type.
	SPISQuery(SPISTable *tbl)
		: _tbl(tbl)
		, _orderBy(tbl->primaryKey())
	{
		Q_ASSERT(tbl);
	}
	
	/// Overwrite the filter used by `SELECT` queries.
	template<typename F>
	void applyFilter(const F &filter) { _filter = filter; }
	/// Overwrite the limit of resulting columns.
	void applyLimit(int l) { _limit = l; }
	/// Return result of `SELECT` queries in ascending order.
	void applyAsc() { _asc = true; }
	/// Return result of `SELECT` queries in descending order.
	void applyDesc() { _asc = false; }
	/// Order by the given column.
	void applyOrderBy(const QByteArray &col) { _orderBy = col; }
	
protected:
	
	/// The table that this query should create for.
	SPISTable *_tbl;
	/// The filter to be used in a SELECT query.
	SPISFilter _filter;
	/// The maximum rows to be fetched in a SELECT query.
	int _limit = -1;
	/// If true, a SELECT query will return in ascending order, otherwise descending.
	bool _asc = true;
	/// The name of the column by that the query should be ordered. Default is the primary
	/// key or none if the table doesn't have a pk.
	QByteArray _orderBy;
};

/**
 * This class is used by every generated database to provide access to the tables.
 * The `spis::db::<db-name>::<tbl-name>()` method will always return a subclass of
 * `SPISTableQuery`.
 * 
 * ## Select Query
 * To select something from the database, use the `query` method, and call `filter`
 * or `limit` before if you want to. A select query can look like this for a database
 * called `foo` and a table called `bar`:
 * 
 * ```
 * using namespace spis;
 * using namespace spis::filters;
 * // ...
 * db::foo db(...);
 * // ...
 * 
 * // get the query object
 * auto query = db.bar(); // auto = db::foo::bar_q
 * // filter the query to only include rows where the column "comment" equals "hello world"
 * query.filter(eq("comment", "hello world"));
 * // limit the query to 5 results
 * query.limit(5);
 * // now, select the rows from the database
 * auto result = query.query(); // auto = std::vector<db::foo::bar_t> (or QList if qtype)
 * ```
 * 
 * ## Insert Query
 * To insert something into the database, use one of the provided `insert` methods.
 * An insert query con look like this for a database called `foo` and a table called `bar`
 * (with `bar` having one column called `id` as an int):
 * 
 * ```
 * using namespace spis;
 * // ...
 * db::foo db(...);
 * // ...
 * 
 * // insert only one object
 * bool success = db.bar().insert({1});
 * // insert multiple objects. Note that those functions only exists in the generated
 * // source because virtual templates are not allowed.
 * std::vector<db::foo::bar_t> elements;
 * elements.push_back({1});
 * elements.push_back({2});
 * elements.push_back({3});
 * // now, either insert via iterators (usefull for arrays etc):
 * bool success = db.bar().insert(elements.begin(), elements.end());
 * // or via the container itself:
 * bool success = db.bar.insert(elements);
 * ```
 */
template<typename T, typename container = std::vector<T>>
class SPIS_PUBLIC SPISTableQuery : public SPISQuery
{
protected:
	/// Creates a new `SPISTableQuery` for the given `SPISTable`.
	SPISTableQuery(SPISTable *tbl)
		: SPISQuery(tbl)
	{
	}
	
public:
	/// Perform a select query using previously applied filters and limits.
	virtual container query() = 0;
	
	/// Insert one element into the table.
	virtual bool insert(const T &row) = 0;
	
	/// Delete the rows from the table that match the previously applied filter.
	virtual bool remove() = 0;
	/// Delete the given row from the table.
	virtual bool remove(const T &row) = 0;
};

}
