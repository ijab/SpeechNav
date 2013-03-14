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

#include "SUIT_MessageBox.h"

#include "SUIT_OverrideCursor.h"

#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QString>

#include <stdarg.h>

/*!
  \class SUIT_MessageBox
  \brief Message dialog box for SUIT-based application

  The class provides a modal dialog with a short message, an icon, 
  and buttons laid out depending on the current style.
  
  Message boxes are used to provide informative messages and to ask 
  simple questions.

  The easiest way to pop up a message box is to call one of the static
  functions information(), question(), critical(), and warning().

  The class provides the static functions to show message boxes with
  standard buttons (like \c Ok, \c Cancel, \c Apply, \c Close, \c Yes, 
  \c No, \c Abort, \c Retry, etc). These methods accept ORed buttons
  flags as one of the parameters. The buttons layouting type and order
  is system-dependant and defined by the current style. In addition,
  these methods allow to define default button (where input focus is
  set by default and which is clicked when user presses \c Enter key).

  Another set of static functions allows to show message boxes with 
  up to four user-defined buttons. It is possible to define default 
  and escape buttons using additional parameters.

  And finally, the last group of static functions allow displaying
  the message boxes with an arbitrary number of buttons.

  For example:
  \code
  // show question message box with two standard buttons
  int result = SUIT_MessageBox::question(desktop(), "Error",
                                         "File already exists? Overwrite?",
                                         SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                         SUIT_MessageBox::No );
  if ( result == SUIT_MessageBox::Yes )
    overwriteFileFunction();

  // show critical message box with user-defined buttons
  // default is second button and escape is third button
  int result = SUIT_MessageBox::critical(desktop(), "Hazard!",
                                         "The situation is critical! What to do?",
                                         "Hide", "Run Away", "Stand still", 1, 2);
  switch ( result )
  {
  case 0:
    hideMyself(); break;
  case 1:
    runAwayFromHere(); break;
  case 2:
    sitDownAndPray(); break;
  default:
    break;
  }

  // show message box with 6 buttons
  // both default and escape buttons are set to first button ("Zero")
  int result = SUIT_MessageBox::information(desktop(), "Question",
                                            "Select your favourite number",
                                            0, 0,
                                            "Zero", "One", "Two", "Three", "Four", "Five");
  useMyFavouriteNumberSomewhere( result );
  \endcode
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
SUIT_MessageBox::SUIT_MessageBox( QWidget* parent )
: QMessageBox( parent )
{
}

/*!
  \brief Constructor.
  \param icon message box icon (QMessageBox::Icon)
  \param title message box title
  \param text message box text
  \param buttons ORed message box standard buttons (QMessageBox::StandardButton)
  \param parent parent widget
  \param f window flags
*/
SUIT_MessageBox::SUIT_MessageBox( Icon icon, const QString& title, const QString& text,
                                  StandardButtons buttons, QWidget* parent, Qt::WindowFlags f )
: QMessageBox( icon, title, text, buttons, parent, f )
{
}

/*!
  \brief Destructor.
*/
SUIT_MessageBox::~SUIT_MessageBox()
{
}

/*!
  \brief Get the standard button text.
  \param btn standard button id
  \return button text
*/
QString SUIT_MessageBox::buttonText( StandardButton btn ) const
{
  QString res;
  QAbstractButton* b = button( btn );
  if ( b )
    res = b->text();
  return res;
}

/*!
  \brief Set the standard button text.
  \param btn standard button id
  \param text new button text
*/
void SUIT_MessageBox::setButtonText( StandardButton btn, const QString& text )
{
  QAbstractButton* b = button( btn );
  if ( b )
    b->setText( text );
}

