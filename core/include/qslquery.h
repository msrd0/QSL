#pragma once

#include "qsl_global.h"
#include "qslfilter.h"
#include "qslnamespace.h"

#include <QSqlDatabase>
#include <QVector>

namespace qsl {
class QSLTable;

/**
 * This class helps creating an SQL Query for a QSLTable. It provides methods
 * for creating SQL queries for different drivers, but it doesn't execute
 * anything.
 * 
 * You probably don't need to use this class directly. Have a look at `QSLTableQuery`.
 */
class QSLQuery
{
private:
	static QHash<QSL::Driver, QJsonObject> _driverJson;
public:
	/// Returns the json for the driver. See the contents of the core/drivers directory
	/// for more information.
	static QJsonObject driverJson(QSL::Driver driver);
	/// Returns the SQL typename of the given type that should have at least minsize
	/// elements/bits if the database supports.
	QByteArray driverType(QSL::Driver driver, const QByteArray &type, uint32_t minsize) const;
	
	/// Creates a new `QSLQuery` for the given `QSLTable` with the given type.
	QSLQuery(QSLTable *tbl, QSL::QueryType type);
	
	/// Overwrite the filter used by `SELECT` queries.
	template<typename F>
	void applyFilter(const F &filter) { _filter = QSharedPointer<QSLFilter>(new F(filter)); }
	/// Overwrite the limit of resulting columns.
	void limit(int l) { _limit = l; }
	
	/// Returns an SQL expression for the given driver.
	QString sql(QSL::Driver driver) const;
	
protected:
	/// Creates a new `QSLQuery` for the given `QSLTable`. Note that the type will be set
	/// to `QSL::UnknownQueryType`, this has to be overwritten by the subclass before the
	/// `sql()` method can be invoked.
	QSLQuery(QSLTable *tbl);
	
	/// The table that this query should create for.
	QSLTable *_tbl;
	/// The type of the query.
	QSL::QueryType _type;
	/// The rows to be inserted if `_type` is `QSL::InsertQuery`.
	QList<QVector<QVariant>> _rows;
	
private:
	QSharedPointer<QSLFilter> _filter = QSharedPointer<QSLFilter>(new QSLFilter);
	int _limit = -1;
};

/**
 * This class is used by every generated database to provide access to the tables.
 * The `qsl::db::<db-name>::<tbl-name>()` method will always return a subclass of
 * `QSLTableQuery`.
 * 
 * ## Select Query
 * To select something from the database, use the `query` method, and call `filter`
 * or `limit` before if you want to. A select query can look like this for a database
 * called `foo` and a table called `bar`:
 * 
 * ```
 * using namespace qsl;
 * using namespace qsl::filters;
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
 * using namespace qsl;
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
class QSLTableQuery : public QSLQuery
{
protected:
	/// Creates a new `QSLTableQuery` for the given `QSLTable`.
	QSLTableQuery(QSLTable *tbl)
		: QSLQuery(tbl)
	{
	}
	
public:
	/// Perform a select query using previously applied filters and limits.
	virtual container query() = 0;
	
	/// Insert one element into the table.
	virtual bool insert(const T &row) = 0;
};

}
