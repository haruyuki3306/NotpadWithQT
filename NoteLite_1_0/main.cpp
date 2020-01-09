#include "NoteLiteMainPanel.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTranslator tr;
	bool ok = tr.load("qt_zh_CN.qm");
	a.installTranslator(&tr);
	NoteLiteMainPanel w;
	w.show();
	return a.exec();
}
