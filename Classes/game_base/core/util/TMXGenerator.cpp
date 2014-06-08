//
//  TMXGenerator.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/11.
//
//

#include "TMXGenerator.h"

#include "cocos2d.h"

#include "pugixml.hpp"

#include <unordered_map>
#include <string>

#include "MapManager.h"
#include "AppMacros.h"

USING_NS_CC;

TMXGenerator::TMXGenerator()
{
    
}

TMXGenerator::~TMXGenerator()
{
    
}

std::string TMXGenerator::generator()
{
    return generator(TMXGenerator::createTMXMapData());
}

std::string TMXGenerator::generator(TMXMapData tmxMapData)
{
    /*
     <?xml version="1.0" encoding="UTF-8"?>
     <map version="1.0" orientation="orthogonal" width="40" height="20" tilewidth="32" tileheight="32">
        <tileset firstgid="1" name="tile_e" tilewidth="32" tileheight="32">
            <image source="tile_e.png" width="512" height="512"/>
        </tileset>
        <layer name="background" width="40" height="20">
            <data>
                <tile gid="0"/>
            </data>
        </layer>
     </map>
     */
    TMXCreateBaseConfig config = tmxMapData._tmxCreateBaseConfig;
    
    std::vector<std::vector<int>> tileIdArray(config.mapSizeWidth);
    for (int x = 0; x < config.mapSizeWidth; x++) {
        tileIdArray[x] = std::vector<int>(config.mapSizeHeight);
        for (int y = 0; y < config.mapSizeHeight; y++) {
            tileIdArray[x][y] = 0;
        }
    }
    // 通路
    for (auto walkMapIndex : tmxMapData._walkMapIndex) {
        tileIdArray[walkMapIndex._x][walkMapIndex._y] = 84;
    }
    
    pugi::xml_document doc;
    auto mapNode = doc.append_child("map");
    {
        mapNode.append_attribute("version").set_value("1.0");
        mapNode.append_attribute("orientation").set_value("orthogonal");
        mapNode.append_attribute("width").set_value(config.mapSizeWidth);
        mapNode.append_attribute("height").set_value(config.mapSizeHeight);
        mapNode.append_attribute("tilewidth").set_value(TILE_SIZE);
        mapNode.append_attribute("tileheight").set_value(TILE_SIZE);
        {
            auto tileSetNode = mapNode.append_child("tileset");
            tileSetNode.append_attribute("firstgid").set_value(1);
            tileSetNode.append_attribute("name").set_value("tile_e");
            tileSetNode.append_attribute("tilewidth").set_value(TILE_SIZE);
            tileSetNode.append_attribute("tileheight").set_value(TILE_SIZE);
            
            auto imageNode = mapNode.append_child("image");
            imageNode.append_attribute("source").set_value("tile_e.png");
            imageNode.append_attribute("width").set_value(512);
            imageNode.append_attribute("height").set_value(512);
        
            //////////////
            
            // 床
            auto backgroundLayerNode = mapNode.append_child("layer");
            backgroundLayerNode.append_attribute("name").set_value("background");
            backgroundLayerNode.append_attribute("width").set_value(config.mapSizeWidth);
            backgroundLayerNode.append_attribute("height").set_value(config.mapSizeHeight);
            {
                auto dataNode = backgroundLayerNode.append_child("data");
                for (int y = 0; y < config.mapSizeHeight; y++) {
                    for (int x = 0; x < config.mapSizeWidth; x++) {
                        if (tileIdArray[x][y] != 0) {
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileIdArray[x][y]);
                        } else {
                            auto tileId = TMXLayerData::checkBackgroundTileId(tmxMapData._tmxLayerDataList, x, y);
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileId);
                        }
                    }
                }
            }
            
            // 壁
            auto colisionLayerNode = mapNode.append_child("layer");
            colisionLayerNode.append_attribute("name").set_value("colision");
            colisionLayerNode.append_attribute("width").set_value(config.mapSizeWidth);
            colisionLayerNode.append_attribute("height").set_value(config.mapSizeHeight);
            {
                auto dataNode = colisionLayerNode.append_child("data");
                for (int y = 0; y < config.mapSizeHeight; y++) {
                    for (int x = 0; x < config.mapSizeWidth; x++) {
                        if (tileIdArray[x][y] != 0) {
                            dataNode.append_child("tile").append_attribute("gid").set_value(TMXLayerData::NonTileId);
                        } else {
                            int tileId = TMXLayerData::checkTileId(tmxMapData._tmxLayerDataList, x, y);
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileId);
                        }
                    }
                }
            }
            
            // 部屋
            for (TMXLayerData layerData : tmxMapData._tmxLayerDataList) {
                std::string nodeName = cocos2d::StringUtils::format("floor_%d", layerData._no);
                auto floorLayerNode = mapNode.append_child("layer");
                floorLayerNode.append_attribute("name").set_value(nodeName.c_str());
                floorLayerNode.append_attribute("width").set_value(config.mapSizeWidth);
                floorLayerNode.append_attribute("height").set_value(config.mapSizeHeight);
                {
                    auto propertiesNode = floorLayerNode.append_child("properties");
                    {
                        auto property2 = propertiesNode.append_child("property");
                        property2.append_attribute("name").set_value("no");
                        property2.append_attribute("value").set_value(layerData._no);
                        
                        auto property3 = propertiesNode.append_child("property");
                        property3.append_attribute("name").set_value("width");
                        property3.append_attribute("value").set_value(layerData._width);
                        
                        auto property1 = propertiesNode.append_child("property");
                        property1.append_attribute("name").set_value("height");
                        property1.append_attribute("value").set_value(layerData._height);
                        
                        auto property4 = propertiesNode.append_child("property");
                        property4.append_attribute("name").set_value("x");
                        property4.append_attribute("value").set_value(layerData._x);
                        
                        auto property5 = propertiesNode.append_child("property");
                        property5.append_attribute("name").set_value("y");
                        property5.append_attribute("value").set_value(layerData._y);
                    }
                    
                    auto dataNode = floorLayerNode.append_child("data");
                    for (int y = 0; y < config.mapSizeHeight; y++) {
                        for (int x = 0; x < config.mapSizeWidth; x++) {
                            int tileId = TMXLayerData::checkFloorTileId(layerData, x, y);
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileId);
                        }
                    }
                }
            }
        }
    }
    std::string tempFilePath = FileUtils::getInstance()->getWritablePath() + "tmp_quest.tmx";
    doc.save_file(tempFilePath.c_str());
    auto tmxString = FileUtils::getInstance()->getStringFromFile(tempFilePath);
    return tmxString;
}

