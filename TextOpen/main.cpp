#include "TextOpen.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTranslator language;
	language.load(":/language/qt_zh_CN.qm");
	a.installTranslator(&language);
	TextOpen w;
	w.show();
	if (argc > 1)
		w.loadOpenFile(QString::fromLocal8Bit(argv[1]));
	return a.exec();
}
