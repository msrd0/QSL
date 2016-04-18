#pragma once

#include "qsl_global.h"

#include <QObject>

namespace qsl {

class QSLColumn : public QObject
{
	Q_OBJECT
	
public:
	enum Constraint : uint8_t
	{
		none       = 0x0,
		primarykey = 0x1,
		unique     = 0x2,
		notnull    = 0x4
	};
	Q_ENUM(Constraint)
	
	QSLColumn(const char* name, const char* type, uint8_t constraints);
	QSLColumn(const QSLColumn &other);
	
	QSLColumn& operator= (const QSLColumn &other);
	
	QByteArray name() const { return _name; }
	const char* type() const { return _type; }
	uint8_t constraints() const { return _constraints; }
	
private:
	QByteArray _name;
	const char* _type;
	uint8_t _constraints;
};

}
