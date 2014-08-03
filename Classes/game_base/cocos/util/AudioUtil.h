//
//  AudioUtil.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/08/03.
//
//

#ifndef Cocos2dRogueLike_AudioUtil_h
#define Cocos2dRogueLike_AudioUtil_h

#include "SimpleAudioEngine.h"

namespace AudioUtil {

    static constexpr auto BGM_MYPAGE         = "sound/bgm/game_maoudamashii_5_village04.mp3";
    static constexpr auto BGM_SHOP           = "sound/bgm/game_maoudamashii_5_town14.mp3";
    static constexpr auto BGM_MIXED          = "sound/bgm/game_maoudamashii_5_town08.mp3";
    
    static constexpr auto BGM_DUNGEON_START  = "sound/bgm/game_maoudamashii_6_dangeon03b.mp3";
    static constexpr auto BGM_DUNGEON_NORMAL = "sound/bgm/game_maoudamashii_6_dangeon22.mp3";
    static constexpr auto BGM_DUNGEON_HARD   = "sound/bgm/game_maoudamashii_6_dangeon05b.mp3";
    
    static constexpr auto SE_OK              = "sound/se/se_maoudamashii_system48.mp3";
    static constexpr auto SE_CANCEL          = "sound/se/se_maoudamashii_system20.mp3";
    static constexpr auto SE_COLLISION       = "sound/se/se_maoudamashii_battle07.mp3";
    static constexpr auto SE_LEVElUP         = "sound/se/se_maoudamashii_jingle05.mp3";
    static constexpr auto SE_ITEM_DROP       = "sound/se/se_maoudamashii_se_footstep02.mp3";
    static constexpr auto SE_ITEM_EQUIP      = "sound/se/se_maoudamashii_system26.mp3";
    static constexpr auto SE_TOUCH_KAIDAN    = "sound/se/se_maoudamashii_system29.mp3";
    static constexpr auto SE_KAIDAN          = "sound/se/se_maoudamashii_se_stairs.mp3";
    
    static void pause() {
        CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    }
    
    static void resume() {
        CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    }
    
    static void playSE(const std::string& seKey) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(seKey.c_str());
    }
    static void playBGM(const std::string& bgmKey) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(bgmKey.c_str());
    }
}
#endif
