#include "spisfilter.h"
#include "spisfilter_p.h"

#include <unistd.h>

#include <QMetaEnum>

using namespace spis;
using namespace spis::driver;

const char* SPISFilter::opName(Operator op)
{
	static const QMetaEnum e = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("Operator"));
	return e.valueToKey(op);
}

static void wrongOp(SPISFilter::Operator op, const char* given)
{
	if (isatty(STDOUT_FILENO))
	{
		fprintf(stderr, "\033[1mSPIS[Filter]: \033[1mSyntax error:\033[m Wrong arguments given to operator %s\n", SPISFilter::opName(op));
		fprintf(stderr, "\t\033[1;31mArguments given:\033[0m    %s\n", given);
		fprintf(stderr, "\t\033[1;31mArguments expected:\033[0m ");
	}
	else
	{
		fprintf(stderr, "SPIS[Filter]: Syntax error: Wrong arguments given to operator %s\n", SPISFilter::opName(op));
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

SPISFilter::SPISFilter()
	: d(new SPISFilterData(noop))
{
}

SPISFilter::SPISFilter(const QString &col, Operator op, const QString &val)
	: d(new SPISFilterData(op))
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

SPISFilter::SPISFilter(const QString &col, Operator op)
	: d(new SPISFilterData(op))
{
	if (op < 0x20 || op >= 0x30)
	{
		wrongOp(op, "A column");
		return;
	}
	d->args.resize(1);
	d->args[0] = col;
}

SPISFilter::SPISFilter(const SPISFilter &filter0, Operator op, const SPISFilter &filter1)
	: d(new SPISFilterData(op))
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

SPISFilter::SPISFilter(Operator op, const SPISFilter &filter)
	: d(new SPISFilterData(op))
{
	if (op < 0x40)
	{
		wrongOp(op, "A filter");
		return;
	}
	d->filters.resize(1);
	d->filters[0] = filter;
}

SPISFilter::Operator SPISFilter::op() const
{
	return d->op;
}

QString SPISFilter::arg(int i) const
{
	return d->args[i];
}

SPISFilter SPISFilter::filter(int i) const
{
	return d->filters[i];
}

SPISFilter spis::spis_filter(const QList<SPISFilterExprType> &args)
{
	// if no args return an empty filter
	if (args.size() == 0)
		return SPISFilter();
	
	if (args[0].isOperator())
	{
		// the op must be followed by another filter
		if (args.size() < 2)
		{
			fprintf(stderr, "SPIS[Filter]: Error: Operator %s should be followed by a filter\n", SPISFilter::opName(args[0].op()));
			return SPISFilter();
		}
		return SPISFilter(args[0].op(), spis_filter(args.mid(1)));
	}
	
	// now the next argument should be an operator
	if (args.size() < 2 || !args[1].isOperator())
	{
		fprintf(stderr, "SPIS[Filter]: Error: Value '%s' should be followed by an operator\n", qPrintable(args[1].str()));
		return SPISFilter();
	}
	
	SPISFilter f;
	int in = 0;
	
	// it could either be an operator with 2 arguments
	if (args[1].op() >= 0x01 && args[1].op() < 0x20)
	{
		if (args.size() < 3)
		{
			fprintf(stderr, "SPIS[Filter]: Error: Operator %s should be followed by its second argument\n", SPISFilter::opName(args[1].op()));
			return SPISFilter();
		}
		f = SPISFilter(args[0].str(), args[1].op(), args[2].str());
		in = 3;
	}
	
	// or an operator with 1 argument
	else
	{
		f = SPISFilter(args[0].str(), args[1].op());
		in = 2;
	}
	
	// everything can be followed by the next operator and the next filter
	if (args.size() > in)
	{
		if (!args[in].isOperator())
		{
			fprintf(stderr, "SPIS[Filter]: Error: Filter can be followed by end-of-expression or operator but is followed by '%s'\n", qPrintable(args[in].str()));
			return SPISFilter();
		}
		f = SPISFilter(f, args[in].op(), spis_filter(args.mid(in+1)));
	}
	
	return f;
}




SPISFilter::SPISFilter(const SPISFilter &other)
	: QObject()
	, d(other.d)
{
}
SPISFilter::SPISFilter(SPISFilter &&other)
	: QObject()
	, d(other.d)
{
}
SPISFilter::~SPISFilter()
{
}

SPISFilter& SPISFilter::operator= (const SPISFilter &other)
{
	d = other.d;
	return *this;
}
SPISFilter& SPISFilter::operator= (SPISFilter &&other)
{
	d = other.d;
	return *this;
}
