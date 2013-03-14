/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced MainWindow for Qt)
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
#ifndef QTN_RIBBONTOOLTIP_H
#define QTN_RIBBONTOOLTIP_H

#include <QLabel>
#include <QTimer>
#include <QIcon>
#include <qevent.h>

#include "QtitanDef.h"

namespace Qtitan
{
    class RibbonKeyTip;
    /* KeyTipEvent */
    class KeyTipEvent : public QEvent
    {
    public:
        //! Constructor for the event.
        KeyTipEvent(RibbonKeyTip* kTip);

    public:
        RibbonKeyTip* getKeyTip() const;

    public:
        // Returns the event type/number for KeyTipEvent.
        // The type is registered on first access. Use this to detect incoming
        static QEvent::Type eventNumber();
    protected:
        RibbonKeyTip* keyTip;
    };

    /* ShowKeyTipEvent */
    class ShowKeyTipEvent : public QEvent
    {
    public:
        //! Constructor for the event.
        ShowKeyTipEvent(QWidget* w);

    public:
        QWidget* getWidget() const;

    public:
        // Returns the event type/number for ShowKeyTipEvent.
        // The type is registered on first access. Use this to detect incoming
        static QEvent::Type eventNumber();
    protected:
        QWidget* widget;
    };

    /* HideKeyTipEvent */
    class HideKeyTipEvent : public QEvent
    {
    public:
        //! Constructor for the event.
        HideKeyTipEvent();
    public:
        // Returns the event type/number for ShowKeyTipEvent.
        // The type is registered on first access. Use this to detect incoming
        static QEvent::Type eventNumber();
    };


    class RibbonToolTipPrivate;
    /* RibbonToolTip */
    class QTITAN_EXPORT RibbonToolTip : public QFrame
    {
        Q_OBJECT
        Q_PROPERTY(QString title READ title WRITE setTitle)
        Q_PROPERTY(QString text READ text WRITE setText)
        Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
        Q_PROPERTY(int margin READ margin WRITE setMargin)
    public:
        RibbonToolTip(const QString& textTitle, const QString& text, const QIcon& icon, QWidget* w);
        virtual ~RibbonToolTip();

        static RibbonToolTip* instance();

    public:
        static void showToolTip(const QPoint& pos, const QString& textTitle, const QString& text, const QIcon& icon, QWidget* w = Q_NULL);
        static inline void hideToolTip();

        static bool isVisibleToolTip();
        static QString textToolTip();

    public:
        int margin() const;
        void setMargin(int);

        const QString& title() const;
        const QString& text() const;

        void setIcon(const QIcon &icon);
        const QIcon& icon() const;

    public:
        void reuseTip(const QString& textTitle, const QString& text);
        void hideTip();
        void closeToolTip();
        void setTipRect(QWidget* w, const QRect& r);
        void restartExpireTimer();
        bool tipChanged(const QPoint& pos, const QString& text, const QString& textTitle, QObject* o);
        void placeTip(const QPoint& pos, QWidget* w);

        static int getTipScreen(const QPoint& pos, QWidget* w);

    public Q_SLOTS:
        void setTitle(const QString&);
        void setText(const QString&);

    public:
        virtual QSize sizeHint() const;

    protected:
        virtual bool eventFilter(QObject*, QEvent* event);
        virtual void timerEvent(QTimerEvent* event);
        virtual void paintEvent(QPaintEvent* event);
        virtual void mouseMoveEvent(QMouseEvent* event);
        virtual void resizeEvent(QResizeEvent* event);

    public:
        bool fadingOut() const;

    private:
        QTN_DECLARE_PRIVATE(RibbonToolTip)
        Q_DISABLE_COPY(RibbonToolTip)
    };

    class RibbonKeyTipPrivate;
    /* KeyTip */
    class RibbonKeyTip : public QFrame
    {
        Q_OBJECT
    public:
        enum AlignTip
        {
            AlignTipTop      = 0x00000000,
            AlignTipLeft     = 0x00000000,
            AlignTipCenter   = 0x00000001,
            AlignTipRight    = 0x00000002,
            AlignTipVCenter  = 0x00000004,
            AlignTipBottom   = 0x00000008,
        };

    public:
        RibbonKeyTip(QWidget* bars, QWidget* owner, const QString& caption, 
            const QPoint& pt, uint align, bool enabled, QAction* action = Q_NULL);
        virtual ~RibbonKeyTip();

    public:
        QPoint posTip() const;
        void setVisibleTip(bool visible);
        bool isVisibleTip() const;
        bool isEnabledTip() const;

        QWidget* getBars() const;
        uint getAlign() const;

        void setExplicit(bool exp);
        bool isExplicit() const;

        QString getStringTip() const;
        void setStringTip(const QString& str);

        QString getCaption() const;
        void setCaption(const QString& str);

        QString getPrefix() const;
        void setPrefix(const QString& pref);

        QWidget* getOwner() const;
        QAction* getAction() const;

    protected:
        virtual void paintEvent(QPaintEvent* event);
        virtual void resizeEvent(QResizeEvent* event);

    private:
        QTN_DECLARE_PRIVATE(RibbonKeyTip)
        Q_DISABLE_COPY(RibbonKeyTip)
    };
}; //namespace Qtitan


#endif // QTN_RIBBONTOOLTIP_H
