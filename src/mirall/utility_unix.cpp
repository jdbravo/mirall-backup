/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 * Copyright (C) by Daniel Molkentin <danimo@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

static void setupFavLink_private(const QString &folder) {
    // Nautilus: add to ~/.gtk-bookmarks
    QFile gtkBookmarks(QDir::homePath()+QLatin1String("/.gtk-bookmarks"));
    QByteArray folderUrl = "file://" + folder.toUtf8();
    if (gtkBookmarks.open(QFile::ReadWrite)) {
        QByteArray places = gtkBookmarks.readAll();
        if (!places.contains(folderUrl)) {
            places += folderUrl;
            gtkBookmarks.reset();
            gtkBookmarks.write(places + '\n');
        }
    }
}

bool hasLaunchOnStartup_private(const QString &appName)
{
    QString userAutoStartPath = QDir::homePath()+QLatin1String("/.config/autostart/");
    QString desktopFileLocation = userAutoStartPath+appName+QLatin1String(".desktop");
    return QFile::exists(desktopFileLocation);
}


void setLaunchOnStartup_private(const QString &appName, const QString& guiName, bool enable)
{
    QString userAutoStartPath = QDir::homePath()+QLatin1String("/.config/autostart/");
    QString desktopFileLocation = userAutoStartPath+appName+QLatin1String(".desktop");
    if (enable) {
        if (!QDir().exists(userAutoStartPath) && !QDir().mkdir(userAutoStartPath)) {
            qDebug() << "Could not create autostart directory";
            return;
        }
        QFile iniFile(desktopFileLocation);
        if (!iniFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Could not write auto start entry" << desktopFileLocation;
            return;
        }
        QTextStream ts(&iniFile);
        ts.setCodec("UTF-8");
        ts << QLatin1String("[Desktop Entry]") << endl
           << QLatin1String("Name=") << guiName << endl
           << QLatin1String("GenericName=") << QLatin1String("File Synchronizer") << endl
           << QLatin1String("Exec=") << QCoreApplication::applicationFilePath() << endl
           << QLatin1String("Terminal=") << "false" << endl
           << QLatin1String("Icon=") << appName << endl
           << QLatin1String("Categories=") << QLatin1String("Network") << endl
           << QLatin1String("Type=") << QLatin1String("Application") << endl
           << QLatin1String("StartupNotify=") << "false" << endl
           << QLatin1String("X-GNOME-Autostart-enabled=") << "true" << endl
            ;
    } else {
        if (!QFile::remove(desktopFileLocation)) {
            qDebug() << "Could not remove autostart desktop file";
        }
    }
}
