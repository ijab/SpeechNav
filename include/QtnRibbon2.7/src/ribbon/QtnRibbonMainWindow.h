/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonMainWindow for Qt)
** 
** Copyright (c) 2009-2012 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_RIBBONMAINWINDOW_H
#define QTN_RIBBONMAINWINDOW_H

#include <QMainWindow>

#include "QtitanDef.h"

namespace Qtitan
{
    class StatusBar;
    class RibbonBar;
    class OfficeFrameHelper;
    class RibbonMainWindowPrivate;

    /* RibbonMainWindow */
    class QTITAN_EXPORT RibbonMainWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        RibbonMainWindow(QWidget* parent = Q_NULL, Qt::WindowFlags flags = 0);
        ~RibbonMainWindow();

    public:
        RibbonBar* ribbonBar() const;
        void setRibbonBar(RibbonBar* ribbonBar);

        void setFrameHelper(OfficeFrameHelper* helper);

    protected:
        virtual void paintEvent(QPaintEvent* event);

    #ifdef Q_OS_WIN
        virtual bool winEvent(MSG* message, long* result);
    #endif // Q_OS_WIN

    private:
        QTN_DECLARE_PRIVATE(RibbonMainWindow)
        Q_DISABLE_COPY(RibbonMainWindow)
    };

}; //namespace Qtitan



#endif // QTN_RIBBONMAINWINDOW_H
