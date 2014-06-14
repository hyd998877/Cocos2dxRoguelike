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
    
    const size_t FLOOR_Y_COUNT = config.baseMapLayout.size();
    const size_t FLOOR_X_COUNT = config.baseMapLayout[0].size();
    const size_t FLOOR_SIZE_MAX = FLOOR_Y_COUNT * FLOOR_X_COUNT;
    for (int floorIdx = 0; floorIdx < FLOOR_SIZE_MAX; floorIdx++) {
        TMXLayerData layerData;
        layerData._no = floorIdx + 1;
        
        // 座標決め
        int randX = GetRandom(1, 3);
        int randY = GetRandom(1, 3);
        
        int x = floorIdx % FLOOR_X_COUNT;
        int y = floorIdx / FLOOR_X_COUNT;
        CCLOG("x = %d y = %d", x, y);
        layerData._x = randX + config.baseMapLayout[y][x].mapIndex._x;
        layerData._y = randY + config.baseMapLayout[y][x].mapIndex._y;
        
        // 通路が作れないケースがあるので-1引く
        int randWidthMax = config.baseMapLayout[y][x].mapSize._width - randX;
        int randHeightMax = config.baseMapLayout[y][x].mapSize._height - randY;
        if (randWidthMax <= 5 || randHeightMax <= 5) {
            // 5以下の部屋は狭すぎるのでナシ
            continue;
        }
        // サイズ決め（最小サイズはMaxとギリギリサイズ5の中間値で出来るだけ大きくする）
        int randWidth = GetRandom((randWidthMax + 5) / 2, randWidthMax-1);
        int randHeight = GetRandom((randHeightMax + 5) / 2, randHeightMax-1);
        
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
        
        // ふさがっていたら探索しない
        if (mapManager.getActorMapItem(mapItem.mapIndex).mapDataType != MapDataType::PLAYER) {
            continue;
        }
        
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
        
        // つないだ通路は埋める
        mapManager.removeMapItem(mapItem);
    }
    
    // 通路を作れなかった通路口を閉じる
    auto it = floorLayerList.begin();
    while (it != floorLayerList.end()) {
        for (auto closeMapIndex : closeMapIndexList) {
            (*it)._gateWayMapIndexList.remove_if([closeMapIndex](TMXLayerData::MapIndex mapIndex) -> bool {
                if (closeMapIndex._x == mapIndex._x && closeMapIndex._y == mapIndex._y) {
                    return true;
                }
                return false;
            });
        }
        it++;
    }
    
    return TMXMapData{config, floorLayerList, walkMapIndexList};
}

