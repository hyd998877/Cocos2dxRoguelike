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
    
    static std::vector<int> lot(const int probCount, const ValueVector probList) {
        std::vector<int> hitIds;
        // 抽選合計値を求める（100%基準を作成）
        int probSum = 0;
        for (Value probData : probList) {
            int prob = probData.asValueMap().at("prob").asInt();
            probSum += prob;
        }
        
        for (int i = 0; i < probCount; i++) {
            // モンスター抽選
            int probTotal = 0;
            for (Value probData : probList) {
                
                ValueMap probDataMap = probData.asValueMap();
                
                int prob = probDataMap.at("prob").asInt();
                probTotal += prob;
                int randProb = GetRandom(1, probSum);
                if (probTotal >= randProb) {
                    
                    int probId = probDataMap.at("id").asInt();
                    if (probId > 0) {
                        // hit
                        hitIds.push_back(probId);
                    }
                }
            }
        }
        return hitIds;
    }
    
};

#endif
