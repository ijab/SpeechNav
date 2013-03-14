// Copyright (C) T-Solution
//

//  
// File   : TSWebProxyObject.h
// Author : Zhan
//


#ifndef TSWEBPROXYOBJECT_H
#define TSWEBPROXYOBJECT_H

#include "TSWebApp.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif


#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>



class TSWEBAPP_EXPORTS TSWebProxyObject : public QObject
{
    Q_OBJECT
public:
    explicit TSWebProxyObject(QObject *parent = 0);
	virtual ~TSWebProxyObject(){};
	

signals:
	// example signal
    void						signal(QMap<QString, QVariant> object);

	

public slots:
	// Sample slots
    QMap<QString, QVariant>		slotThatReturns(const QMap<QString, QVariant>& object);
    void						slotThatEmitsSignal();

	void						unbind();
	void						unbind(const QString& sig);
	
	void						addNamedJSObject(const QString& _name);


private slots:
	void						onSpeechStart(){};
	


private:
    int							m_signalEmited;
    QMap<QString, QVariant>		m_returnObject;
    QMap<QString, QVariant>		m_emitSignal;

};


#endif // TSWEBPROXYOBJECT_H