// private
TMXCreateBaseConfig TMXGenerator::createTMXCreateConfig()
{
    // TODO: とりあえず固定40x20 10x10 最低4x2フロア数
    int mapWidth    = GetRandom(4, 8) * 10;
    int mapHeight   = GetRandom(2, 3) * 10;
    int floorWidth  = 10;
    int floorHeight = 10;
    
    int floorCountY = mapHeight / floorHeight;
    int floorCountX = mapWidth / floorWidth;
    
    // 1フロア基準でデータ作成
    std::vector<std::vector<TMXLayerData::MapSize>> randomSizeArray(floorCountY);
    for (int y = 0; y < floorCountY; y++) {
        std::vector<TMXLayerData::MapSize> mapSizeArray(floorCountX);
        for (int x = 0; x < floorCountX; x++) {
            mapSizeArray[x] = TMXLayerData::MapSize{1, 1};
        }
        randomSizeArray[y] = mapSizeArray;
    }
    
    // ランダムに部屋をつなげる
    for (int y = 0; y < floorCountY; y++) {
        for (int x = 0; x < floorCountX; x++) {
            // すでに拡張で潰された部屋はスルーするー
            if (randomSizeArray[y][x]._width == 0 || randomSizeArray[y][x]._height == 0) {
                continue;
            }
            
            // 拡張可能なサイズを計測する
            int xSize = 0;
            for (int x2 = x; x2 < floorCountX; x2++) {
                if (randomSizeArray[y][x2]._width == 0 || randomSizeArray[y][x2]._height == 0) {
                    break;
                }
                xSize++;
            }
            int ySize = 0;
            for (int y2 = y; y2 < floorCountY; y2++) {
                if (randomSizeArray[y2][x]._width == 0 || randomSizeArray[y2][x]._height == 0) {
                    break;
                }
                ySize++;
            }
            
            // 表現可能な範囲でランダムにサイズを取る
            int randW = GetRandom(1, xSize);
            int randH = GetRandom(1, ySize);
            
            // 横か縦どちらかが拡張される場合のみ処理する
            if (randW > 1 || randH > 1) {
                // 拡張分の部屋をすべて0で埋める
                for (int yy = y; yy < (y + randH); yy++) {
                    for (int xx = x; xx < (x + randW); xx++) {
                        randomSizeArray[yy][xx]._width = 0;
                        randomSizeArray[yy][xx]._height = 0;
                    }
                }
                // 拡張対象だけ再設定
                randomSizeArray[y][x]._width = randW;
                randomSizeArray[y][x]._height = randH;
            }
        }
    }
    
    // ランダムデータを元にマップの部屋レイアウトを作成
    TMXCreateBaseConfig::BaseMapLayout layout(floorCountY);
    for (int y = 0; y < floorCountY; y++) {
        
        std::vector<TMXFloorConfig> configVector(floorCountX);
        for (int x = 0; x < floorCountX; x++) {
            TMXLayerData::MapIndex mapIndex{floorWidth*x, floorHeight*y};
            TMXLayerData::MapSize mapSize{floorWidth, floorHeight};
            
            TMXLayerData::MapSize floorLayoutSize = randomSizeArray[y][x];
            if (floorLayoutSize._width == 0 || floorLayoutSize._height == 0) {
                mapSize._width  = 0;
                mapSize._height = 0;
            } else {
                mapSize._width  *= floorLayoutSize._width;
                mapSize._height *= floorLayoutSize._height;
            }
            
            configVector[x] = TMXFloorConfig {mapIndex, mapSize};
        }
        layout[y] = configVector;
    }
    
    return TMXCreateBaseConfig {mapWidth, mapHeight, layout};
}

bool TMXGenerator::addFloorGate(MapManager* mapManager,
                                const TMXCreateBaseConfig& config,
                                const TMXLayerData& layerData,
                                const TMXLayerData::MapIndex& gateMapIndex)
{
    if (!layerData.isKado(gateMapIndex._x, gateMapIndex._y) && isMapInLine(config, gateMapIndex._x, gateMapIndex._y)) {
        auto obj = MapManager::createNoneMapItem<ActorMapItem>(gateMapIndex._x, gateMapIndex._y);
        obj.mapDataType = MapDataType::OBSTACLE;
        mapManager->removeMapItem(obj);
        auto mapItem = MapManager::createNoneMapItem<ActorMapItem>(gateMapIndex._x, gateMapIndex._y);
        mapItem.mapDataType = MapDataType::PLAYER;
        mapItem.seqNo = layerData._no; // フロアNoを持たせて管理する
        mapManager->addActor(mapItem);
        
        return true;
    }
    return false;
}

std::list<MapIndex> TMXGenerator::createWalkMapIndexList(MapManager* mapManager, const TMXCreateBaseConfig& config, const MapItem& baseMapItem)
{
    std::list<MapIndex> moveMapIndexList;
    // TODO: 通路口の探索範囲10マス以内
    mapManager->createActorFindDist(baseMapItem.mapIndex, 15);
    mapManager->showDebug();
    
    for (auto targetMapItem : mapManager->findActorMapItem()) {
        if (MapManager::equalMapItem(targetMapItem, baseMapItem)) {
            // 自分
            //CCLOG("equal targetMapItem x = %d y = %d ->", targetMapItem.mapIndex.x, targetMapItem.mapIndex.y);
            continue;
        }
        if (baseMapItem.seqNo == targetMapItem.seqNo) {
            // 同じフロア
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
                // つないだ通路は埋めるので保持する
                moveTargetMapItem = targetMapItem;
            }
        }
    }

    // つないだ通路は埋める
    if (moveTargetMapItem.mapDataType == PLAYER) {
        mapManager->removeMapItem(moveTargetMapItem);
    }
    
    return moveMapIndexList;
}
