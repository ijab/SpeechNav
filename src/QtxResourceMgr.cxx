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

// File:      QtxResourceMgr.cxx
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#include "QtxResourceMgr.h"
#include "QtxTranslator.h"

#include <QSet>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QTextStream>
#include <QApplication>
#include <QLibraryInfo>
#include <QtDebug>
#ifndef QT_NO_DOM
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#endif

#include <stdlib.h>

/*!
  \class QtxResourceMgr::Resources
  \internal
  \brief Represents container for settings read from the resource file.
*/

class QtxResourceMgr::Resources
{
public:
  Resources( QtxResourceMgr*, const QString& );
  virtual ~Resources();

  QString                file() const;
  void                   setFile( const QString& );

  QString                value( const QString&, const QString&, const bool ) const;
  void                   setValue( const QString&, const QString&, const QString& );

  bool                   hasSection( const QString& ) const;
  bool                   hasValue( const QString&, const QString& ) const;

  void                   removeSection( const QString& );
  void                   removeValue( const QString&, const QString& );

  QPixmap                loadPixmap( const QString& ) const;
  QPixmap                loadPixmap( const QString&, const QString&, const QString& ) const;
  QTranslator*           loadTranslator( const QString&, const QString&, const QString& ) const;

  QString                makeSubstitution( const QString&, const QString&, const QString& ) const;

  void                   clear();

  QStringList            sections() const;
  QStringList            parameters( const QString& ) const;

  QString                path( const QString&, const QString&, const QString& ) const;

protected:
  QtxResourceMgr*        resMgr() const;

private:
  Section                section( const QString& );
  const Section          section( const QString& ) const;

  QString                fileName( const QString&, const QString&, const QString& ) const;

private:
  typedef QMap<QString, Section> SectionMap;

private:
  QtxResourceMgr*        myMgr;             //!< resources manager
  SectionMap             mySections;        //!< sections map
  QString                myFileName;        //!< resources file name
  QMap<QString,QPixmap>  myPixmapCache;     //!< pixmaps cache

  friend class QtxResourceMgr::Format;
};

/*!
  \brief Constructor.
  \param mgr parent resources manager
  \param fileName resources file name
*/
QtxResourceMgr::Resources::Resources( QtxResourceMgr* mgr, const QString& fileName )
: myMgr( mgr ),
  myFileName( fileName )
{
}

/*!
  \brief Destructor.
*/
QtxResourceMgr::Resources::~Resources()
{
}

/*!
  \brief Get resources file name.

  This file is used to load/save operations.

  \return file name
  \sa setFile()
*/
QString QtxResourceMgr::Resources::file() const
{
  return myFileName;
}

/*!
  \brief Set resources file name.
  \param fn file name
  \sa file()
*/
void QtxResourceMgr::Resources::setFile( const QString& fn )
{
  myFileName = fn;
}

/*!
  \brief Get string representation of parameter value.
  \param sect section name
  \param name parameter name
  \param subst if \c true, perform variables substitution
  \return parameter value or null QString if there is no such parameter
  \sa setValue(), makeSubstitution()
*/
QString QtxResourceMgr::Resources::value( const QString& sect, const QString& name, const bool subst ) const
{
  QString val;

  if ( hasValue( sect, name ) )
  {
    val = section( sect )[name];
    if ( subst )
      val = makeSubstitution( val, sect, name );
  }
  return val;
}

/*!
  \brief Set parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
  \sa value(), makeSubstitution()
*/
void QtxResourceMgr::Resources::setValue( const QString& sect, const QString& name, const QString& val )
{
  if ( !mySections.contains( sect ) )
    mySections.insert( sect, Section() );

  mySections[sect].insert( name, val );
}

/*!
  \brief Check section existence.
  \param sect section name
  \return \c true if section exists
*/
bool QtxResourceMgr::Resources::hasSection( const QString& sect ) const
{
  return mySections.contains( sect );
}

/*!
  \brief Check parameter existence.
  \param sect section name
  \param name parameter name
  \return \c true if parameter exists in specified section
*/
bool QtxResourceMgr::Resources::hasValue( const QString& sect, const QString& name ) const
{
  return hasSection( sect ) && section( sect ).contains( name );
}

/*!
  \brief Remove resourcs section.
  \param sect secton name
*/
void QtxResourceMgr::Resources::removeSection( const QString& sect )
{
  mySections.remove( sect );
}

/*!
  \brief Remove parameter from the section.
  \param sect section name
  \param name parameter name
*/
void QtxResourceMgr::Resources::removeValue( const QString& sect, const QString& name )
{
  if ( !mySections.contains( sect ) )
    return;

  mySections[sect].remove( name );

  if ( mySections[sect].isEmpty() )
    mySections.remove( sect );
}

/*!
  \brief Remove all sections.
*/
void QtxResourceMgr::Resources::clear()
{
  mySections.clear();
}

/*!
  \brief Get all sections names.
  \return list of section names
*/
QStringList QtxResourceMgr::Resources::sections() const
{
  return mySections.keys();
}

/*!
  \brief Get all parameters name in specified section.
  \param sec section name
  \return list of settings names
*/
QStringList QtxResourceMgr::Resources::parameters( const QString& sec ) const
{
  if ( !hasSection( sec ) )
    return QStringList();

  return section( sec ).keys();
}

/*!
  \brief Get absolute path to the file which name is defined by the parameter.

  The file name is defined by \a name argument, while directory name is retrieved
  from resources parameter \a prefix of section \a sec. Both directory and file name
  can be relative. If the directory is relative, it is calculated from the initial
  resources file name (see file()). Directory parameter can contain environment 
  variables, which are substituted automatically.

  \param sec section name
  \param prefix parameter containing directory name
  \param name file name
  \return absolute file path or null QString if file does not exist
  \sa fileName(), file(), makeSubstitution()
*/
QString QtxResourceMgr::Resources::path( const QString& sec, const QString& prefix, const QString& name ) const
{
  QString filePath = fileName( sec, prefix, name );
  if ( !filePath.isEmpty() )
  {
    if ( !QFileInfo( filePath ).exists() )
      filePath = QString();
  }
  return filePath;
}

/*!
  \brief Get resource manager
  \return resource manager pointer
*/
QtxResourceMgr* QtxResourceMgr::Resources::resMgr() const
{
  return myMgr;
}

/*!
  \brief Get resources section by specified name.

  If section does not exist it is created (empty).

  \param sn section name
  \return resources section
*/
QtxResourceMgr::Section QtxResourceMgr::Resources::section( const QString& sn )
{
  if ( !mySections.contains( sn ) )
    mySections.insert( sn, Section() );

  return mySections[sn];
}

/*!
  \brief Get resources section by specified name.
  \param sn section name
  \return resources section
*/
const QtxResourceMgr::Section QtxResourceMgr::Resources::section( const QString& sn ) const
{
  return mySections[sn];
}

/*!
  \brief Get file path.

  The file name is defined by \a name argument, while directory name is retrieved
  from resources parameter \a prefix of section \a sec. Both directory and file name
  can be relative. If the directory is relative, it is calculated from the initial
  resources file name (see file()). Directory parameter can contain environment 
  variables, which are substituted automatically.
  File existence is not checked.

  \param sec section name
  \param prefix parameter containing directory name
  \param name file name
  \return absolute file path or null QString if \a prefix parameter
          does not exist in section \sec
  \sa path(), file(), makeSubstitution()
*/
QString QtxResourceMgr::Resources::fileName( const QString& sect, const QString& prefix, const QString& name ) const
{
  QString path;
  if ( hasValue( sect, prefix ) )
  {
    path = value( sect, prefix, true );
    if ( !path.isEmpty() )
    {
      if ( QFileInfo( path ).isRelative() )
        path = Qtx::addSlash( Qtx::dir( myFileName, true ) ) + path;

      path = Qtx::addSlash( path ) + name;
    }
  }
  if( !path.isEmpty() )
  {
    QString fname = QDir::convertSeparators( path );
    QFileInfo inf( fname );
    fname = inf.absoluteFilePath();
    return fname;
  }
  return QString();
}

/*!
  \brief Load and return pixmap from external file.
  
  If QtxResourceMgr::isPixmapCached() is \c true then cached pixmap is returned
  (if it is already loaded), otherwise it is loaded from file.
  If the file name is invalid, null pixmap is returned.

  \param fname pixmap file name
  \return pixmap loaded from file
*/
QPixmap QtxResourceMgr::Resources::loadPixmap( const QString& fname ) const
{
  
  bool toCache = resMgr() ? resMgr()->isPixmapCached() : false;
  QPixmap p;
  if( toCache && myPixmapCache.contains( fname ) )
    p = myPixmapCache[fname];
  else
  {
    p.load( fname );
    if( toCache )
      ( ( QMap<QString,QPixmap>& )myPixmapCache ).insert( fname, p );
  }
  return p;
}

