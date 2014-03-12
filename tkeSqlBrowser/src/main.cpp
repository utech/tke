#include <QApplication>
#include "browser.h"
//
int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	Browser win(0,0,"data");
	win.setRoot(true);
	win.show(); 
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
