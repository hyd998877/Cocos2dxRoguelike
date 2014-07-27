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
    template <class T>
    class LotteryObject
    {
    public:
        LotteryObject()
        : isLot(false)
        , hitId(0)
        , prob(0)
        , object()
        {
            
        }
        LotteryObject(bool isLot, int hitId, int prob, const T& object)
        : isLot(isLot)
        , hitId(hitId)
        , prob(prob)
        , object(object)
        {
            
        }
        virtual ~LotteryObject()
        {
            
        }
        
        bool isLot; // falseはずれ
        int hitId;
        int prob;
        T object;
    };
    
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
        
        template <class T>
        static std::vector<LotteryObject<T>> lotValues(const int probCount, const std::vector<LotteryObject<T>>& probList) {
            std::vector<LotteryObject<T>> hitValues;
            
            // 抽選合計値を求める（100%基準を作成）
            int probSum = 0;
            for (auto probData : probList) {
                int prob = probData.prob;
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
                    int prob = probData.prob;
                    tempProbability += prob;

                    if (tempProbability >= randomProbability) {
                        if (probData.isLot) {
                            // hit
                            hitValues.push_back(probData);
                        }
                        break;
                    }
                }
            }
            return hitValues;
        }
        
        template <class T>
        static int lot(const std::vector<LotteryObject<T>> probList) {
            auto ids = lot(1, probList);
            if (ids.empty()) {
                return 0;
            }
            return ids.at(0);
        }
        
        template <class T>
        static std::vector<int> lot(const int probCount, const std::vector<LotteryObject<T>> probList) {
            std::vector<int> hitIds;
            
            auto hitValues = lotValues(probCount, probList);
            for (auto value : hitValues) {
                hitIds.push_back(value.hitId);
            }
            hitValues.clear();
            return hitIds;
        }
    };
}

#endif
