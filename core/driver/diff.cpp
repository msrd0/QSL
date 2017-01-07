#include "diff.h"
#include "spisdb.h"
#include "spisnamespace.h"

#include <string.h>

#include <QDebug>
#include <QHash>

using namespace spis;
using namespace spis::driver;

#define DUMMY_COLUMN SPISColumn("dummy", "dummy", -1, SPIS::none, QVariant())


ConstraintDifference::ConstraintDifference(const SPISColumn &a, const SPISColumn &b)
	: _colName(a.name())
{
	Q_ASSERT(a.name() == b.name());
	
	uint8_t both = a.constraints() & b.constraints();
	_constraintsAdded = b.constraints() - both;
	_constraintsRemoved = a.constraints() - both;
}


TableDiff::TableDiff(const SPISTable &a, SPISTable &b)
	: _a(a)
	, _b(&b)
{
	computeDiff();
}

static SPISColumn findCol(const SPISColumn &col, const QHash<QByteArray, SPISColumn> &tbl)
{
	for (auto name : col.alternateNames())
		if (tbl.contains(name))
			return tbl.value(name, DUMMY_COLUMN);
	return DUMMY_COLUMN;
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
		if (findCol(bcols.value(col, DUMMY_COLUMN), acols).type() == "dummy")
			_addedCols << bcols.value(col, DUMMY_COLUMN);
	// removed cols
	for (QByteArray col : acols.keys())
		if (findCol(acols.value(col, DUMMY_COLUMN), bcols).type() == "dummy")
			_removedCols << acols.value(col, DUMMY_COLUMN);
	
	// compare cols that are present in both tables
	for (QByteArray colname : acols.keys())
	{
		SPISColumn cola = acols.value(colname, DUMMY_COLUMN);
		Q_ASSERT(cola.type() != "dummy");
		SPISColumn colb = findCol(cola, bcols);
		if (colb.type() == "dummy")
			continue;
		
		QByteArray typea = cola.type(), typeb = colb.type();
		int minsizea = cola.minsize(), minsizeb = colb.minsize();
		
		// resolve foreign keys
		if (typea[0] == '&' || typeb[0] == '&')
		{
			SPISDB *db = a().db();
			if (!db)
				db = b().db();
			if (db)
			{
				if (typea.startsWith('&'))
				{
					QByteArray tbl = typea.mid(1, typea.indexOf('.') - 1);
					QByteArray field = typea.mid(tbl.size() + 2);
					SPISTable *t = db->table(tbl);
					if (!t)
						qWarning() << "SPIS[Driver]: Unable to find table" << tbl;
					else
					{
						SPISColumn col = t->column(field);
						if (strcoll(col.type(), "invalid") == 0)
							qWarning() << "SPIS[Driver]: Unable to find column" << field << "of table" << tbl;
						typea = col.type();
						minsizea = col.minsize();
					}
				}
				if (typeb.startsWith('&'))
				{
					QByteArray tbl = typeb.mid(1, typeb.indexOf('.') - 1);
					QByteArray field = typeb.mid(tbl.size() + 2);
					SPISTable *t = db->table(tbl);
					if (!t)
						qWarning() << "SPIS[Driver]: Unable to find table" << tbl;
					else
					{
						SPISColumn col = t->column(field);
						if (strcoll(col.type(), "invalid") == 0)
							qWarning() << "SPIS[Driver]: Unable to find column" << field << "of table" << tbl;
						typeb = col.type();
						minsizeb = col.minsize();
					}
				}
			}
			else
				qWarning() << "SPIS[Driver]: Unable to find SPISDB for given tables; cannot resolve foreign keys";
		}
		
		if ((minsizea != -1 && minsizeb != -1 && minsizea != minsizeb)
				|| (typea != typeb
					&& !(typea == "password" && typeb == "text")
					&& !(typea == "text" && typeb == "password")))
			_typeChanged << colb;
		
		else if (cola.constraints() != colb.constraints())
			_constraintsChanged << ConstraintDifference(cola, colb);
		
		else if (cola.def().toString() != colb.def().toString())
			_defChanged << colb;
	}
}
