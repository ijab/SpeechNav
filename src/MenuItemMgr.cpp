//MenuItemMgr.cpp

#include "MenuItemMgr.h"

#include "TSMainWindow.h"
#include "SUIT_MessageBox.h"

#include <parser.h>

#include <QFileInfo>
#include <QFile>
#include <QWidgetAction>
#include <QVariant>


using namespace QJson;


MenuItemMgr* MenuItemMgr::sInstance = NULL ;

MenuItemMgr* MenuItemMgr::instance()
{
	if( sInstance == NULL )
	{
		sInstance = new MenuItemMgr() ;
	}

	return sInstance ;
}

//////////////////////////////////////////////////////////////////////////
MenuItemMgr::MenuItemMgr( QObject* parent)
: QtxActionMgr( parent ) ,
  myMainWindow(0)
{
	myMenuItems = new QWidgetAction( new QObject ) ;
}

MenuItemMgr::~MenuItemMgr()
{
	if(myMenuItems)
		delete myMenuItems ;
}


bool MenuItemMgr::loadMenuItems(const QList<QVariant>& _menus)
{
	QVariantList::const_iterator it ;
	for( it = _menus.begin() ; it != _menus.end() ; it++ )
		setMenuItems( (*it).toMap() , myMenuItems ) ;

	return true;
}

bool MenuItemMgr::loadMenuItems( const QString& menuFile ) 
{
	Q_ASSERT( myMainWindow!= NULL ) ;

	QString appName = myMainWindow->resourceMgr()->appName() ;
	QString errorMsg ;

	QFileInfo fi (menuFile ) ;
	if( !fi.exists() )
	{
		errorMsg = QString ( "File(%1) is not exist." ).arg(menuFile);
		SUIT_MessageBox::critical( NULL , appName , errorMsg ) ;
		return false;
	}

	QByteArray data ;
    QFile file( menuFile );
	file.open( QIODevice::ReadOnly);
    data = file.readAll();
    file.close();

	Parser parser;
    bool ok;

	QVariant result = parser.parse(data, &ok);
	QVariantList resultList = result.toList();

    if (ok)
    {
		loadMenuItems(resultList);
	}
	else
	{
		errorMsg = QString ( "File(%1) json parse error." ).arg(menuFile);
		SUIT_MessageBox::critical( NULL , appName , errorMsg ) ;
		return false ;
	}

	return true ;
}

bool MenuItemMgr::updateRibbonMenu( Qtitan::RibbonBar* ribonBar)
{
	QObjectList::iterator it , git ;
	Q_ASSERT( myMainWindow!= NULL ) ;
	Q_ASSERT( myMenuItems!= NULL ) ;

	QObjectList pages = myMenuItems->children() ;

	for( it = pages.begin() ; it != pages.end() ; it++ )
	{
		QWidgetAction *wa = (QWidgetAction*)*it ; 
		if( wa != NULL )
		{
			Qtitan::RibbonPage* page = ribonBar->addPage( wa->text() );
			if( page != NULL )
			{
				insertMenu( page , wa ) ;
			}
		}// if
	}// for
	

	
	connect( myMainWindow->getActionGroup() , SIGNAL( triggered( QAction* ) ), myMainWindow, SLOT( onMenuAction( QAction*) ) );

	return true ;
}


//----------------------------------------------------------------------------------

void  MenuItemMgr::insertMenu( Qtitan::RibbonPage* rPage , QWidgetAction* item )
{
	QObjectList::iterator git ;
	QObjectList groups = item->children() ;
	for( git = groups.begin() ; git != groups.end() ; git++ )
	{
		QWidgetAction *wa = (QWidgetAction*)*git ; 
		Qtitan::RibbonGroup* group = rPage->addGroup( wa->text() ) ;
		insertMenu( group , wa ) ;
	}
}

void MenuItemMgr::insertMenu( Qtitan::RibbonGroup* rGroup , QWidgetAction* item )
{
	QObjectList::iterator it ;

	QActionGroup* pActionGroup = myMainWindow->getActionGroup() ;
	Q_ASSERT( pActionGroup!= NULL ) ;

	QObjectList menuitems = item->children() ;
	if( menuitems.isEmpty() )
		return  ;

	for( it = menuitems.begin() ; it != menuitems.end() ; it++ )
	{
		QWidgetAction *wa = (QWidgetAction*)*it ; 
		if( wa->children().isEmpty() )
		{
			// 不含有子菜单
			//QAction* action = rGroup->addAction( wa->icon(), 
			//	              wa->text(), Qt::ToolButtonTextUnderIcon);
			rGroup->addAction(wa, Qt::ToolButtonTextUnderIcon);
			registerAction( wa ) ;
			pActionGroup->addAction( wa ) ;
            
		}
		else
		{
			// 含有子菜单
			 QMenu* menuPopup = rGroup->addMenu( wa->icon(), 
				 wa->text() , Qt::ToolButtonTextUnderIcon);

			insertMenu( menuPopup , wa ) ;
		}
	}

}

void MenuItemMgr::insertMenu( QMenu* menu , QWidgetAction* item )
{
	Q_ASSERT( menu!= NULL ) ;

	QAction* action = NULL ;
	QObjectList::iterator it ;

	QActionGroup* pActionGroup = myMainWindow->getActionGroup() ;
	Q_ASSERT( pActionGroup!= NULL ) ;

	QObjectList menuitems = item->children() ;
	for( it = menuitems.begin() ; it != menuitems.end() ; it++ )
	{
		QWidgetAction *wa = (QWidgetAction*)*it ; 
		if( wa->children().isEmpty() )
		{
			//action = menu->addAction( wa->icon() , wa->text() ) ;
			menu->addAction(wa);
			registerAction( wa ) ;
			pActionGroup->addAction( wa ) ;
		}
		else
			insertMenu( menu , wa ) ;
	}
}

void MenuItemMgr::setMenuItems( const QVariantMap& map , QWidgetAction* item)
{
	QWidgetAction* childItem = 0;
	childItem = new QWidgetAction( item );

	QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        QString key = i.key();

		if( !key.compare("children", Qt::CaseInsensitive) ) // 判断是否key是否为children
		{
			QVariantList children = i.value().toList() ;
			QVariantList::iterator it ;
			for( it = children.begin() ; it != children.end() ; it++ )
				setMenuItems( (*it).toMap() , childItem ) ;
		}
		else
		{
			setItemData( childItem, key, i.value( ) );
		}        
    }
    return;
}

void MenuItemMgr::setItemData(  QWidgetAction* item , const QString& key , const QVariant& value )
{
	QString strComp = key.toLower() ;
	if( strComp == "text" )
	{
		item->setText( value.toString() ) ;
		item->setToolTip( value.toString() );
	}
	else if( strComp == "iconcls" )
	{
		if( !value.toString().isEmpty() && !value.toString().isNull() )
		{
			QString iconFile = QString( "/Resources/%1").arg( value.toString() ) ;
			
			QIcon icon(  myMainWindow->resourceMgr()->loadPixmap( iconFile ) );
			item->setIcon( icon ) ;
		}
	}
	else if( strComp == "attributes" )
	{
		item->setData( value );		
	}

	// 保存key
}


int  MenuItemMgr::checkItemType( QWidgetAction* item )
{
	int ret = MenuItemMgr::MENU_ITEM ;

	if( item->parent() == (QObject*)myMenuItems )
		ret = MenuItemMgr::PAGE_ITEM ;
	else if( item->parent()->parent() == (QObject*)myMenuItems )
		ret = MenuItemMgr::GROUP_ITEM ;

	return ret ;
}