TMXMapData TMXGenerator::createTMXMapData()
{
    // 部屋割り
    TMXCreateBaseConfig config = createTMXCreateConfig();
    
    auto mapManager = MapManager();
    // 部屋のサイズ
    mapManager.initMapping(0, config.mapSizeHeight, 0, config.mapSizeWidth);
    
    std::list<TMXLayerData> floorLayerList;
    
    const int FLOOR_SIZE_MAX = config.baseMapLayout.size() * config.baseMapLayout[0].size();
    for (int floorIdx = 0; floorIdx < FLOOR_SIZE_MAX; floorIdx++) {
        TMXLayerData layerData;
        layerData._no = floorIdx + 1;
        
        // 座標決め
        int randX = GetRandom(0, 2);
        int randY = GetRandom(0, 2);
        
        int x = floorIdx % 4;
        int y = floorIdx / 4;
        CCLOG("x = %d y = %d", x, y);
        layerData._x = randX + config.baseMapLayout[y][x].mapIndex._x;
        layerData._y = randY + config.baseMapLayout[y][x].mapIndex._y;
        
        // サイズ決め
        int randWidth = GetRandom(5, config.baseMapLayout[y][x].mapSize._width - randX);
        int randHeight = GetRandom(5, config.baseMapLayout[y][x].mapSize._height - randY);
        
        layerData._width = randWidth;
        layerData._height = randHeight;

        for (int x = layerData._x; x < (layerData._x + layerData._width); x++) {
            for (int y = layerData._y; y < (layerData._y + layerData._height); y++) {
                mapManager.addObstacle(MapIndex{x, y, MoveDirectionType::MOVE_NONE});
            }
        }
        
        // TODO: 通路口を決める とりあえず/2して真ん中くらいにおいてるけど、角以外ならOKにしたい
        std::list<TMXLayerData::MapIndex> gateList;
        
        // 4方向すべて抽選する
        // ただし壁際の場合は通路作成を無効化
        //  x <= 1 || y <= 1 || x >= MaxX -1 || y >= MaxY -1
        // また、部屋の角の場合も通路作成を無効化
        // 下
        {
            auto gateMapIndex = TMXLayerData::MapIndex{
                layerData._x + layerData._width / 2,
                layerData._y + layerData._height - 1
            };
            if (addFloorGate(&mapManager, config, layerData, gateMapIndex)) {
                gateList.push_back(gateMapIndex);
            }
        }
        // 上
        {
            auto gateMapIndex = TMXLayerData::MapIndex{
                layerData._x + layerData._width / 2,
                layerData._y
            };
            if (addFloorGate(&mapManager, config, layerData, gateMapIndex)) {
                gateList.push_back(gateMapIndex);
            }
        }
        // 左
        {
            auto gateMapIndex = TMXLayerData::MapIndex{
                layerData._x,
                layerData._y + layerData._height / 2
            };
            if (addFloorGate(&mapManager, config, layerData, gateMapIndex)) {
                gateList.push_back(gateMapIndex);
            }
        }
        // 右
        {
            auto gateMapIndex = TMXLayerData::MapIndex{
                layerData._x + layerData._width -1,
                layerData._y + layerData._height / 2
            };
            if (addFloorGate(&mapManager, config, layerData, gateMapIndex)) {
                gateList.push_back(gateMapIndex);
            }
        }
        layerData._gateWayMapIndexList = gateList;
        
        floorLayerList.push_back(layerData);
    }
    
    // 最短経路の通路を作成
    
    std::list<TMXLayerData::MapIndex> walkMapIndexList;
    std::list<TMXLayerData::MapIndex> closeMapIndexList;
    
    auto actorMapList = mapManager.findActorMapItem();
    for (auto mapItem : actorMapList) {
        //CCLOG("mapItem x = %d y = %d ->", mapItem.mapIndex.x, mapItem.mapIndex.y);
        
        std::list<MapIndex> moveMapIndexList = createWalkMapIndexList(&mapManager, config, mapItem);
        if (moveMapIndexList.empty()) {
            // 通路ができなかった通路口を確保
            closeMapIndexList.push_back(TMXLayerData::MapIndex{mapItem.mapIndex.x, mapItem.mapIndex.y});
        } else {
            for (auto mapIndex : moveMapIndexList) {
                //CCLOG("move x=%d y=%d", mapIndex.x, mapIndex.y);
                walkMapIndexList.push_back(TMXLayerData::MapIndex{mapIndex.x, mapIndex.y});
            }
        }
    }
    
    // 通路を作れなかった通路口を閉じる
    for (auto closeMapIndex : closeMapIndexList) {
        closeNotWalkMapIndex(closeMapIndex, &floorLayerList);
    }
    
    return TMXMapData{config, floorLayerList, walkMapIndexList};
}