/*!
  \brief Load and return pixmap from external file.
  
  If QtxResourceMgr::isPixmapCached() is \c true then cached pixmap is returned
  (if it is already loaded), otherwise it is loaded from file.
  If the file name is invalid, null pixmap is returned.

  \param sect section name
  \param prefix parameter containing resources directory name
  \param name pixmap file name
  \return pixmap loaded from file
*/
QPixmap QtxResourceMgr::Resources::loadPixmap( const QString& sect, const QString& prefix, const QString& name ) const
{
  QString fname = fileName( sect, prefix, name );
  bool toCache = resMgr() ? resMgr()->isPixmapCached() : false;
  QPixmap p;
  if( toCache && myPixmapCache.contains( fname ) )
    p = myPixmapCache[fname];
  else
  {
    p.load( fname );
    if( toCache )
      ( ( QMap<QString,QPixmap>& )myPixmapCache ).insert( fname, p );
  }
  return p;
}

/*!
  \brief Load translator.
  \param sect section name
  \param prefix parameter containing resources directory
  \param name translation file name
  \return just created and loaded translator or 0 in case of error
*/
QTranslator* QtxResourceMgr::Resources::loadTranslator( const QString& sect, const QString& prefix, const QString& name ) const
{
  QTranslator* trans = new QtxTranslator( 0 );
  QString fname = fileName( sect, prefix, name );
  if ( !trans->load( Qtx::file( fname, false ), Qtx::dir( fname ) ) )
  {
    delete trans;
    trans = 0;
  }
  return trans;
}

/*!
  \brief Substitute variables by their values.

  Environment variable is substituted by its value. For other variables resource
  manager tries to find value among defined resources parameters.

  \param str string to be processed
  \param sect section, where variables are searched
  \param name name of variable which must be ignored during substitution
  \return processed string (with all substitutions made)
*/
QString QtxResourceMgr::Resources::makeSubstitution( const QString& str, const QString& sect, const QString& name ) const
{
  QString res = str;

  QMap<QString, int> ignoreMap;
  ignoreMap.insert( name, 0 );

  int start( 0 ), len( 0 );
  while ( true )
  {
    QString envName = Qtx::findEnvVar( res, start, len );
    if ( envName.isNull() )
      break;

    QString newStr;
    if ( ::getenv( envName.toLatin1() ) )
      newStr = QString( ::getenv( envName.toLatin1() ) );

    if ( newStr.isNull() )
    {
      if ( ignoreMap.contains( envName ) )
      {
        start += len;
        continue;
      }

      if ( hasValue( sect, envName ) )
        newStr = value( sect, envName, false );
      ignoreMap.insert( envName, 0 );
    }
    res.replace( start, len, newStr );
  }

  res.replace( "$$", "$" );
  res.replace( "%%", "%" );

  return res;
}

/*!
  \class QtxResourceMgr::IniFormat
  \internal
  \brief Reader/writer for .ini resources files.
*/

class QtxResourceMgr::IniFormat : public Format
{
public:
  IniFormat();
  ~IniFormat();

protected:
  virtual bool load( const QString&, QMap<QString, Section>& );
  virtual bool save( const QString&, const QMap<QString, Section>& );

private:
  bool         load( const QString&, QMap<QString, Section>&, QSet<QString>& );
};

/*!
  \brief Constructor.
*/
QtxResourceMgr::IniFormat::IniFormat()
: Format( "ini" )
{
}

/*!
  \brief Destructor.
*/
QtxResourceMgr::IniFormat::~IniFormat()
{
}

/*!
  \brief Load resources from ini-file.
  \param fname resources file name
  \param secMap resources map to be filled in
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::IniFormat::load( const QString& fname, QMap<QString, Section>& secMap )
{
  QSet<QString> importHistory;
  return load( fname, secMap, importHistory );
}


/*!
  \brief Load resources from xml-file.
  \param fname resources file name
  \param secMap resources map to be filled in
  \param importHistory list of already imported resources files (to prevent import loops)
  \return \c true on success or \c false on error
*/
bool QtxResourceMgr::IniFormat::load( const QString& fname, QMap<QString, Section>& secMap, QSet<QString>& importHistory)
{
  QString aFName = fname.trimmed();
  if ( !QFileInfo( aFName ).exists() )
  {
    if ( QFileInfo( aFName + ".ini" ).exists() )
      aFName += ".ini";
    else if ( QFileInfo( aFName + ".INI" ).exists() )
      aFName += ".INI";
    else
      return false; // file does not exist
  }
  QFileInfo aFinfo( aFName );
  aFName = aFinfo.canonicalFilePath();

  if ( !importHistory.contains( aFName ) )
    importHistory.insert( aFName );
  else
    return true;   // already imported (prevent import loops)

  QFile file( aFName );
  if ( !file.open( QFile::ReadOnly ) )
    return false;  // file is not accessible

  QTextStream ts( &file );

  QString data;
  int line = 0;
  bool res = true;
  QString section;

  QString separator = option( "separator" );
  if ( separator.isNull() )
    separator = QString( "=" );

  QString comment = option( "comment" );
  if ( comment.isNull() )
    comment = QString( "#" );

  while ( true )
  {
    data = ts.readLine();
    line++;

    if ( data.isNull() )
      break;

    data = data.trimmed();
    if ( data.isEmpty() )
      continue;

    if ( data.startsWith( comment ) )
      continue;

    QRegExp rx( "^\\[([\\w\\s\\._]*)\\]$" );
    if ( rx.indexIn( data ) != -1 )
    {
      section = rx.cap( 1 );
      if ( section.isEmpty() )
      {
        res = false;
        qWarning() << "QtxResourceMgr: Empty section in line:" << line;
      }
    }
    else if ( data.contains( separator ) && !section.isEmpty() )
    {
      int pos = data.indexOf( separator );
      QString key = data.left( pos ).trimmed();
      QString val = data.mid( pos + 1 ).trimmed();
      secMap[section].insert( key, val );
    }
    else if ( section == "import" )
    {
      QFileInfo impFInfo( data );
      if ( impFInfo.isRelative() )
	impFInfo.setFile( aFinfo.absoluteDir(), data );
    
      QMap<QString, Section> impMap;
      if ( !load( impFInfo.absoluteFilePath(), impMap, importHistory ) )
      {
        qDebug() << "QtxResourceMgr: Error with importing file:" << data;
      }
      else 
      {
	QMap<QString, Section>::const_iterator it = impMap.constBegin();
	for ( ; it != impMap.constEnd() ; ++it )
	{ 
	  if ( !secMap.contains( it.key() ) )
	  {
	    // insert full section
	    secMap.insert( it.key(), it.value() );
	  }
	  else
	  {
	    // insert all parameters from the section
	    Section::ConstIterator paramIt = it.value().begin();
	    for ( ; paramIt != it.value().end() ; ++paramIt )
	    {
	      if ( !secMap[it.key()].contains( paramIt.key() ) )
		secMap[it.key()].insert( paramIt.key(), paramIt.value() );
	    }
	  }
	}
      }
    }
    else
    {
      res = false;
      if ( section.isEmpty() )
	qWarning() << "QtxResourceMgr: Current section is empty";
      else
	qWarning() << "QtxResourceMgr: Error in line:" << line;
    }
  }

  file.close();

  return res; 
}

/*!
  \brief Save resources to the ini-file.
  \param fname resources file name
  \param secMap resources map
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::IniFormat::save( const QString& fname, const QMap<QString, Section>& secMap )
{
  if ( !Qtx::mkDir( QFileInfo( fname ).absolutePath() ) )
    return false;

  QFile file( fname );
  if ( !file.open( QFile::WriteOnly ) )
    return false;

  QTextStream ts( &file );

  bool res = true;
  for ( QMap<QString, Section>::ConstIterator it = secMap.begin(); it != secMap.end() && res; ++it )
  {
    QStringList data( QString( "[%1]" ).arg( it.key() ) );
    for ( Section::ConstIterator iter = it.value().begin(); iter != it.value().end(); ++iter )
      data.append( iter.key() + " = " + iter.value() );
    data.append( "" );

    for ( QStringList::ConstIterator itr = data.begin(); itr != data.end(); ++itr )
      ts << *itr << endl;
  }

  file.close();

  return res;
}

/*!
  \class QtxResourceMgr::XmlFormat
  \internal
  \brief Reader/writer for .xml resources files.
*/

class QtxResourceMgr::XmlFormat : public Format
{
public:
  XmlFormat();
  ~XmlFormat();

protected:
  virtual bool load( const QString&, QMap<QString, Section>& );
  virtual bool save( const QString&, const QMap<QString, Section>& );

private:
  QString      docTag() const;
  QString      sectionTag() const;
  QString      parameterTag() const;
  QString      importTag() const;
  QString      nameAttribute() const;
  QString      valueAttribute() const;

