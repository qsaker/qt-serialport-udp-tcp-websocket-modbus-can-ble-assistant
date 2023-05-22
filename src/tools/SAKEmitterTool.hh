/******************************************************************************
 * Copyright 2023 Qsaker(wuuhaii@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#ifndef SAKEMITTERTOOL_HH
#define SAKEMITTERTOOL_HH

#include <QTimer>
#include <QMutex>
#include <QVariant>
#include <QAbstractTableModel>

#include "SAKBaseTool.hh"

class SAKEmitterTableModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class SAKEmitterTool;
public:
    SAKEmitterTableModel(QObject *parent = nullptr);

    virtual int rowCount(
        const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(
        const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(
        const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index,
                         const QVariant &value,
                         int role = Qt::EditRole) override;
    virtual bool insertRows(
        int row, int count,
        const QModelIndex &parent = QModelIndex()) override;
    virtual bool removeRows(
        int row, int count,
        const QModelIndex &parent = QModelIndex()) override;
    virtual QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

public:
    struct EDEmiterData {
        bool itemEnable;
        QString itemDescription{"Demo"};
        int itemTextFormat;
        int itemEscapeCharacter;
        int itemPrefix;
        QString itemText;
        int itemSuffix;
        int itemInterval;

        bool itemCrcEnable;
        int itemCrcAlgorithm;
        int itemCrcStartIndex;
        int itemCrcEndIndex;
    };

    struct EDEmiterItem {
        EDEmiterData data;
        int elapsedTime{0};
    };

private:
    QList<EDEmiterItem> mItems;
    QMutex mItemsMutex;
    const int mDescriptionColumnIndex{0};
    const int mFormatColumnIndex{1};
    const int mItemTextColumnIndex{2};
    QStringList mHeaders;

private:
    struct EDEmitterDataKeys {
        const QString itemEnable{"Enable"};
        const QString itemDescription{"Description"};
        const QString itemTextFormat{"Format"};
        const QString itemEscapeCharacter{"Escape"};
        const QString itemPrefix{"Prefix"};
        const QString itemSuffix{"Suffix"};
        const QString itemInterval{"Interval"};
        const QString itemCrcEnable{"CrcEnable"};
        const QString itemCrcAlgorithm{"Algorithm"};
        const QString itemCrcStartIndex{"Start"};
        const QString itemCrcEndIndex{"End"};
        const QString itemText{"Data"};
    } mDataKeys;
    const int mTableColumnCount{12};

private:
    QVariant columnDisplayRoleData(
        const EDEmiterItem &item, int column) const;

private:
    QByteArray itemBytes(const EDEmiterData &item);
};

class SAKEmitterTool : public SAKBaseTool
{
    Q_OBJECT
    Q_PROPERTY(QVariant tableModel READ tableModel CONSTANT)
    Q_PROPERTY(QStringList headers READ headers CONSTANT)

    Q_PROPERTY(QString itemEnable READ itemEnable CONSTANT)
    Q_PROPERTY(QString itemDescription READ itemDescription CONSTANT)
    Q_PROPERTY(QString itemTextFormat READ itemTextFormat CONSTANT)
    Q_PROPERTY(QString itemEscapeCharacter READ itemEscapeCharacter CONSTANT)
    Q_PROPERTY(QString itemInterval READ itemInterval CONSTANT)
    Q_PROPERTY(QString itemPrefix READ itemPrefix CONSTANT)
    Q_PROPERTY(QString itemSuffix READ itemSuffix CONSTANT)
    Q_PROPERTY(QString itemCrcEnable READ itemCrcEnable CONSTANT)
    Q_PROPERTY(QString itemCrcAlgorithm READ itemCrcAlgorithm CONSTANT)
    Q_PROPERTY(QString itemCrcStartIndex READ itemCrcStartIndex CONSTANT)
    Q_PROPERTY(QString itemCrcEndIndex READ itemCrcEndIndex CONSTANT)
    Q_PROPERTY(QString itemText READ itemText CONSTANT)

public:
    explicit SAKEmitterTool(QObject *parent = Q_NULLPTR);

    Q_INVOKABLE void addItem(const QString &jsonCtx, int index = -1);
    Q_INVOKABLE QVariant itemContext(int index);
    Q_INVOKABLE QVariant itemsContext();
    SAKEmitterTableModel *getModel();
    QStringList getHeaders();

protected:
    virtual void run() final;

private:
    QTimer *mEmittingTimer;
    const int mScanInterval{5};

private:
    void try2emit();

private:
    SAKEmitterTableModel *mTableModel;
    QVariant tableModel(){
        return QVariant::fromValue<SAKEmitterTableModel*>(mTableModel);
    }

    QStringList mHeaders;
    QStringList headers(){return mHeaders;}

    QString itemEnable(){return mTableModel->mDataKeys.itemEnable;}
    QString itemDescription(){return mTableModel->mDataKeys.itemDescription;}
    QString itemTextFormat(){return mTableModel->mDataKeys.itemTextFormat;}
    QString itemEscapeCharacter(){
        return mTableModel->mDataKeys.itemEscapeCharacter;
    }
    QString itemInterval(){return mTableModel->mDataKeys.itemInterval;}
    QString itemPrefix(){return mTableModel->mDataKeys.itemPrefix;}
    QString itemSuffix(){return mTableModel->mDataKeys.itemSuffix;}
    QString itemCrcEnable(){return mTableModel->mDataKeys.itemCrcEnable;}
    QString itemCrcAlgorithm(){return mTableModel->mDataKeys.itemCrcAlgorithm;}
    QString itemCrcStartIndex(){
        return mTableModel->mDataKeys.itemCrcStartIndex;
    }
    QString itemCrcEndIndex(){return mTableModel->mDataKeys.itemCrcEndIndex;}
    QString itemText(){return mTableModel->mDataKeys.itemText;}
};

#endif // SAKEMITTERTOOL_H
