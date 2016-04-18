#pragma once

#include "qsl_global.h"

#include <QObject>

class QSL : public QObject
{
	Q_OBJECT
	
public:
	
	/// This enum contains all supported drivers.
	enum Driver
	{
		PostgreSQL,
		MySQL,
		SQLite
	};
	Q_ENUM(Driver)
	
	enum ColumnConstraint : uint8_t
	{
		none       = 0x0,
		primarykey = 0x1,
		unique     = 0x2,
		notnull    = 0x4
	};
	Q_ENUM(ColumnConstraint)
	
	/// The different types of queries that can be created.
	enum QueryType
	{
		CreateTable,
		SelectTable,
		InsertTable,
		UpdateTable
	};
	Q_ENUM(QueryType)
};
