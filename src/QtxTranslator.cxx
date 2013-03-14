// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxTranslator.cxx
// Author:    Alexander SOLOVYOV
//
#include "QtxTranslator.h"

/*!
  \class QtxTranslator
  \brief Extended version of QTranslator

  In addition to the functionality provided by QTranslator class, QtxTranslator
  allows translating resources defined in the global context (i.e. common resources).
  The QtxTranslator can be used to workaround QTranslator's limitation which 
  does not allow to process messages with global context.

  For the current moment global context should be specified in translation
  (*.ts/*.qm) files as "@default" string. For example:
  \verbatim
  <!DOCTYPE TS><TS>
  <context>
    <name>@default</name>
    <message>
        <source>MY_MESSAGE</source>
        <translation>My translated message</translation>
    </message>
  </context>
  </TS>
  \endverbatim
*/

#define GLOBAL_CONTEXT "@default"

/*!
  \brief Constructor.
*/
QtxTranslator::QtxTranslator( QObject* parent )
: QTranslator( parent )
{
}

/*!
  \brief Destructor.
*/
QtxTranslator::~QtxTranslator()
{
}

/*!
  \brief Returns the translation for the key.
  \param context message context
  \param sourceText message source name
  \param comment message comment (optional)
  \return Translated text if found or \a sourceText otherwise
*/
QString QtxTranslator::translate( const char* context, const char* sourceText, const char* comment ) const
{
  QString res = QTranslator::translate( context, sourceText, comment );
  if( res.isNull() )
    res = QTranslator::translate( GLOBAL_CONTEXT, sourceText, comment );
  return res;
}
