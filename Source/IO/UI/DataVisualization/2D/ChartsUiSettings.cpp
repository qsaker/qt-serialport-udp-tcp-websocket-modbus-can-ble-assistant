﻿/***************************************************************************************************
 * Copyright 2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of eTools project.
 *
 * eTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "ChartsUiSettings.h"
#include "ui_ChartsUiSettings.h"

#include <QAbstractSeries>
#include <QChartView>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPointF>
#include <QTimer>

#include "ChartsUi.h"
#include "IO/IO/DataVisualization/2D/Charts.h"

namespace xTools {

ChartsUiSettings::ChartsUiSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChartsUiSettings)
{
    ui->setupUi(this);

    QComboBox *cb = ui->comboBoxDataType;
    cb->addItem(tr("Binary") + "-Y", static_cast<int>(Charts::DataFormat::BinaryY));
    cb->addItem(tr("Binary") + "-XY", static_cast<int>(Charts::DataFormat::BinaryXY));
    cb->addItem(tr("Text") + "-Y", static_cast<int>(Charts::DataFormat::TextY));
    cb->addItem(tr("Text") + "-XY", static_cast<int>(Charts::DataFormat::TextXY));
    connect(ui->comboBoxDataType, &QComboBox::currentIndexChanged, this, [=]() {
        emit this->invokeSetDataType(ui->comboBoxDataType->currentData().toInt());
    });

    connect(ui->pushButtonClear,
            &QPushButton::clicked,
            this,
            &ChartsUiSettings::invokeClearChannels);
    connect(ui->pushButtonImport,
            &QPushButton::clicked,
            this,
            &ChartsUiSettings::invokeImportChannels);
    connect(ui->pushButtonExport,
            &QPushButton::clicked,
            this,
            &ChartsUiSettings::invokeExportChannels);

    QGridLayout *parametersGridLayout = new QGridLayout(ui->widgetControl);
    parametersGridLayout->addWidget(new QLabel(tr("Channel"), this), 0, 0, Qt::AlignCenter);
    parametersGridLayout->addWidget(new QLabel(tr("Visible"), this), 0, 1, Qt::AlignCenter);
    parametersGridLayout->addWidget(new QLabel(tr("Type"), this), 0, 2, Qt::AlignCenter);
    parametersGridLayout->addWidget(new QLabel(tr("Color"), this), 0, 3, Qt::AlignCenter);
    parametersGridLayout->addWidget(new QLabel(tr("Name"), this), 0, 4, Qt::AlignCenter);

    const int channelNumber = channelCount();
    m_channelContexts.resize(channelNumber);
    for (int i = 0; i < channelNumber; ++i) {
        int row = i + 1;
        QString str = QString::number(row);

        parametersGridLayout->addWidget(new QLabel(str, this), row, 0, Qt::AlignCenter);
        auto *visibleCheckedBox = new QCheckBox(this);
        parametersGridLayout->addWidget(visibleCheckedBox, row, 1, Qt::AlignCenter);
        setupVisibleCheckBox(visibleCheckedBox, i);

        auto *typeComboBox = new QComboBox(this);
        parametersGridLayout->addWidget(typeComboBox, row, 2, Qt::AlignCenter);
        setupTypeComboBox(typeComboBox, i);

        auto *colorButton = new QPushButton(this);
        parametersGridLayout->addWidget(colorButton, row, 3, Qt::AlignCenter);
        setupColorButton(colorButton, i);

        auto *nameLineEdit = new QLineEdit(this);
        parametersGridLayout->addWidget(nameLineEdit, row, 4, Qt::AlignCenter);
        setupNameLineEdit(nameLineEdit, i);
    }
}

ChartsUiSettings::~ChartsUiSettings()
{
    delete ui;
}

int ChartsUiSettings::channelCount()
{
    return 16;
}

int ChartsUiSettings::dataType()
{
    return ui->comboBoxDataType->currentData().toInt();
}

void ChartsUiSettings::setDataType(int type)
{
    int index = ui->comboBoxDataType->findData(type);
    if (index != -1) {
        ui->comboBoxDataType->setCurrentIndex(index);
    }
}

bool ChartsUiSettings::legendVisible()
{
    return ui->checkBoxLegend->isChecked();
}

void ChartsUiSettings::setLegendVisible(bool visible)
{
    ui->checkBoxLegend->setChecked(true);
}

void ChartsUiSettings::load(const QVariantMap &parameters)
{
    if (parameters.isEmpty()) {
        return;
    }

    ChartsUiDataKeys keys;
    setDataType(parameters.value(keys.dataType).toInt());
    QJsonArray channels = parameters.value(keys.channels).toJsonArray();
    if (channels.count() != m_channelContexts.size()) {
        qWarning() << "The number of channels is not equal to the number of series.";
        return;
    }

    bool legendVisible = parameters.value(keys.legendVisible).toBool();
    ui->checkBoxLegend->setChecked(legendVisible);

    for (int i = 0; i < channels.size(); ++i) {
        QJsonObject obj = channels[i].toObject();
        QString name = obj.value(keys.channelName).toString();
        bool visible = obj.value(keys.channelVisible).toBool();
        QString color = obj.value(keys.channelColor).toString();
        int type = obj.value(keys.channelType).toInt();

        m_channelContexts[i].nameLineEdit->setText(name);
        m_channelContexts[i].visibleCheckBox->setChecked(visible);
        m_channelContexts[i].colorButton->setStyleSheet("background-color: " + color + ";");
        int index = m_channelContexts[i].typeComboBox->findData(type);
        m_channelContexts[i].typeComboBox->setCurrentIndex(index);
    }
}

void ChartsUiSettings::setupVisibleCheckBox(QCheckBox *checkBox, int channelIndex)
{
    checkBox->setCheckable(true);
    checkBox->setChecked(true);
    m_channelContexts[channelIndex].visibleCheckBox = checkBox;
    connect(checkBox, &QCheckBox::clicked, this, [=](bool checked) {
        emit invokeSetChannelVisible(channelIndex, checked);
    });
}

void ChartsUiSettings::setupTypeComboBox(QComboBox *comboBox, int channelIndex)
{
    m_channelContexts[channelIndex].typeComboBox = comboBox;
    comboBox->clear();
    comboBox->addItem(QIcon(":/Resources/Icons/IconLineSeries.svg"),
                      seriesTypeToString(QAbstractSeries::SeriesType::SeriesTypeLine),
                      QAbstractSeries::SeriesType::SeriesTypeLine);
    comboBox->addItem(QIcon(":/Resources/Icons/IconSplineSeries.svg"),
                      seriesTypeToString(QAbstractSeries::SeriesType::SeriesTypeSpline),
                      QAbstractSeries::SeriesType::SeriesTypeSpline);
    comboBox->addItem(QIcon(":/Resources/Icons/IconScatterSeries.svg"),
                      seriesTypeToString(QAbstractSeries::SeriesType::SeriesTypeScatter),
                      QAbstractSeries::SeriesType::SeriesTypeScatter);
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        emit invokeSetChannelType(channelIndex, comboBox->itemData(index).toInt());
    });
}

void ChartsUiSettings::setupColorButton(QPushButton *button, int channelIndex)
{
    m_channelContexts[channelIndex].colorButton = button;
    button->setFlat(true);
    button->setStyleSheet("background-color: rgb(0, 0, 255);");

    connect(button, &QPushButton::clicked, this, [=]() {
        QColor color = QColorDialog::getColor(Qt::blue, this);
        if (color.isValid()) {
            button->setStyleSheet("background-color: " + color.name() + ";");
            emit invokeSetChannelColor(channelIndex, color);
        }
    });
}

void ChartsUiSettings::setupNameLineEdit(QLineEdit *lineEdit, int channelIndex)
{
    m_channelContexts[channelIndex].nameLineEdit = lineEdit;
    lineEdit->setText(tr("Channel") + " " + QString::number(channelIndex + 1));

    connect(lineEdit, &QLineEdit::textEdited, this, [=]() {
        emit invokeSetChannelName(channelIndex, lineEdit->text());
    });
}

QString ChartsUiSettings::seriesTypeToString(int type) const
{
    switch (type) {
    case QAbstractSeries::SeriesType::SeriesTypeLine:
        return tr("Line");
    case QAbstractSeries::SeriesType::SeriesTypeSpline:
        return tr("Spline");
    case QAbstractSeries::SeriesType::SeriesTypeScatter:
        return tr("Scatter");
    default:
        return QString();
    }
}

} // namespace xTools
