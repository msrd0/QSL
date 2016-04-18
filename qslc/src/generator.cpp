#include "generator.h"

#include <database.h>
#include <table.h>

#include <QFile>

bool qsl::qslc::generate(Database *db, const QDir &dir)
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
	out.write("#include <qsltable.h>\n\n");
	
	out.write("namespace qsl {\n");
	out.write("namespace db {\n\n");
	
	
	out.write("class " + db->name() + " : public QSLDB\n");
	out.write("{\n");
//	out.write("  Q_OBJECT\n");
	out.write("public:\n");
	out.write("  static constexpr const char* charset = \"" + db->charset() + "\";\n");
	out.write("  static constexpr const bool usevar = " + QByteArray(db->usevar() ? "true" : "false") + ";\n\n");
	
	for (Table *t : db->tables())
	{
		out.write("public:\n");
		out.write("  class " + t->name() + "_t\n");
		out.write("  {\n");
		out.write("  private:\n");
		out.write("    " + db->name() + " *_parent;\n");
		out.write("  public:\n");
		
		// ctor
		out.write("    " + t->name() + "_t(" + db->name() + " *parent");
		for (Column &f : t->fields())
		{
			out.write(", " + f.cppType() + " " + f.name());
		}
		out.write(")\n");
		out.write("      : _parent(parent)\n");
		for (Column &f : t->fields())
			out.write("      , _" + f.name() + "(" + f.name() + ")\n");
		out.write("    {\n");
		out.write("    }\n");
		
		// variables & getters
		for (Column &f : t->fields())
		{
			out.write("  private:\n");
			out.write("    " + f.cppType() + " _" + f.name() + ";\n");
			out.write("  public:\n");
			out.write("    " + f.cppType() + " " + f.name() + "() const { return _" + f.name() + "; }\n");
		}
		
		out.write("  };\n");
		out.write("private:\n");
		out.write("  QSLTable _tbl_" + t->name() + ";\n\n");
	}
	
	out.write("public:\n");
	out.write("  " + db->name() + "(Driver driver)\n");
	out.write("    : QSLDB(\"" + db->name() + "\", driver)\n");
	for (Table *t : db->tables())
		out.write("    , _tbl_" + t->name() + "(\"" + t->name() + "\", this)\n");
	out.write("  {\n");
	for (Table *t : db->tables())
	{
		for (Column &f : t->fields())
			out.write("    _tbl_" + t->name() + ".addColumn(QSLColumn(\"" + f.name() + "\", \"" + f.type() + "\", " + QByteArray::number(f.constraints()) + "));\n");
		out.write("    registerTable(&_tbl_" + t->name() + ");\n");
	}
	out.write("  }\n");
	
	out.write("};\n\n"); // class db->name()
	
	out.write("}\n"); // namespace db
	out.write("}\n"); // namespace qsl
	
	out.close();
	return true;
}
