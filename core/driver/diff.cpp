#include "diff.h"
#include "qslnamespace.h"

#include <string.h>

#include <QHash>

using namespace qsl;
using namespace qsl::driver;

#define DUMMY_COLUMN QSLColumn("dummy", "dummy", -1, QSL::none)


ConstraintDifference::ConstraintDifference(const QSLColumn &a, const QSLColumn &b)
	: _colName(a.name())
{
	Q_ASSERT(a.name() == b.name());
	
	uint8_t both = a.constraints() & b.constraints();
	_constraintsAdded = b.constraints() - both;
	_constraintsRemoved = a.constraints() - both;
}


TableDiff::TableDiff(const QSLTable &a, const QSLTable &b)
	: _a(a)
	, _b(b)
{
	computeDiff();
}

void TableDiff::computeDiff()
{
	QHash<QByteArray, QSLColumn> acols, bcols;
	for (auto col : a().columns())
		acols.insert(col.name(), col);
	for (auto col : b().columns())
		bcols.insert(col.name(), col);
	
	// added cols
	for (QByteArray col : bcols.keys())
		if (!acols.contains(col))
			_addedCols << bcols.value(col, DUMMY_COLUMN);
	// removed cols
	for (QByteArray col : acols.keys())
		if (!bcols.contains(col))
			_removedCols << acols.value(col, DUMMY_COLUMN);
	
	// compare cols that are present in both tables
	for (QByteArray colname : acols.keys())
	{
		if (!bcols.contains(colname))
			continue;
		QSLColumn cola = acols.value(colname, DUMMY_COLUMN);
		QSLColumn colb = bcols.value(colname, DUMMY_COLUMN);
		
		if ((cola.minsize() != -1 && colb.minsize() != -1 && cola.minsize() != colb.minsize())
				|| strcoll(cola.type(), colb.type()) != 0)
			_typeChanged << colb;
		
		else if (cola.constraints() != colb.constraints())
			_constraintsChanged << ConstraintDifference(cola, colb);
	}
}