/*!
  \brief Show critical message box with specified standard buttons.
  \param parent parent widget
  \param title message box title
  \param text message box text
  \param buttons ORed message box buttons (QMessageBox::StandardButton)
  \param defaultButton default button (QMessageBox::StandardButton)
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
SUIT_MessageBox::StandardButton SUIT_MessageBox::critical( QWidget* parent, const QString& title, const QString& text,
                                                           StandardButtons buttons, StandardButton defaultButton )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::ArrowCursor );
  return QMessageBox::critical( parent, title, text, buttons, defaultButton );
}

/*!
  \brief Show warning message box with specified standard buttons.
  \param parent parent widget
  \param title message box title
  \param text message box text
  \param buttons ORed message box buttons (QMessageBox::StandardButton)
  \param defaultButton default button (QMessageBox::StandardButton)
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
SUIT_MessageBox::StandardButton SUIT_MessageBox::warning( QWidget* parent, const QString& title, const QString& text,
                                                          SUIT_MessageBox::StandardButtons buttons, StandardButton defaultButton )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::ArrowCursor );
  return QMessageBox::warning( parent, title, text, buttons, defaultButton );
}

/*!
  \brief Show information message box with specified standard buttons.
  \param parent parent widget
  \param title message box title
  \param text message box text
  \param buttons ORed message box buttons (QMessageBox::StandardButton)
  \param defaultButton default button (QMessageBox::StandardButton)
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
SUIT_MessageBox::StandardButton SUIT_MessageBox::information( QWidget* parent, const QString& title, const QString& text,
                                                              StandardButtons buttons, StandardButton defaultButton )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::ArrowCursor );
  return QMessageBox::information( parent, title, text, buttons, defaultButton );
}

/*!
  \brief Show question message box with specified standard buttons.
  \param parent parent widget
  \param title message box title
  \param text message box text
  \param buttons ORed message box buttons (QMessageBox::StandardButton)
  \param defaultButton default button (QMessageBox::StandardButton)
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
SUIT_MessageBox::StandardButton SUIT_MessageBox::question( QWidget* parent, const QString& title, const QString& text,
                                                           StandardButtons buttons, StandardButton defaultButton )
{
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::ArrowCursor );
  return QMessageBox::question( parent, title, text, buttons, defaultButton );
}

/*!
  \brief Show critical message box with one custom button.

  Specified button becomes "default" button and "escape" button, i.e.
  pressing \c Return or \c Enter and \c Escape keys is equivalent to clicking
  this button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button button text
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
int SUIT_MessageBox::critical( QWidget* parent, const QString& title, const QString& text,
                               const QString& button )
{
  ButtonInfos lst;
  lst.append( ButtonInfo( 0, button ) );
  return messageBox( SUIT_MessageBox::Critical, parent, title, text, lst );
}

/*!
  \brief Show warning message box with one custom button.
  
  Specified button becomes "default" button and "escape" button, i.e.
  pressing \c Return or \c Enter and \c Escape keys is equivalent to clicking
  this button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button button text
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
int SUIT_MessageBox::warning( QWidget* parent, const QString& title, const QString& text,
                              const QString& button )
{
  ButtonInfos lst;
  lst.append( ButtonInfo( 0, button ) );
  return messageBox( SUIT_MessageBox::Warning, parent, title, text, lst );
}

/*!
  \brief Show information message box with one custom button.

  Specified button becomes "default" button and "escape" button, i.e.
  pressing \c Return or \c Enter and \c Escape keys is equivalent to clicking
  this button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button button text
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
int SUIT_MessageBox::information( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button )
{
  ButtonInfos lst;
  lst.append( ButtonInfo( 0, button ) );
  return messageBox( SUIT_MessageBox::Information, parent, title, text, lst );
}

/*!
  \brief Show question message box with one custom button.

  \warning This function does not make a lot of sense because it provides
  message box with only one  button, i.e. it is impossible to give several
  answers for the question (at least 'yes'/'no').
  This function is implemented only for completeness.

  Specified button becomes "default" button and "escape" button, i.e.
  pressing \c Return or \c Enter and \c Escape keys is equivalent to clicking
  this button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button button text
  \return button id clicked by the user (QMessageBox::StandardButton)
*/
int SUIT_MessageBox::question( QWidget* parent, const QString& title, const QString& text,
                               const QString& button )
{
  ButtonInfos lst;
  lst.append( ButtonInfo( 0, button ) );
  return messageBox( SUIT_MessageBox::Question, parent, title, text, lst );
}

