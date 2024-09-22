﻿/***************************************************************************************************
 * Copyright 2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of eTools project.
 *
 * eTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QMutex>

#include "Socket.h"

namespace xTools {

class SocketServer : public Socket
{
    Q_OBJECT
public:
    explicit SocketServer(QObject *parent = nullptr);
    ~SocketServer() override;

    QVariantMap save() const override;
    void load(const QVariantMap &data) override;

    virtual void disconnectAllClients() {};

    Q_INVOKABLE QStringList clients() const;
    Q_INVOKABLE QString currentClientFlag() const;
    Q_INVOKABLE void setCurrentClientFlag(const QString &flag);

signals:
    void clientsChanged(const SocketPrivateSignal &);

protected:
    void addClient(const QString &flag);
    void removeClient(const QString &flag);
    void clearClients();

private:
    QStringList m_clients;
    mutable QMutex m_clientsMutex;
    QString m_currentClientFlag;
    mutable QMutex m_currentClientMutex;
};

} // namespace xTools