  bool         load( const QString&, QMap<QString, Section>&, QSet<QString>& );
};

/*!
  \brief Constructor.
*/
QtxResourceMgr::XmlFormat::XmlFormat()
: Format( "xml" )
{
}

/*!
  \brief Destructor.
*/
QtxResourceMgr::XmlFormat::~XmlFormat()
{
}

/*!
  \brief Load resources from xml-file.
  \param fname resources file name
  \param secMap resources map to be filled in
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::XmlFormat::load( const QString& fname, QMap<QString, Section>& secMap )
{
  QSet<QString> importHistory;
  return load( fname, secMap, importHistory );
}

/*!
  \brief Load resources from xml-file.
  \param fname resources file name
  \param secMap resources map to be filled in
  \param importHistory list of already imported resources files (to prevent import loops)
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::XmlFormat::load( const QString& fname, QMap<QString, Section>& secMap, QSet<QString>& importHistory )
{
  QString aFName = fname.trimmed();
  if ( !QFileInfo( aFName ).exists() )
  {
    if ( QFileInfo( aFName + ".xml" ).exists() )
      aFName += ".xml";
    else if ( QFileInfo( aFName + ".XML" ).exists() )
      aFName += ".XML";
    else
      return false; // file does not exist
  }
  QFileInfo aFinfo( aFName );
  aFName = aFinfo.canonicalFilePath();

  if ( !importHistory.contains(  aFName ) )
    importHistory.insert( aFName );
  else
    return true;   // already imported (prevent import loops)

  bool res = false;

#ifndef QT_NO_DOM

  QFile file( aFName );
  if ( !file.open( QFile::ReadOnly ) )
  {
    qDebug() << "QtxResourceMgr: File is not accessible:" << aFName;
    return false;
  }

  QDomDocument doc;

  res = doc.setContent( &file );
  file.close();

  if ( !res )
  {
    qDebug() << "QtxResourceMgr: File is empty:" << aFName;
    return false;
  }

  QDomElement root = doc.documentElement();
  if ( root.isNull() || root.tagName() != docTag() )
  {
    qDebug() << "QtxResourceMgr: Invalid root in file:" << aFName;
    return false;
  }

  QDomNode sectNode = root.firstChild();
  while ( res && !sectNode.isNull() )
  {
    res = sectNode.isElement();
    if ( res )
    {
      QDomElement sectElem = sectNode.toElement();
      if ( sectElem.tagName() == sectionTag() && sectElem.hasAttribute( nameAttribute() ) )
      {
        QString section = sectElem.attribute( nameAttribute() );
        QDomNode paramNode = sectNode.firstChild();
        while ( res && !paramNode.isNull() )
        {
          res = paramNode.isElement();
          if ( res )
          {
            QDomElement paramElem = paramNode.toElement();
            if ( paramElem.tagName() == parameterTag() &&
                 paramElem.hasAttribute( nameAttribute() ) && paramElem.hasAttribute( valueAttribute() ) )
            {
              QString paramName = paramElem.attribute( nameAttribute() );
              QString paramValue = paramElem.attribute( valueAttribute() );
              secMap[section].insert( paramName, paramValue );
            }
            else
            {
              qDebug() << "QtxResourceMgr: Invalid parameter element in file:" << aFName;
              res = false;
            }
          }
          else
          {
            res = paramNode.isComment();
            if( !res )
              qDebug() << "QtxResourceMgr: Node is neither element nor comment in file:" << aFName;
          }

          paramNode = paramNode.nextSibling();
        }
      }
      else if ( sectElem.tagName() == importTag() && sectElem.hasAttribute( nameAttribute() ) )
      {
	QFileInfo impFInfo( sectElem.attribute( nameAttribute() ) );
	if ( impFInfo.isRelative() )
	  impFInfo.setFile( aFinfo.absoluteDir(), sectElem.attribute( nameAttribute() ) );

        QMap<QString, Section> impMap;
        if ( !load( impFInfo.absoluteFilePath(), impMap, importHistory ) )
	{
          qDebug() << "QtxResourceMgr: Error with importing file:" << sectElem.attribute( nameAttribute() );
	}
	else
	{
	  QMap<QString, Section>::const_iterator it = impMap.constBegin();
	  for ( ; it != impMap.constEnd() ; ++it )
	  {
	    if ( !secMap.contains( it.key() ) )
	    {
	    // insert full section
	      secMap.insert( it.key(), it.value() );
	    }
	    else
	    {
	      // insert all parameters from the section
	      Section::ConstIterator paramIt = it.value().begin();
	      for ( ; paramIt != it.value().end() ; ++paramIt )
	      {
		if ( !secMap[it.key()].contains( paramIt.key() ) )
		  secMap[it.key()].insert( paramIt.key(), paramIt.value() );
	      }
	    }
	  }
        }
      }
      else
      {
        qDebug() << "QtxResourceMgr: Invalid section in file:" << aFName;
        res = false;
      }
    }
    else
    {
      res = sectNode.isComment(); // if it's a comment -- let it be, pass it..
      if ( !res )
        qDebug() << "QtxResourceMgr: Node is neither element nor comment in file:" << aFName;
    }

    sectNode = sectNode.nextSibling();
  }

#endif
  
  if ( res )
    qDebug() << "QtxResourceMgr: File" << fname << "is loaded successfully";
  return res;
}

/*!
  \brief Save resources to the xml-file.
  \param fname resources file name
  \param secMap resources map
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::XmlFormat::save( const QString& fname, const QMap<QString, Section>& secMap )
{
  bool res = false;

#ifndef QT_NO_DOM

  if ( !Qtx::mkDir( QFileInfo( fname ).absolutePath() ) )
    return false;

  QFile file( fname );
  if ( !file.open( QFile::WriteOnly ) )
    return false;

  QDomDocument doc( docTag() );
  QDomElement root = doc.createElement( docTag() );
  doc.appendChild( root );

  for ( QMap<QString, Section>::ConstIterator it = secMap.begin(); it != secMap.end(); ++it )
  {
    QDomElement sect = doc.createElement( sectionTag() );
    sect.setAttribute( nameAttribute(), it.key() );
    root.appendChild( sect );
    for ( Section::ConstIterator iter = it.value().begin(); iter != it.value().end(); ++iter )
    {
      QDomElement val = doc.createElement( parameterTag() );
      val.setAttribute( nameAttribute(), iter.key() );
      val.setAttribute( valueAttribute(), iter.value() );
      sect.appendChild( val );
    }
  }

  QTextStream ts( &file );
  QStringList docStr = doc.toString().split( "\n" );
  for ( QStringList::ConstIterator itr = docStr.begin(); itr != docStr.end(); ++itr )
    ts << *itr << endl;

  file.close();

#endif

  return res;
}

/*!
  \brief Get document tag name
  \return XML document tag name
*/
QString QtxResourceMgr::XmlFormat::docTag() const
{
  QString tag = option( "doc_tag" );
  if ( tag.isEmpty() )
    tag = QString( "document" );
  return tag;
}

/*!
  \brief Get section tag name
  \return XML section tag name
*/
QString QtxResourceMgr::XmlFormat::sectionTag() const
{
  QString tag = option( "section_tag" );
  if ( tag.isEmpty() )
    tag = QString( "section" );
  return tag;
}

/*!
  \brief Get parameter tag name
  \return XML parameter tag name
*/
QString QtxResourceMgr::XmlFormat::parameterTag() const
{
  QString tag = option( "parameter_tag" );
  if ( tag.isEmpty() )
    tag = QString( "parameter" );
  return tag;
}

/*!
  \brief Get import tag name
  \return XML import tag name
*/
QString QtxResourceMgr::XmlFormat::importTag() const
{
  QString tag = option( "import_tag" );
  if ( tag.isEmpty() )
   tag = QString( "import" );
  return tag;
}

/*!
  \brief Get parameter tag's "name" attribute name
  \return XML parameter tag's "name" attribute name
*/
QString QtxResourceMgr::XmlFormat::nameAttribute() const
{
  QString str = option( "name_attribute" );
  if ( str.isEmpty() )
    str = QString( "name" );
  return str;
}

/*!
  \brief Get parameter tag's "value" attribute name
  \return XML parameter tag's "value" attribute name
*/
QString QtxResourceMgr::XmlFormat::valueAttribute() const
{
  QString str = option( "value_attribute" );
  if ( str.isEmpty() )
    str = QString( "value" );
  return str;
}

/*!
  \class QtxResourceMgr::Format
  \brief Generic resources files reader/writer class.
*/

/*!
  \brief Constructor.
  \param fmt format name (for example, "xml" or "ini")
*/
QtxResourceMgr::Format::Format( const QString& fmt )
: myFmt( fmt )
{
}

