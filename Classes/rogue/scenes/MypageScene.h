//
//  MypageScene.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/23.
//
//

#ifndef __Cocos2dRogueLike__MypageScene__
#define __Cocos2dRogueLike__MypageScene__

#include "cocos2d.h"
#include "AppMacros.h"

#include "ItemInventoryDto.h"

NS_ROGUE_BEGIN

class MypageScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
    MypageScene();
    virtual ~MypageScene();
    
    virtual bool init();
private:
    void initQuestSave();
    void initMyPage();

    ItemInventoryDto _itemInventory;
    ItemInventoryDto _itemInventoryStock;
    
    CREATE_FUNC(MypageScene);
    
    /** グロナビ生成 */
    void initGlobalMenu();
};

NS_ROGUE_END

#endif /* defined(__Cocos2dRogueLike__MypageScene__) */
