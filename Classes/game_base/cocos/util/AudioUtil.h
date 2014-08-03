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
    
    static constexpr auto SE_OK              = "sound/se/se_maoudamashii_se_pc01.mp3";
    static constexpr auto SE_OK2             = "sound/se/se_maoudamashii_system48.mp3";
    static constexpr auto SE_CANCEL          = "sound/se/se_maoudamashii_system20.mp3";
    static constexpr auto SE_COLLISION       = "sound/se/se_maoudamashii_battle07.mp3";
    static constexpr auto SE_LEVElUP         = "sound/se/se_maoudamashii_jingle05.mp3";
    static constexpr auto SE_ITEM_USE        = "sound/se/se_maoudamashii_onepoint17.mp3";
    static constexpr auto SE_ITEM_DROP       = "sound/se/se_maoudamashii_se_footstep02.mp3";
    static constexpr auto SE_ITEM_EQUIP      = "sound/se/se_maoudamashii_system26.mp3";
    static constexpr auto SE_ITEM_THROW      = "sound/se/se_maoudamashii_retro08.mp3";
    
    static constexpr auto SE_TOUCH_KAIDAN    = "sound/se/se_maoudamashii_system29.mp3";
    static constexpr auto SE_KAIDAN          = "sound/se/se_maoudamashii_se_stairs.mp3";
    
    static constexpr auto SE_ATTACK          = "sound/se/se_maoudamashii_battle03.mp3";
    static constexpr auto SE_DAMAGE          = "sound/se/se_maoudamashii_battle15.mp3";
    static constexpr auto SE_MISS            = "se_maoudamashii_se_paper01.mp3";
    
    static std::string playedBGM;
    
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
        if (playedBGM == bgmKey) {
            return;
        }
        playedBGM = bgmKey;
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(bgmKey.c_str());
    }
}
#endif
