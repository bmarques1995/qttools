// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "qdoc/boundaries/refined_typedef.h"

#include <optional>

#include <QtCore/qstring.h>
#include <QtCore/qfileinfo.h>

QDOC_REFINED_TYPEDEF(QString, FilePath) {
    QFileInfo info{value};

    return (info.isFile() && info.isReadable()) ? std::optional(FilePath{info.canonicalFilePath()}) : std::nullopt;
}
