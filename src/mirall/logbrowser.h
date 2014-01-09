/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
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

#ifndef LOGBROWSER_H
#define LOGBROWSER_H

#include <QPlainTextEdit>
#include <QTextStream>
#include <QFile>
#include <QObject>
#include <QList>
#include <QDateTime>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

namespace Mirall {

class LogWidget : public QPlainTextEdit 
{
    Q_OBJECT
public:
    explicit LogWidget(QWidget *parent = 0);

signals:

};

class LogBrowser : public QDialog
{
  Q_OBJECT
public:
    explicit LogBrowser(QWidget *parent = 0);
    ~LogBrowser();

    void setLogFile(const QString& , bool );

protected:
    void closeEvent(QCloseEvent *);

protected slots:
    void slotNewLog( const QString &msg );
    void slotFind();
    void search( const QString& );
    void slotSave();
    void slotClearLog();

private:
    LogWidget *_logWidget;
    QLineEdit *_findTermEdit;
    QPushButton *_saveBtn;
    QPushButton *_clearBtn;
    QLabel      *_statusLabel;

};

} // namespace

#endif // LOGBROWSER_H
