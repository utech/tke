#include "browser.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

void addConnectionsFromCommandline(const QStringList &args, Browser *browser)
{
    for (int i = 1; i < args.count(); ++i) {
        QUrl url(args.at(i), QUrl::TolerantMode);
        if (!url.isValid()) {
            qWarning("Invalid URL: %s", qPrintable(args.at(i)));
            continue;
        }
        QSqlError err = browser->addConnection(url.scheme(), url.path().mid(1), url.host(),
                                               url.userName(), url.password(), url.port(-1));
        if (err.type() != QSqlError::NoError)
            qDebug() << "Unable to open connection:" << err;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));
	
    QMainWindow mainWin;
    mainWin.setWindowTitle("ТКЕнерго менеджер Qt SQL Browser");

    Browser browser(&mainWin);
    mainWin.setCentralWidget(&browser);

    QMenu *fileMenu = mainWin.menuBar()->addMenu("&Файл");
    fileMenu->addAction("Створити &з'єднання...", &browser, SLOT(addConnection()));
    fileMenu->addSeparator();
    fileMenu->addAction("&Вихід", &app, SLOT(quit()));

    QMenu *helpMenu = mainWin.menuBar()->addMenu("&Допомога");
    helpMenu->addAction("Про програму", &browser, SLOT(about()));
    helpMenu->addAction("Про Qt", qApp, SLOT(aboutQt()));

    QObject::connect(&browser, SIGNAL(statusMessage(QString)),
                     mainWin.statusBar(), SLOT(showMessage(QString)));
	
	QStringList dbList;
	dbList << "";
	dbList << "QSQLITE://localhost/tkeDBUpdate.tkeDB";
	dbList << "QSQLITE://localhost/tkeDBUpdateResults.tkeDB";
	dbList << "QSQLITE://localhost/tke.testDb";
	addConnectionsFromCommandline(dbList, &browser);
    addConnectionsFromCommandline(app.arguments(), &browser);
    mainWin.show();
    if (QSqlDatabase::connectionNames().isEmpty())
        QMetaObject::invokeMethod(&browser, "addConnection", Qt::QueuedConnection);

    return app.exec();
}
