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

#ifndef SUIT_MESSAGEBOX_H
#define SUIT_MESSAGEBOX_H

#include <QMap>
#include <QList>
#include <QMessageBox>

class SUIT_MessageBox : public QMessageBox
{
public:
  // construction/destruction
  SUIT_MessageBox( QWidget* = 0 );
  SUIT_MessageBox( Icon, const QString&, const QString&, StandardButtons buttons = NoButton,
                   QWidget* = 0, Qt::WindowFlags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
  ~SUIT_MessageBox();

  // customize the standard buttons text
  QString               buttonText( StandardButton ) const;
  void                  setButtonText( StandardButton, const QString& );

  // message box with standard buttons
  static StandardButton critical( QWidget* parent, const QString& title, const QString& text,
                                  StandardButtons buttons = Ok, StandardButton defaultButton = NoButton );
  static StandardButton warning( QWidget* parent, const QString& title, const QString& text,
                                 StandardButtons buttons = Ok, StandardButton defaultButton = NoButton );
  static StandardButton information( QWidget* parent, const QString& title, const QString& text,
                                     StandardButtons buttons = Ok, StandardButton defaultButton = NoButton );
  static StandardButton question( QWidget* parent, const QString& title, const QString& text,
                                  StandardButtons buttons = Ok, StandardButton defaultButton = NoButton );
  
  // message boxes with one custom button
  static int            critical( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button );
  static int            warning( QWidget* parent, const QString& title, const QString& text,
                                 const QString& button );
  static int            information( QWidget* parent, const QString& title, const QString& text,
                                     const QString& button );
  static int            question( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button );

  // message boxes with two custom buttons
  static int            critical( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2, 
                                  const int defaultButton = -1, const int escapeButton = -1 );
  static int            warning( QWidget* parent, const QString& title, const QString& text,
                                 const QString& button1, const QString& button2, 
                                 const int defaultButton = -1, const int escapeButton = -1 );
  static int            information( QWidget* parent, const QString& title, const QString& text,
                                     const QString& button1, const QString& button2, 
                                     const int defaultButton = -1, const int escapeButton = -1 );
  static int            question( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2, 
                                  const int defaultButton = -1, const int escapeButton = -1 );
  
  // message boxes with three custom buttons
  static int            critical( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2, const QString& button3,
                                  const int defaultButton = -1, const int escapeButton = -1 );
  static int            warning( QWidget* parent, const QString& title, const QString& text,
                                 const QString& button1, const QString& button2, const QString& button3,
                                 const int defaultButton = -1, const int escapeButton = -1 );
  static int            information( QWidget* parent, const QString& title, const QString& text,
                                     const QString& button1, const QString& button2, const QString& button3,
                                     const int defaultButton = -1, const int escapeButton = -1 );
  static int            question( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2, const QString& button3,
                                  const int defaultButton = -1, const int escapeButton = -1 );
  
  // message boxes with four custom buttons
  static int            critical( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2,
                                  const QString& button3, const QString& button4,
                                  const int defaultButton = -1, const int escapeButton = -1 );
  static int            warning( QWidget* parent, const QString& title, const QString& text,
                                 const QString& button1, const QString& button2,
                                 const QString& button3, const QString& button4,
                                 const int defaultButton = -1, const int escapeButton = -1 );
  static int            information( QWidget* parent, const QString& title, const QString& text,
                                     const QString& button1, const QString& button2,
                                     const QString& button3, const QString& button4,
                                     const int defaultButton = -1, const int escapeButton = -1 );
  static int            question( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2,
                                  const QString& button3, const QString& button4,
                                  const int defaultButton = -1, const int escapeButton = -1 );
  
  // message boxes with arbitrary number of buttons
  static int            critical( QWidget* parent, const QString& title, const QString& text, 
                                  const int defaultButton, const int escapeButton, 
                                  char*, ... );
  static int            warning( QWidget* parent, const QString& title, const QString& text,
                                 const int defaultButton, const int escapeButton, 
                                 char*, ... );
  static int            information( QWidget* parent, const QString& title, const QString& text,
                                     const int defaultButton, const int escapeButton, 
                                     char*, ... );
  static int            question( QWidget* parent, const QString& title, const QString& text,
                                  const int defaultButton, const int escapeButton, 
                                  char*, ... );

private:
  class ButtonInfo
  {
  public:
    ButtonInfo( const int id, 
                const QString& text, 
                const ButtonRole role = ActionRole )
      : myId( id ), myText( text ), myRole( role ) {}
    int        id()   const { return myId;   }
    QString    text() const { return myText; }
    ButtonRole role() const { return myRole; }
  private:
    int        myId;      //!< button id
    QString    myText;    //!< button text
    ButtonRole myRole;    //!< button role
  };

  typedef QList<ButtonInfo> ButtonInfos;

private:
  static int          messageBox( SUIT_MessageBox::Icon icon, QWidget* parent, 
                                  const QString& title, const QString& text, 
                                  const ButtonInfos& lst, 
                                  const int defaultButton = -1, 
                                  const int escapeButton  = -1 );
  static ButtonInfos  messageList( char*, va_list& );
};

#endif
