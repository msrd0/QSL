#include "diff.h"
#include "spisdb.h"
#include "spisnamespace.h"

#include <string.h>

#include <QDebug>
#include <QHash>

using namespace spis;
using namespace spis::driver;

#define DUMMY_COLUMN SPISColumn("dummy", "dummy", -1, SPIS::none)


ConstraintDifference::ConstraintDifference(const SPISColumn &a, const SPISColumn &b)
	: _colName(a.name())
{
	Q_ASSERT(a.name() == b.name());
	
	uint8_t both = a.constraints() & b.constraints();
	_constraintsAdded = b.constraints() - both;
	_constraintsRemoved = a.constraints() - both;
}


TableDiff::TableDiff(const SPISTable &a, const SPISTable &b)
	: _a(a)
	, _b(b)
{
	computeDiff();
}

void TableDiff::computeDiff()
{
	QHash<QByteArray, SPISColumn> acols, bcols;
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
		SPISColumn cola = acols.value(colname, DUMMY_COLUMN);
		SPISColumn colb = bcols.value(colname, DUMMY_COLUMN);
		
		if (cola.type()[0] == '&' || colb.type()[0] == '&')
		{
			SPISDB *db = a().db();
			if (!db)
				db = b().db();
			if (db)
			{
				QByteArray typea = cola.type(), typeb = colb.type();
				if (typea.startsWith('&'))
				{
					QByteArray tbl = typea.mid(1, typea.indexOf('.') - 1);
					QByteArray field = typea.mid(tbl.size() + 2);
					SPISTable *t = db->table(tbl);
					if (!t)
						qWarning() << "SPIS[Driver]: Unable to find table" << tbl;
				}
			}
			else
				qWarning() << "SPIS[Driver]: Unable to find SPISDB for given tables; cannot resolve foreign keys";
		}
		
		if ((cola.minsize() != -1 && colb.minsize() != -1 && cola.minsize() != colb.minsize())
				|| (strcoll(cola.type(), colb.type()) != 0
					&& !(strcoll(cola.type(), "password") == 0 && strcoll(colb.type(), "text") == 0)
					&& !(strcoll(cola.type(), "text") == 0 && strcoll(colb.type(), "password") == 0)))
			_typeChanged << colb;
		
		else if (cola.constraints() != colb.constraints())
			_constraintsChanged << ConstraintDifference(cola, colb);
	}
}