/*!
  \brief Destructor
*/
QtxResourceMgr::Format::~Format()
{
}

/*!
  \brief Get the format name.
  \return format name
*/
QString QtxResourceMgr::Format::format() const
{
  return myFmt;
}

/*!
  \brief Get options names.
  \return list of the format options
*/
QStringList QtxResourceMgr::Format::options() const
{
  return myOpt.keys();
}

/*!
  \brief Get the value of the option with specified name.

  If option doesn't exist then null QString is returned.
         
  \param opt option name
  \return option value
*/
QString QtxResourceMgr::Format::option( const QString& opt ) const
{
  QString val;
  if ( myOpt.contains( opt ) )
    val = myOpt[opt];
  return val;
}

/*!
  \brief Set the value of the option with specified name.
  \param opt option name
  \param val option value
*/
void QtxResourceMgr::Format::setOption( const QString& opt, const QString& val )
{
  myOpt.insert( opt, val );
}

/*!
  \brief Load resources from the resource file.
  \param res resources object
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::Format::load( Resources* res )
{
  if ( !res )
    return false;

  QMap<QString, Section> sections;
  bool status = load( res->myFileName, sections );
  if ( status )
    res->mySections = sections;
  else
    qDebug() << "QtxResourceMgr: Can't load resource file:" << res->myFileName;

  return status;
}

/*!
  \brief Save resources to the resource file.
  \param res resources object
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::Format::save( Resources* res )
{
  if ( !res )
    return false;

  Qtx::mkDir( Qtx::dir( res->myFileName ) );

  QtxResourceMgr* mgr = res->resMgr();
  QString name = mgr ? mgr->userFileName( mgr->appName(), false ) : res->myFileName;
  return save( name, res->mySections );
}

/*!
  \fn virtual bool QtxResourceMgr::Format::load( const QString& fname,
                                                 QMap<QString, Section>& secMap )
  \brief Load resources from the specified resources file.

  Should be implemented in the successors.

  \param fname resources file name
  \param secMap resources map to be filled in
  \return \c true on success and \c false on error
*/

/*!
 \fn virtual bool QtxResourceMgr::Format::save( const QString& fname, 
                                                const QMap<QString, Section>& secMap )

  \brief Save resources to the specified resources file.

  Should be implemented in the successors.

  \param fname resources file name
  \param secMap resources map
  \return \c true on success and \c false on error
*/

/*!
  \class QtxResourceMgr
  \brief Application resources manager.

  This class can be used to define settings, save/load settings and 
  application preferences to the resource file(s), load translation files
  (internationalization mechanism), load pixmaps and other resources from
  external files, etc.

  Currently it supports .ini and .xml resources file formats. To implement
  own resources file format, inherit from the Format class and implement virtual
  Format::load() and Format::save() methods.

  Resources manager is initialized by the (symbolic) name of the application.
  The parameter \a resVarTemplate specifies the template for the environment
  variable which should point to the resource directory or list of directories.
  Environment variable  name is calculated by substitution of "%1" substring in
  the \a resVarTemplate parameter (if it contains such substring) by the 
  application name (\a appName).
  By default, \a resVarTemplate is set to "%1Resources". For example, if the application name
  is "MyApp", the environment variable "MyAppResources" will be inspected in this case.
  
  Resource manager can handle several global application configuration files and
  one user configuration file. Location of global configuration files is defined
  by the environment variable (see above) and these files are always read-only.
  The name of the global configuration files is retrieved by calling virtual method
  globalFileName() which can be redefined in the QtxResourceMgr class successors.
  User configuration file always situated in the user's home directory. It's name
  is defined by calling virtual method userFileName() which can be also redefined
  in the QtxResourceMgr class successors. This is the only file which the preferences
  changed by the user during the application session are written to (usually 
  when the application closes).

  Resources environment variable should contain one or several resource directories
  (separated by ";" symbol on Windows and ":" or ";" on Linux). Each resource directory 
  can contain application global configuration file. The user configuration file has
  the highest priority, for the global configuration files the priority is decreasing from
  left to right, i.e. the first directory in the directoris list, defined by the 
  resources environment variable has higher priority. Priority has the meaning when
  searching requested resources (application preference, pixmap file name, translation
  file, etc).

  When retrieving preferences, it is sometimes helpful to ignore values coming from the
  user preference file and take into account only global preferences.
  To do this, use setWorkingMode() method passing QtxResourceMgr::IgnoreUserValues enumerator
  as parameter.

  Resources manager operates with such terms like options, sections and parameters. 
  Parametets are named application resources, for example, application preferences like
  integer, double, boolean or string values, pictures, font and color definitions, etc.
  Parameters are organized inside the resources files into the named groups - sections.
  Options are special kind of resoures which allow customizing resource files interpreting.
  For example, by default language settings are defined in the resource file in the
  section "language". It is possible to change this section name by setting "language" 
  option to another value (see setOption()).
  
  Retrieving preferences values can be done by using one of value() methods, each returns
  \c true if the corresponding preference is found. Another way is to use integerValue(),
  doubleValue(), etc methods, which allow specifying default value which is used if the
  specified preference is not found. Removing of preferences or sections can be done using
  remove(const QString& sect) or remove(const QString& sect, const QString& name) methods.
  To add the preference or to change exiting preference value use setValue() methods family.
  Methods hasSection() and hasValue() can be used to check existence of section or
  preference (in the specified section). List of all sections can be retrieved with the
  sections() method, and list of all settings names in some specified section can be 
  obtained with parameters() method.

  Pixmaps can be loaded with the loadPixmap() methods. If the specified pixmap is not found,
  the default one is returned. Default pixmap can be set by setDefaultPixmap().

  One of the key feature of the resources manager is support of application 
  internationalization mechanism. Translation files for the specified language can be loaded
  with loadLanguage() method.
*/

/*!
  \brief Constructs the resource manager.
  \param appName application name
  \param resVarTemplate resource environment variable pattern
*/
QtxResourceMgr::QtxResourceMgr( const QString& appName, const QString& resVarTemplate )
: myAppName( appName ),
  myCheckExist( true ),
  myDefaultPix( 0 ),
  myIsPixmapCached( true ),
  myHasUserValues( true ),
  myWorkingMode( AllowUserValues )
{
  QString envVar = !resVarTemplate.isEmpty() ? resVarTemplate : QString( "%1Resources" );
  if ( envVar.contains( "%1" ) )
    envVar = envVar.arg( appName );

  QString dirs;
  if ( ::getenv( envVar.toLatin1() ) )
    dirs = ::getenv( envVar.toLatin1() );
#ifdef WIN32
  QString dirsep = ";";      // for Windows: ";" is used as directories separator
#else
  QString dirsep = "[:|;]";  // for Linux: both ":" and ";" can be used
#endif
  setDirList( dirs.split( QRegExp( dirsep ), QString::SkipEmptyParts ) );

  installFormat( new XmlFormat() );
  installFormat( new IniFormat() );

  setOption( "translators", QString( "%P_msg_%L.qm|%P_images.qm" ) );
}

/*!
  \brief Destructor.
  
  Destroy the resource manager and free allocated memory.
*/
QtxResourceMgr::~QtxResourceMgr()
{
  QStringList prefList = myTranslator.keys();
  for ( QStringList::ConstIterator it = prefList.begin(); it != prefList.end(); ++it )
    removeTranslators( *it );

  qDeleteAll( myResources );
  qDeleteAll( myFormats );

  delete myDefaultPix;
}

/*!
  \brief Get the application name.
  \return application name
*/
QString QtxResourceMgr::appName() const
{
  return myAppName;
}

/*!
  \brief Get the "check existance" flag

  If this flag is \c true then preference can be set (with setValue() method) 
  only if it doesn't exist or if the value is changed.

  \return \c true if "check existance" flag is set
*/
bool QtxResourceMgr::checkExisting() const
{
  return myCheckExist;
}

/*!
  \brief Set the "check existance" flag.
  \param on new flag value
*/
void QtxResourceMgr::setCheckExisting( const bool on )
{
  myCheckExist = on;
}

/*!
  \brief Get the resource directories list.

  Home user directory (where the user application configuration file is situated)
  is not included. This is that directories list defined by the application
  resources environment variable.

  \return list of directories names
*/
QStringList QtxResourceMgr::dirList() const
{
  return myDirList;
}

/*!
  \brief Initialise resources manager.

  Prepare the resources containers and load resources (if \a autoLoad is \c true).

  \param autoLoad if \c true (default) then all resources are loaded
*/
void QtxResourceMgr::initialize( const bool autoLoad ) const
{
  if ( !myResources.isEmpty() )
    return;

  QtxResourceMgr* that = (QtxResourceMgr*)this;

  if ( !userFileName( appName() ).isEmpty() )
    that->myResources.append( new Resources( that, userFileName( appName() ) ) );

  that->myHasUserValues = myResources.count() > 0;

  for ( QStringList::ConstIterator it = myDirList.begin(); it != myDirList.end(); ++it )
  {
    QString path = Qtx::addSlash( *it ) + globalFileName( appName() );
    that->myResources.append( new Resources( that, path ) );
  }

  if ( autoLoad )
    that->load();
}

