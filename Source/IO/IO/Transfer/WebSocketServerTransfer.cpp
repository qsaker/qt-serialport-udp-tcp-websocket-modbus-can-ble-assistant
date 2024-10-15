﻿/***************************************************************************************************
 * Copyright 2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "WebSocketServerTransfer.h"

#include "WebSocketServerTransferModel.h"

namespace xTools {

WebSocketServerTransfer::WebSocketServerTransfer(QObject *parent)
    : SocketTransfer(parent)
    , m_model(new WebSocketServerTransferModel(this))

{
    connect(m_model, &AbstractTransferModel::outputBytes, this, &AbstractTransfer::outputBytes);
}

WebSocketServerTransfer::~WebSocketServerTransfer() {}

QVariant WebSocketServerTransfer::tableModel() const
{
    return QVariant::fromValue(m_model);
}

} // namespace xTools