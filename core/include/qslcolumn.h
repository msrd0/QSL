#pragma once

#include "qsl_global.h"

#include <QByteArray>

namespace qsl {

class QSLColumn
{
public:
	QSLColumn(const char* name, const char* type, uint8_t constraints);
	
	QByteArray name() const { return _name; }
	const char* type() const { return _type; }
	uint8_t constraints() const { return _constraints; }
	
private:
	QByteArray _name;
	const char* _type;
	uint8_t _constraints;
};

}