/*!
  \brief Get "cached pixmaps" option value.

  Resources manager allows possibility to cache loaded pixmaps that allow to
  improve application performance. This feature is turned on by default - all 
  loaded pixmaps are stored in the internal map. Switching of this feature on/off
  can be done by setIsPixmapCached() method.

  \return \c true if pixmap cache is turned on
  \sa setIsPixmapCached()
*/
bool QtxResourceMgr::isPixmapCached() const
{
  return myIsPixmapCached;
}

/*!
  \brief Switch "cached pixmaps" option on/off.
  \param on enable pixmap cache if \c true and disable it if \c false
  \sa isPixmapCached()
*/
void QtxResourceMgr::setIsPixmapCached( const bool on )
{
  myIsPixmapCached = on;
}

/*!
  \brief Remove all resources from the resources manager.
*/
void QtxResourceMgr::clear()
{
  for ( ResList::Iterator it = myResources.begin(); it != myResources.end(); ++it )
    (*it)->clear();
}

/*!
  \brief Get current working mode.
  
  \return current working mode
  \sa setWorkingMode(), value(), hasValue(), hasSection(), setValue()
*/
QtxResourceMgr::WorkingMode QtxResourceMgr::workingMode() const
{
  return myWorkingMode;
}

/*!
  \brief Set resource manager's working mode.

  The resource manager can operate in the following working modes:
  * AllowUserValues  : methods values(), hasValue(), hasSection() take into account user values (default)
  * IgnoreUserValues : methods values(), hasValue(), hasSection() do not take into account user values

  Note, that setValue() method always put the value to the user settings file.
  
  \param mode new working mode
  \sa workingMode(), value(), hasValue(), hasSection(), setValue()
*/
void QtxResourceMgr::setWorkingMode( WorkingMode mode )
{
  myWorkingMode = mode;
}

