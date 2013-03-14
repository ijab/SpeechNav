/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonStatusBar for Qt)
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
#ifndef QTN_RIBBONSTATUSBAR_H
#define QTN_RIBBONSTATUSBAR_H

#include <QStatusBar>

#include "QtitanDef.h"


namespace Qtitan
{
    /* RibbonStatusBar */
    class QTITAN_EXPORT RibbonStatusBar : public QStatusBar
    {
        Q_OBJECT
    public:
        RibbonStatusBar(QWidget* parent = Q_NULL);
        virtual ~RibbonStatusBar();

    public:
        int widthPanes() const;
    private:
        Q_DISABLE_COPY(RibbonStatusBar)
    };

}; //namespace Qtitan


#endif // QTN_RIBBONSTATUSBAR_H
