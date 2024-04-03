﻿/***************************************************************************************************
 * Copyright 2018-2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "Application.h"
#include "MainWindow.h"
#include "xTools.h"

int main(const int argc, char *argv[])
{
#if 0
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#ifdef X_TOOLS_BUILD_WITH_CMAKE
    Q_INIT_RESOURCE(xToolsCommon);
#endif
    return xToolsExec<MainWindow, MainWindow, Application>(argc, argv, QString("xTools"));
}