/*!
  \brief Get interger parameter value.
  \param sect section name
  \param name parameter name
  \param iVal parameter to return resulting integer value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a iVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, int& iVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  bool ok;
  iVal = val.toInt( &ok );

  return ok;
}

/*!
  \brief Get double parameter value.
  \param sect section name
  \param name parameter name
  \param dVal parameter to return resulting double value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a dVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, double& dVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  bool ok;
  dVal = val.toDouble( &ok );

  return ok;
}

/*!
  \brief Get boolean parameter value.
  \param sect section name
  \param name parameter name
  \param bVal parameter to return resulting boolean value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a bVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, bool& bVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  static QMap<QString, bool> boolMap;
  if ( boolMap.isEmpty() )
  {
    boolMap["true"]  = boolMap["yes"] = boolMap["on"]  = true;
    boolMap["false"] = boolMap["no"]  = boolMap["off"] = false;
  }

  val = val.toLower();
  bool res = boolMap.contains( val );
  if ( res )
    bVal = boolMap[val];
  else
  {
    double num = val.toDouble( &res );
    if ( res )
      bVal = num != 0;
  }

  return res;
}

/*!
  \brief Get color parameter value.
  \param sect section name
  \param name parameter name
  \param cVal parameter to return resulting color value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a cVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QColor& cVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  return Qtx::stringToColor( val, cVal );
}

/*!
  \brief Get font parameter value.
  \param sect section name
  \param name parameter name
  \param fVal parameter to return resulting font value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a fVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QFont& fVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  QStringList fontDescr = val.split( ",", QString::SkipEmptyParts );

  if ( fontDescr.count() < 2 )
    return false;

  QString family = fontDescr[0];
  if ( family.isEmpty() )
    return false;

  fVal = QFont( family );

  for ( int i = 1; i < (int)fontDescr.count(); i++ )
  {
    QString curval = fontDescr[i].trimmed().toLower();
    if ( curval == QString( "bold" ) )
      fVal.setBold( true );
    else if ( curval == QString( "italic" ) )
      fVal.setItalic( true );
    else if ( curval == QString( "underline" ) )
      fVal.setUnderline( true );
    else if ( curval == QString( "shadow" ) || curval == QString( "overline" ) )
      fVal.setOverline( true );
    else
    {
      bool isOk = false;
      int ps = curval.toInt( &isOk );
      if ( isOk )
        fVal.setPointSize( ps );
    }
  }

  return true;
}

/*!
  \brief Get byte array parameter value.
  \param sect section name
  \param name parameter name
  \param baVal parameter to return resulting byte array value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a baVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QByteArray& baVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  baVal.clear();
  QStringList lst = val.split( QRegExp( "[\\s|,]" ), QString::SkipEmptyParts );
  for ( QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it )
  {
    int base = 10;
    QString str = *it;
    if ( str.startsWith( "#" ) )
    {
      base = 16;
      str = str.mid( 1 );
    }
    bool ok = false;
    int num = str.toInt( &ok, base );
    if ( !ok || num < 0 || num > 255 )
      continue;

    baVal.append( (char)num );
  }
  return !baVal.isEmpty();
}

/*!
  \brief Get linear gradient parameter value.
  \param sect section name
  \param name parameter name
  \param gVal parameter to return resulting linear gradient value value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a gVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QLinearGradient& gVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  return Qtx::stringToLinearGradient( val, gVal );
}

/*!
  \brief Get radial gradient parameter value.
  \param sect section name
  \param name parameter name
  \param gVal parameter to return resulting radial gradient value value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a gVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QRadialGradient& gVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  return Qtx::stringToRadialGradient( val, gVal );
}

/*!
  \brief Get conical gradient parameter value.
  \param sect section name
  \param name parameter name
  \param gVal parameter to return resulting conical gradient value value
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a gVal value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QConicalGradient& gVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  return Qtx::stringToConicalGradient( val, gVal );
}

/*!
  \brief Get string parameter value (native format).
  \param sect section name
  \param name parameter name
  \param val parameter to return resulting byte array value
  \param subst if \c true perform environment variables substitution
  \return \c true if parameter is found and \c false if parameter is not found
          (in this case \a val value is undefined)
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QString& val, const bool subst ) const
{
  initialize();

  bool ok = false;
 
  ResList::ConstIterator it = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++it;

  for ( ; it != myResources.end() && !ok; ++it )
  {
    ok = (*it)->hasValue( sect, name );
    if ( ok )
      val = (*it)->value( sect, name, subst );
  }

  return ok;
}

/*!
  \brief Get interger parameter value.

  If the specified parameter is not found or can not be converted to the integer value,
  the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
int QtxResourceMgr::integerValue( const QString& sect, const QString& name, const int def ) const
{
  int val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get double parameter value.

  If the specified parameter is not found or can not be converted to the double value,
  the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
double QtxResourceMgr::doubleValue( const QString& sect, const QString& name, const double def ) const
{
  double val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get boolean parameter value.

  If the specified parameter is not found or can not be converted to the boolean value,
  the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
bool QtxResourceMgr::booleanValue( const QString& sect, const QString& name, const bool def ) const
{
  bool val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get font parameter value.

  If the specified parameter is not found or can not be converted to the font value,
  the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
QFont QtxResourceMgr::fontValue( const QString& sect, const QString& name, const QFont& def ) const
{
  QFont font;
  if( !value( sect, name, font ) )
    font = def;
  return font;
}

/*!
  \brief Get color parameter value.

  If the specified parameter is not found or can not be converted to the color value,
  the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
QColor QtxResourceMgr::colorValue( const QString& sect, const QString& name, const QColor& def ) const
{
  QColor val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get string parameter value.

  If the specified parameter is not found, the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
QString QtxResourceMgr::stringValue( const QString& sect, const QString& name, const QString& def ) const
{
  QString val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get byte array parameter value.

  If the specified parameter is not found, the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
QByteArray QtxResourceMgr::byteArrayValue( const QString& sect, const QString& name, const QByteArray& def ) const
{
  QByteArray val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get linear gradient parameter value.

  If the specified parameter is not found, the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
QLinearGradient QtxResourceMgr::linearGradientValue( const QString& sect, const QString& name, const QLinearGradient& def ) const
{
  QLinearGradient val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get radial gradient parameter value.

  If the specified parameter is not found, the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
QRadialGradient QtxResourceMgr::radialGradientValue( const QString& sect, const QString& name, const QRadialGradient& def ) const
{
  QRadialGradient val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Get conical gradient parameter value.

  If the specified parameter is not found, the specified default value is returned instead.

  \param sect section name
  \param name parameter name
  \param def default value
  \return parameter value (or default value if parameter is not found)
*/
QConicalGradient QtxResourceMgr::conicalGradientValue( const QString& sect, const QString& name, const QConicalGradient& def ) const
{
  QConicalGradient val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Check parameter existence.
  \param sect section name
  \param name parameter name
  \return \c true if parameter exists in specified section
*/
bool QtxResourceMgr::hasValue( const QString& sect, const QString& name ) const
{
  initialize();

  bool ok = false;

  ResList::ConstIterator it = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++it;

  for ( ; it != myResources.end() && !ok; ++it )
    ok = (*it)->hasValue( sect, name );

  return ok;
}

/*!
  \brief Check section existence.
  \param sect section name
  \return \c true if section exists
*/
bool QtxResourceMgr::hasSection( const QString& sect ) const
{
  initialize();

  bool ok = false;

  ResList::ConstIterator it = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++it;

  for ( ; it != myResources.end() && !ok; ++it )
    ok = (*it)->hasSection( sect );

  return ok;
}

/*!
  \brief Set integer parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, int val )
{
  int res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, QString::number( val ) );
}

/*!
  \brief Set double parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, double val )
{
  double res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, QString::number( val, 'g', 12 ) );
}

/*!
  \brief Set boolean parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, bool val )
{
  bool res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, QString( val ? "true" : "false" ) );
}

/*!
  \brief Set color parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QColor& val )
{
  QColor res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, Qtx::colorToString( val ) );
}

/*!
  \brief Set font parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QFont& val )
{
  QFont res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  QStringList fontDescr;
  fontDescr.append( val.family() );
  if ( val.bold() )
    fontDescr.append( "Bold" );
  if ( val.italic() )
    fontDescr.append( "Italic" );
  if ( val.underline() )
    fontDescr.append( "Underline" );
  if ( val.overline() )
    fontDescr.append( "Overline" );
  fontDescr.append( QString( "%1" ).arg( val.pointSize() ) );

  setResource( sect, name, fontDescr.join( "," ) );
}

/*!
  \brief Set string parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QString& val )
{
  QString res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, val );
}

/*!
  \brief Set byte array parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QByteArray& val )
{
  QByteArray res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  char buf[8];
  QStringList lst;
  for ( int i = 0; i < val.size();  i++ )
  {
    ::sprintf( buf, "#%02X", (unsigned char)val.at( i ) );
    lst.append( QString( buf ) );
  }
  setResource( sect, name, lst.join( " " ) );
}

/*!
  \brief Set linear gradient parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QLinearGradient& val )
{
  QLinearGradient res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, Qtx::gradientToString( val ) );
}

/*!
  \brief Set radial gradient parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QRadialGradient& val )
{
  QRadialGradient res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, Qtx::gradientToString( val ) );
}

/*!
  \brief Set conical gradient parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QConicalGradient& val )
{
  QConicalGradient res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, Qtx::gradientToString( val ) );
}

/*!
  \brief Remove resources section.
  \param sect section name
*/
void QtxResourceMgr::remove( const QString& sect )
{
  initialize();

  for ( ResList::Iterator it = myResources.begin(); it != myResources.end(); ++it )
    (*it)->removeSection( sect );
}

/*!
  \brief Remove the specified parameter.
  \param sect section name
  \param name parameter name
*/
void QtxResourceMgr::remove( const QString& sect, const QString& name )
{
  initialize();

  for ( ResList::Iterator it = myResources.begin(); it != myResources.end(); ++it )
    (*it)->removeValue( sect, name );
}

/*!
  \brief Get current configuration files format.
  \return configuration files format name
*/
QString QtxResourceMgr::currentFormat() const
{
  QString fmt;
  if ( !myFormats.isEmpty() )
    fmt = myFormats[0]->format();
  return fmt;
}

/*!
  \brief Set current configuration files format.
  \param fmt configuration files format name
*/
void QtxResourceMgr::setCurrentFormat( const QString& fmt )
{
  Format* form = format( fmt );
  if ( !form )
    return;

  myFormats.removeAll( form );
  myFormats.prepend( form );

  if ( myResources.isEmpty() )
    return;

  ResList::Iterator resIt = myResources.begin();
  if ( myResources.count() > myDirList.count() && resIt != myResources.end() )
  {
    (*resIt)->setFile( userFileName( appName() ) );
    ++resIt;
  }

  for ( QStringList::ConstIterator it = myDirList.begin(); it != myDirList.end() && resIt != myResources.end(); ++it, ++resIt )
    (*resIt)->setFile( Qtx::addSlash( *it ) + globalFileName( appName() ) );
}

/*!
  \brief Get configuration files format by specified format name.
  \param fmt configuration files format name
  \return format object or 0 if format is not defined
*/
QtxResourceMgr::Format* QtxResourceMgr::format( const QString& fmt ) const
{
  Format* form = 0;
  for ( FormatList::ConstIterator it = myFormats.begin(); it != myFormats.end() && !form; ++it )
  {
    if ( (*it)->format() == fmt )
      form = *it;
  }

  return form;
}

/*!
  \brief Install configuration files format.
  
  Added format becomes current.

  \param form format object to be installed
*/
void QtxResourceMgr::installFormat( QtxResourceMgr::Format* form )
{
  if ( !myFormats.contains( form ) )
    myFormats.prepend( form );
}

/*!
  \brief Remove configuration files format.
  \param form format object to be uninstalled
*/
void QtxResourceMgr::removeFormat( QtxResourceMgr::Format* form )
{
  myFormats.removeAll( form );
}

/*!
  \brief Get resource format options names.
  \return list of options names
*/
QStringList QtxResourceMgr::options() const
{
  return myOptions.keys();
}

/*!
  \brief Get the string value of the specified resources format option.

  If option does not exist, null QString is returned.

  \param opt option name
  \return option value
  \sa setOption(), options()
*/
QString QtxResourceMgr::option( const QString& opt ) const
{
  QString val;
  if ( myOptions.contains( opt ) )
    val = myOptions[opt];
  return val;
}

/*!
  \brief Set the string value of the specified resources format option.
  \param opt option name
  \param val option value
  \sa option(), options()
*/
void QtxResourceMgr::setOption( const QString& opt, const QString& val )
{
  myOptions.insert( opt, val );
}

/*!
  \brief Load all resources from all resource files (global and user).
  \return \c true on success and \c false on error
  \sa save()
*/
bool QtxResourceMgr::load()
{
  initialize( false );

  Format* fmt = format( currentFormat() );
  if ( !fmt )
    return false;

  bool res = true;
  for ( ResList::Iterator it = myResources.begin(); it != myResources.end(); ++it )
    res = fmt->load( *it ) && res;

  return res;
}

/*!
  \brief Import resources from specified resource file.
  \param fname resources file name
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::import( const QString& fname )
{
  Format* fmt = format( currentFormat() );
  if ( !fmt )
    return false;

  if ( myResources.isEmpty() || !myHasUserValues )
    return false;

  Resources* r = myResources[0];
  if ( !r )
    return false;

  QString old = r->file();
  r->setFile( fname );
  bool res = fmt->load( r );
  r->setFile( old );
  return res;
}

/*!
  \brief Save all resources to the user resource files.
  \return \c true on success and \c false on error
*/
bool QtxResourceMgr::save()
{
  initialize( false );

  Format* fmt = format( currentFormat() );
  if ( !fmt )
    return false;

  if ( myResources.isEmpty() || !myHasUserValues )
    return true;

  return fmt->save( myResources[0] );
}

/*!
  \brief Get all sections names.
  \return list of section names
*/
QStringList QtxResourceMgr::sections() const
{
  initialize();

  QMap<QString, int> map;

  ResList::ConstIterator it = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++it;

  for ( ; it != myResources.end(); ++it )
  {
    QStringList lst = (*it)->sections();
    for ( QStringList::ConstIterator itr = lst.begin(); itr != lst.end(); ++itr )
      map.insert( *itr, 0 );
  }

  return map.keys();
}

/*!
  \brief Get all sections names matching specified regular expression.
  \param re searched regular expression
  \return list of sections names
*/
QStringList QtxResourceMgr::sections(const QRegExp& re) const
{
  return sections().filter( re );
}

/*!
  \brief Get all sections names with the prefix specified by passed
  list of parent sections names. 

  Sub-sections are separated inside the section name by the sections 
  separator token, for example "splash:color:label".

  \param names parent sub-sections names 
  \return list of sections names
*/
QStringList QtxResourceMgr::sections(const QStringList& names) const
{
  QStringList nm = names;
  nm << ".+";
  QRegExp re( QString( "^%1$" ).arg( nm.join( sectionsToken() ) ) );
  return sections( re );
}

/*!
  \brief Get list of sub-sections names for the specified parent section name.

  Sub-sections are separated inside the section name by the sections 
  separator token, for example "splash:color:label".

  \param section parent sub-section name
  \param full if \c true return full names of child sub-sections, if \c false,
         return only top-level sub-sections names
  \return list of sub-sections names
*/
QStringList QtxResourceMgr::subSections(const QString& section, const bool full) const
{
  QStringList names = sections( QStringList() << section );
  QMutableListIterator<QString> it( names );
  while ( it.hasNext() ) {
    QString name = it.next().mid( section.size() + 1 ).trimmed();
    if ( name.isEmpty() ) {
      it.remove();
      continue;
    }
    if ( !full ) name = name.split( sectionsToken() ).first();
    it.setValue( name );
  }
  names.removeDuplicates();
  names.sort();
  return names;
}

/*!
  \brief Get all parameters name in specified section.
  \param sec section name
  \return list of settings names
*/
QStringList QtxResourceMgr::parameters( const QString& sec ) const
{
  initialize();

#if defined(QTX_NO_INDEXED_MAP)
  typedef QMap<QString, int> PMap;
#else
  typedef IMap<QString, int> PMap;
#endif
  PMap pmap;
  
  Resources* ur = !myResources.isEmpty() && workingMode() == IgnoreUserValues ? myResources[0] : 0;
  
  QListIterator<Resources*> it( myResources );
  it.toBack();
  while ( it.hasPrevious() )
  {
    Resources* r = it.previous();
    if ( r == ur ) break;
    QStringList lst = r->parameters( sec );
    for ( QStringList::ConstIterator itr = lst.begin(); itr != lst.end(); ++itr )
#if defined(QTX_NO_INDEXED_MAP)
      if ( !pmap.contains( *itr ) ) pmap.insert( *itr, 0 );
#else
      pmap.insert( *itr, 0, false );
#endif
  }

  return pmap.keys();
}

/*!
  \brief Get all parameters name in specified
  list of sub-sections names. 

  Sub-sections are separated inside the section name by the sections 
  separator token, for example "splash:color:label".

  \param names parent sub-sections names 
  \return list of settings names
*/
QStringList QtxResourceMgr::parameters( const QStringList& names ) const
{
  return parameters( names.join( sectionsToken() ) );
}

/*!
  \brief Get absolute path to the file which name is defined by the parameter.

  The file name is defined by \a name argument, while directory name is retrieved
  from resources parameter \a prefix of section \a sec. Both directory and file name
  can be relative. If the directory is relative, it is calculated from the initial
  resources file name. Directory parameter can contain environment 
  variables, which are substituted automatically.

  \param sec section name
  \param prefix parameter containing directory name
  \param name file name
  \return absolute file path or null QString if file does not exist
*/
QString QtxResourceMgr::path( const QString& sect, const QString& prefix, const QString& name ) const
{
  QString res;

  ResList::ConstIterator it = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++it;

  for ( ; it != myResources.end() && res.isEmpty(); ++it )
    res = (*it)->path( sect, prefix, name );
  return res;
}

/*!
  \brief Get application resources section name.

  By default, application resources section name is "resources" but
  it can be changed by setting the "res_section_name" resources manager option.
  
  \return section corresponding to the resources directories
  \sa option(), setOption()
*/
QString QtxResourceMgr::resSection() const
{
  QString res = option( "res_section_name" );
  if ( res.isEmpty() )
    res = QString( "resources" );
  return res;
}

/*!
  \brief Get application language section name.

  By default, application language section name is "language" but
  it can be changed by setting the "lang_section_name" resources manager option.
  
  \return section corresponding to the application language settings
  \sa option(), setOption()
*/
QString QtxResourceMgr::langSection() const
{
  QString res = option( "lang_section_name" );
  if ( res.isEmpty() )
    res = QString( "language" );
  return res;
}

/*!
  \brief Get sections separator token.

  By default, sections separator token is colon symbol ":" but
  it can be changed by setting the "section_token" resources manager option.
  
  \return string corresponding to the current section separator token
  \sa option(), setOption()
*/
QString QtxResourceMgr::sectionsToken() const
{
  QString res = option( "section_token" );
  if ( res.isEmpty() )
    res = QString( ":" );
  return res;
}

/*!
  \brief Get default pixmap.
  
  Default pixmap is used when requested pixmap resource is not found.

  \return default pixmap
  \sa setDefaultPixmap(), loadPixmap()
*/
QPixmap QtxResourceMgr::defaultPixmap() const
{
  QPixmap res;
  if ( myDefaultPix && !myDefaultPix->isNull() )
    res = *myDefaultPix;
  return res;
}

/*!
  \brief Set default pixmap.
  
  Default pixmap is used when requested pixmap resource is not found.

  \param pix default pixmap
  \sa defaultPixmap(), loadPixmap()
*/
void QtxResourceMgr::setDefaultPixmap( const QPixmap& pix )
{
  delete myDefaultPix;
  if ( pix.isNull() )
    myDefaultPix = 0;
  else
    myDefaultPix = new QPixmap( pix );
}

/*!
  \brief Load pixmap resource.
  \param prefix parameter which refers to the resources directory (directories)
  \param name pixmap file name
  \return pixmap loaded from the file 
  \sa defaultPixmap(), setDefaultPixmap()
*/
QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name ) const
{
  return loadPixmap( prefix, name, true );
}