// private
TMXCreateBaseConfig TMXGenerator::createTMXCreateConfig()
{
    // TODO: とりあえず固定40x20 10x10 4x2フロア数
    int mapWidth    = 40;
    int mapHeight   = 20;
    int floorWidth  = 10;
    int floorHeight = 10;
    
    return TMXCreateBaseConfig {
        mapWidth, mapHeight, TMXCreateBaseConfig::BaseMapLayout {
            {
                {
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*0, floorHeight*0},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    },
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*1, floorHeight*0},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    },
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*2, floorHeight*0},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    },
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*3, floorHeight*0},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    }
                }
            },
            {
                {
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*0, floorHeight*1},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    },
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*1, floorHeight*1},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    },
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*2, floorHeight*1},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    },
                    TMXFloorConfig{
                        TMXLayerData::MapIndex{floorWidth*3, floorHeight*1},
                        TMXLayerData::MapSize{floorWidth, floorHeight}
                    }
                }
            }
        }
    };
}

bool TMXGenerator::addFloorGate(MapManager* mapManager, const TMXCreateBaseConfig& config, const TMXLayerData& layerData, const TMXLayerData::MapIndex& gateMapIndex)
{
    if (!layerData.isKado(gateMapIndex._x, gateMapIndex._y) && isMapInLine(config, gateMapIndex._x, gateMapIndex._y)) {
        //gateList.push_back(gateMapIndex);
        
        auto obj = MapManager::createNoneMapItem<ActorMapItem>(gateMapIndex._x, gateMapIndex._y);
        obj.mapDataType = MapDataType::OBSTACLE;
        mapManager->removeMapItem(obj);
        auto mapItem = MapManager::createNoneMapItem<ActorMapItem>(gateMapIndex._x, gateMapIndex._y);
        mapItem.mapDataType = MapDataType::PLAYER;
        mapManager->addActor(mapItem);
        
        return true;
    }
    return false;
}

