//
//  MenuItemUtil.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/03.
//
//

#ifndef Cocos2dRogueLike_MenuItemUtil_h
#define Cocos2dRogueLike_MenuItemUtil_h

#include "cocos2d.h"

class MenuItemUtil
{
public:
    static void touchItemRefreshColor(cocos2d::Ref *ref,
                                      const cocos2d::Color3B touchColor,
                                      const cocos2d::Color3B untouchColor = cocos2d::Color3B::WHITE) {
        auto menuItem = dynamic_cast<cocos2d::MenuItem*>(ref);
        if (menuItem->getParent()->getChildrenCount() < 1) {
            return;
        }
        
        // Menuにぶら下がってるMenuItemの色を全部戻してタッチしたやつだけ色つける
        for (auto node : menuItem->getParent()->getChildren()) {
            node->setColor(untouchColor);
        }
        menuItem->setColor(touchColor);
    }
};

#endif
