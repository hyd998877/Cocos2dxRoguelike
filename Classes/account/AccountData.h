//
//  AccountData.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#ifndef __Cocos2dxSRPGQuest__AccountData__
#define __Cocos2dxSRPGQuest__AccountData__

#include "cocos2d.h"

#include "RogueScene.h"
#include "MapManager.h"

#include "ActorSprite.h"
#include "DropItemSprite.h"

NS_ROGUE_BEGIN

class AccountData
{
public:
    // シングルトン
    static AccountData* getInstance();
    
    // --- ローグプレイ情報 ---
    RogueLikeGame::RogueScene::RoguePlayData rogue_play_data_;
    
    // --- プレイヤー情報 ---
    ActorDto player_actor_;
    
    // --- インベントリ ---
    // 所持品、装備状態
    std::list<DropItemSprite::DropItemDto> item_list_;
    
    // --- 倉庫 ---
    // アイテムリスト
    // ゴールド
    // TODO: (kyokomi)別クラスかなぁ
    
    // --- システムデータ ---
    // TODO: (kyokomi)別クラスかなぁ
    
public:
    void load();
    void save();
    void reset();
private:
    AccountData()
    :rogue_play_data_({0, 0, RogueScene::GameStatus::INIT, 0, 0, 0}),
    player_actor_(ActorDto()),
    item_list_(std::list<DropItemSprite::DropItemDto>())
    {
    };
    
    ~AccountData()
    {
        item_list_.clear();
    };
    
    // XXX: deprecated
    template <typename TYPE>
    void setDataForKey(const std::string& key, const TYPE& value)
    {
        cocos2d::Data data;
        data.copy((unsigned char* )&value, sizeof(TYPE));
        UserDefault::getInstance()->setDataForKey(key.c_str(), data);
        data.clear();
    }

    // XXX: deprecated
    template <typename TYPE>
    void setListDataForKey(const std::string& key, const std::string& data_key, const std::list<TYPE>& value_list)
    {
        int count = 0;
        int size = value_list.size();
        if (size > 0)
        {
            for (TYPE value : value_list)
            {
                setDataForKey(StringUtils::format("%s.%s%d", key.c_str(), data_key.c_str(), count), value);
                count++;
            }
        }
        UserDefault::getInstance()->setIntegerForKey(StringUtils::format("%s_count", key.c_str()).c_str(), count);
    }

    // XXX: deprecated
    void resetListDataForKey(const std::string& key, const std::string& data_key)
    {
        int size = UserDefault::getInstance()->getIntegerForKey(StringUtils::format("%s_count", key.c_str()).c_str(), 0);
        for (int i = 0; i < size; i++)
        {
            UserDefault::getInstance()->setStringForKey(StringUtils::format("%s.%s%d", key.c_str(), data_key.c_str(), i).c_str(), "");
        }
        UserDefault::getInstance()->setIntegerForKey(StringUtils::format("%s_count", key.c_str()).c_str(), 0);
    }
    
    // XXX: deprecated
    template <typename TYPE>
    TYPE getDataForKey(const std::string& key)
    {
        CCLOG("getDataForKey key = %s", key.c_str());
        
        cocos2d::Data data = UserDefault::getInstance()->getDataForKey(key.c_str());
        if (!data.isNull())
        {
            auto pValue = static_cast<TYPE*>((void *)data.getBytes());
            TYPE value = *pValue;
            data.clear();
            return value;
        }
        return TYPE();
    }

    // XXX: deprecated
    template <typename TYPE>
    std::list<TYPE> getListDataForKey(const std::string& key, const std::string& data_key)
    {
        std::list<TYPE> list;
        
        int list_count = UserDefault::getInstance()->getIntegerForKey(StringUtils::format("%s_count", key.c_str()).c_str(), 0);
        for (int i = 0; i < list_count; i++)
        {
            list.push_back(getDataForKey<TYPE>(StringUtils::format("%s.%s%d", key.c_str(), data_key.c_str(), i)));
        }
        return list;
    }
    
    void init();
};

NS_ROGUE_END

#endif /* defined(__Cocos2dxSRPGQuest__AccountData__) */
