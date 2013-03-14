// MyResourceMgr.cpp

#include "MyResourceMgr.h"

#include <QDir>
#include <QFile>
#include <QApplication>

#define WEBKIT_VERSION		"1.0.0"

/* XPM */
static const char* pixmap_not_found_xpm[] = {
"16 16 3 1",
"       c None",
".      c #000000",
"+      c #A80000",
"                ",
"                ",
"    .     .     ",
"   .+.   .+.    ",
"  .+++. .+++.   ",
"   .+++.+++.    ",
"    .+++++.     ",
"     .+++.      ",
"    .+++++.     ",
"   .+++.+++.    ",
"  .+++. .+++.   ",
"   .+.   .+.    ",
"    .     .     ",
"                ",
"                ",
"                "};

QString MyResourceMgr::myExtAppName    = QString();
QString MyResourceMgr::myExtAppVersion = QString();
QString MyResourceMgr::myExtAppPath    = QString();

//===========================================================================================
void MyResourceMgr::initResourceMgr()
{
    if ( myExtAppName.isNull() || myExtAppVersion.isNull() || myExtAppPath.isNull() ) 
	{
      MyResourceMgr resMgr( "SpeechNav", QString( "%1Config" ) );
      resMgr.loadLanguage( "SpeechNav",  "en" );

      myExtAppName = QObject::tr( "APP_NAME" ).trimmed();
      if ( myExtAppName == "APP_NAME" || myExtAppName.toLower() == "webkit" ) 
        myExtAppName = "SpeechNav";

	  myExtAppVersion = WEBKIT_VERSION ;

	  QStringList arguments = QApplication::arguments();

	  QString file = arguments[0];
      QFileInfo fi (file);
      myExtAppPath = fi.absolutePath();

    }
}

//===========================================================================================
MyResourceMgr::MyResourceMgr( const QString& app, const QString& resVarTemplate )
: QtxResourceMgr( app, resVarTemplate )
{
	setDefaultPixmap( QPixmap( pixmap_not_found_xpm ) ) ;
	setCurrentFormat( "xml" );
}

MyResourceMgr::~MyResourceMgr()
{

}

/*!
    Returns the user file name for specified application
*/
QString MyResourceMgr::userFileName( const QString& appName, const bool for_load ) const
{
  QString pathName;

  QStringList arguments = QApplication::arguments();
  // Try config file, given in arguments
  for (int i = 1; i < arguments.count(); i++) {
    QRegExp rx ("--resources=(.+)");
    if ( rx.indexIn( arguments[i] ) >= 0 && rx.numCaptures() > 1 ) {
      QString file = rx.cap(1);
      QFileInfo fi (file);
      pathName = fi.absoluteFilePath();
    }
  }

  if (!pathName.isEmpty())
    return pathName;

  // QtxResourceMgr::userFileName() + '.' + version()
  pathName = QtxResourceMgr::userFileName( appName );

  if ( !version().isEmpty() )
    pathName += QString( "." ) + version();

  if ( !QFileInfo( pathName ).exists() && for_load )
  {
    QString newName = findAppropriateUserFile( pathName );
    if ( !newName.isEmpty() )
      pathName = newName;
  }

  return pathName;
}

/*!
    Finds other the most appropriate user file instead missing one
*/
QString MyResourceMgr::findAppropriateUserFile( const QString& fname ) const
{
  QString appr_file;

  // calculate default file id from user file name
  long id0 = userFileId( fname );
  if ( id0 < 0 ) // can't calculate file id from user file name, no further processing
    return appr_file;

  long id, appr = -1;

  // get all files from the same dir where use file is (should be) situated
  QDir d( QFileInfo( fname ).dir() );
  if ( d.exists() ) {
    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QStringList l = d.entryList();
    for( QStringList::const_iterator anIt = l.begin(), aLast = l.end(); anIt!=aLast; anIt++ )
    {
      id = userFileId( *anIt );
      if ( id < 0 )
	continue;
      if( appr < 0 || qAbs( id-id0 ) < qAbs( appr-id0 ) )
      {
	appr = id;
	appr_file = d.absoluteFilePath( *anIt );
      }
    }
  }

  // backward compatibility: check also user's home directory (if it differs from above one)
  QDir home = QDir::home();
  if ( home.exists() && d.canonicalPath() != home.canonicalPath() ) {
    home.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QStringList l = home.entryList();

    for( QStringList::const_iterator anIt = l.begin(), aLast = l.end(); anIt!=aLast; anIt++ )
    {
      id = userFileId( *anIt );
      if ( id < 0 )
	continue;
      if( appr < 0 || qAbs( id-id0 ) < qAbs( appr-id0 ) )
      {
	appr = id;
	appr_file = home.absoluteFilePath( *anIt );
      }
    }
  }
  
  return appr_file;
}

long MyResourceMgr::userFileId( const QString& _fname ) const
{
    long id = -1;
    if ( !myExtAppName.isEmpty() ) {
#ifdef WIN32
      QRegExp exp( QString( "%1\\.%2\\.([a-zA-Z0-9.]+)$" ).arg( myExtAppName ).arg( currentFormat() ) );
#else
      QRegExp exp( QString( "\\.%1rc\\.([a-zA-Z0-9.]+)$" ).arg( myExtAppName ) );
#endif
      QRegExp vers_exp( "^([0-9]+)([A-Za-z]?)([0-9]*)$" );
      
      QString fname = QFileInfo( _fname ).fileName();
      if( exp.exactMatch( fname ) ) {
        QStringList vers = exp.cap( 1 ).split( ".", QString::SkipEmptyParts );
        int major=0, minor=0;
        int release = 0, dev1 = 0, dev2 = 0;
	if ( vers.count() > 0 ) major = vers[0].toInt();
	if ( vers.count() > 1 ) minor = vers[1].toInt();
	if ( vers.count() > 2 ) {
	  if( vers_exp.indexIn( vers[2] ) != -1 ) {
	    release = vers_exp.cap( 1 ).toInt();
	    dev1 = vers_exp.cap( 2 )[ 0 ].toLatin1();
	    dev2 = vers_exp.cap( 3 ).toInt();
	  }
	}
        
        int dev = dev1*100+dev2;
	id = major;
        id*=100; id+=minor;
        id*=100; id+=release;
        id*=10000;
        if ( dev > 0 ) id+=dev-10000;
      }
    }
    return id;
}


QPixmap MyResourceMgr::loadPixmap( const QString& filename ) const
{
	return QtxResourceMgr::loadPixmap( myExtAppPath + filename ) ;
}