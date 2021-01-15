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

#include "translatedialog.h"

QT_BEGIN_NAMESPACE

TranslateDialog::TranslateDialog(QWidget *parent)
  : QDialog(parent)
{
    m_ui.setupUi(this);
    connect(m_ui.findNxt, SIGNAL(clicked()), this, SLOT(emitFindNext()));
    connect(m_ui.translate, SIGNAL(clicked()), this, SLOT(emitTranslateAndFindNext()));
    connect(m_ui.translateAll, SIGNAL(clicked()), this, SLOT(emitTranslateAll()));
    connect(m_ui.ledFindWhat, SIGNAL(textChanged(QString)), SLOT(verifyText()));
    connect(m_ui.ckMatchCase, SIGNAL(toggled(bool)), SLOT(verifyText()));
}

void TranslateDialog::showEvent(QShowEvent *)
{
    verifyText();
    m_ui.ledFindWhat->setFocus();
}

void TranslateDialog::verifyText()
{
    QString text = m_ui.ledFindWhat->text();
    bool canFind = !text.isEmpty();
    bool hit = false;
    if (canFind)
        emit requestMatchUpdate(hit);
    m_ui.findNxt->setEnabled(canFind);
    m_ui.translate->setEnabled(canFind && hit);
    m_ui.translateAll->setEnabled(canFind);
}

void TranslateDialog::emitFindNext()
{
    emit activated(Skip);
}

void TranslateDialog::emitTranslateAndFindNext()
{
    emit activated(Translate);
}

void TranslateDialog::emitTranslateAll()
{
    emit activated(TranslateAll);
}

QT_END_NAMESPACE
