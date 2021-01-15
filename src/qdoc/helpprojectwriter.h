/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef HELPPROJECTWRITER_H
#define HELPPROJECTWRITER_H

#include "node.h"

#include <QtCore/qstring.h>
#include <QtCore/qxmlstream.h>

QT_BEGIN_NAMESPACE

class QDocDatabase;
class Generator;
typedef QPair<QString, const Node *> QStringNodePair;

using NodeTypeSet = QSet<unsigned char>;

struct SubProject
{
    QString title;
    QString indexTitle;
    NodeTypeSet selectors;
    bool sortPages;
    QString type;
    QHash<QString, const Node *> nodes;
    QStringList groups;
};

struct HelpProject
{
    using NodeStatusSet = QSet<unsigned char>;

    QString name;
    QString helpNamespace;
    QString virtualFolder;
    QString version;
    QString fileName;
    QString indexRoot;
    QString indexTitle;
    QVector<QStringList> keywords;
    QSet<QString> files;
    QSet<QString> extraFiles;
    QSet<QString> filterAttributes;
    QHash<QString, QSet<QString>> customFilters;
    QSet<QString> excluded;
    QVector<SubProject> subprojects;
    QHash<const Node *, NodeStatusSet> memberStatus;
    bool includeIndexNodes;
};

class HelpProjectWriter
{
    Q_DECLARE_TR_FUNCTIONS(QDoc::HelpProjectWriter)

public:
    HelpProjectWriter(const QString &defaultFileName, Generator *g);
    void reset(const QString &defaultFileName, Generator *g);
    void addExtraFile(const QString &file);
    void addExtraFiles(const QSet<QString> &files);
    void generate();

private:
    void generateProject(HelpProject &project);
    void generateSections(HelpProject &project, QXmlStreamWriter &writer, const Node *node);
    bool generateSection(HelpProject &project, QXmlStreamWriter &writer, const Node *node);
    QStringList keywordDetails(const Node *node) const;
    void writeHashFile(QFile &file);
    void writeNode(HelpProject &project, QXmlStreamWriter &writer, const Node *node);
    void readSelectors(SubProject &subproject, const QStringList &selectors);
    void addMembers(HelpProject &project, QXmlStreamWriter &writer, const Node *node);
    void writeSection(QXmlStreamWriter &writer, const QString &path, const QString &value);

    QDocDatabase *qdb_;
    Generator *gen_;

    QString outputDir;
    QVector<HelpProject> projects;
};

QT_END_NAMESPACE

#endif
