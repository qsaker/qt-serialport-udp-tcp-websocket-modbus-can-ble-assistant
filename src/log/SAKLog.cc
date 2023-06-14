/******************************************************************************
 * Copyright 2023 Qsaker(wuuhaii@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#include <QDir>
#include <QUrl>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QFileInfoList>
#include <QStandardPaths>

#include "SAKLog.hh"
#include "SAKSettings.hh"

QVector<SAKLog::LogContext> SAKLog::mLogContextVector;
QMutex SAKLog::mLogContextVectorMutex;
SAKLog::SAKLog(QObject *parent)
    : QThread(parent)
    , mLogLevel(QtDebugMsg)
    , mLogLifeCycle(30)
    , mIsPaused(false)
    , mMaxTemp(10240)
{
    QDir dir(logPath());
    if (!dir.exists()) {
        if (dir.mkdir(logPath())) {
            qCInfo(mLoggingCategory) << "Create log dir successfully.";
        } else {
            qCInfo(mLoggingCategory) << "Create log dir failed!";
        }
    } else {
        qCInfo(mLoggingCategory) << "The log dir is exists,"
                                    " need not to create.";
    }

    mLogLifeCycle = logLifeCycle();

    mTableModel = new SAKTableModel(this);
    connect(mTableModel, &SAKTableModel::invokeGetRowCount,
            this, &SAKLog::onInvokeGetRowCount,
            Qt::DirectConnection);
    connect(mTableModel, &SAKTableModel::invokeGetColumnCount,
            this, &SAKLog::onInvokeGetColumnCount,
            Qt::DirectConnection);
    connect(mTableModel, &SAKTableModel::invokeGetData,
            this, &SAKLog::onInvokeGetData,
            Qt::DirectConnection);
    connect(mTableModel, &SAKTableModel::invokeSetData,
            this, &SAKLog::onInvokeSetData,
            Qt::DirectConnection);
    connect(mTableModel, &SAKTableModel::invokeInsertRows,
            this, &SAKLog::onInvokeInsertRows,
            Qt::DirectConnection);
    connect(mTableModel, &SAKTableModel::invokeRemoveRows,
            this, &SAKLog::onInvokeRemoveRows,
            Qt::DirectConnection);
    connect(mTableModel, &SAKTableModel::invokeGetHeaderData,
            this, &SAKLog::onInvokeGetHeaderData,
            Qt::DirectConnection);
}

SAKLog::~SAKLog()
{
    qCDebug(mLoggingCategory) << __FUNCTION__;

    exit();
    wait();
}

void SAKLog::messageOutput(QtMsgType type, const QMessageLogContext &context,
                           const QString &msg)
{
    mLogContextVectorMutex.lock();

    LogContext ctx{type, context.category, msg};
    mLogContextVector.append(ctx);

    mLogContextVectorMutex.unlock();
}

void SAKLog::clear()
{
    int count = mTableModel->rowCount();
    mTableModel->removeRows(0, count);
}

QString SAKLog::logPath()
{
    QString fileName = SAKSettings::instance()->fileName();
    QUrl url(fileName);
    QString path = fileName;
    QString logPath = path.remove(url.fileName());
    logPath += "log";
    return logPath;
}

SAKLog *SAKLog::instance()
{
    static SAKLog log;
    return &log;
}

qint64 SAKLog::logLifeCycle()
{
    return mLogLifeCycle;
}

void SAKLog::setLogLifeCycle(qint64 t)
{
    mLogLifeCycle = t;
    if (mLogLifeCycle < 1) {
        mLogLifeCycle = 1;
        qCWarning(mLoggingCategory) << "The life cycle is too short,"
                                       " it has been set to 1 day.";
    }

    if (mLogLifeCycle > 30) {
        mLogLifeCycle = 30;
        qCWarning(mLoggingCategory) << "The life cycle is too long,"
                                       " it has been set to 30 days.";
    }

    emit logLifeCycleChanged();
}

int SAKLog::logLevel()
{
    return mLogLevel;
}

void SAKLog::setLogLevel(int level)
{
    mLogLevel = level;
    emit logLevelChanged();
}

bool SAKLog::isPaused()
{
    return mIsPaused;
}

void SAKLog::setIsPaused(bool paused)
{
    mIsPaused = paused;
    emit isPausedChanged();
}

QAbstractTableModel *SAKLog::tableModel()
{
    return mTableModel;
}

void SAKLog::run()
{
    QTimer *writeTimer = new QTimer();
    writeTimer->setInterval(1000);
    writeTimer->setSingleShot(true);
    connect(writeTimer, &QTimer::timeout, writeTimer, [=](){
#if 0
        QDateTime dt = QDateTime::currentDateTime();
        qDebug() << dt.toString("yyyy-MM-dd hh:mm:ss");
#endif
        writeLog();
        writeTimer->start();
    });

    QTimer *clearTimer = new QTimer();
    clearTimer->setInterval(10*1000);
    clearTimer->setSingleShot(true);
    connect(clearTimer, &QTimer::timeout, clearTimer, [=](){
        clearLog();
        clearTimer->start();
    });

    writeTimer->start();
    clearTimer->start();
    exec();
    qCDebug(mLoggingCategory) << __FUNCTION__;

    writeTimer->stop();
    writeTimer->deleteLater();
    writeTimer = nullptr;

    clearTimer->stop();
    clearTimer->deleteLater();
    clearTimer = nullptr;
}

void SAKLog::onInvokeGetRowCount(int &count)
{
    mTempMutex.lock();
    count = mTemp.count();
    mTempMutex.unlock();
}

void SAKLog::onInvokeGetColumnCount(int &count)
{
    count = 3;
}

void SAKLog::onInvokeGetData(QVariant &data,
                             const QModelIndex &index,
                             int role)
{
    if (role != Qt::DisplayRole) {
        return;
    }

    int row = index.row();
    mTempMutex.lock();
    if (row >= 0 && row < mTemp.count()) {
        int column = index.column();
        if (column >= 0 && column < 3) {
            auto ctx = mTemp.at(row);
            if (column == 0) {
                data = logTypeFlag(ctx.type);
            } else if (column == 1) {
                data = ctx.category;
            } else if (column == 2) {
                data = ctx.msg;
            } else {
                qWarning(mLoggingCategory) << "Index is invalid(column).!";
            }
        } else {
            qWarning(mLoggingCategory) << "Index is invalid(column)!";
        }
    } else {
        qWarning(mLoggingCategory) << "Index is invalid(row)!";
    }
    mTempMutex.unlock();
}

void SAKLog::onInvokeSetData(bool &result,
                             const QModelIndex &index,
                             const QVariant &value,
                             int role)
{
    Q_UNUSED(role)
    int column = index.column();
    int row = index.row();
    result = true;
    mTempMutex.lock();
    if (row >= 0 && row <= mTemp.count()) {
        auto ctx = mTemp.at(row);
        if (column >= 0 && column < 3) {
            if (column == 0) {
                ctx.type = QtMsgType(value.toInt());
            } else if (column == 1) {
                ctx.category = value.toString();
            } else if (column == 2) {
                ctx.msg = value.toString();
            } else {
                result = false;
            }
            mTemp.replace(row, ctx);
        } else {
            result = false;
        }
    } else {
        result = false;
    }
    mTempMutex.unlock();
}

void SAKLog::onInvokeInsertRows(bool &result,
                                int row,
                                int count,
                                const QModelIndex &parent)
{
    Q_UNUSED(parent)
    mTempMutex.lock();
    if (row >=0 && row <= mTemp.count()) {
        for (int i = 0; i < count; i++) {
            mTemp.insert(row, LogContext{});
        }
    } else {
        for (int i = 0; i < count; i++) {
            mTemp.append(LogContext{});
        }
    }

    while (mTemp.count() > mMaxTemp) {
        mTemp.removeFirst();
    }

    mTempMutex.unlock();
    result = true;
}

void SAKLog::onInvokeRemoveRows(bool &result,
                                int row,
                                int count,
                                const QModelIndex &parent)
{
    Q_UNUSED(parent)
    mTempMutex.lock();
    mTemp.remove(row, count);
    mTempMutex.unlock();
    result = true;
}

void SAKLog::onInvokeGetHeaderData(QVariant &data,
                                   int section,
                                   Qt::Orientation orientation,
                                   int role)
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        data = QVariant();
        return;
    }

    if (section == 0) {
        data = tr("Log Type");
    } else if (section == 1) {
        data = tr("Log Category");
    } else if (section == 2) {
        data = tr("Log Detail");
    }
}

void SAKLog::writeLog()
{
    mLogContextVectorMutex.lock();
    QVector<LogContext> logCtxVector = this->mLogContextVector;
    this->mLogContextVector.clear();
    mLogContextVectorMutex.unlock();

    if (logCtxVector.isEmpty()) {
        return;
    }

    const QDateTime dt = QDateTime::currentDateTime();
    const QString fileName = logPath() + "/sak_log_" + dt.toString("yyyy_MM_dd");
    const QString suffix = ".log";
    QFile file(fileName + suffix);
    if (QFile::exists(file.fileName()) && (file.size() >= 1024*1024)) {
        QString newName = fileName + dt.toString("_hh_mm_ss") + suffix;
        if (QFile::rename(fileName, newName)) {
            qCInfo(mLoggingCategory) << file.fileName()
                                     << "has been rename to:" << newName;
        } else {
            qCInfo(mLoggingCategory) << "rename file failed!";
        }
    }

    if (file.open(QFile::WriteOnly|QFile::Text|QFile::Append)) {
        QTextStream out(&file);
        for (auto &logCtx : logCtxVector) {
            QString flag = logTypeFlag(logCtx.type);
            out << flag << " " << logCtx.category << " " << logCtx.msg << "\n";

            if (mLogLevel == -1) {
                continue;
            }

            if (mLogLevel == QtInfoMsg) {
                if (logCtx.type == QtDebugMsg) {
                    continue;
                }
            }

            if (mLogLevel == QtWarningMsg) {
                if ((logCtx.type == QtDebugMsg) || (logCtx.type == QtInfoMsg)) {
                    continue;
                }
            }

            if (!mIsPaused) {
                int count = mTableModel->rowCount();
                this->mTableModel->insertRows(count, 1);
                QModelIndex index = mTableModel->index(count, 0);
                this->mTableModel->setData(index, flag);
                index = mTableModel->index(count, 1);
                this->mTableModel->setData(index, logCtx.category);
                index = mTableModel->index(count, 2);
                this->mTableModel->setData(index, logCtx.msg);
            } else {
                mTempMutex.lock();
                mTemp.append(logCtx);
                while (mTemp.count() > mMaxTemp) {
                    mTemp.removeFirst();
                }
                mTempMutex.unlock();
            }
        }

        file.close();
    } else {
        QStringList list;
        list << file.errorString() << " > " << "lor_error.log";
        QProcess::startDetached("echo", list);
    }
}

void SAKLog::clearLog()
{
    QDir dir(logPath());
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    for (auto &info : infoList) {
        qint64 dateTime = info.birthTime().toMSecsSinceEpoch();
        qint64 currentDateTime = QDateTime::currentMSecsSinceEpoch();
        quint64 interval = qAbs(mLogLifeCycle*24*60*60*1000);
        if (quint64(qAbs(dateTime - currentDateTime)) > interval) {
            QString fileName = info.absoluteFilePath();
            if (QFile::remove(fileName)) {
                QString log = QString("log(%1) removed").arg(fileName);
                qCInfo(mLoggingCategory) << qPrintable(log);
            } else {
                QString log = QString("remove log(%1) failed").arg(fileName);
                qCInfo(mLoggingCategory) << qPrintable(log);
            }
        }
    }
}

QString SAKLog::logTypeFlag(int type)
{
    QString flag = "[U]";
    if (type == QtDebugMsg) {
        flag = "[D]";
    } else if (type == QtInfoMsg) {
        flag = "[I]";
    } else if (type == QtWarningMsg) {
        flag = "[W]";
    } else if (type == QtCriticalMsg) {
        flag = "[C]";
    } else if (type == QtFatalMsg) {
        flag = "[F]";
    }

    return flag;
}
