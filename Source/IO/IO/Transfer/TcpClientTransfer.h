﻿/***************************************************************************************************
 * Copyright 2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include "SocketTransfer.h"

namespace xTools {

class TcpClientTransferModel;
class TcpClientTransfer : public SocketTransfer
{
    Q_OBJECT
public:
    TcpClientTransfer(QObject *parent = nullptr);
    virtual ~TcpClientTransfer();

    QVariant tableModel() const override;

protected:
    TcpClientTransferModel *m_model;
};

} // namespace xTools
