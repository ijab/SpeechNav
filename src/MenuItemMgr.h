// MenuItemMgr.h
#ifndef __MENUITEMMGR_H__
#define __MENUITEMMGR_H__

#include "QtxActionMgr.h"
#include <QtitanRibbon.h>
#include <QVariantMap>
#include <QVariantList>

class QString;
class QWidgetAction;
class QtxActionMgr;
class TSMainWindow;

class MenuItemMgr : public QtxActionMgr
{
public:
	static MenuItemMgr* instance();

	void setMainWindow( TSMainWindow* mw ){ myMainWindow = mw ; };

	bool loadMenuItems(const QList<QVariant>& _menus);
	bool loadMenuItems( const QString& menuFile ) ;
	
	bool updateRibbonMenu( Qtitan::RibbonBar* ribonBar );

protected:
	MenuItemMgr( QObject* parent = NULL ) ;
	virtual ~MenuItemMgr() ;

	enum{ PAGE_ITEM ,GROUP_ITEM , MENU_ITEM };

private:
	void setMenuItems( const QVariantMap& map , QWidgetAction* item) ;

	void setItemData( QWidgetAction* item ,const QString& key , const QVariant& value ) ;

	void insertMenu( Qtitan::RibbonPage* rPage , QWidgetAction* item );
	void insertMenu( Qtitan::RibbonGroup* rGroup , QWidgetAction* item );
	void insertMenu( QMenu* menu , QWidgetAction* item );

	int  checkItemType( QWidgetAction* item );

private:
	static MenuItemMgr* sInstance ;

	QWidgetAction   *myMenuItems ;

	TSMainWindow	*myMainWindow ;
};

#endif