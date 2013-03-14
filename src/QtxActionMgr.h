// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:      QtxActionMgr.h
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#ifndef QTXACTIONMGR_H
#define QTXACTIONMGR_H

#include "Qtx.h"

#include <QMap>
#include <QObject>
#include <QPointer>

class QTimer;
class QAction;
class QDomNode;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif


class QTX_EXPORT QtxActionMgr : public QObject
{
  Q_OBJECT 

  class SeparatorAction;

public:
  class Reader;
  class XMLReader;

protected:
  class Creator;

public:
  QtxActionMgr( QObject* parent );
  virtual ~QtxActionMgr();

  virtual int      registerAction( QAction*, const int = -1 );
  virtual void     unRegisterAction( const int );

  QAction*         action( const int ) const;
  int              actionId( const QAction* ) const;
  bool             contains( const int ) const;

  int              count() const;
  bool             isEmpty() const;
  QIntList         idList() const;

  bool             isUpdatesEnabled() const;
  virtual void     setUpdatesEnabled( const bool );

  virtual bool     isVisible( const int, const int ) const;
  virtual void     setVisible( const int, const int, const bool );

  void             update();

  virtual bool     isEnabled( const int ) const;
  virtual void     setEnabled( const int, const bool );

  static QAction*  separator( const bool = false );

protected:
  virtual void     internalUpdate();
  int              generateId() const;

  void             triggerUpdate();
  virtual void     updateContent();

private slots:
  void             onUpdateContent();

private:
  typedef QPointer<QAction>    ActionPtr; //!< Action guarded pointer
  typedef QMap<int, ActionPtr> ActionMap; //!< Actions map

private:
  bool             myUpdate;     //!< update flag
  ActionMap        myActions;    //!< actions map
  QTimer*          myUpdTimer;   //!< update timer
};


QTX_EXPORT typedef QMap<QString, QString> ItemAttributes; //!< attributes map

class QTX_EXPORT QtxActionMgr::Creator
{
public:
  Creator( QtxActionMgr::Reader* );
  virtual ~Creator();

  Reader* reader() const;

  virtual int    append( const QString&, const bool,
                         const ItemAttributes&, const int ) = 0;
  virtual void   connect( QAction* ) const;

  virtual bool   loadPixmap( const QString&, QPixmap& ) const;

protected:
  static int     intValue( const ItemAttributes&, const QString&, const int );
  static QString strValue( const ItemAttributes&, const QString&,
                           const QString& = QString() );
private:
  QtxActionMgr::Reader*  myReader;  //!< actions reader
};

class QTX_EXPORT QtxActionMgr::Reader
{
public:
  Reader();
  virtual ~Reader();

  QStringList    options() const;
  QString        option( const QString&, const QString& = QString() ) const;
  void           setOption( const QString&, const QString& );

  virtual bool   read( const QString&, Creator& ) const = 0;

private:
  QMap< QString, QString > myOptions;  //!< options map
};

class QTX_EXPORT QtxActionMgr::XMLReader : public Reader
{
public:
  XMLReader( const QString&, const QString&, const QString& );
  virtual ~XMLReader();

  virtual bool   read( const QString&, Creator& ) const;

protected:
  virtual void   read( const QDomNode&, const int, Creator& ) const;
  virtual bool   isNodeSimilar( const QDomNode&, const QString& ) const;
};


#endif
