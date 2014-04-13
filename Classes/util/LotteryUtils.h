//
//  LotteryUtils.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/13.
//
//

#ifndef Cocos2dRogueLike_LotteryUtils_h
#define Cocos2dRogueLike_LotteryUtils_h

class LotteryUtils
{
public:
    
    // ValueVector{Value(ValueMap{"id", "prob"})}
    static ValueVector lotValues(const int probCount, const ValueVector probList) {
        ValueVector hitValues = {};
        // 抽選合計値を求める（100%基準を作成）
        int probSum = 0;
        for (Value probData : probList) {
            int prob = probData.asValueMap().at("prob").asInt();
            probSum += prob;
        }
        
        for (int i = 0; i < probCount; i++) {
            // 抽選
            int randomProbability = GetRandom(1, probSum);
            
            int tempProbability = 0;
            for (Value probData : probList) {
                ValueMap probDataMap = probData.asValueMap();
                int prob = probData.asValueMap().at("prob").asInt();
                tempProbability += prob;

                if (tempProbability >= randomProbability) {
                    int probId = probDataMap.at("id").asInt();
                    if (probId > 0) {
                        // hit
                        hitValues.push_back(Value(probDataMap));
                    }
                    break;
                }
            }
        }
        return hitValues;
    }
    
    // ValueVector{Value(ValueMap{"id", "prob"})}
    static std::vector<int> lot(const int probCount, const ValueVector probList) {
        std::vector<int> hitIds;
        ValueVector hitValues = lotValues(probCount, probList);
        for (auto value : hitValues) {
            hitIds.push_back(value.asValueMap().at("id").asInt());
        }
        hitValues.clear();
        return hitIds;
    }
    
};

#endif
