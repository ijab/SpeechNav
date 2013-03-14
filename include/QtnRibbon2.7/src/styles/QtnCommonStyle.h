/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced CommonStyle for Qt)
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
#ifndef QTN_COMMONSTYLE_H
#define QTN_COMMONSTYLE_H

#include <QWindowsStyle>
#include <QStyleOption>

#include "QtitanDef.h"
#include "QtnOfficeDefines.h"

namespace Qtitan
{
    class StyleHelper;
    class CommonStylePrivate;
    /* CommonStyle */
    class QTITAN_EXPORT CommonStyle : public QWindowsStyle
    {
      Q_OBJECT
    public:
        enum QPrimitiveElement
        {
            // PE_CustomBase = 0xf0000000 - base
            PE_Workspace = PE_CustomBase+1,
            PE_QtnCustomBase  = 0xff000000,
        };

    public:
      CommonStyle();
      virtual ~CommonStyle();

    public:
        void enableAnimation(bool enable = true);
        bool isAnimationEnabled() const;

        void setIgnoreDialogs(bool ignore = false);
        bool ignoreDialogs() const;

        QColor getThemeColor(const QString& sectionName, const QString& keyName, QColor color = QColor()) const;

    public:
        virtual void polish(QApplication*);
        virtual void polish(QWidget* widget);
        virtual void unpolish(QApplication*);
        virtual void unpolish(QWidget* widget);

    public:
        virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w = 0) const;
        virtual void drawControl(ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w = 0) const;
        virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* w = 0) const;

        virtual QSize sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget = 0) const;
        virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const;
        virtual int pixelMetric(PixelMetric pm, const QStyleOption* option = 0, const QWidget* widget = 0) const;
        virtual int styleHint(StyleHint hint, const QStyleOption* opt = Q_NULL, const QWidget* widget = Q_NULL, QStyleHintReturn *returnData = Q_NULL) const;
        virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget = 0) const;

    public:
        static void splitString(const QString& str, QString& strFirstRow, QString& strSecondRow);

    protected:
        virtual bool drawWorkspace(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawWidget(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        // for QForm
        virtual bool drawFrame(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawFrameTabWidget(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawShapedFrame(const QStyleOption*, QPainter*, const QWidget*) const { return false; }

        // for stausBar
        virtual bool drawPanelStatusBar(const QStyleOption*, QPainter*, const QWidget*) const { return false; }

        // for menuBar
        virtual bool drawMenuBarEmptyArea(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawPanelMenu(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawMenuItem(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawFrameMenu(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawMenuBarItem(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawIndicatorMenuCheckMark(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawIndicatorToolBarHandle(const QStyleOption*, QPainter*, const QWidget*) const { return false; }

        // for toolBar
        virtual bool drawToolBar(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawToolButtonLabel(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawToolButton(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawPanelButtonTool(const QStyleOption*, QPainter*, const QWidget*) const  { return false; } 
        virtual bool drawGroupControlEntry(const QStyleOption*, QPainter*, const QWidget*) const { return false; }

        // for Buttons
        virtual bool drawIndicatorToolBarSeparator(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawPanelButtonCommand(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawIndicatorCheckRadioButton(PrimitiveElement, const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawFrameDefaultButton(const QStyleOption*, QPainter*, const QWidget*) const { return false; }

        virtual bool drawSizeGrip(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawSlider(const QStyleOptionComplex*, QPainter*, const QWidget*) const { return false; }
        // for ScrollBar
        virtual bool drawScrollBar(const QStyleOptionComplex*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawScrollBarLine(ControlElement, const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawScrollBarSlider(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawScrollBarPage(ControlElement, const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        // for editors
        virtual bool drawControlEdit(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawFrameLineEdit(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawComboBox(const QStyleOptionComplex*, QPainter*, const QWidget*) const { return false; }
        // for DockWidget
        virtual bool drawDockWidgetTitle(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawIndicatorDockWidgetResizeHandle(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        // for MDI
        virtual bool drawMdiControls(const QStyleOptionComplex*, QPainter*, const QWidget*) const { return false; }
        // for TitleBar
        virtual bool drawTitleBar(const QStyleOptionComplex*, QPainter*, const QWidget*) const { return false; }
        // for TabBar
        virtual bool drawTabBarTab(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawTabBarTabShape(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawTabBarTabLabel(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        // for SpinBox
        virtual bool drawSpinBox(const QStyleOptionComplex*, QPainter*, const QWidget*) const { return false; }
        // for ProgressBar
        virtual bool drawProgressBarGroove(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        // for ToolBox
        virtual bool drawToolBoxTab(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawToolBoxTabShape(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawToolBoxTabLabel(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        // for ViewItem
        virtual bool drawItemViewItem(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawPanelItemViewItem(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        // 
        virtual bool drawHeader(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawHeaderSection(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawHeaderEmptyArea(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawIndicatorHeaderArrow(const QStyleOption*, QPainter*, const QWidget*) const { return false; }

        // shared
        virtual void drawDropDownGlyph(QPainter*, QPoint, bool, bool, bool, bool) const {}
        virtual bool drawIndicatorArrow(PrimitiveElement, const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual bool drawPanelTipLabel(const QStyleOption*, QPainter*, const QWidget*) const { return false; }
        virtual void drawRectangle(QPainter*, const QRect&, bool, bool, bool, bool, bool, BarType, BarPosition) const {}

    protected: // shared
        virtual QPixmap cached(const QString &img) const;

    protected:
        QRect getSource(QRect rcSrc, int nState = 0, int nCount = 1) const;
        void  clearCache();
        void  drawImagePart(const QPixmap& dcSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, bool bAlpha) const;

        void  drawImage(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
            QRect rcSizingMargins, QColor clrTransparent, bool alphaBlend = false) const;
        void  drawImage(const QPixmap& dcSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
            QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;

        void  drawPixmap(const QPixmap& dcSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, 
            bool alpha, QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;

        void  setDefaultStyle(QStyle* pStyle);
        QStyle* defaultStyle() const;

        StyleHelper& helper() const;

    protected:
        enum TypePaint
        {
            tp_PrimitiveElement,
            tp_ControlElement,
            tp_ComplexControl,
        };
        bool paintAnimation(TypePaint, int nPrim, const QStyleOption*, 
            QPainter*, const QWidget*, int iDurationOver = 40) const;
        bool transitionsEnabled() const;

    protected:
        virtual bool showToolTip(const QPoint& pos, QWidget* w);

    protected:
        virtual bool event(QEvent* event);
        virtual bool eventFilter(QObject *watched, QEvent* event);

    private:
        QTN_DECLARE_PRIVATE(CommonStyle)
        Q_DISABLE_COPY(CommonStyle)
    };

}; //namespace Qtitan

#endif // QTN_COMMONSTYLE_H
