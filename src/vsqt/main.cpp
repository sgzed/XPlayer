#include "vsqt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Vsqt w;
	w.show();
	return a.exec();
}
