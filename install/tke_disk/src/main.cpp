#include <QApplication>
#include <QTextCodec>
#include <QCleanlooksStyle>

#include "UForm_tkeDisk.h"

//--------------------utech--------------------utech--------------------utech--------------------
int main(int argc, char **argv)
{
	QApplication app(argc,argv);
	QApplication::setStyle(new QCleanlooksStyle());
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));
	
	UForm_tkeDisk *f = new UForm_tkeDisk;
	f->show();

	app.connect(&app,SIGNAL(lastWindowClosed()), &app,SLOT(quit()));
	return app.exec();
}
//--------------------utech--------------------utech--------------------utech--------------------
