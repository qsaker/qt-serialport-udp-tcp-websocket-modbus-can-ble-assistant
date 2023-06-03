﻿/******************************************************************************
 * Copyright 2018-2023 Qsaker(wuuhaii@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMenu>
#include <QLabel>
#include <QAction>
#include <QSettings>
#include <QMetaEnum>
#include <QTabWidget>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QTranslator>
#include <QButtonGroup>
#include <QStackedWidget>

namespace Ui {
    class SAKMainWindow;
}

class SAKMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SAKMainWindow(QWidget *parent = Q_NULLPTR);
    ~SAKMainWindow();

#ifdef Q_OS_WIN
protected:
    void closeEvent(QCloseEvent *event);
#endif

private:
    struct SettingsKeyContext {
        const QString currentPageIndex{"currentPageIndex"};
        const QString exitToSystemTray{"exitToSystemTray"};
    } mSettingsKey;

    QActionGroup *mLanguagesActionGroup;
    QButtonGroup *mNavBtGroup{nullptr};

private:
    void initMenuBar();
    void initFileMenu();
    void initToolMenu();
    void initOptionMenu();
    void initOptionMenuAppStyleMenu(QMenu *optionMenu);
    void initOptionMenuMainWindowMenu(QMenu *optionMenu);
    void initOptionMenuSettingsMenu(QMenu *optionMenu);
    void initOptionMenuUiType(QMenu *optionMenu);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    void initOptionMenuHdpiPolicy(QMenu *optionMenu);
#endif
    void initWindowMenu();
    void initLanguageMenu();
    void initHelpMenu();
    void initLinksMenu();
    void initDemoMenu();
    void aboutQsak();
    void clearConfiguration();
    void rebootRequestion();
    void showReleaseHistoryActionDialog();
    void showQrCodeDialog();
    void onDonationActionTriggered();
    void activePage();
    void installLanguage();

private:
    Ui::SAKMainWindow *ui;
};

#endif // MAINWINDOW_H
