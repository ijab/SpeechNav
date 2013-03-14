/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonGroup for Qt)
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
#ifndef QTN_RIBBONGROUP_H
#define QTN_RIBBONGROUP_H

#include <QtGui/QWidget>
#include <QToolButton>

#include "QtitanDef.h"


class QStyleOptionGroupBox;
class QModelIndex;
class QMenu;

namespace Qtitan
{
    class RibbonGroupPrivate;
    class OfficePopupMenu;
    /* RibbonGroup */
    class QTITAN_EXPORT RibbonGroup : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY(bool isControlsGrouping READ isControlsGrouping WRITE setControlsGrouping)
        Q_PROPERTY(bool isControlsCentering READ isControlsCentering WRITE setControlsCentering)
        Q_PROPERTY(const QString& title READ title WRITE setTitle)
        Q_PROPERTY(bool isOptionButtonVisible READ isOptionButtonVisible WRITE setOptionButtonVisible DESIGNABLE false)

    public:
        RibbonGroup(QWidget* parent, const QString& title);
        virtual ~RibbonGroup();

    public:
        bool isControlsGrouping() const;
        void setControlsGrouping(bool controlsGrouping = true);

        void setControlsCentering(bool controlsCentering = true);
        bool isControlsCentering() const;

        void setTitle(const QString& title);
        const QString& title() const;

        void setOptionButtonVisible(bool visible = true);
        bool isOptionButtonVisible() const;

        QAction* getOptionButtonAction() const;

    public:
        QAction* addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, 
            QMenu* menu = Q_NULL, QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
        QAction* addAction(QAction* action, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL, 
            QToolButton::ToolButtonPopupMode mode = QToolButton::MenuButtonPopup);
        QAction* addWidget(QWidget* widget);
        QAction* addWidget(const QIcon& icon, const QString& text, QWidget* widget);
        QAction* addWidget(const QIcon& icon, const QString& text, bool align, QWidget* widget);
        QMenu* addMenu(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style = Qt::ToolButtonFollowStyle);
        QAction* addSeparator();

        void remove(QWidget* widget);
        void clear();

    protected:
        QWidget* getNextWidget(const QWidget* w) const;
        QWidget* getWidget(int index) const;
        int getIndexWidget(const QWidget* w) const;
        int getNextIndex(const QWidget* w) const;

    public:
        virtual QSize sizeHint() const;
        using QWidget::addAction;

    Q_SIGNALS:
        void released();
        void actionTriggered(QAction*);

    protected slots:
        void currentIndexChanged(int index);

    protected:
        void initStyleOption(QStyleOptionGroupBox& opt) const;
        bool extendSize(int widthAvail);
        void resetLayout();

        void preparationReduced(bool reduced);
        bool isMinimizedGroup() const;

    protected:
        virtual bool event(QEvent* event);
        virtual void paintEvent(QPaintEvent* event);
        virtual void actionEvent(QActionEvent* event);
        virtual void changeEvent(QEvent* event);
        virtual void enterEvent(QEvent* event);
        virtual void leaveEvent(QEvent* event);
        virtual void resizeEvent(QResizeEvent* event);
        virtual void mouseReleaseEvent(QMouseEvent* event);

    private:
        friend class RibbonStyle;
        friend class GroupLayout;
        friend class RibbonGroupWrapper;
        QTN_DECLARE_PRIVATE(RibbonGroup)
        Q_DISABLE_COPY(RibbonGroup)
    };

}; //namespace Qtitan

#endif // QTN_RIBBONGROUP_H
