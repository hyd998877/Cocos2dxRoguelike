//
//  MRogueMap.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/12.
//
//

#ifndef Cocos2dRogueLike_MRogueMap_h
#define Cocos2dRogueLike_MRogueMap_h

/*
 {
 "mapId": 1,
 "floors": [
        {
             // 初期モンスター数
             "mobCount": 4,
             // 出現可能モンスターIDと確率
             "mobIds": [1,2,3,4,5],
             // 落ちているアイテムIDと確率（はずれ込）
             "dropItemIds": [1,2,3,4,5,6],
             // 落ちているアイテムの数
             "dropItemCnt": 10,
             // 獲得ゴールドの乱数の範囲値
             "goldMin": 100,
             "goldMax": 1000
         },
         {
             // 初期モンスター数
             "mobCount": 4,
             // 出現可能モンスターIDと確率
             "mobIds": [1,2,3,4,5],
             // 落ちているアイテムIDと確率（はずれ込）
             "dropItemIds": [1,2,3,4,5,6],
             // 落ちているアイテムの数
             "dropItemCnt": 10,
             // 獲得ゴールドの乱数の範囲値
             "goldMin": 100,
             "goldMax": 1000
         }
    ]
 }
 */

namespace RogueGameConfig {

    // 所持可能アイテム数
    const int USE_ITEM_MAX = 20;
    
    const std::string MobCount = "mobCount";
    const std::string MobIds = "mobIds";
    const std::string DropItemCount = "dropItemCount";
    const std::string DropItemIds = "dropItemIds";
    
    const std::string Id = "id";
    const std::string Prob = "prob";
    const std::string DropProb = "dropProb";
    const std::string ItemType = "type";
    
    const std::string GoldMin = "goldMin";
    const std::string GoldMax = "goldMax";
    const std::string DropItemUpValueLimits = "dropItemUpValueLimits";
    
    
    const static cocos2d::ValueMap floor1_{
        {MobCount, cocos2d::Value(4)},
        {MobIds, cocos2d::Value(cocos2d::ValueVector{
            // 20%
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(0)}, {Prob, cocos2d::Value(2000)}, {DropProb, cocos2d::Value(0)} }), // hazure
            // 80%
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value(8000)}, {DropProb, cocos2d::Value(1000)} })  // sura
        })},
        {DropItemCount, cocos2d::Value(10)},
        {DropItemIds, cocos2d::Value(cocos2d::ValueVector{
            // 60% ハズレ
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 0)}, {Id, cocos2d::Value(0)}, {Prob, cocos2d::Value(6000)} }), // hazure
            //  8% 武器
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(3)}, {Prob, cocos2d::Value( 100)} }), // w
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value( 200)} }), // w
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value( 500)} }), // w
            //  8% 防具
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 2)}, {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value( 200)} }), // a
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 2)}, {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value( 600)} }), // a
            // 12% ゴールド
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(10)}, {Id, cocos2d::Value(7)}, {Prob, cocos2d::Value(1200)} }), // gold
            // 12% 消費アイテム
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(5)}, {Prob, cocos2d::Value( 100)} }), // et
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(4)}, {Prob, cocos2d::Value( 200)} }), // et
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value( 300)} }), // pt
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value( 600)} }), // pt
        })},
        {GoldMin, cocos2d::Value( 100)},
        {GoldMax, cocos2d::Value(1000)},
        {DropItemUpValueLimits, cocos2d::Value(cocos2d::ValueVector{
            // 70%  +0
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(0)}, {Prob, cocos2d::Value(7000)} }),
            // 10%  +1
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value(1000)} }),
            // 10%  +2
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value(1000)} }),
            //  5%  +3
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(3)}, {Prob, cocos2d::Value( 500)} }),
            //  5%  +4
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(4)}, {Prob, cocos2d::Value( 500)} }),
        })}
    };

    const static cocos2d::ValueMap floor2_{
        {MobCount, cocos2d::Value(5)},
        {MobIds, cocos2d::Value(cocos2d::ValueVector{
            // 20%
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(0)}, {Prob, cocos2d::Value(2000)}, {DropProb, cocos2d::Value(0)} }), // hazure
            // 40%
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value(4000)}, {DropProb, cocos2d::Value(1000)} }),  // sura
            // 20%
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value(2000)}, {DropProb, cocos2d::Value(1000)} }),  // youhei
            // 20%
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(3)}, {Prob, cocos2d::Value(2000)}, {DropProb, cocos2d::Value(1000)} }),  // youkan
        })},
        {DropItemCount, cocos2d::Value(10)},
        {DropItemIds, cocos2d::Value(cocos2d::ValueVector{
            // 60% ハズレ
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 0)}, {Id, cocos2d::Value(0)}, {Prob, cocos2d::Value(6000)} }), // hazure
            //  8% 武器
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(6)}, {Prob, cocos2d::Value(  10)} }), // w
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(5)}, {Prob, cocos2d::Value(  40)} }), // w
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(4)}, {Prob, cocos2d::Value( 100)} }), // w
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(3)}, {Prob, cocos2d::Value( 100)} }), // w
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value( 150)} }), // w
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 1)}, {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value( 400)} }), // w
            //  8% 防具
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 2)}, {Id, cocos2d::Value(6)}, {Prob, cocos2d::Value(  10)} }), // a
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 2)}, {Id, cocos2d::Value(5)}, {Prob, cocos2d::Value(  40)} }), // a
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 2)}, {Id, cocos2d::Value(3)}, {Prob, cocos2d::Value( 150)} }), // a
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 2)}, {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value( 200)} }), // a
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value( 2)}, {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value( 400)} }), // a
            // 12% ゴールド
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(10)}, {Id, cocos2d::Value(7)}, {Prob, cocos2d::Value(1200)} }), // gold
            // 12% 消費アイテム
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(6)}, {Prob, cocos2d::Value( 100)} }), // et
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(5)}, {Prob, cocos2d::Value( 100)} }), // et
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(4)}, {Prob, cocos2d::Value( 300)} }), // et
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(3)}, {Prob, cocos2d::Value( 100)} }), // pt
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value( 100)} }), // pt
            cocos2d::Value(cocos2d::ValueMap{ {ItemType, cocos2d::Value(20)}, {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value( 500)} }), // pt
        })},
        {GoldMin, cocos2d::Value( 100)},
        {GoldMax, cocos2d::Value(2000)},
        {DropItemUpValueLimits, cocos2d::Value(cocos2d::ValueVector{
            // 70%  +0
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(0)}, {Prob, cocos2d::Value(7000)} }),
            // 10%  +1
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(1)}, {Prob, cocos2d::Value(1000)} }),
            // 10%  +2
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(2)}, {Prob, cocos2d::Value(1000)} }),
            //  5%  +3
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(3)}, {Prob, cocos2d::Value( 500)} }),
            //  5%  +4
            cocos2d::Value(cocos2d::ValueMap{ {Id, cocos2d::Value(4)}, {Prob, cocos2d::Value( 500)} }),
        })}
    };
    
    const static cocos2d::ValueVector datas_{
        // 1〜3
        cocos2d::Value(floor1_),
        cocos2d::Value(floor1_),
        cocos2d::Value(floor1_),
        // 4〜9
        cocos2d::Value(floor2_),
        cocos2d::Value(floor2_),
        cocos2d::Value(floor2_),
        cocos2d::Value(floor2_),
        cocos2d::Value(floor2_),
        cocos2d::Value(floor2_),
        // 10〜
        cocos2d::Value(floor2_)
    };
}

#endif
