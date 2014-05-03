//
//  ActorDto.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#ifndef __Cocos2dRogueLike__ActorDto__
#define __Cocos2dRogueLike__ActorDto__

#include <iostream>
#include "ActorEquipDto.h"

class ActorDto {
  
public:
    ActorDto(int playerId = 0,
             int imageResId = 0,
             int faceImgId = 0,
             std::string name = "",
             int attackPoint = 0,
             int defencePoint = 0,
             int lv = 0,
             int exp = 0,
             int nextExp = 0,
             int hitPoint = 0,
             int hitPointLimit = 0,
             int magicPoint = 0,
             int magicPointLimit = 0,
             int movePoint = 0,
             int attackRange = 0,
             ActorEquipDto weaponEquip = ActorEquipDto(),
             ActorEquipDto accessoryEquip = ActorEquipDto(),
             int gold = 0);
    
    static ActorDto createActorDto(const std::string data_string);
    
    std::string actorToString();
    
    bool isEquipWeapon() const {
        if (weaponEquip_.getObjectId() > 0) {
            return true;
        } else {
            return false;
        }
    }
    bool isEquipAccessory() const {
        if (accessoryEquip_.getObjectId() > 0) {
            return true;
        } else {
            return false;
        }
    }
    void equipWeapon(const long objectId, const int powerUpCount, const MWeapon& mWeapon);
    void equipAccessory(const long objectId, const int powerUpCount, const MAccessory& mAccessory);
    void equipReleaseWeapon();
    void equipReleaseAccessory();
    
    const int getPlayerId()                 const { return playerId_; }
    const int getImageResId()               const { return imageResId_; }
    const int getFaceImgId()                const { return faceImgId_; }
    const std::string getName()             const { return name_; }
    const int getAttackPoint()              const { return attackPoint_; }
    const int getDefencePoint()             const { return defencePoint_; }
    const int getLv()                       const { return lv_; }
    const int getExp()                      const { return exp_; }
    const int getNextExp()                  const { return nextExp_; }
    const int getHitPoint()                 const { return hitPoint_; }
    const int getHitPointLimit()            const { return hitPointLimit_; }
    const int getMagicPoint()               const { return magicPoint_; }
    const int getMagicPointLimit()          const { return magicPointLimit_; }
    const int getMovePoint()                const { return movePoint_; }
    const int getAttackRange()              const { return attackRange_; }
    const ActorEquipDto getWeaponEquip()    const { return weaponEquip_; }
    const ActorEquipDto getAccessoryEquip() const { return accessoryEquip_; }
    const int getGold()                     const { return gold_; }

    void addGold(int addGold) { gold_ += addGold; }
    
    void countUpHitPoint() {
        if (hitPoint_ < hitPointLimit_) {
            hitPoint_++;
        }
    }
    void countUpMagicPoint() {
        if (magicPoint_ < magicPointLimit_) {
            magicPoint_++;
        }
    }
    void countDownHitPoint() {
        if (hitPoint_ > 0) {
            hitPoint_--;
        }
    }
    void countDownMagicPoint() {
        if (magicPoint_ > 0) {
            magicPoint_--;
        }
    }
    void curehitPoint(int execValue) {
        if (hitPoint_ + execValue > hitPointLimit_) {
            execValue = hitPointLimit_ - hitPoint_;
        }
        hitPoint_ += execValue;
    }
    
    void cureMagicPoint(int execValue) {
        if (magicPoint_ + execValue > magicPointLimit_) {
            execValue = magicPointLimit_ - magicPoint_;
        }
        magicPoint_ += execValue;
    }
    
    // true : 死亡
    bool damageHitPoint(int damage) {
        if (hitPoint_ <= damage) {
            hitPoint_ = 0;
            return true;
        } else {
            hitPoint_ = hitPoint_ - damage;
            return false;
        }
    }
    
    bool growExpAndLevelUpCheck(int addExp);
    
    void setWeaponEquip(const ActorEquipDto actorEquipDto) { weaponEquip_ = actorEquipDto; }
    void setAccessoryEquip(const ActorEquipDto actorEquipDto) { accessoryEquip_ = actorEquipDto; }
    
private:
    /** プレイヤーを一意に識別するID. */
    int playerId_;
    /** キャライメージのID. */
    int imageResId_;
    /** 顔画像のデフォルトID. */
    int faceImgId_;
    /** キャラ名. */
    std::string name_;
    /** 攻撃力. */
    int attackPoint_;
    /** 防御力. */
    int defencePoint_;
    /** レベル. */
    int lv_;
    /** 経験値. */
    int exp_;
    /** 次のレベルまでの経験値 */
    int nextExp_;
    /** HP. */
    int hitPoint_;
    /** HP最大値. */
    int hitPointLimit_;
    /** MP. */
    int magicPoint_;
    /** MP最大値. */
    int magicPointLimit_;
    /** 移動力(ステータス表示用). */
    int movePoint_;
    /** 攻撃範囲(ステータス表示用). */
    int attackRange_;
    
    /** 装備. */
    ActorEquipDto weaponEquip_;
    ActorEquipDto accessoryEquip_;
    
    /** スキルリスト. */
    //List<ActorPlayerSkillDto> skillDtoList;
    
    /** 所持ゴールド */
    int gold_;
};

#endif /* defined(__Cocos2dRogueLike__ActorDto__) */
