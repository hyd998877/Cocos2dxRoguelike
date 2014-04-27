//
//  ItemWindow.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#ifndef __Cocos2dRogueLike__ItemWindow__
#define __Cocos2dRogueLike__ItemWindow__

#include <iostream>
#include "ModalLayer.h"
#include "ItemWindowLayer.h"

class ItemWindow {
public:
    static ModalLayer* createItemWindowLayer(cocos2d::Size contentSize, std::list<DropItemSprite::DropItemDto> itemList);
};
#endif /* defined(__Cocos2dRogueLike__ItemWindow__) */
