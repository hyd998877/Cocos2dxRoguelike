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
    
    std::vector<std::vector<int>> tileIdArray(MAP_SIZE_WIDTH);
    for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
        tileIdArray[x] = std::vector<int>(MAP_SIZE_HEIGHT);
        for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
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
        mapNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
        mapNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
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
            backgroundLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
            backgroundLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
            {
                auto dataNode = backgroundLayerNode.append_child("data");
                for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
                    for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
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
            colisionLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
            colisionLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
            {
                auto dataNode = colisionLayerNode.append_child("data");
                for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
                    for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
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
                floorLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
                floorLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
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
                    for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
                        for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
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
    auto mapManager = MapManager();
    mapManager.initMapping(0, MAP_SIZE_HEIGHT, 0, MAP_SIZE_WIDTH);
    
    TMXLayerData::MapIndex FLOOR_BASE_INDEX[2][4] = {
        {{ 0,              0}, {ONE_FLOOR_SIZE,              0}, {ONE_FLOOR_SIZE*2,              0}, {ONE_FLOOR_SIZE*3,              0}},
        {{ 0, ONE_FLOOR_SIZE}, {ONE_FLOOR_SIZE, ONE_FLOOR_SIZE}, {ONE_FLOOR_SIZE*2, ONE_FLOOR_SIZE}, {ONE_FLOOR_SIZE*3, ONE_FLOOR_SIZE}}
    };
    
    std::list<TMXLayerData> floorLayerList;
    
    const int FLOOR_SIZE_MAX = 8;
    for (int floorIdx = 0; floorIdx < FLOOR_SIZE_MAX; floorIdx++) {
        TMXLayerData layerData;
        layerData._no = floorIdx + 1;
        
        // TODO: 座標決め
        int randX = 1;
        int randY = 1;
        
        int x = floorIdx % 4;
        int y = floorIdx / 4;
        CCLOG("x = %d y = %d", x, y);
        layerData._x = randX + FLOOR_BASE_INDEX[y][x]._x;
        layerData._y = randY + FLOOR_BASE_INDEX[y][x]._y;
        
        // TODO: サイズ決め
        int randWidth = GetRandom(5, ONE_FLOOR_SIZE - randX);
        int randHeight = GetRandom(5, ONE_FLOOR_SIZE - randY);
        
        layerData._width = randWidth;
        layerData._height = randHeight;

        for (int x = layerData._x; x < layerData._width; x++) {
            for (int y = layerData._y; y < layerData._height; y++) {
                mapManager.addObstacle(MapIndex{x, y, MoveDirectionType::MOVE_NONE});
            }
        }
        
        // TODO: 通路口を決める
        std::list<TMXLayerData::MapIndex> gateList;
        
        // 4方向すべて抽選する
        // ただし壁際の場合は通路作成を無効化
        //  x <= 1 || y <= 1 || x >= MaxX -1 || y >= MaxY -1
        // また、部屋の角の場合も通路作成を無効化
        // 下
        {
            int gateX = layerData._x + layerData._width / 2;
            int gateY = layerData._y + layerData._height - 1;
            
            CCLOG("gateX = %d gateY = %d", gateX, gateY);
            if (!layerData.isKado(gateX, gateY) && isMapInLine(gateX, gateY)) {
                gateList.push_back(TMXLayerData::MapIndex{gateX, gateY});
                CCLOG("gate In!!!");
                mapManager.removeMapItem(MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY));
                auto mapItem = MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY);
                mapItem.mapDataType = MapDataType::PLAYER;
                mapManager.addActor(mapItem);
            }
        }
        // 上
        {
            int gateX = layerData._x + layerData._width / 2;
            int gateY = layerData._y;
            
            CCLOG("gateX = %d gateY = %d", gateX, gateY);
            if (!layerData.isKado(gateX, gateY) && isMapInLine(gateX, gateY)) {
                gateList.push_back(TMXLayerData::MapIndex{gateX, gateY});
                CCLOG("gate In!!!");
                mapManager.removeMapItem(MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY));
                auto mapItem = MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY);
                mapItem.mapDataType = MapDataType::PLAYER;
                mapManager.addActor(mapItem);
            }
        }
        // 左
        {
            int gateX = layerData._x;
            int gateY = layerData._y + layerData._height / 2;
            
            CCLOG("gateX = %d gateY = %d", gateX, gateY);
            if (!layerData.isKado(gateX, gateY) && isMapInLine(gateX, gateY)) {
                gateList.push_back(TMXLayerData::MapIndex{gateX, gateY});
                CCLOG("gate In!!!");
                mapManager.removeMapItem(MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY));
                auto mapItem = MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY);
                mapItem.mapDataType = MapDataType::PLAYER;
                mapManager.addActor(mapItem);
            }
        }
        // 右
        {
            int gateX = layerData._x + layerData._width -1;
            int gateY = layerData._y + layerData._height / 2;
            
            CCLOG("gateX = %d gateY = %d", gateX, gateY);
            if (!layerData.isKado(gateX, gateY) && isMapInLine(gateX, gateY)) {
                gateList.push_back(TMXLayerData::MapIndex{gateX, gateY});
                CCLOG("gate In!!!");
                mapManager.removeMapItem(MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY));
                auto mapItem = MapManager::createNoneMapItem<ActorMapItem>(gateX, gateY);
                mapItem.mapDataType = MapDataType::PLAYER;
                mapManager.addActor(mapItem);
            }
        }
        
        layerData._gateWayMapIndexList = gateList;
        
        floorLayerList.push_back(layerData);
    }
    
    // 最短経路の通路を作成
    
    std::list<TMXLayerData::MapIndex> walkMapIndexList;
    
    auto actorMapList = mapManager.findActorMapItem();
    for (auto mapItem : actorMapList) {
        CCLOG("mapItem x = %d y = %d ->", mapItem.mapIndex.x, mapItem.mapIndex.y);
        
        std::list<MapIndex> moveMapIndexList;
        mapManager.createActorFindDist(mapItem.mapIndex, 10);
        for (auto targetMapItem : actorMapList) {
            if (MapManager::equalMapItem(targetMapItem, mapItem)) {
                // 自分
                //CCLOG("equal targetMapItem x = %d y = %d ->", targetMapItem.mapIndex.x, targetMapItem.mapIndex.y);
                continue;
            }
            std::list<MapIndex> searchMapIndexList = MapManager::createRelatedMapIndexList(targetMapItem.mapIndex);
            MapItem targetMoveDistMapItem = mapManager.searchTargetMapItem(searchMapIndexList);
            //CCLOG("target x =%d y = %d", targetMoveDistMapItem.mapIndex.x, targetMoveDistMapItem.mapIndex.y);
            auto targetMoveMapIndexList = mapManager.createMovePointList(targetMoveDistMapItem.mapIndex, mapItem);
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
                moveMapIndexList = targetMoveMapIndexList;
            }
        }
        for (auto mapIndex : moveMapIndexList) {
            CCLOG("move x=%d y=%d", mapIndex.x, mapIndex.y);
            walkMapIndexList.push_back(TMXLayerData::MapIndex{mapIndex.x, mapIndex.y});
        }
    }
    
    return TMXMapData{floorLayerList, walkMapIndexList};
}

