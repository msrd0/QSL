#include "generator.h"

#include <database.h>
#include <table.h>

#include <QFile>

bool qsl::qslc::generate(Database *db, const QDir &dir, bool qtype)
{
	if (!db)
		return false;
	QFile out(dir.absoluteFilePath("db_" + db->name() + ".h"));
	if (!out.open(QIODevice::WriteOnly))
	{
		fprintf(stderr, "Unable to write file %s: %s\n", qPrintable(out.fileName()), qPrintable(out.errorString()));
		return false;
	}
	out.write("/* GENERATED FILE - DO NOT EDIT */\n");
	out.write("#pragma once\n\n");
	out.write("#include <qsldb.h>\n");
	out.write("#include <qslquery.h>\n");
	out.write("#include <qsltable.h>\n");
	out.write("#include <qslvariant.h>\n\n");
	out.write("#include <QSqlError>\n");
	out.write("#include <QSqlQuery>\n\n");
	
	out.write("namespace qsl {\n");
	out.write("namespace db {\n\n");
	
	
	out.write("class " + db->name() + " : public QSLDB\n");
	out.write("{\n");
	out.write("private:\n");
	out.write("  static constexpr const char* _charset = \"" + db->charset() + "\";\n");
	out.write("  static constexpr const bool _usevar = " + QByteArray(db->usevar() ? "true" : "false") + ";\n");
	out.write("public:\n");
	out.write("  const char* charset() const { return _charset; }\n");
	out.write("  bool usevar() const { return _usevar; }\n\n");
	
	QByteArray list_t = qtype ? "QList" : "std::vector";
	
	for (Table *t : db->tables())
	{
		if (t->primaryKey().isEmpty())
			fprintf(stderr, "WARNING: No primary key found in table %s, some features may not be available\n", t->name().data());
		
		out.write("public:\n");
		out.write("  class " + t->name() + "_t\n");
		out.write("  {\n");
		out.write("  private:\n");
		out.write("    QSLTable *_parent;\n");
		out.write("  public:\n");
		
		// ctor for select
		out.write("    " + t->name() + "_t(QSLTable *parent");
		for (Column &f : t->fields())
			out.write(", " + f.cppArgType() + " " + f.name());
		out.write(")\n");
		out.write("      : _parent(parent)\n");
		for (Column &f : t->fields())
			out.write("      , _" + f.name() + "(" + f.name() + ")\n");
		out.write("    {\n");
		out.write("      Q_ASSERT(parent);\n");
		out.write("    }\n");
		
		// ctor for insert
		out.write("    " + t->name() + "_t(");
		int i = 0;
		for (Column &f : t->fields())
		{
			if (f.constraints() & QSL::primarykey != 0)
				continue;
			if (i > 0)
				out.write(", ");
			out.write(f.cppArgType() + " " + f.name());
		}
		out.write(")\n");
		out.write("      : _parent(0)\n");
		i = 0;
		for (Column &f : t->fields())
		{
			if (f.constraints() & QSL::primarykey != 0)
				continue;
			out.write("      , _" + f.name() + "(" + f.name() + ")\n");
		}
		out.write("    {\n");
		out.write("    }\n\n");
		
		// variables & getters
		for (Column &f : t->fields())
		{
			out.write("  private:\n");
			out.write("    " + f.cppType() + " _" + f.name() + ";\n");
			out.write("  public:\n");
			out.write("    " + f.cppType() + " " + f.name() + "() const { return _" + f.name() + "; }\n");
			if ((f.constraints() & QSL::primarykey) == 0)
			{
				out.write("    bool set" + f.name().mid(0,1).toUpper() + f.name().mid(1) + "(" + f.cppArgType() + " " + f.name() + ")\n");
				out.write("    {\n");
				out.write("      _" + f.name() + " = " + f.name() + ";\n");
				if (!t->primaryKey().isEmpty())
				{
					
					out.write("      QSLQuery qq(_parent, QSL::UpdateQuery);\n");
					out.write("      qq.updateq(\"" + f.name() + "\", qslvariant(" + f.name() + "), qslvariant(_" + t->primaryKey() + "));\n");
					out.write("      QSqlQuery q(_parent->db()->db);\n");
					out.write("      if (!q.exec(qq.sql(_parent->db()->driver())))\n");
					out.write("      {\n");
					out.write("        fprintf(stderr, \"QSLQuery: Failed to update " + db->name() + "." + t->name() + ": %s\\n\", qPrintable(q.lastError().text()));\n");
					out.write("        return false;\n");
					out.write("      }\n");
					out.write("      else\n");
					out.write("        return true;\n");
				}
				else
				{
					out.write("      fprintf(stderr, \"WARNING: " + db->name() + "." + t->name() + " has no primary key, won't update table\\n\");\n");
					out.write("      return false;\n");
				}
				out.write("    }\n\n");
			}
		}
		
		// method to create a QVector<QVariant>
		out.write("  public:\n");
		out.write("    QVector<QVariant> toVector() const\n");
		out.write("    {\n");
		out.write("      QVector<QVariant> v(" + QByteArray::number(t->fields().size() - 1) + ");\n");
		i = 0;
		for (Column &f : t->fields())
		{
			if (f.constraints() & QSL::primarykey != 0)
				continue;
			out.write("      v[" + QByteArray::number(i) + "] = qslvariant(_" + f.name() + ");\n");
			i++;
		}
		out.write("      return v;\n");
		out.write("    }\n");
		
		out.write("  };\n");
		
		// class to create the query
		out.write("  class " + t->name() + "_q : public QSLTableQuery<" + t->name() + "_t, " + list_t + "<" + t->name() + "_t>>\n");
		out.write("  {\n");
		out.write("  public:\n");
		out.write("    " + t->name() + "_q(QSLTable *tbl)\n");
		out.write("      : QSLTableQuery(tbl)\n");
		out.write("    {\n");
		out.write("    }\n");
		out.write("    template<typename F>\n");
		out.write("    " + t->name() + "_q& filter(const F &filter) { applyFilter(filter); return *this; }\n");
		out.write("    virtual " + list_t + "<" + t->name() + "_t> query()\n");
		out.write("    {\n");
		out.write("      _type = QSL::SelectQuery;\n");
		out.write("      QSqlQuery q(_tbl->db()->db);\n");
		out.write("      if (!q.exec(sql(_tbl->db()->driver())))\n");
		out.write("      {\n");
		out.write("        fprintf(stderr, \"QSLQuery: Failed to query " + db->name() + "." + t->name() + ": %s\\n\", qPrintable(q.lastError().text()));\n");
		out.write("        return " + list_t + "<" + t->name() + "_t>();\n");
		out.write("      }\n");
		out.write("      " + list_t + "<" + t->name() + "_t> entries;\n");
		out.write("      while (q.next())\n");
		out.write("      {\n");
		out.write("        " + t->name() + "_t entry(_tbl\n");
		for (Column &f : t->fields())
		{
			out.write("          , q.value(\"" + f.name() + "\")\n");
			if (f.type() == "int")
			{
				out.write("#if INT_MAX >= " + QByteArray::number(((qulonglong)1) << (f.minsize() - 1) - 1) + "\n");
				out.write("             .toInt()\n");
				out.write("#else\n");
				out.write("             .toLongLong()\n");
				out.write("#endif\n");
			}
			else if (f.type() == "uint")
			{
				out.write("#if UINT_MAX >= " + QByteArray::number(((qulonglong)1) << f.minsize() - 1) + "\n");
				out.write("             .toUInt()\n");
				out.write("#else\n");
				out.write("             .toULongLong()\n");
				out.write("#endif\n");
			}
			else if (f.type() == "double")
			{
				if (f.minsize() <= 4)
					out.write("             .toFloat()\n");
				else
					out.write("             .toDouble()\n");
			}
			else if (f.type() == "char" || f.type() == "text" || f.type() == "byte" || f.type() == "blob")
			{
				if (qtype)
				{
					if (f.type() == "char" || f.type() == "text")
						out.write("             .toString()\n");
					else
						out.write("             .toByteArray()\n");
				}
				else
					out.write("             .toByteArray().data()\n");
			}
		}
		out.write("        );\n");
		out.write("        entries.push_back(entry);\n");
		out.write("      }\n");
		out.write("      return entries;\n");
		out.write("    }\n");
		out.write("    virtual bool insert(const " + t->name() + "_t &row)\n");
		out.write("    {\n");
		out.write("      _type = QSL::InsertQuery;\n");
		out.write("      _rows.push_back(row.toVector());\n");
		out.write("      QSqlQuery q(_tbl->db()->db);\n");
		out.write("      if (!q.exec(sql(_tbl->db()->driver())))\n");
		out.write("      {\n");
		out.write("        fprintf(stderr, \"QSLQuery: Failed to query " + db->name() + "." + t->name() + ": %s\\n\", qPrintable(q.lastError().text()));\n");
		out.write("        return false;\n");
		out.write("      }\n");
		out.write("      return true;\n");
		out.write("    }\n");
		out.write("    template<typename ForwardIterator>\n");
		out.write("    bool insert(const ForwardIterator &begin, const ForwardIterator &end)\n");
		out.write("    {\n");
		out.write("      _type = QSL::InsertQuery;\n");
		out.write("      return false;\n");
		out.write("    }\n");
		out.write("    template<typename Container>\n");
		out.write("    bool insert(const Container &container)\n");
		out.write("    {\n");
		out.write("      return insert(container.begin(), container.end());\n");
		out.write("    }\n");
		out.write("  };\n");
		
		out.write("private:\n");
		out.write("  QSLTable _tbl_" + t->name() + ";\n");
		out.write("public:\n");
		out.write("  " + t->name() + "_q " + t->name() + "() { return " + t->name() + "_q(&_tbl_" + t->name() + "); }\n\n");
	}
	
	out.write("public:\n");
	out.write("  " + db->name() + "(QSL::Driver driver)\n");
	out.write("    : QSLDB(\"" + db->name() + "\", driver)\n");
	for (Table *t : db->tables())
		out.write("    , _tbl_" + t->name() + "(\"" + t->name() + "\", \"" + t->primaryKey() + "\", this)\n");
	out.write("  {\n");
	for (Table *t : db->tables())
	{
		for (Column &f : t->fields())
			out.write("    _tbl_" + t->name() + ".addColumn(QSLColumn(\"" + f.name() + "\", \"" + f.type() + "\", " + QByteArray::number(f.minsize())
					  + ", " + QByteArray::number(f.constraints()) + "));\n");
		out.write("    registerTable(&_tbl_" + t->name() + ");\n");
	}
	out.write("  }\n");
	
	out.write("};\n\n"); // class db->name()
	
	out.write("}\n"); // namespace db
	out.write("}\n"); // namespace qsl
	
	out.close();
	return true;
}
