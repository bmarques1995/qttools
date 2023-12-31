// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "metadatabase_p.h"
#include "widgetdatabase_p.h"

// sdk
#include <QtDesigner/abstractformeditor.h>

// Qt
#include <QtWidgets/qwidget.h>
#include <QtCore/qalgorithms.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace {
    const bool debugMetaDatabase = false;
}

namespace qdesigner_internal {

MetaDataBaseItem::MetaDataBaseItem(QObject *object)
    : m_object(object),
      m_enabled(true)
{
}

MetaDataBaseItem::~MetaDataBaseItem() = default;

QString MetaDataBaseItem::name() const
{
    Q_ASSERT(m_object);
    return m_object->objectName();
}

void MetaDataBaseItem::setName(const QString &name)
{
    Q_ASSERT(m_object);
    m_object->setObjectName(name);
}

QString MetaDataBaseItem::customClassName() const
{
    return m_customClassName;
}
void MetaDataBaseItem::setCustomClassName(const QString &customClassName)
{
    m_customClassName = customClassName;
}


QWidgetList MetaDataBaseItem::tabOrder() const
{
    return m_tabOrder;
}

void MetaDataBaseItem::setTabOrder(const QWidgetList &tabOrder)
{
    m_tabOrder = tabOrder;
}

bool MetaDataBaseItem::enabled() const
{
    return m_enabled;
}

void MetaDataBaseItem::setEnabled(bool b)
{
    m_enabled = b;
}

QStringList MetaDataBaseItem::fakeSlots() const
{
    return m_fakeSlots;
}

void MetaDataBaseItem::setFakeSlots(const QStringList &fs)
{
    m_fakeSlots = fs;
}

QStringList MetaDataBaseItem::fakeSignals() const
{
     return m_fakeSignals;
}

void MetaDataBaseItem::setFakeSignals(const QStringList &fs)
{
    m_fakeSignals = fs;
}

// -----------------------------------------------------
MetaDataBase::MetaDataBase(QDesignerFormEditorInterface *core, QObject *parent)
    : QDesignerMetaDataBaseInterface(parent),
      m_core(core)
{
}

MetaDataBase::~MetaDataBase()
{
    qDeleteAll(m_items);
}

MetaDataBaseItem *MetaDataBase::metaDataBaseItem(QObject *object) const
{
    MetaDataBaseItem *i = m_items.value(object);
    if (i == nullptr || !i->enabled())
        return nullptr;
    return i;
}

void MetaDataBase::add(QObject *object)
{
    MetaDataBaseItem *item = m_items.value(object);
    if (item != nullptr) {
        item->setEnabled(true);
        if (debugMetaDatabase) {
            qDebug() << "MetaDataBase::add: Existing item for " << object->metaObject()->className() << item->name();
        }
        return;
    }

    item = new MetaDataBaseItem(object);
    m_items.insert(object, item);
    if (debugMetaDatabase) {
        qDebug() << "MetaDataBase::add: New item " << object->metaObject()->className() << item->name();
    }
    connect(object, &QObject::destroyed, this, &MetaDataBase::slotDestroyed);

    emit changed();
}

void MetaDataBase::remove(QObject *object)
{
    Q_ASSERT(object);

    if (MetaDataBaseItem *item = m_items.value(object)) {
        item->setEnabled(false);
        emit changed();
    }
}

QObjectList MetaDataBase::objects() const
{
    QObjectList result;

    for (auto it = m_items.cbegin(), cend = m_items.cend(); it != cend; ++it) {
        if (it.value()->enabled())
            result.append(it.key());
    }

    return result;
}

QDesignerFormEditorInterface *MetaDataBase::core() const
{
    return m_core;
}

void MetaDataBase::slotDestroyed(QObject *object)
{
    if (m_items.contains(object)) {
        MetaDataBaseItem *item = m_items.value(object);
        delete item;
        m_items.remove(object);
    }
}

// promotion convenience
QDESIGNER_SHARED_EXPORT bool promoteWidget(QDesignerFormEditorInterface *core,QWidget *widget,const QString &customClassName)
{

    MetaDataBase *db = qobject_cast<MetaDataBase *>(core->metaDataBase());
    if (!db)
        return false;
    MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    if (!item) {
        db ->add(widget);
        item = db->metaDataBaseItem(widget);
    }
    // Recursive promotion occurs if there is a plugin missing.
    const QString oldCustomClassName = item->customClassName();
    if (!oldCustomClassName.isEmpty()) {
        qDebug() << "WARNING: Recursive promotion of " << oldCustomClassName << " to " << customClassName
            << ". A plugin is missing.";
    }
    item->setCustomClassName(customClassName);
    if (debugMetaDatabase) {
        qDebug() << "Promoting " << widget->metaObject()->className() << " to " << customClassName;
    }
    return true;
}

QDESIGNER_SHARED_EXPORT void demoteWidget(QDesignerFormEditorInterface *core,QWidget *widget)
{
    MetaDataBase *db = qobject_cast<MetaDataBase *>(core->metaDataBase());
    if (!db)
        return;
    MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    item->setCustomClassName(QString());
    if (debugMetaDatabase) {
        qDebug() << "Demoting " << widget;
    }
}

QDESIGNER_SHARED_EXPORT bool isPromoted(QDesignerFormEditorInterface *core, QWidget* widget)
{
    const MetaDataBase *db = qobject_cast<const MetaDataBase *>(core->metaDataBase());
    if (!db)
        return false;
    const MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    if (!item)
        return false;
    return !item->customClassName().isEmpty();
}

QDESIGNER_SHARED_EXPORT QString promotedCustomClassName(QDesignerFormEditorInterface *core, QWidget* widget)
{
    const MetaDataBase *db = qobject_cast<const MetaDataBase *>(core->metaDataBase());
    if (!db)
        return QString();
    const MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    if (!item)
        return QString();
    return item->customClassName();
}

QDESIGNER_SHARED_EXPORT QString promotedExtends(QDesignerFormEditorInterface *core, QWidget* widget)
{
    const QString customClassName = promotedCustomClassName(core,widget);
    if (customClassName.isEmpty())
        return QString();
    const int i = core->widgetDataBase()->indexOfClassName(customClassName);
    if (i == -1)
        return QString();
    return core->widgetDataBase()->item(i)->extends();
}


} // namespace qdesigner_internal

QT_END_NAMESPACE