void TMXGenerator::closeNotWalkMapIndex(const TMXLayerData::MapIndex& closeMapIndex, std::list<TMXLayerData>* floorLayerList)
{
    auto floorIt = floorLayerList->begin();
    while (!(floorIt == floorLayerList->end())) {
        
        auto it = (*floorIt)._gateWayMapIndexList.begin();
        while (!(it == (*floorIt)._gateWayMapIndexList.end())) {
            auto gateWayMapIndex = *it;
            
            if (closeMapIndex._x == gateWayMapIndex._x && closeMapIndex._y == gateWayMapIndex._y) {
                //CCLOG("通路作れなかったので閉じる(%ld) x=%d y=%d", (*floorIt)._gateWayMapIndexList.size(), gateWayMapIndex._x, gateWayMapIndex._y);
                (*floorIt)._gateWayMapIndexList.erase(it);
                //CCLOG("通路作れなかったので閉じた(%ld)", (*floorIt)._gateWayMapIndexList.size());
            }
            it++;
        }
        floorIt++;
    }
}

std::list<MapIndex> TMXGenerator::createWalkMapIndexList(MapManager* mapManager, const TMXCreateBaseConfig& config, const MapItem& baseMapItem)
{
    std::list<MapIndex> moveMapIndexList;
    // TODO: 通路口の探索範囲10マス以内
    mapManager->createActorFindDist(baseMapItem.mapIndex, 10);
    mapManager->showDebug();
    
    for (auto targetMapItem : mapManager->findActorMapItem()) {
        if (MapManager::equalMapItem(targetMapItem, baseMapItem)) {
            // 自分
            //CCLOG("equal targetMapItem x = %d y = %d ->", targetMapItem.mapIndex.x, targetMapItem.mapIndex.y);
            continue;
        }
        std::list<MapIndex> searchMapIndexList = MapManager::createRelatedMapIndexList(targetMapItem.mapIndex);
        MapItem targetMoveDistMapItem = mapManager->searchTargetMapItem(searchMapIndexList);
        //CCLOG("target x =%d y = %d", targetMoveDistMapItem.mapIndex.x, targetMoveDistMapItem.mapIndex.y);
        auto targetMoveMapIndexList = mapManager->createMovePointList(targetMoveDistMapItem.mapIndex, baseMapItem);
        if (targetMoveMapIndexList.size() == 1) {
            auto moveMapIndex = *(targetMoveMapIndexList.begin());
            if (moveMapIndex.x == 0 && moveMapIndex.y == 0) {
                // 移動なし
                //CCLOG("移動なし x = %d y = %d ->", targetMapItem.mapIndex.x, targetMapItem.mapIndex.y);
                continue;
            }
        }
        //CCLOG("size %ld", targetMoveMapIndexList.size());
        
        if (moveMapIndexList.empty() || moveMapIndexList.size() > targetMoveMapIndexList.size()) {
            
            bool isMapLineIn = false;
            for (auto moveMapIndexCheck : targetMoveMapIndexList) {
                if (!TMXGenerator::isMapInLine(config, moveMapIndexCheck.x, moveMapIndexCheck.y)) {
                    isMapLineIn = true;
                }
            }
            if (!isMapLineIn) {
                moveMapIndexList = targetMoveMapIndexList;
            }
        }
    }
    return moveMapIndexList;
}
