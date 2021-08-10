﻿/*
 * Copyright 2021 Qter(qsaker@qq.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 */
#ifndef SAKDEBUGGERPLUGINS_HH
#define SAKDEBUGGERPLUGINS_HH

#include <QLabel>
#include <QWidget>
#include <QObject>
#include <QDialog>
#include <QSettings>
#include <QPushButton>
#include <QSqlDatabase>

#include "SAKPlugin3d.hh"
#include "SAKPluginCharts.hh"
#include "SAKPluginDataForwarding.hh"
#include "SAKDebuggerPluginRegularlySending.hh"
#include "SAKDebuggerPluginAutoResponse.hh"

class SAKDebuggerPlugins : public QObject
{
    Q_OBJECT
public:
    explicit SAKDebuggerPlugins(QPushButton *readmeBt,
                                QPushButton *menuBt,
                                QSettings *settings,
                                const QString &settingsGroup,
                                QLabel *titleLabel,
                                QSqlDatabase *sqlDatabase,
                                QWidget *panelWidget,
                                QObject *parent = Q_NULLPTR);
    ~SAKDebuggerPlugins();


private:
    SAKPlugin3d *m3d;
    SAKPluginCharts *mCharts;
    SAKPluginDataForwarding *mDataForwarding;
    SAKDebuggerPluginRegularlySending *mRegularlySending;
    SAKDebuggerPluginAutoResponse *mAutoResponse;


private:
    QLabel *mTitleLabel;
    QWidget *mPanelWidget;
    QDialog *mPluginDialog;
    QWidget *mActiveWidgetInPanel;
    QWidget *mActiveWidgetInDialog;


private:
    void showPluin3D();
    void showPluinCharts();
    void showPluginAutoResponse();
    void showPluinDataForwarding();
    void showPluginRegularlySending();
    void showPluginDialog(QWidget *contentWidget);

    void embedPluin3D();
    void embedPluinCharts();
    void embedPluginAutoResponse();
    void embedPluinDataForwarding();
    void embedPluginRegularlySending();
    void embedPlugin(QWidget *contentWidget);
    void cancelEmbedPlugin();

    void clearPluginDialog();
    void clearPluginPanel();


signals:
    void bytesRead(QByteArray bytes);
    void bytesWritten(QByteArray bytes);
};

#endif // SAKDEBUGGERPLUGINS_HH
