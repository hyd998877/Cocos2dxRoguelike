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
namespace m_rogue_map{
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
    
    
    
    const static ValueMap floor1_{
        {MobCount, Value(4)},
        {MobIds, Value(ValueVector{
            // 20%
            Value(ValueMap{ {Id, Value(0)}, {Prob, Value(2000)}, {DropProb, Value(0)} }), // hazure
            // 80%
            Value(ValueMap{ {Id, Value(1)}, {Prob, Value(8000)}, {DropProb, Value(1000)} })  // sura
        })},
        {DropItemCount, Value(10)},
        {DropItemIds, Value(ValueVector{
            // 60% ハズレ
            Value(ValueMap{ {ItemType, Value( 0)}, {Id, Value(0)}, {Prob, Value(6000)} }), // hazure
            //  8% 武器
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(3)}, {Prob, Value( 100)} }), // w
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(2)}, {Prob, Value( 200)} }), // w
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(1)}, {Prob, Value( 500)} }), // w
            //  8% 防具
            Value(ValueMap{ {ItemType, Value( 2)}, {Id, Value(2)}, {Prob, Value( 200)} }), // a
            Value(ValueMap{ {ItemType, Value( 2)}, {Id, Value(1)}, {Prob, Value( 600)} }), // a
            // 12% ゴールド
            Value(ValueMap{ {ItemType, Value(10)}, {Id, Value(7)}, {Prob, Value(1200)} }), // gold
            // 12% 消費アイテム
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(5)}, {Prob, Value( 100)} }), // et
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(4)}, {Prob, Value( 200)} }), // et
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(2)}, {Prob, Value( 300)} }), // pt
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(1)}, {Prob, Value( 600)} }), // pt
        })},
        {GoldMin, Value(100)},
        {GoldMax, Value(1000)}
    };

    const static ValueMap floor2_{
        {MobCount, Value(5)},
        {MobIds, Value(ValueVector{
            // 20%
            Value(ValueMap{ {Id, Value(0)}, {Prob, Value(2000)}, {DropProb, Value(0)} }), // hazure
            // 40%
            Value(ValueMap{ {Id, Value(1)}, {Prob, Value(4000)}, {DropProb, Value(1000)} }),  // sura
            // 20%
            Value(ValueMap{ {Id, Value(2)}, {Prob, Value(2000)}, {DropProb, Value(1000)} }),  // youhei
            // 20%
            Value(ValueMap{ {Id, Value(3)}, {Prob, Value(2000)}, {DropProb, Value(1000)} }),  // youkan
        })},
        {DropItemCount, Value(10)},
        {DropItemIds, Value(ValueVector{
            // 60% ハズレ
            Value(ValueMap{ {ItemType, Value( 0)}, {Id, Value(0)}, {Prob, Value(6000)} }), // hazure
            //  8% 武器
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(6)}, {Prob, Value(  10)} }), // w
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(5)}, {Prob, Value(  40)} }), // w
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(4)}, {Prob, Value( 100)} }), // w
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(3)}, {Prob, Value( 100)} }), // w
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(2)}, {Prob, Value( 150)} }), // w
            Value(ValueMap{ {ItemType, Value( 1)}, {Id, Value(1)}, {Prob, Value( 400)} }), // w
            //  8% 防具
            Value(ValueMap{ {ItemType, Value( 2)}, {Id, Value(6)}, {Prob, Value(  10)} }), // a
            Value(ValueMap{ {ItemType, Value( 2)}, {Id, Value(5)}, {Prob, Value(  40)} }), // a
            Value(ValueMap{ {ItemType, Value( 2)}, {Id, Value(3)}, {Prob, Value( 150)} }), // a
            Value(ValueMap{ {ItemType, Value( 2)}, {Id, Value(2)}, {Prob, Value( 200)} }), // a
            Value(ValueMap{ {ItemType, Value( 2)}, {Id, Value(1)}, {Prob, Value( 400)} }), // a
            // 12% ゴールド
            Value(ValueMap{ {ItemType, Value(10)}, {Id, Value(7)}, {Prob, Value(1200)} }), // gold
            // 12% 消費アイテム
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(6)}, {Prob, Value( 100)} }), // et
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(5)}, {Prob, Value( 100)} }), // et
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(4)}, {Prob, Value( 300)} }), // et
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(3)}, {Prob, Value( 100)} }), // pt
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(2)}, {Prob, Value( 100)} }), // pt
            Value(ValueMap{ {ItemType, Value(20)}, {Id, Value(1)}, {Prob, Value( 500)} }), // pt
        })},
        {GoldMin, Value(100)},
        {GoldMax, Value(2000)}
    };
    
    const static ValueVector datas_{
        // 1〜3
        Value(floor1_),
        Value(floor1_),
        Value(floor1_),
        // 4〜9
        Value(floor2_),
        Value(floor2_),
        Value(floor2_),
        Value(floor2_),
        Value(floor2_),
        Value(floor2_),
        // 10〜
        Value(floor2_)
    };
}

#endif
