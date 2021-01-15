/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QHELPENGINE_H
#define QHELPENGINE_H

#include <QtHelp/qhelpenginecore.h>

QT_BEGIN_NAMESPACE


class QHelpContentModel;
class QHelpContentWidget;
class QHelpIndexModel;
class QHelpIndexWidget;
class QHelpEnginePrivate;
class QHelpSearchEngine;

class QHELP_EXPORT QHelpEngine : public QHelpEngineCore
{
    Q_OBJECT

public:
    explicit QHelpEngine(const QString &collectionFile, QObject *parent = nullptr);
    ~QHelpEngine();

    QHelpContentModel *contentModel() const;
    QHelpIndexModel *indexModel() const;

    QHelpContentWidget *contentWidget();
    QHelpIndexWidget *indexWidget();

    QHelpSearchEngine *searchEngine();

private:
    QHelpEnginePrivate *d;

    friend class HelpEngineWrapper;
};

QT_END_NAMESPACE

#endif
