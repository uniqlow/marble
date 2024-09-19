// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2007 Tim Sutton <tim@linfiniti.com>
//

#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include <QApplication>

#include <QElapsedTimer>
#include <QTest>
#include <QtCore>
#include <QtGui> //needed because this is a gui test

namespace Marble
{

class MarbleWidgetSpeedTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void timeTest();
    void initTestCase(); // will be called before the first testfunction is executed.
    void cleanupTestCase(); // will be called after the last testfunction was executed.
    void init() {}; // will be called before each testfunction is executed.
    void cleanup() {}; // will be called after every testfunction.
private:
    MarbleWidget *m_marbleWidget;
};

void MarbleWidgetSpeedTest::initTestCase()
{
    MarbleDirs::setMarbleDataPath(DATA_PATH);
    MarbleDirs::setMarblePluginPath(PLUGIN_PATH);
    m_marbleWidget = new MarbleWidget();
    m_marbleWidget->show();
}
void MarbleWidgetSpeedTest::cleanupTestCase()
{
    delete m_marbleWidget;
}
void MarbleWidgetSpeedTest::timeTest()
{
    m_marbleWidget->setMapThemeId("plain/plain.dgml");
    m_marbleWidget->setZoom(1500);
    //    m_marbleWidget->resize( 800, 600 );

    QElapsedTimer t;
    // m_marbleWidget->setMapTheme( "plain/plain.dgml" );
    // m_marbleWidget->setMapTheme( "bluemarble/bluemarble.dgml" );

    /*
        m_marbleWidget->setShowGrid( false );
        m_marbleWidget->setShowPlaces( false );
        m_marbleWidget->setShowBorders( false );
        m_marbleWidget->setShowRivers( false );
        m_marbleWidget->setShowLakes( false );
    */

    t.start();

    for (int j = 0; j < 10; ++j) {
        for (int k = 0; k < 10; ++k) {
            m_marbleWidget->moveRight();
            QCoreApplication::sendPostedEvents();
        }
        for (int k = 0; k < 10; ++k) {
            m_marbleWidget->moveLeft();
            QCoreApplication::sendPostedEvents();
        }
    }

    // required maximum elapsed time for test to pass
    QVERIFY(t.elapsed() < 10);
    // required frames per second for test to pass
    // redundant with above really but I leave it in
    // for now...
    unsigned int fps = (uint)(200.0 * 1000.0 / (double)(t.elapsed()));
    QVERIFY(fps > 10);
}

}

// QTEST_MAIN(MarbleWidgetSpeedTest)
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Marble::MarbleWidgetSpeedTest speedTest;
    QTest::qExec(&speedTest);
    return 0;
}

#include "MarbleWidgetSpeedTest.moc"