/*!
  \brief Load pixmap resource.
  \overload
  \param prefix parameter which refers to the resources directory (directories)
  \param name pixmap file name
  \param useDef if \c false, default pixmap is not returned if resource is not found,
         in this case null pixmap is returned instead
  \return pixmap loaded from the file 
  \sa defaultPixmap(), setDefaultPixmap()
*/
QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name, const bool useDef ) const
{
  return loadPixmap( prefix, name, useDef ? defaultPixmap() : QPixmap() );
}

/*!
  \brief Load pixmap resource.
  \overload
  \param prefix parameter which refers to the resources directory (directories)
  \param name pixmap file name
  \param defPix default which should be used if the resource file doesn't exist
  \return pixmap loaded from the file 
  \sa defaultPixmap(), setDefaultPixmap()
*/
QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name, const QPixmap& defPix ) const
{
  initialize();

  QPixmap pix;

  ResList::ConstIterator it = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++it;

  for ( ; it != myResources.end() && pix.isNull(); ++it )
    pix = (*it)->loadPixmap( resSection(), prefix, name );
  if ( pix.isNull() )
    pix = defPix;
  return pix;
}

/*!
  \brief Load pixmap resource.
  \param filename pixmap file name
  \return pixmap loaded from the file 
  \sa defaultPixmap(), setDefaultPixmap()
*/
QPixmap QtxResourceMgr::loadPixmap( const QString& filename ) const
{
  initialize();

  QPixmap pix;

  ResList::ConstIterator it = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++it;

  for ( ; it != myResources.end() && pix.isNull(); ++it )
    pix = (*it)->loadPixmap( filename );
  if ( pix.isNull() )
    pix = defaultPixmap();
  return pix;
}

/*!
  \brief Load translation files according to the specified language.

  Names of the translation files are calculated according to the pattern specified
  by the "translators" option (this option is read from the section "language" of resources files).
  By default, "%P_msg_%L.qm" pattern is used.
  Keywords \%A, \%P, \%L in the pattern are substituted by the application name, prefix and language name
  correspondingly.
  For example, for prefix "SUIT" and language "en", all translation files "SUIT_msg_en.qm" are searched and
  loaded.

  If prefix is empty or null string, all translation files specified in the "resources" section of resources
  files are loaded (actually, the section is retrieved from resSection() method). 
  If language is not specified, it is retrieved from the langSection() method, and if the latest is also empty,
  by default "en" (English) language is used.
  By default, settings from the user preferences file are also loaded (if user resource file is valid, 
  see userFileName()). To avoid loading user settings, pass \c false as first parameter.

  \param pref parameter which defines translation context (for example, package name)
  \param l language name

  \sa resSection(), langSection(), loadTranslators()
*/
void QtxResourceMgr::loadLanguage( const QString& pref, const QString& l )
{
  initialize( true );

  QMap<QChar, QString> substMap;
  substMap.insert( 'A', appName() );

  QString lang = l;
  if ( lang.isEmpty() )
    value( langSection(), "language", lang );

  if ( lang.isEmpty() )
  {
    lang = QString( "en" );
    qWarning() << "QtxResourceMgr: Language not specified. Assumed:" << lang;
  }

  substMap.insert( 'L', lang );

  QString trs;
  if ( value( langSection(), "translators", trs, false ) && !trs.isEmpty() )
  {
    QStringList translators    = option( "translators" ).split( "|", QString::SkipEmptyParts );
    QStringList newTranslators = trs.split( "|", QString::SkipEmptyParts );
    for ( int i = 0; i < (int)newTranslators.count(); i++ )
    {
      if ( translators.indexOf( newTranslators[i] ) < 0 )
        translators += newTranslators[i];
    }
    setOption( "translators", translators.join( "|" ) );
  }

  QStringList trList = option( "translators" ).split( "|", QString::SkipEmptyParts );
  if ( trList.isEmpty() )
  {
    trList.append( "%P_msg_%L.qm" );
    qWarning() << "QtxResourceMgr: Translators not defined. Assumed:" << trList[0];
  }

  QStringList prefixList;
  if ( !pref.isEmpty() )
    prefixList.append( pref );
  else
    prefixList = parameters( resSection() );

  if ( pref.isEmpty() && lang != "en" ) {
    // load Qt resources
    QString qt_translations = QLibraryInfo::location( QLibraryInfo::TranslationsPath );
    QString qt_dir_trpath;
    if ( ::getenv( "QTDIR" ) )
      qt_dir_trpath = QString( ::getenv( "QTDIR" ) );
    if ( !qt_dir_trpath.isEmpty() )
      qt_dir_trpath = QDir( qt_dir_trpath ).absoluteFilePath( "translations" );

    QTranslator* trans = new QtxTranslator( 0 );
    if ( trans->load( QString("qt_%1").arg( lang ), qt_translations ) || trans->load( QString("qt_%1").arg( lang ), qt_dir_trpath ) )
      QApplication::instance()->installTranslator( trans );
  }

  for ( QStringList::ConstIterator iter = prefixList.begin(); iter != prefixList.end(); ++iter )
  {
    QString prefix = *iter;
    substMap.insert( 'P', prefix );

    QStringList trs;
    for ( QStringList::ConstIterator it = trList.begin(); it != trList.end(); ++it )
      trs.append( substMacro( *it, substMap ).trimmed() );

    loadTranslators( prefix, trs );
  }
}