// ////////////////////////////////////////////////////////////
// TODO: 未使用
typedef std::vector<std::vector<int>> FloorMap;
typedef std::vector<std::vector<FloorMap>> MapData;

void TMXGenerator::makeMapData()
{
#if 0
    // 2 x 4 = 8区画（1区画 Max 10x10マス）

    ///////////// init /////////////
    
    const int MAP_Y_NUM = 2;
    const int MAP_X_NUM = 4;
    const int FLOOR_Y_NUM = 10;
    const int FLOOR_X_NUM = 10;
    
    MapData mapData(MAP_Y_NUM);
    for (int map_y = 0; map_y < MAP_Y_NUM; map_y++) {
        std::vector<FloorMap> floorMapArray(MAP_X_NUM);
        for (int map_x = 0; map_x < MAP_X_NUM; map_x++) {
            FloorMap floorMap(FLOOR_Y_NUM);
            for (int y = 0; y < FLOOR_Y_NUM; y++) {
                std::vector<int> floorArray(FLOOR_X_NUM);
                for (int x = 0; x < FLOOR_X_NUM; x++) {
                    floorArray[x] = 0;
                }
                floorMap[y] = floorArray;
            }
            floorMapArray[map_x] = floorMap;
        }
        mapData[map_y] = floorMapArray;
    }
    
    //////////// create ///////////////
    // 0 ... 壁　真っ黒
    // 1 ... 床
    
    // TODO: 部屋の開始位置をランダムに決める左上を起点とする
    int startX = 1;
    int startY = 1;
    
    // TODO: 最大9x9の部屋のランダム（起点が1,1なら9x9）
    // floorSize - 起点 = maxSize
    int size_x = 9;
    int size_y = 9;
    
    for (int map_y = 0; map_y < MAP_Y_NUM; map_y++) {
        for (int map_x = 0; map_x < MAP_X_NUM; map_x++) {
            for (int y = startY; y < size_y; y++) {
                for (int x = startX; x < size_x; x++) {
                    // 最初と最後は壁
                    if (x == startX || x == (size_x -1)) {
                        mapData[map_y][map_x][y][x] = 2;
                    } else {
                        mapData[map_y][map_x][y][x] = 1;
                    }
                }
            }
        }
    }
    ///////// Log //////////////
    
    for (int map_y = 0; map_y < MAP_Y_NUM; map_y++) {
        
        std::vector<std::string> temp_x(FLOOR_Y_NUM);
        
        for (int map_x = 0; map_x < MAP_X_NUM; map_x++) {

            for (int y = 0; y < FLOOR_Y_NUM; y++) {
                std::string temp;
                for (int x = 0; x < FLOOR_X_NUM; x++) {
                    temp.append(std::to_string(mapData[map_y][map_x][y][x]) + " ");
                    
                }
                temp_x[y].append(temp);
                temp_x[y].append(" ");
            }
        }
        for (auto temp : temp_x) {
            printf("%s\n", temp.c_str());
        }
        printf("\n");
    }
#endif
}







