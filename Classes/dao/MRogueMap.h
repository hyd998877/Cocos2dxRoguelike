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
        {"dropItemIds", Value(ValueVector{
            // 60% ハズレ
            Value(ValueMap{ {"type", Value( 0)}, {"id", Value(0)}, {"prob", Value(6000)} }), // hazure
            //  8% 武器
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(3)}, {"prob", Value( 100)} }), // w
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(2)}, {"prob", Value( 200)} }), // w
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(1)}, {"prob", Value( 500)} }), // w
            //  8% 防具
            Value(ValueMap{ {"type", Value( 2)}, {"id", Value(2)}, {"prob", Value( 200)} }), // a
            Value(ValueMap{ {"type", Value( 2)}, {"id", Value(1)}, {"prob", Value( 600)} }), // a
            // 12% ゴールド
            Value(ValueMap{ {"type", Value(10)}, {"id", Value(7)}, {"prob", Value(1200)} }), // gold
            // 12% 消費アイテム
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(5)}, {"prob", Value( 100)} }), // et
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(4)}, {"prob", Value( 200)} }), // et
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(2)}, {"prob", Value( 300)} }), // pt
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(1)}, {"prob", Value( 600)} }), // pt
        })},
        {"goldMin", Value(100)},
        {"goldMax", Value(1000)}
    };

    const static ValueMap floor2_{
        {"mobCount", Value(5)},
        {"mobIds", Value(ValueVector{
            // 20%
            Value(ValueMap{ {"id", Value(0)}, {"prob", Value(2000)} }), // hazure
            // 40%
            Value(ValueMap{ {"id", Value(1)}, {"prob", Value(4000)} }),  // sura
            // 20%
            Value(ValueMap{ {"id", Value(2)}, {"prob", Value(2000)} }),  // youhei
            // 20%
            Value(ValueMap{ {"id", Value(3)}, {"prob", Value(2000)} }),  // youkan
        })},
        {"dropItemCount", Value(10)},
        {"dropItemIds", Value(ValueVector{
            // 60% ハズレ
            Value(ValueMap{ {"type", Value( 0)}, {"id", Value(0)}, {"prob", Value(6000)} }), // hazure
            //  8% 武器
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(6)}, {"prob", Value(  10)} }), // w
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(5)}, {"prob", Value(  40)} }), // w
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(4)}, {"prob", Value( 100)} }), // w
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(3)}, {"prob", Value( 100)} }), // w
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(2)}, {"prob", Value( 150)} }), // w
            Value(ValueMap{ {"type", Value( 1)}, {"id", Value(1)}, {"prob", Value( 400)} }), // w
            //  8% 防具
            Value(ValueMap{ {"type", Value( 2)}, {"id", Value(6)}, {"prob", Value(  10)} }), // a
            Value(ValueMap{ {"type", Value( 2)}, {"id", Value(5)}, {"prob", Value(  40)} }), // a
            Value(ValueMap{ {"type", Value( 2)}, {"id", Value(3)}, {"prob", Value( 150)} }), // a
            Value(ValueMap{ {"type", Value( 2)}, {"id", Value(2)}, {"prob", Value( 200)} }), // a
            Value(ValueMap{ {"type", Value( 2)}, {"id", Value(1)}, {"prob", Value( 400)} }), // a
            // 12% ゴールド
            Value(ValueMap{ {"type", Value(10)}, {"id", Value(7)}, {"prob", Value(1200)} }), // gold
            // 12% 消費アイテム
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(6)}, {"prob", Value( 100)} }), // et
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(5)}, {"prob", Value( 100)} }), // et
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(4)}, {"prob", Value( 300)} }), // et
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(3)}, {"prob", Value( 100)} }), // pt
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(2)}, {"prob", Value( 100)} }), // pt
            Value(ValueMap{ {"type", Value(20)}, {"id", Value(1)}, {"prob", Value( 500)} }), // pt
        })},
        {"goldMin", Value(100)},
        {"goldMax", Value(2000)}
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
        // 10
        Value(floor2_)
    };
}

#endif
