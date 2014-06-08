//
//  Cocos2dRogueLike_iOS_Tests.m
//  Cocos2dRogueLike iOS Tests
//
//  Created by kyokomi on 2014/06/07.
//
//

#import <XCTest/XCTest.h>
#include "TMXGenerator.h"

#include "cocos2d.h"
#include "CCTMXTiledMap.h"

USING_NS_CC;

@interface Cocos2dRogueLike_iOS_Tests : XCTestCase

@end

@implementation Cocos2dRogueLike_iOS_Tests

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExample
{
    auto tmxString = TMXGenerator::generator();
    printf("%s", tmxString.c_str());
//    auto tmxTiledMap = TMXTiledMap::createWithXML(tmxString.c_str(), "tmx");
//    printf("%s", tmxTiledMap->getDescription().c_str());
    
//    XCTFail(@"No implementation for \"%s\"", __PRETTY_FUNCTION__);
}

@end
