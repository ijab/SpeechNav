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

#include "SUIT_OverrideCursor.h"

#include <QApplication>

/*!Constructor. Initialize wait cursor.*/
SUIT_OverrideCursor::SUIT_OverrideCursor()
{
  QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
}

/*!Copy constructor.*/
SUIT_OverrideCursor::SUIT_OverrideCursor( const QCursor& cursor )
{
  QApplication::setOverrideCursor( cursor );
}

/*!Destructor. restoring override cursor.*/
SUIT_OverrideCursor::~SUIT_OverrideCursor()
{
  QApplication::restoreOverrideCursor();
}

/*! Check cursors is empty */
bool SUIT_OverrideCursor::isActive() const
{
  return myCursors.isEmpty();
}

/*!Suspend cursors.*/
void SUIT_OverrideCursor::suspend()
{
  if ( !isActive() )
    return;

  while ( QApplication::overrideCursor() )
  {
    myCursors.prepend( *QApplication::overrideCursor() );
    QApplication::restoreOverrideCursor();
  }
}

/*!Resume cursors.*/
void SUIT_OverrideCursor::resume()
{
  if ( isActive() )
    return;

  for ( QList<QCursor>::const_iterator it = myCursors.begin(); it != myCursors.end(); ++it )
    QApplication::setOverrideCursor( *it );

  myCursors.clear();
}
