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
    
    const static ValueMap floor1_{
        {"mobCount", Value(4)},
        {"mobIds", Value(ValueVector{
            // 20%
            Value(ValueMap{ {"id", Value(0)}, {"prob", Value(2000)} }), // hazure
            // 80%
            Value(ValueMap{ {"id", Value(1)}, {"prob", Value(8000)} })  // sura
        })},
        {"dropItemCount", Value(10)},
        {"useItemIds", Value(ValueVector{
            // 50%
            Value(ValueMap{ {"type", Value(0)}, {"id", Value(0)}, {"prob", Value(5000)} }), // hazure
            // 20%
            Value(ValueMap{ {"type", Value(1)}, {"id", Value(1)}, {"prob", Value(1000)} }), // pt
            Value(ValueMap{ {"type", Value(1)}, {"id", Value(2)}, {"prob", Value( 500)} }),
            Value(ValueMap{ {"type", Value(1)}, {"id", Value(4)}, {"prob", Value( 300)} }), // et
            Value(ValueMap{ {"type", Value(1)}, {"id", Value(5)}, {"prob", Value( 200)} }),
            // 10%
            Value(ValueMap{ {"type", Value(1)}, {"id", Value(7)}, {"prob", Value(1000)} }), // gold
            // 10%
            Value(ValueMap{ {"type", Value(2)}, {"id", Value(1)}, {"prob", Value( 600)} }),
            Value(ValueMap{ {"type", Value(2)}, {"id", Value(2)}, {"prob", Value( 300)} }),
            Value(ValueMap{ {"type", Value(2)}, {"id", Value(3)}, {"prob", Value( 100)} }),
            // 10%
            Value(ValueMap{ {"type", Value(3)}, {"id", Value(1)}, {"prob", Value( 800)} }),
            Value(ValueMap{ {"type", Value(3)}, {"id", Value(2)}, {"prob", Value( 200)} }),
        })},
        {"goldMin", Value(100)},
        {"goldMax", Value(1000)}
    };
    
    const static ValueVector datas_{
        // 1〜3
        Value(floor1_),
        Value(floor1_),
        Value(floor1_),
        
        Value(floor1_),
        Value(floor1_),
        Value(floor1_),
        Value(floor1_),
        Value(floor1_),
        Value(floor1_),
        Value(floor1_)
    };
}

#endif
