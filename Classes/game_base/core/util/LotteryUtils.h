//
//  LotteryUtils.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/13.
//
//

#ifndef Cocos2dRogueLike_LotteryUtils_h
#define Cocos2dRogueLike_LotteryUtils_h

#include "cocos2d.h"

namespace RogueLikeGame {

    //
    // probの合計値を100%として抽選
    //
    // id = 0 ハズレ扱いです。
    //
    // ValueVector{
    //     Value(ValueMap{ {"id", Value(0)}, {"prob", Value(2000)} }), // 20% ハズレ
    //     Value(ValueMap{ {"id", Value(1)}, {"prob", Value(4000)} }), // 40%
    //     Value(ValueMap{ {"id", Value(2)}, {"prob", Value(2000)} }), // 20%
    //     Value(ValueMap{ {"id", Value(3)}, {"prob", Value(2000)} }), // 20%
    // }
    //
    class LotteryUtils
    {
    public:
        static int getRandomProb(int min, int probSum) {
            return min + (int)(rand()*(probSum-min+1.0)/(1.0+RAND_MAX));
        }
        
        static bool isHit(int prob, int probMax = 10000) {
            int lot = getRandomProb(1, probMax);
            if (lot <= prob) {
                return true;
            } else {
                return false;
            }
        }
        
        static const cocos2d::ValueVector lotValues(const int probCount, const cocos2d::ValueVector probList) {
            cocos2d::ValueVector hitValues;
            
            // 抽選合計値を求める（100%基準を作成）
            int probSum = 0;
            for (auto probData : probList) {
                int prob = probData.asValueMap().at("prob").asInt();
                probSum += prob;
            }
            
            // 合計0以下はハズレ扱い
            if (probSum <= 0) {
                return hitValues;
            }
            
            for (int i = 0; i < probCount; i++) {
                // 抽選
                int randomProbability = getRandomProb(1, probSum);
                
                int tempProbability = 0;
                for (auto probData : probList) {
                    auto probDataMap = probData.asValueMap();
                    int prob = probData.asValueMap().at("prob").asInt();
                    tempProbability += prob;

                    if (tempProbability >= randomProbability) {
                        int probId = probDataMap.at("id").asInt();
                        if (probId > 0) {
                            // hit
                            hitValues.push_back(cocos2d::Value(probDataMap));
                        }
                        break;
                    }
                }
            }
            return hitValues;
        }
        
        static const int lot(const cocos2d::ValueVector probList) {
            auto ids = lot(1, probList);
            if (ids.empty()) {
                return 0;
            }
            return ids.at(0);
        }
        
        static const std::vector<int> lot(const int probCount, const cocos2d::ValueVector probList) {
            std::vector<int> hitIds;
            
            auto hitValues = lotValues(probCount, probList);
            for (auto value : hitValues) {
                hitIds.push_back(value.asValueMap().at("id").asInt());
            }
            hitValues.clear();
            return hitIds;
        }
    };
}

#endif
