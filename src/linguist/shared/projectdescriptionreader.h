/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef PROJECTDESCRIPTIONREADER_H
#define PROJECTDESCRIPTIONREADER_H

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

#include <memory>
#include <vector>

struct Project;

typedef std::vector<Project> Projects;

struct Project
{
    QString filePath;
    QString codec;
    QStringList excluded;
    QStringList includePaths;
    QStringList sources;
    Projects subProjects;
    // ### Change type of translations to std::optional<QStringList> once we can.
    std::unique_ptr<QStringList> translations;
};

inline bool hasTranslations(const Project &project)
{
    return (bool)project.translations;
}

Projects readProjectDescription(const QString &filePath, QString *errorString);

#endif // PROJECTDESCRIPTIONREADER_H
