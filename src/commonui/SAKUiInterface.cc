/******************************************************************************
 * Copyright 2023 Qsaker(wuuhaii@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#include <QMap>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "SAKUiInterface.hh"
#include "SAKDataStructure.hh"

SAKUiInterface::SAKUiInterface(QObject *parent)
    : QObject{parent}
{

}

void SAKUiInterface::setValidator(QLineEdit *le, int textFormat)
{
    if (!le) {
        return;
    }

    static QMap<int, QRegularExpression> regExpMap;
    if (regExpMap.isEmpty()) {
        regExpMap.insert(SAKDataStructure::TextFormatBin,
                         QRegularExpression("([01][01][01][01][01][01][01][01][ ])*"));
        regExpMap.insert(SAKDataStructure::TextFormatOct,
                         QRegularExpression("([0-7][0-7][ ])*"));
        regExpMap.insert(SAKDataStructure::TextFormatDec,
                         QRegularExpression("([0-9][0-9][ ])*"));
        regExpMap.insert(SAKDataStructure::TextFormatHex,
                         QRegularExpression("([0-9a-fA-F][0-9a-fA-F][ ])*"));
        regExpMap.insert(SAKDataStructure::TextFormatAscii,
                         QRegularExpression("([ -~])*"));
    }

    if (le->validator()){
        delete le->validator();
    }

    if (regExpMap.contains(textFormat)){
        QRegularExpression re = regExpMap.value(textFormat);
        auto validator = new QRegularExpressionValidator(re, le);
        le->setValidator(validator);
    } else {
        le->setValidator(nullptr);
    }

    le->clear();
}