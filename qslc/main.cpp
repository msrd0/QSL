#include "generator.h"

#include "parser/parser.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

using namespace qsl::qslc;

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("QSLC");
	QCoreApplication::setApplicationVersion(QSL_VERSION_STR);
	
	QCommandLineParser parser;
	parser.setApplicationDescription("QSL Compiler");
	parser.addHelpOption();
	parser.addVersionOption();
	QCommandLineOption targetDirOption(QStringList() << "d" << "dir", "The target directory to put the generated files", "dir", ".");
	parser.addOption(targetDirOption);
	QCommandLineOption qtypeOption(QStringList() << "q" << "qtype", "Use Qt types instead of std:: types");
	parser.addOption(qtypeOption);
	parser.addPositionalArgument("file", "The input file(s) to compile", "<file> [<file> ...]");
	parser.process(app);
	QStringList args = parser.positionalArguments();
	
	if (args.size() < 1)
	{
		parser.showHelp(1);
		return 1;
	}
	
	QDir dir(parser.value(targetDirOption));
	if (!dir.exists())
	{
		fprintf(stderr, "Target directory %s doesn't exist", qPrintable(dir.absolutePath()));
		return 1;
	}
	bool qtype = parser.isSet(qtypeOption);
	
	for (QString file : args)
	{
		Database *db = parse(file, qtype);
		if (!generate(db, dir, qtype))
		{
			fprintf(stderr, "Failed to process file %s\n", qPrintable(file));
			return 1;
		}
	}
	
	return 0;
}
