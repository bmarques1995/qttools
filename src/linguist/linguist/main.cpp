/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "globals.h"

#include <QtCore/QFile>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtGui/QPixmap>
#include <QtWidgets/QSplashScreen>

#ifdef Q_OS_MAC
#include <QtCore/QUrl>
#include <QtGui/QFileOpenEvent>
#endif // Q_OS_MAC

QT_USE_NAMESPACE

#ifdef Q_OS_MAC
class ApplicationEventFilter : public QObject
{
    Q_OBJECT

public:
    ApplicationEventFilter()
        : m_mainWindow(0)
    {
    }

    void setMainWindow(MainWindow *mw)
    {
        m_mainWindow = mw;
        if (!m_filesToOpen.isEmpty() && m_mainWindow) {
            m_mainWindow->openFiles(m_filesToOpen);
            m_filesToOpen.clear();
        }
    }

protected:
    bool eventFilter(QObject *object, QEvent *event)
    {
        if (object == qApp && event->type() == QEvent::FileOpen) {
            QFileOpenEvent *e = static_cast<QFileOpenEvent*>(event);
            QString file = e->url().toLocalFile();
            if (!m_mainWindow)
                m_filesToOpen << file;
            else
                m_mainWindow->openFiles(QStringList() << file);
            return true;
        }
        return QObject::eventFilter(object, event);
    }

private:
    MainWindow *m_mainWindow;
    QStringList m_filesToOpen;
};
#endif // Q_OS_MAC

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(linguist);

    QApplication app(argc, argv);
    QApplication::setOverrideCursor(Qt::WaitCursor);

#ifdef Q_OS_MAC
    ApplicationEventFilter eventFilter;
    app.installEventFilter(&eventFilter);
#endif // Q_OS_MAC

    QStringList files;
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QStringList args = app.arguments();

    for (int i = 1; i < args.count(); ++i) {
        QString argument = args.at(i);
        if (argument == QLatin1String("-resourcedir")) {
            if (i + 1 < args.count()) {
                resourceDir = QFile::decodeName(args.at(++i).toLocal8Bit());
            } else {
                // issue a warning
            }
        } else if (!files.contains(argument)) {
            files.append(argument);
        }
    }

    QTranslator translator;
    QTranslator qtTranslator;
    QString sysLocale = QLocale::system().name();
    if (translator.load(QLatin1String("linguist_") + sysLocale, resourceDir)) {
        app.installTranslator(&translator);
        if (qtTranslator.load(QLatin1String("qt_") + sysLocale, resourceDir))
            app.installTranslator(&qtTranslator);
        else
            app.removeTranslator(&translator);
    }

    app.setOrganizationName(QLatin1String("QtProject"));
    app.setApplicationName(QLatin1String("Linguist"));

    QSettings config;

    QWidget tmp;
    tmp.restoreGeometry(config.value(settingPath("Geometry/WindowGeometry")).toByteArray());

    QSplashScreen *splash = 0;
    int screenId = QApplication::desktop()->screenNumber(tmp.geometry().center());
    splash = new QSplashScreen(QApplication::desktop()->screen(screenId),
        QPixmap(QLatin1String(":/images/splash.png")));
    if (QApplication::desktop()->isVirtualDesktop()) {
        QRect srect(0, 0, splash->width(), splash->height());
        splash->move(QApplication::desktop()->availableGeometry(screenId).center() - srect.center());
    }
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->show();

    MainWindow mw;
#ifdef Q_OS_MAC
    eventFilter.setMainWindow(&mw);
#endif // Q_OS_MAC
    mw.show();
    splash->finish(&mw);
    QApplication::restoreOverrideCursor();

    mw.openFiles(files, true);

    return app.exec();
}

#ifdef Q_OS_MAC
#include "main.moc"
#endif // Q_OS_MAC
