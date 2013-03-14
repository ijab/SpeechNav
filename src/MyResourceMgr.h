#ifndef __MYRESOURCEMGR_H__
#define __MYRESOURCEMGR_H__

#include "QtxResourceMgr.h"

class MyResourceMgr: public QtxResourceMgr
{

public:
  MyResourceMgr( const QString& app, const QString& resVarTemplate = QString() );
  virtual ~MyResourceMgr();

  static void initResourceMgr() ;

   QString version() const { return myExtAppVersion; }
   QString appName() const { return myExtAppName; }
   QString appPath() const { return myExtAppPath; }

   QPixmap loadPixmap( const QString& filename ) const;

protected:
  virtual QString userFileName( const QString&, const bool = true ) const;
  virtual QString findAppropriateUserFile( const QString& ) const;
  virtual long    userFileId( const QString& ) const;

 

public:
  static QString myExtAppName;
  static QString myExtAppVersion;
  static QString myExtAppPath;

  

};

#endif