/*!
  \brief Load translation files for the specified translation context.
  \param prefix parameter which defines translation context (for example, package name)
  \param translators list of translation files 
  \sa loadLanguage()
*/
void QtxResourceMgr::loadTranslators( const QString& prefix, const QStringList& translators )
{
  initialize();

  ResList lst;

  ResList::ConstIterator iter = myResources.begin();
  if ( myHasUserValues && workingMode() == IgnoreUserValues )
    ++iter;

  for ( ; iter != myResources.end(); ++iter )
    lst.prepend( *iter );

  QTranslator* trans = 0;
  
  for ( ResList::Iterator it = lst.begin(); it != lst.end(); ++it )
  {
    for ( QStringList::ConstIterator itr = translators.begin(); itr != translators.end(); ++itr )
    {
      trans = (*it)->loadTranslator( resSection(), prefix, *itr );
      if ( trans )
      {
        if ( !myTranslator[prefix].contains( trans ) )
          myTranslator[prefix].append( trans );
        QApplication::instance()->installTranslator( trans );
      }
    }
  }
}

/*!
  \brief Load translation file.
  \param prefix parameter which defines translation context (for example, package name)
  \param name translator file name
  \sa loadLanguage(), loadTranslators()
*/
void QtxResourceMgr::loadTranslator( const QString& prefix, const QString& name )
{
  initialize();

  QTranslator* trans = 0;

  Resources* ur = !myResources.isEmpty() && workingMode() == IgnoreUserValues ? myResources[0] : 0;
  
  QListIterator<Resources*> it( myResources );
  it.toBack();
  while ( it.hasPrevious() )
  {
    Resources* r = it.previous();
    if ( r == ur ) break;

    trans = r->loadTranslator( resSection(), prefix, name );
    if ( trans )
    {
      if ( !myTranslator[prefix].contains( trans ) )
        myTranslator[prefix].append( trans );
      QApplication::instance()->installTranslator( trans );
    }
  }
}

/*!
  \brief Remove all translators corresponding to the specified translation context.
  \param prefix parameter which defines translation context (for example, package name)
*/
void QtxResourceMgr::removeTranslators( const QString& prefix )
{
  if ( !myTranslator.contains( prefix ) )
    return;

  for ( TransList::Iterator it = myTranslator[prefix].begin(); it != myTranslator[prefix].end(); ++it )
  {
    QApplication::instance()->removeTranslator( *it );
    delete *it;
  }

  myTranslator.remove( prefix );
}

/*!
  \brief Move all translators corresponding to the specified translation context 
         to the top of translators stack (increase their priority).
  \param prefix parameter which defines translation context (for example, package name)
*/
void QtxResourceMgr::raiseTranslators( const QString& prefix )
{
  if ( !myTranslator.contains( prefix ) )
    return;

  for ( TransList::Iterator it = myTranslator[prefix].begin(); it != myTranslator[prefix].end(); ++it )
  {
    QApplication::instance()->removeTranslator( *it );
    QApplication::instance()->installTranslator( *it );
  }
}

/*!
  \brief Copy all parameters to the user resources in order to
         saved them lately in the user home folder.
*/
void QtxResourceMgr::refresh()
{
  QStringList sl = sections();
  for ( QStringList::ConstIterator it = sl.begin(); it != sl.end(); ++it )
  {
    QStringList pl = parameters( *it );
    for ( QStringList::ConstIterator itr = pl.begin(); itr != pl.end(); ++itr )
      setResource( *it, *itr, stringValue( *it, *itr ) );
  }
}

/*!
  \brief Set the resource directories (where global confguration files are searched).
  
  This function also clears all currently set resources.

  \param dl directories list
*/
void QtxResourceMgr::setDirList( const QStringList& dl )
{
  myDirList = dl;
  for ( ResList::Iterator it = myResources.begin(); it != myResources.end(); ++it )
    delete *it;

  myResources.clear();
}

/*!
  \brief Set parameter value.
  \param sect section name
  \param name parameter name
  \param val parameter value
*/
void QtxResourceMgr::setResource( const QString& sect, const QString& name, const QString& val )
{
  initialize();

  if ( !myResources.isEmpty() && myHasUserValues )
    myResources.first()->setValue( sect, name, val );
}

/*!
  \brief Get user configuration file name.

  This method can be redefined in the successor class to customize the user configuration file name.
  User configuration file is always situated in the user's home directory. By default .<appName>rc
  file is used on Linux (e.g. .MyApprc) and <appName>.<format> under Windows (e.g. MyApp.xml).

  Parameter \a for_load (not used in default implementation) specifies the usage mode, i.e. if
  user configuration file is opened for reading or writing. This allows customizing a way of application
  resources initializing (for example, if the user configuraion file includes version number and there is
  no file corresponding to this version in the user's home directory, it could be good idea to try 
  the configuration file from the previous versions of the application).
  
  \param appName application name
  \param for_load boolean flag indicating that file is opened for loading or saving (not used in default implementation) 
  \return user configuration file name
  \sa globalFileName()
*/
QString QtxResourceMgr::userFileName( const QString& appName, const bool /*for_load*/ ) const
{
  QString fileName;
  QString pathName = QDir::homePath();

  QString cfgAppName = QApplication::applicationName();
  if ( !cfgAppName.isEmpty() )
    pathName = Qtx::addSlash( Qtx::addSlash( pathName ) + QString( ".config" ) ) + cfgAppName;

#ifdef WIN32
  fileName = QString( "%1.%2" ).arg( appName ).arg( currentFormat() );
#else
  fileName = QString( ".%1rc" ).arg( appName );
#endif

  if ( !fileName.isEmpty() )
    pathName = Qtx::addSlash( pathName ) + fileName;

  return pathName;
}

/*!
  \brief Get global configuration file name.
  
  This method can be redefined in the successor class to customize the global configuration file name.
  Global configuration files are searched in the directories specified by the application resources
  environment variable (e.g. MyAppResources). By default <appName>.<format> file name is used
  (e.g. MyApp.xml).

  \param appName application name
  \return global configuration file name
  \sa userFileName()
*/
QString QtxResourceMgr::globalFileName( const QString& appName ) const
{
  return QString( "%1.%2" ).arg( appName ).arg( currentFormat() );
}

/*!
  \brief Perform substitution of the patterns like \%A, \%B, etc by values from the map.

  Used by loadLanguage().

  \param src sring to be processed
  \param substMap map of values for replacing
  \return processed string
*/
QString QtxResourceMgr::substMacro( const QString& src, const QMap<QChar, QString>& substMap ) const
{
  QString trg = src;

  QRegExp rx( "%[A-Za-z%]" );

  int idx = 0;
  while ( ( idx = rx.indexIn( trg, idx ) ) >= 0 )
  {
    QChar spec = trg.at( idx + 1 );
    QString subst;
    if ( spec == '%' )
      subst = "%";
    else if ( substMap.contains( spec ) )
      subst = substMap[spec];

    if ( !subst.isEmpty() )
    {
      trg.replace( idx, rx.matchedLength(), subst );
      idx += subst.length();
    }
    else
      idx += rx.matchedLength();
  }

  return trg;
}