/*!
  \brief Show critical message box with two custom buttons.

  Parameters \a button1 and \a button2 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the second one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::critical( QWidget* parent, const QString& title, const QString& text,
                               const QString& button1, const QString& button2,
                               const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  return messageBox( SUIT_MessageBox::Critical, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show warning message box with two custom buttons.

  Parameters \a button1 and \a button2 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the second one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::warning( QWidget* parent, const QString& title, const QString& text,
                              const QString& button1, const QString& button2,
                              const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  return messageBox( SUIT_MessageBox::Warning, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show information message box with two custom buttons.

  Parameters \a button1 and \a button2 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the second one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::information( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2,
                                  const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  return messageBox( SUIT_MessageBox::Information, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show question message box with two custom buttons.

  Parameters \a button1 and \a button2 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the second one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::question( QWidget* parent, const QString& title, const QString& text,
                               const QString& button1, const QString& button2,
                               const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  return messageBox( SUIT_MessageBox::Question, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show critical message box with three custom buttons.

  Parameters \a button1, \a button2 and \a button3 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::critical( QWidget* parent, const QString& title, const QString& text,
                               const QString& button1, const QString& button2, 
                               const QString& button3,
                               const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  return messageBox( SUIT_MessageBox::Critical, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show warning message box with three custom buttons.

  Parameters \a button1, \a button2 and \a button3 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::warning( QWidget* parent, const QString& title, const QString& text,
                              const QString& button1, const QString& button2,
                              const QString& button3,
                              const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  return messageBox( SUIT_MessageBox::Warning, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show information message box with three custom buttons.

  Parameters \a button1, \a button2 and \a button3 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::information( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2,
                                  const QString& button3,
                                  const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  return messageBox( SUIT_MessageBox::Information, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show question message box with three custom buttons.

  Parameters \a button1, \a button2 and \a button3 specify the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::question( QWidget* parent, const QString& title, const QString& text,
                               const QString& button1, const QString& button2,
                               const QString& button3,
                               const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  return messageBox( SUIT_MessageBox::Question, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show critical message box with four custom buttons.

  Parameters \a button1, \a button2, \a button3 and \a button4 specify 
  the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param button4 fourth button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::critical( QWidget* parent, const QString& title, const QString& text,
                               const QString& button1, const QString& button2, 
                               const QString& button3, const QString& button4,
                               const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  lst.append( ButtonInfo( id++, button4 ) );
  return messageBox( SUIT_MessageBox::Critical, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show warning message box with four custom buttons.

  Parameters \a button1, \a button2, \a button3 and \a button4 specify 
  the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param button4 fourth button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::warning( QWidget* parent, const QString& title, const QString& text,
                              const QString& button1, const QString& button2, 
                              const QString& button3, const QString& button4,
                              const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  lst.append( ButtonInfo( id++, button4 ) );
  return messageBox( SUIT_MessageBox::Warning, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show information message box with four custom buttons.

  Parameters \a button1, \a button2, \a button3 and \a button4 specify 
  the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param button4 fourth button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::information( QWidget* parent, const QString& title, const QString& text,
                                  const QString& button1, const QString& button2, 
                                  const QString& button3, const QString& button4,
                                  const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  lst.append( ButtonInfo( id++, button4 ) );
  return messageBox( SUIT_MessageBox::Information, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show question message box with four custom buttons.

  Parameters \a button1, \a button2, \a button3 and \a button4 specify 
  the buttons text.
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param button1 first button text
  \param button2 second button text
  \param button3 third button text
  \param button4 fourth button text
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::question( QWidget* parent, const QString& title, const QString& text,
                               const QString& button1, const QString& button2, 
                               const QString& button3, const QString& button4,
                               const int defaultButton, const int escapeButton )
{
  ButtonInfos lst;
  int id = 0;
  lst.append( ButtonInfo( id++, button1 ) );
  lst.append( ButtonInfo( id++, button2 ) );
  lst.append( ButtonInfo( id++, button3 ) );
  lst.append( ButtonInfo( id++, button4 ) );
  return messageBox( SUIT_MessageBox::Question, parent, title, text, lst, 
                     defaultButton, escapeButton );
}

/*!
  \brief Show critical message box with arbitrary number of user-specified
         buttons.

  The function accepts arbitrary number of parameters. Each parameter starting
  from \a btn should be of type const char* to specify the button text.
  After the last button parameter and additional 0 (zero) value should be 
  specified.
  
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param defaultButton default button
  \param escapeButton escape button
  \param btn first button text
  \return button used button id
*/
int SUIT_MessageBox::critical( QWidget* parent, const QString& title, const QString& text, 
                               const int defaultButton, const int escapeButton, 
                               char* btn, ... )
{
  va_list args;
  va_start( args, btn );
  return messageBox( SUIT_MessageBox::Critical, parent, title, text,
                     messageList( btn, args ),
                     defaultButton, escapeButton );
}

