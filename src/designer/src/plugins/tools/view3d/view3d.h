/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef VIEW3D_H
#define VIEW3D_H

#include "view3d_global.h"
#include <QtWidgets/qwidget.h>

QT_BEGIN_NAMESPACE

class QScrollBar;
class QGLWidget;
class QDesignerFormWindowInterface;

class QView3DWidget;

class QView3D : public QWidget
{
    Q_OBJECT

public:
    QView3D(QDesignerFormWindowInterface *form_window, QWidget *parent);

public slots:
    void updateForm();

private:
    QView3DWidget *m_3d_widget;
    QDesignerFormWindowInterface *m_form_window;

    void addWidget(int depth, QWidget *w);
    void addTexture(QWidget *w);
};

#endif // VIEW3D_H


QT_END_NAMESPACE
