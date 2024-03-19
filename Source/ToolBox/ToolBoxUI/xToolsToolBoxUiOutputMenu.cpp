/***************************************************************************************************
 * Copyright 2023 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "xToolsToolBoxUiOutputMenu.h"
#include "ui_xToolsToolBoxUiOutputMenu.h"

#include <QWidgetAction>

#include "sakhighlighter.h"
#include "saksettings.h"


xToolsToolBoxUiOutputMenu::xToolsToolBoxUiOutputMenu(const QString& settingsGroup,
                                               QTextDocument* doc,
                                               QWidget* parent)
    : QMenu(parent)
    , ui(new Ui::xToolsToolBoxUiOutputMenu)
{
    QWidget* w = new QWidget(this);
    ui->setupUi(w);
    QWidgetAction* action = new QWidgetAction(this);
    action->setDefaultWidget(w);
    addAction(action);

    const QString key = settingsGroup + "/highlighter/items";
    QString txt = SAKSettings::instance()->value(key).toString();
    txt = QString::fromUtf8(QByteArray::fromHex(txt.toLatin1()));
    ui->keyword->setText(txt);

    SAKHighlighter* highlighter = new SAKHighlighter(this);
    auto updateDoc = [=]() {
        QString text = ui->keyword->text();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QStringList list = text.split(";", Qt::SkipEmptyParts);
#else
        QStringList list = text.split(QString(";"), QString::SkipEmptyParts);
#endif
        highlighter->removeKeyWord("");
        highlighter->setKeyWords(list);
    };

    highlighter->setDoc(QVariant::fromValue(doc));
    connect(ui->keyword, &QLineEdit::textChanged, this, [=]() {
        QString text = ui->keyword->text();
        QString hex = QString::fromLatin1(text.toUtf8().toHex());
        SAKSettings::instance()->setValue(key, hex);

        updateDoc();
    });

    updateDoc();

    const QString fillterKey = settingsGroup + "/fillter";
    txt = SAKSettings::instance()->value(fillterKey).toString();
    ui->filter->setText(txt);
    connect(ui->filter, &QLineEdit::editingFinished, this, [=]() {
        SAKSettings::instance()->setValue(fillterKey, ui->filter->text().trimmed());
    });
}

xToolsToolBoxUiOutputMenu::~xToolsToolBoxUiOutputMenu()
{
    delete ui;
}

QString xToolsToolBoxUiOutputMenu::filter()
{
    return ui->filter->text().trimmed();
}