/*!
  \brief Show warning message box with arbitrary number of user-specified
         buttons.

  The function accepts arbitrary number of parameters. Each parameter starting
  from \a btn should be of type const char* to specify the button text.
  After the last button parameter and additional 0 (zero) value should be 
  specified.
  
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param defaultButton default button
  \param escapeButton escape button
  \param btn first button text
  \return button used button id
*/
int SUIT_MessageBox::warning( QWidget* parent, const QString& title, const QString& text, 
                              const int defaultButton, const int escapeButton, 
                              char* btn, ... )
{
  va_list args;
  va_start( args, btn );
  return messageBox( SUIT_MessageBox::Warning, parent, title, text,
                     messageList( btn, args ),
                     defaultButton, escapeButton );
}

/*!
  \brief Show information message box with arbitrary number of user-specified
         buttons.

  The function accepts arbitrary number of parameters. Each parameter starting
  from \a btn should be of type const char* to specify the button text.
  After the last button parameter and additional 0 (zero) value should be 
  specified.
  
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param defaultButton default button
  \param escapeButton escape button
  \param btn first button text
  \return button used button id
*/
int SUIT_MessageBox::information( QWidget* parent, const QString& title, const QString& text, 
                                  const int defaultButton, const int escapeButton, 
                                  char* btn, ... )
{
  va_list args;
  va_start( args, btn );
  return messageBox( SUIT_MessageBox::Information, parent, title, text,
                     messageList( btn, args ),
                     defaultButton, escapeButton );
}

/*!
  \brief Show question message box with arbitrary number of user-specified
         buttons.

  The function accepts arbitrary number of parameters. Each parameter starting
  from \a btn should be of type const char* to specify the button text.
  After the last button parameter and additional 0 (zero) value should be 
  specified.
  
  The function returns clicked button id. The identifiers for the buttons
  are assigned automatically. The first button is identified as 0, the
  second one as 1, etc.

  The \a defaultButton parameter allows to specify the button which is assigned
  for the \c Return or \c Enter key. Similarly, \a escapeButton parameter
  allows specifing the button which is assigned for \c Escape key.
  If these parameters are not specified (-1 by default), the first button
  is set as default button and the last one is defined as escape button.

  \param parent parent widget
  \param title message box title
  \param text message box text
  \param defaultButton default button
  \param escapeButton escape button
  \param btn first button text
  \return button used button id
*/
int SUIT_MessageBox::question( QWidget* parent, const QString& title, const QString& text, 
                               const int defaultButton, const int escapeButton, 
                               char* btn, ... )
{
  va_list args;
  va_start( args, btn );
  return messageBox( SUIT_MessageBox::Question, parent, title, text,
                     messageList( btn, args ),
                     defaultButton, escapeButton );
}

/*!
  \brief Parse arbitrary arguments list.

  The last parameter in a sequence should be 0 (zero) value.

  \param txt first argument which starts arbitrary sequence
  \param args arguments list from the stack
  \return list of buttons infos
*/
SUIT_MessageBox::ButtonInfos SUIT_MessageBox::messageList( char* txt, va_list& args )
{
  int i = 0;
  ButtonInfos lst;
  char* cur = txt;
  while ( cur )
  {
    lst.append( ButtonInfo( i++, cur ) );
    cur = va_arg( args, char* );
  }

  va_end( args );

  return lst;
}

/*!
  \brief Create and show the message box.
  \param icon icon type
  \param parent parent widget
  \param title message box title
  \param text message box text
  \param lst list of buttons infos
  \param defaultButton default button
  \param escapeButton escape button
  \return button used button id
*/
int SUIT_MessageBox::messageBox( Icon icon, QWidget* parent, 
                                 const QString& title, const QString& text, 
                                 const ButtonInfos& lst, 
                                 const int defaultButton, 
                                 const int escapeButton )
{
  SUIT_MessageBox msgBox( icon, title, text, NoButton, parent );

  QMap<QAbstractButton*, int> bm;
  for ( int i = 0; i < lst.count(); i++ )
  {
    int btn         = lst[i].id();
    QString txt     = lst[i].text();
    ButtonRole role = lst[i].role();

    QPushButton* pb = msgBox.addButton( txt, role );
    bm.insert( pb, btn );

    if ( ( defaultButton == -1 && i == 0 ) || btn == defaultButton )
      msgBox.setDefaultButton( pb );
    if ( ( escapeButton == -1 && i == lst.count() - 1 ) || btn == escapeButton )
      msgBox.setEscapeButton( pb );
  }
    
  SUIT_OverrideCursor cw( parent ? parent->cursor() : Qt::ArrowCursor );

  int res = msgBox.exec();
  if ( res != -1 )
    res = bm[msgBox.clickedButton()];

  QApplication::processEvents();

  return res;
}

