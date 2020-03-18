/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qvector.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qdialogbuttonbox.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpainterpath.h>
#include <QtWidgets/qstyleoption.h>
#include "versiondialog.h"

QT_BEGIN_NAMESPACE

class VersionLabel : public QLabel
{
    Q_OBJECT
public:
    VersionLabel(QWidget *parent = nullptr);

signals:
    void triggered();

protected:
    void mousePressEvent(QMouseEvent *me) override;
    void mouseMoveEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;
    void paintEvent(QPaintEvent *pe) override;
private:
    QVector<QPoint> hitPoints;
    QVector<QPoint> missPoints;
    QPainterPath m_path;
    bool secondStage = false;
    bool m_pushed = false;
};

VersionLabel::VersionLabel(QWidget *parent)
        : QLabel(parent)
{
    setPixmap(QPixmap(QStringLiteral(":/qt-project.org/designer/images/designer.png")));
    hitPoints.append(QPoint(56, 25));
    hitPoints.append(QPoint(29, 55));
    hitPoints.append(QPoint(56, 87));
    hitPoints.append(QPoint(82, 55));
    hitPoints.append(QPoint(58, 56));

    secondStage = false;
    m_pushed = false;
}

void VersionLabel::mousePressEvent(QMouseEvent *me)
{
    if (me->button() == Qt::LeftButton) {
        if (!secondStage) {
            m_path = QPainterPath(me->pos());
        } else {
            m_pushed = true;
            update();
        }
    }
}

void VersionLabel::mouseMoveEvent(QMouseEvent *me)
{
    if (me->buttons() & Qt::LeftButton)
        if (!secondStage)
            m_path.lineTo(me->pos());
}

void VersionLabel::mouseReleaseEvent(QMouseEvent *me)
{
    if (me->button() == Qt::LeftButton) {
        if (!secondStage) {
            m_path.lineTo(me->pos());
            bool gotIt = true;
            for (const QPoint &pt : qAsConst(hitPoints)) {
                if (!m_path.contains(pt)) {
                    gotIt = false;
                    break;
                }
            }
            if (gotIt) {
                for (const QPoint &pt : qAsConst(missPoints)) {
                    if (m_path.contains(pt)) {
                        gotIt = false;
                        break;
                    }
                }
            }
            if (gotIt && !secondStage) {
                secondStage = true;
                m_path = QPainterPath();
                update();
            }
        } else {
            m_pushed = false;
            update();
            emit triggered();
        }
    }
}

void VersionLabel::paintEvent(QPaintEvent *pe)
{
    if (secondStage) {
        QPainter p(this);
        QStyleOptionButton opt;
        opt.init(this);
        if (!m_pushed)
            opt.state |= QStyle::State_Raised;
        else
            opt.state |= QStyle::State_Sunken;
        opt.state &= ~QStyle::State_HasFocus;
        style()->drawControl(QStyle::CE_PushButtonBevel, &opt, &p, this);
    }
    QLabel::paintEvent(pe);
}

VersionDialog::VersionDialog(QWidget *parent)
    : QDialog(parent
#ifdef Q_OS_MACOS
            , Qt::Tool
#endif
            )
{
    setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::MSWindowsFixedSizeDialogHint);
    QGridLayout *layout = new QGridLayout(this);
    VersionLabel *label = new VersionLabel;
    QLabel *lbl = new QLabel;
    QString version = tr("<h3>%1</h3><br/><br/>Version %2");
    version = version.arg(tr("Qt Designer")).arg(QLatin1String(QT_VERSION_STR));
    version.append(tr("<br/>Qt Designer is a graphical user interface designer for Qt applications.<br/>"));

    lbl->setText(tr("%1"
                    "<br/>Copyright (C) %2 The Qt Company Ltd."
                    ).arg(version, QStringLiteral("2020")));

    lbl->setWordWrap(true);
    lbl->setOpenExternalLinks(true);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox , &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(label, &VersionLabel::triggered, this, &QDialog::accept);
    layout->addWidget(label, 0, 0, 1, 1);
    layout->addWidget(lbl, 0, 1, 4, 4);
    layout->addWidget(buttonBox, 4, 2, 1, 1);
}

QT_END_NAMESPACE

#include "versiondialog.moc"
