#include "qslfilter.h"
#include "qslfilter_p.h"

#include <unistd.h>

#include <QMetaEnum>

using namespace qsl;
using namespace qsl::driver;

const char* QSLFilter::opName(Operator op)
{
	static const QMetaEnum e = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("Operator"));
	return e.valueToKey(op);
}

static void wrongOp(QSLFilter::Operator op, const char* given)
{
	if (isatty(STDOUT_FILENO))
	{
		fprintf(stderr, "\033[1mQSL[Filter]: \033[1mSyntax error:\033[m Wrong arguments given to operator %s\n", QSLFilter::opName(op));
		fprintf(stderr, "\t\033[1;31mArguments given:\033[0m    %s\n", given);
		fprintf(stderr, "\t\033[1;31mArguments expected:\033[0m ");
	}
	else
	{
		fprintf(stderr, "QSL[Filter]: Syntax error: Wrong arguments given to operator %s\n", QSLFilter::opName(op));
		fprintf(stderr, "\tArguments given:    %s\n", given);
		fprintf(stderr, "\tArguments expected: ");
	}
	
	if (op == 0x0) // no-op
		fprintf(stderr, "None\n");
	if (op >= 0x01 && op < 0x20)
		fprintf(stderr, "A column and a value\n");
	if (op >= 0x20 && op < 0x30)
		fprintf(stderr, "A column\n");
	if (op >= 0x30 && op < 0x40)
		fprintf(stderr, "Two filters\n");
	if (op >= 0x40)
		fprintf(stderr, "A filter\n");
}

QSLFilter::QSLFilter()
	: d(new QSLFilterData(noop))
{
}

QSLFilter::QSLFilter(const QString &col, Operator op, const QString &val)
	: d(new QSLFilterData(op))
{
	if (op < 0x1 || op >= 0x20)
	{
		wrongOp(op, "A column and a value");
		return;
	}
	d->args.resize(2);
	d->args[0] = col;
	d->args[1] = val;
}

QSLFilter::QSLFilter(const QString &col, Operator op)
	: d(new QSLFilterData(op))
{
	if (op < 0x20 || op >= 0x30)
	{
		wrongOp(op, "A column");
		return;
	}
	d->args.resize(1);
	d->args[0] = col;
}

QSLFilter::QSLFilter(const QSLFilter &filter0, Operator op, const QSLFilter &filter1)
	: d(new QSLFilterData(op))
{
	if (op < 0x30 || op >= 0x40)
	{
		wrongOp(op, "Two filters");
		return;
	}
	d->filters.resize(2);
	d->filters[0] = filter0;
	d->filters[1] = filter1;
}

QSLFilter::QSLFilter(Operator op, const QSLFilter &filter)
	: d(new QSLFilterData(op))
{
	if (op < 0x40)
	{
		wrongOp(op, "A filter");
		return;
	}
	d->filters.resize(1);
	d->filters[0] = filter;
}

QSLFilter::Operator QSLFilter::op() const
{
	return d->op;
}

QString QSLFilter::arg(int i) const
{
	return d->args[i];
}

QSLFilter QSLFilter::filter(int i) const
{
	return d->filters[i];
}

QSLFilter qsl::qsl_filter(const QList<QSLFilterExprType> &args)
{
	// if no args return an empty filter
	if (args.size() == 0)
		return QSLFilter();
	
	if (args[0].isOperator())
	{
		// the op must be followed by another filter
		if (args.size() < 2)
		{
			fprintf(stderr, "QSL[Filter]: Error: Operator %s should be followed by a filter\n", QSLFilter::opName(args[0].op()));
			return QSLFilter();
		}
		return QSLFilter(args[0].op(), qsl_filter(args.mid(1)));
	}
	
	// now the next argument should be an operator
	if (args.size() < 2 || !args[1].isOperator())
	{
		fprintf(stderr, "QSL[Filter]: Error: Value '%s' should be followed by an operator\n", qPrintable(args[1].str()));
		return QSLFilter();
	}
	
	QSLFilter f;
	int in = 0;
	
	// it could either be an operator with 2 arguments
	if (args[1].op() >= 0x01 && args[1].op() < 0x20)
	{
		if (args.size() < 3)
		{
			fprintf(stderr, "QSL[Filter]: Error: Operator %s should be followed by its second argument\n", QSLFilter::opName(args[1].op()));
			return QSLFilter();
		}
		f = QSLFilter(args[0].str(), args[1].op(), args[2].str());
		in = 3;
	}
	
	// or an operator with 1 argument
	else
	{
		f = QSLFilter(args[0].str(), args[1].op());
		in = 2;
	}
	
	// everything can be followed by the next operator and the next filter
	if (args.size() > in)
	{
		if (!args[in].isOperator())
		{
			fprintf(stderr, "QSL[Filter]: Error: Filter can be followed by end-of-expression or operator but is followed by '%s'\n", qPrintable(args[in].str()));
			return QSLFilter();
		}
		f = QSLFilter(f, args[in].op(), qsl_filter(args.mid(in+1)));
	}
	
	return f;
}




QSLFilter::QSLFilter(const QSLFilter &other)
	: QObject()
	, d(other.d)
{
}
QSLFilter::QSLFilter(QSLFilter &&other)
	: QObject()
	, d(other.d)
{
}
QSLFilter::~QSLFilter()
{
}

QSLFilter& QSLFilter::operator= (const QSLFilter &other)
{
	d = other.d;
	return *this;
}
QSLFilter& QSLFilter::operator= (QSLFilter &&other)
{
	d = other.d;
	return *this;
}
