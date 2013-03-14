// Copyright (C) T-Solution
//

// File   : TSWebProxyObject.cxx
// Author : Zhan

#include "TSWebProxyObject.h"

#include "TSBrowserApplication.h"

// QT
#include <QByteArray>
#include <QBuffer>
#include <QFileDialog>
#include <QImage>
#include <QList>
#include <QMap>
#include <QDebug>

TSWebProxyObject::TSWebProxyObject(QObject *parent) :
    QObject(parent)
{
    m_signalEmited = 0;
}


////////////////////////////////////////////////////////////////////////////
// Slots

void TSWebProxyObject::unbind()
{
	disconnect();
}

void TSWebProxyObject::unbind(const QString& sig)
{
	const char* _signal = 0;
	if( !sig.compare("TaskAdded" ), Qt::CaseInsensitive )
		_signal = SIGNAL(TaskAdded(QMap<QString, QVariant>));

	else if( !sig.compare("TaskInited" ), Qt::CaseInsensitive )
		_signal = SIGNAL(TaskInited(QMap<QString, QVariant>));

	else if( !sig.compare("TaskStarted" ), Qt::CaseInsensitive )
		_signal = SIGNAL(TaskStarted(QMap<QString, QVariant>));

	else if( !sig.compare("TaskCompleted" ), Qt::CaseInsensitive )
		_signal = SIGNAL(TaskCompleted(QMap<QString, QVariant>));

	else if( !sig.compare("TaskError" ), Qt::CaseInsensitive )
		_signal = SIGNAL(TaskError(QMap<QString, QVariant>));

	else if( !sig.compare("TaskProgress" ), Qt::CaseInsensitive )
		_signal = SIGNAL(TaskProgress(QMap<QString, QVariant>, int));

	else if( !sig.compare("TaskStopped" ), Qt::CaseInsensitive )
		_signal = SIGNAL(TaskStopped(QMap<QString, QVariant>));

	if( _signal && receivers(_signal) > 0 )
		disconnect(_signal, 0, 0);
}


void TSWebProxyObject::addNamedJSObject(const QString& _name)
{
	if( _name.isEmpty() || _name.isNull() ) return;

	TSBrowserApplication::instance()->addNamedJSObject(this->parent(), _name);
}

QMap<QString, QVariant> TSWebProxyObject::slotThatReturns(const QMap<QString, QVariant>& object)
{
    QLOG_DEBUG() << "TSWebProxyObject::slotThatReturns";
    this->m_returnObject.clear();
    this->m_returnObject.unite(object);
    QString addedBonus = QString::number(object["intValue"].toInt(),
                                         10).append(" added bonus.");
    this->m_returnObject["stringValue"] = QVariant(addedBonus);
    QLOG_DEBUG() << "TSWebProxyObject::slotThatReturns" << this->m_returnObject;
    return this->m_returnObject;
}

void TSWebProxyObject::slotThatEmitsSignal()
{
    QLOG_DEBUG() << "TSWebProxyObject::slotThatEmitsSignal";
    this->m_signalEmited++;
    this->m_emitSignal.clear();
    this->m_emitSignal["signalsEmited"] = QVariant(this->m_signalEmited);
    this->m_emitSignal["sender"] = QVariant("TSWebProxyObject::slotThatEmitsSignal");
    QLOG_DEBUG() << "TSWebProxyObject::slotThatEmitsSignal" << this->m_emitSignal;
    emit signal(this->m_emitSignal);
}