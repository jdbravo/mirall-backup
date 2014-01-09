/*
 * Copyright (C) by Daniel Molkentin <danimo@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include <sys/inotify.h>

#include "mirall/inotify.h"
#include "mirall/folderwatcher.h"

#include "mirall/folderwatcher_inotify.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>

namespace Mirall {

static const uint32_t standard_event_mask =
    IN_CLOSE_WRITE | IN_ATTRIB | IN_MOVE |
    IN_CREATE |IN_DELETE | IN_DELETE_SELF |
    IN_MOVE_SELF |IN_UNMOUNT |IN_ONLYDIR |
    IN_DONT_FOLLOW;

FolderWatcherPrivate::FolderWatcherPrivate()
    :QObject(), _inotify(0), _parent(0), _lastMask(0)
{

}

FolderWatcherPrivate::FolderWatcherPrivate(FolderWatcher *p)
    : QObject(), _parent(p), _lastMask(0)

{
    _inotify = new INotify(this, standard_event_mask);
    QObject::connect(_inotify, SIGNAL(notifyEvent(int, int, const QString &)),
                     this, SLOT(slotINotifyEvent(int, int, const QString &)));

    QMetaObject::invokeMethod(this, "slotAddFolderRecursive", Q_ARG(QString, _parent->root()));
}

// attention: result list passed by reference!
bool FolderWatcherPrivate::findFoldersBelow( const QDir& dir, QStringList& fullList )
{
    bool ok = true;
    if( !(dir.exists() && dir.isReadable()) ) {
        qDebug() << "Non existing path coming in: " << dir.absolutePath();
        ok = false;
    } else {
        QStringList nameFilter;
        nameFilter << QLatin1String("*");
        QDir::Filters filter = QDir::Dirs | QDir::NoDotAndDotDot;
        const QStringList pathes = dir.entryList(nameFilter, filter);

        QStringList::const_iterator constIterator;
        for (constIterator = pathes.constBegin(); constIterator != pathes.constEnd();
               ++constIterator) {
            const QString fullPath(dir.path()+QLatin1String("/")+(*constIterator));
            fullList.append(fullPath);
            ok = findFoldersBelow(QDir(fullPath), fullList);
        }
    }

    return ok;
}

void FolderWatcherPrivate::slotAddFolderRecursive(const QString &path)
{
    int subdirs = 0;
    qDebug() << "(+) Watcher:" << path;

    if (!_inotify->addPath(path)) {
        emit _parent->error(tr("Could not monitor directories due to system limitations.\n"
                          "The application will not work reliably. Please check the\n"
                          "documentation for possible fixes."));
    }
    const QStringList watchedFolders(_inotify->directories());

    QStringList allSubfolders;
    if( !findFoldersBelow(QDir(path), allSubfolders)) {
        qDebug() << "Could not traverse all sub folders";
    }
    // qDebug() << "currently watching " << watchedFolders;
    QStringListIterator subfoldersIt(allSubfolders);
    while (subfoldersIt.hasNext()) {
        QString subfolder = subfoldersIt.next();
        // qDebug() << "  (**) subfolder: " << subfolder;
        QDir folder (subfolder);
        if (folder.exists() && !watchedFolders.contains(folder.path())) {
            subdirs++;
            // check that it does not match the ignore list
            foreach ( const QString& pattern, _parent->ignores()) {
                QRegExp regexp(pattern);
                regexp.setPatternSyntax(QRegExp::Wildcard);
                if ( regexp.exactMatch(folder.path()) ) {
                    qDebug() << "* Not adding" << folder.path();
                    continue;
                }

            }
            _inotify->addPath(folder.path());
        }
        else
            qDebug() << "    `-> discarded:" << folder.path();
    }
    if (subdirs >0)
        qDebug() << "    `-> and" << subdirs << "subdirectories";
}

void FolderWatcherPrivate::slotINotifyEvent(int mask, int /*cookie*/, const QString &path)
{
    int lastMask = _lastMask;
    QString lastPath = _lastPath;

    _lastMask = mask;
    _lastPath = path;

    // TODO: Unify behaviour acress backends!
    if( ! _parent->eventsEnabled() ) return;
    qDebug() << "** Inotify Event " << mask << " on " << path;
    // cancel close write events that come after create
    if (lastMask == IN_CREATE && mask == IN_CLOSE_WRITE
        && lastPath == path ) {
        return;
    }

    if (IN_IGNORED & mask) {
        //qDebug() << "IGNORE event";
        return;
    }

    if (IN_Q_OVERFLOW & mask) {
        //qDebug() << "OVERFLOW";
    }

    if (mask & IN_CREATE) {
        //qDebug() << cookie << " CREATE: " << path;
        if (QFileInfo(path).isDir()) {
            //setEventsEnabled(false);
            slotAddFolderRecursive(path);
            //setEventsEnabled(true);
        }
    }
    else if (mask & IN_DELETE) {
        //qDebug() << cookie << " DELETE: " << path;
        if ( QFileInfo(path).isDir() ) {
            removePath(path);
        }
    }
    else if (mask & IN_CLOSE_WRITE) {
        //qDebug() << cookie << " WRITABLE CLOSED: " << path;
    }
    else if (mask & IN_MOVE) {
        //qDebug() << cookie << " MOVE: " << path;
    }
    else {
        //qDebug() << cookie << " OTHER " << mask << " :" << path;
    }

     QStringList ignores = _parent->ignores();

     if( path.endsWith(".csync_journal.db.ctmp") ||
            path.endsWith(".csync_journal.db.ctmp-journal") ||
	    path.endsWith(".csync_journal.db-journal") ||
            path.endsWith(".csync_journal.db")) {
        qDebug() << " ** Inotify ignored for " <<path;
        return;
    }

    foreach (const QString& pattern, ignores) {
        QRegExp regexp(pattern);
        regexp.setPatternSyntax(QRegExp::Wildcard);

        if (regexp.exactMatch(path)) {
            qDebug() << "* Discarded by ignore pattern: " << path;
            return;
        }
        QFileInfo fInfo(path);
        if( regexp.exactMatch(fInfo.fileName())) {
            qDebug() << "* Discarded by ignore pattern:" << path;
            return;
        }
        if( fInfo.isHidden() ) {
            qDebug() << "* Discarded as is hidden!";
            return;
        }
    }

    if( !_parent->_pendingPathes.contains( path )) {
        _parent->_pendingPathes[path] = 0;
    }
    _parent->_pendingPathes[path] = _parent->_pendingPathes[path]+mask;
    _parent->setProcessTimer();
}

void FolderWatcherPrivate::removePath(const QString &path )
{
    if (_inotify->directories().contains(path) ) {
        qDebug() << "(-) Watcher:" << path;
        _inotify->removePath(path);
    }
}


} // namespace Mirall
