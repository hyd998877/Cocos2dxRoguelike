//
//  TMXGenerator.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/11.
//
//

#ifndef __Cocos2dRogueLike__TMXGenerator__
#define __Cocos2dRogueLike__TMXGenerator__

#include "MapManager.h"

class MapManager;

class TMXLayerData
{
public:
    struct MapSize {
        int _width;
        int _height;
    };
    
    struct MapIndex {
        int _x;
        int _y;
    };
    int _no;
    int _width;
    int _height;
    int _x;
    int _y;
    std::list<TMXLayerData::MapIndex> _gateWayMapIndexList;
    
    bool isGateWay(int x, int y) const {
        for (auto mapIndex : _gateWayMapIndexList) {
            if (x == mapIndex._x && y == mapIndex._y) {
                return true;
            }
        }
        return false;
    }
    
    bool isKado(int x, int y) const {
        if ((x == _x || maxX() == x) && (y == _y || maxY() == y)) {
            return true;
        }
        return false;
    }
    
    bool isLeft(int x, int y) const {
        if (x == _x && ((y >= _y && maxY() > y))) {
            return true;
        }
        return false;
    }
    
    bool isRight(int x, int y) const {
        if (maxX() == x && ((y >= _y && maxY() > y))) {
            return true;
        }
        return false;
    }
    
    bool isTop(int x, int y) const {
        if ((x >= _x && maxX() > x) && y == _y) {
            return true;
        }
        return false;
    }
    
    bool isBottom(int x, int y) const {
        if ((x >= _x && maxX() > x) && maxY() == y) {
            return true;
        }
        return false;
    }
    
    int maxX() const {
        return (_x + _width-1);
    }
    int maxY() const {
        return (_y + _height-1);
    }
    
    bool isRect(int x, int y) const {
        if (x >= _x && maxX() >= x && y >= _y && maxY() >= y) {
            return true;
        }
        return false;
    }
    
    static const int NonTileId = 0;
    static const int ClearTileId = 1;
    static const int BackgroundTileId = 84;
    static const int FloorTileId = 80;
    
    static int checkFloorTileId(const TMXLayerData& layerData, int x, int y) {
        if (layerData.isRect(x, y)) {
            return TMXLayerData::FloorTileId;
        } else {
            return TMXLayerData::NonTileId;
        }
    }
    static int checkBackgroundTileId(const std::list<TMXLayerData>& floorLayerList, int x, int y) {
        for (TMXLayerData layerData : floorLayerList) {
            if (layerData.isRect(x, y)) {
                return TMXLayerData::BackgroundTileId;
            }
        }
        return TMXLayerData::ClearTileId;
    }
    
    static int checkTileId(const std::list<TMXLayerData>& floorLayerList, int x, int y) {
        for (TMXLayerData layerData : floorLayerList) {
            // 壁判定
            if (layerData.isGateWay(x, y)) {
                return TMXLayerData::NonTileId;
            } else if (layerData.isKado(x, y)) {
                return 178;
            } else if (layerData.isRight(x, y)) {
                return 177;
            } else if (layerData.isLeft(x, y)) {
                return 179;
            } else if (layerData.isTop(x, y)) {
                return 194;
            } else if (layerData.isBottom(x, y)) {
                return 162;
            } else if (layerData.isRect(x, y)) {
                return TMXLayerData::NonTileId;
            }
        }
        return TMXLayerData::ClearTileId;
    }
};

class TMXFloorConfig
{
public:
    TMXLayerData::MapIndex mapIndex;
    TMXLayerData::MapSize mapSize;
};

class TMXCreateBaseConfig
{
public:
    typedef std::vector<std::vector<TMXFloorConfig>> BaseMapLayout;
    
    int mapSizeWidth;     /// tile数
    int mapSizeHeight;    /// tile数
    
    BaseMapLayout baseMapLayout;
};

class TMXMapData
{
public:
    TMXCreateBaseConfig _tmxCreateBaseConfig;
    std::list<TMXLayerData> _tmxLayerDataList;
    std::list<TMXLayerData::MapIndex> _walkMapIndex;
};

class TMXGenerator
{
public:
    const static int TILE_SIZE = 32;
    
    static std::string generator();
    static std::string generator(TMXMapData tmxMapData);
    static TMXMapData createTMXMapData();
    
    static bool isMapInLine(const TMXCreateBaseConfig& config, int x, int y) {
        if (x <= 1 || x >= config.mapSizeWidth-1) {
            return false;
        }
        if (y <= 1 || y >= config.mapSizeHeight-1) {
            return false;
        }
        return true;
    }
private:
    TMXGenerator();
    virtual ~TMXGenerator();
    
    // Helper
    static bool addFloorGate(MapManager* mapManager, const TMXCreateBaseConfig& config, const TMXLayerData& layerData, const TMXLayerData::MapIndex& gateMapIndex);
    static std::list<MapIndex> createWalkMapIndexList(MapManager* mapManager, const TMXCreateBaseConfig& config, const MapItem& baseMapItem);
    static TMXCreateBaseConfig createTMXCreateConfig();
};


#endif /* defined(__Cocos2dRogueLike__TMXGenerator__) */
