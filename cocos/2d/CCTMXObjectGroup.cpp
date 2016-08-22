/****************************************************************************
Copyright (c) 2010      Neophit
Copyright (c) 2010      Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "2d/CCTMXObjectGroup.h"
#include "base/ccMacros.h"
#include "2d/CCTMXXMLParser.h"
#include "base/CCDirector.h"
#include "renderer/CCTextureCache.h"
#include "2d/CCTMXTiledMap.h"
#include "base/ccTypes.h"

NS_CC_BEGIN

//implementation TMXObject
TMXObject::TMXObject(ValueMap objectInfo)
{
    _objectName = objectInfo["name"].asString();
    _type = static_cast<TMXObjectType>(objectInfo["type"].asInt());
    _gid = objectInfo["gid"].isNull() ? 0 : objectInfo["gid"].asInt();
    _id = objectInfo["id"].asInt();
    _offset = Vec2(objectInfo["x"].asFloat(), objectInfo["y"].asFloat());
    _objectSize = Size(objectInfo["width"].asFloat(), objectInfo["height"].asFloat());
    _objectVisible = objectInfo["visible"].asBool();
    _objectRotation = objectInfo["rotation"].asFloat();
    setProperties(objectInfo);
}

TMXObject::~TMXObject()
{
    CCLOGINFO("deallocing TMXObject: %p", this);
}

Value TMXObject::getProperty(const std::string& propertyName) const
{
    if (_properties.find(propertyName) != _properties.end())
        return _properties.at(propertyName);
    
    return Value();
}

//implementation TMXObjectImage
TMXObjectImage::TMXObjectImage(const ValueMap& objectInfo, TMXMapInfo* mapInfo, const Size& groupSize)
: TMXObject(objectInfo)
, _groupSize(groupSize)
{
    _initWithMapInfo(mapInfo);
}

TMXObjectImage::~TMXObjectImage()
{
    CCLOGINFO("deallocing TMXObjectImage: %p", this);
}

bool TMXObjectImage::_initWithMapInfo(TMXMapInfo* mapInfo)
{
    if (_gid == 0) {
        return false;
    }
    
    TMXTilesetInfo* useTileset = nullptr;
    auto& tilesets = mapInfo->getTilesets();
    if (tilesets.size()>0)
    {
        TMXTilesetInfo* tileset = nullptr;
        for (auto iter = tilesets.crbegin(); iter != tilesets.crend(); ++iter)
        {
            tileset = *iter;
            if( (_gid & kTMXFlippedMask) >= tileset->_firstGid ) {
                useTileset = tileset;
                break;
            }
        }
    }
    
    if (useTileset == nullptr) {
        return false;
    }
    
    setVisible(_objectVisible);
    auto texture = Director::getInstance()->getTextureCache()->addImage(useTileset->_sourceImage);
    if (texture == nullptr)
    {
        return false;
    }
    auto rect = useTileset->getRectForGID(_gid);
    initWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(rect));
    setScale(_objectSize.width / rect.size.width, _objectSize.height / rect.size.height);
    setRotation(_objectRotation);

    setFlippedX(false);
    if (_gid & kTMXTileHorizontalFlag) {
        setFlippedX(true);
    }
    
    setFlippedY(false);
    if (_gid & kTMXTileVerticalFlag) {
        setFlippedY(true);
    }
    
    _initPosWithMapInfo(mapInfo);

    return true;
}

void TMXObjectImage::_initPosWithMapInfo(TMXMapInfo* mapInfo)
{
    auto mapOri = mapInfo->getOrientation();
    auto mapSize = mapInfo->getMapSize();
    auto tileSize = mapInfo->getTileSize();
    switch (mapOri) {
        case TMXOrientationOrtho:
        case TMXOrientationHex:
        case TMXOrientationStaggered:
            {
                Vec2 offsetInPoints = CC_POINT_PIXELS_TO_POINTS(_offset);
                setAnchorPoint(Vec2(0, 0));
                setPosition(Vec2(offsetInPoints.x, _groupSize.height - offsetInPoints.y));
            }
            break;
        case TMXOrientationIso:
            {
                setAnchorPoint(Vec2(0.5, 0));
                auto posIdx = Vec2(_offset.x / tileSize.width * 2, _offset.y / tileSize.height);
                auto pos = Vec2(tileSize.width / 2 * (mapSize.width + posIdx.x - posIdx.y),
                                tileSize.height / 2 * (mapSize.height * 2 - posIdx.x - posIdx.y));
                setPosition(CC_POINT_PIXELS_TO_POINTS(pos));
            }
            break;
        default:
            break;
    }
}

//implementation TMXObjectShape
TMXObjectShape::TMXObjectShape(const ValueMap& objectInfo, TMXMapInfo* mapInfo, const Size& groupSize, const Color3B& color)
: TMXObject(objectInfo)
, DrawNode(1)
{
    _mapInfo = mapInfo;
    _mapOrientation = mapInfo->getOrientation();
    _groupSize = groupSize;
    _groupColor = Color4F((float) color.r / 255, (float) color.g / 255, (float) color.b / 255, 1.0);
    _initShape(objectInfo);
}

TMXObjectShape::~TMXObjectShape() {
    CCLOGINFO("deallocing TMXObjectShape: %p", this);
}

void TMXObjectShape::_initShape(const ValueMap& objectInfo)
{
    DrawNode::init();
    
    Vec2 originPos;
    if (TMXOrientationIso != _mapOrientation) {
        Vec2 startPos = Vec2(0, _groupSize.height);
        Vec2 offsetInPoints = CC_POINT_PIXELS_TO_POINTS(_offset);
        originPos = Vec2(startPos.x + offsetInPoints.x, startPos.y - offsetInPoints.y);
    } else {
        originPos = _getPosByOffset(Vec2(0, 0));
    }
    setPosition(originPos);
    setRotation(_objectRotation);

    switch (_type) {
        case TMXObjectType::RECT:
            _drawRect();
            break;
        case TMXObjectType::ELLIPSE:
            _drawEllipse();
            break;
        case TMXObjectType::POLYGON:
            _drawPoly(objectInfo, originPos, true);
            break;
        case TMXObjectType::POLYLINE:
            _drawPoly(objectInfo, originPos, false);
            break;
        default:
            break;
    }
    setVisible(_objectVisible);
}

Vec2 TMXObjectShape::_getPosByOffset(const Vec2& offset)
{
    auto mapSize = _mapInfo->getMapSize();
    auto tileSize = _mapInfo->getTileSize();
    auto posIdx = Vec2((_offset.x + offset.x) / tileSize.width * 2, (_offset.y + offset.y) / tileSize.height);
    return CC_POINT_PIXELS_TO_POINTS(Vec2(tileSize.width / 2 * (mapSize.width + posIdx.x - posIdx.y),
                                     tileSize.height / 2 * (mapSize.height * 2 - posIdx.x - posIdx.y)));
}

void TMXObjectShape::_drawRect()
{
    if (TMXOrientationIso != _mapOrientation) {
        auto objSize = _objectSize;
        if (objSize.equals(Size::ZERO)) {
            objSize = Size(20, 20);
            setAnchorPoint(Vec2(0.5, 0.5));
        } else {
            setAnchorPoint(Vec2(0, 1));
        }
        Size objSizeInPoints = CC_SIZE_PIXELS_TO_POINTS(objSize);
        Vec2 bl = Vec2(0, 0);
        Vec2 tr = Vec2(objSizeInPoints.width, objSizeInPoints.height);
        drawRect(bl, tr, _groupColor);
        
        setContentSize(objSizeInPoints);
    } else {
        if (_objectSize.equals(Size::ZERO)) {
            return;
        }

        auto pos1 = _getPosByOffset(Vec2(0, 0));
        auto pos2 = _getPosByOffset(Vec2(_objectSize.width, 0));
        auto pos3 = _getPosByOffset(Vec2(_objectSize.width, _objectSize.height));
        auto pos4 = _getPosByOffset(Vec2(0, _objectSize.height));
        
        float width = pos2.x - pos4.x, height = pos1.y - pos3.y;
        setContentSize(Size(width, height));
        setAnchorPoint(Vec2((pos1.x - pos4.x) / width, 1));

        auto origin = Vec2(pos4.x, pos3.y);
        pos1.subtract(origin);
        pos2.subtract(origin);
        pos3.subtract(origin);
        pos4.subtract(origin);
        if (_objectSize.width > 0) {
            drawLine(pos1, pos2, _groupColor);
            drawLine(pos3, pos4, _groupColor);
        }
        
        if (_objectSize.height > 0) {
            drawLine(pos1, pos4, _groupColor);
            drawLine(pos3, pos2, _groupColor);
        }
    }
}

void TMXObjectShape::_drawEllipse()
{
    if (TMXOrientationIso != _mapOrientation) {
        auto objSize = _objectSize;
        if (objSize.equals(Size::ZERO)) {
            objSize = Size(20, 20);
            setAnchorPoint(Vec2(0.5, 0.5));
        } else {
            setAnchorPoint(Vec2(0, 1));
        }
        Size objSizeInPoints = CC_SIZE_PIXELS_TO_POINTS(objSize);
        
        float scaleX = 1.0, scaleY = 1.0, radius = 0.0;
        Vec2 center = Vec2(objSizeInPoints.width / 2, objSizeInPoints.height / 2);
        if (objSizeInPoints.width > objSizeInPoints.height) {
            scaleX = objSizeInPoints.width / objSizeInPoints.height;
            radius = objSizeInPoints.height / 2;
        } else {
            scaleY = objSizeInPoints.height / objSizeInPoints.width;
            radius = objSizeInPoints.width / 2;
        }
        drawCircle(center, radius, 0, 50, false, scaleX, scaleY, _groupColor);
        
        setContentSize(objSizeInPoints);
    } else {
        if (_objectSize.equals(Size::ZERO)) {
            return;
        }

        // draw the rect
        auto pos1 = _getPosByOffset(Vec2(0, 0));
        auto pos2 = _getPosByOffset(Vec2(_objectSize.width, 0));
        auto pos3 = _getPosByOffset(Vec2(_objectSize.width, _objectSize.height));
        auto pos4 = _getPosByOffset(Vec2(0, _objectSize.height));
        
        float width = pos2.x - pos4.x, height = pos1.y - pos3.y;
        setContentSize(Size(width, height));
        setAnchorPoint(Vec2((pos1.x - pos4.x) / width, 1));
        
        auto origin = Vec2(pos4.x, pos3.y);
        pos1.subtract(origin);
        pos2.subtract(origin);
        pos3.subtract(origin);
        pos4.subtract(origin);
        if (_objectSize.width > 0) {
            drawLine(pos1, pos2, _groupColor);
            drawLine(pos3, pos4, _groupColor);
        }
        
        if (_objectSize.height > 0) {
            drawLine(pos1, pos4, _groupColor);
            drawLine(pos3, pos2, _groupColor);
        }

        // add a drawnode to draw the ellipse
        Size objSizeInPoints = CC_SIZE_PIXELS_TO_POINTS(_objectSize);
        Vec2 center = _getPosByOffset(Vec2(_objectSize.width / 2, _objectSize.height / 2));
        center.subtract(origin);

        auto ellipseNode = DrawNode::create(_lineWidth);
        ellipseNode->setContentSize(Size(width, height));
        ellipseNode->setAnchorPoint(Vec2(0.5, 0.5));
        ellipseNode->setPosition(center);
        addChild(ellipseNode);
        
        float scaleX = 1.0, scaleY = 1.0, radius = 0.0;
        if (objSizeInPoints.width > objSizeInPoints.height) {
            scaleX = objSizeInPoints.width / objSizeInPoints.height;
            radius = objSizeInPoints.height / 2;
        } else {
            scaleY = objSizeInPoints.height / objSizeInPoints.width;
            radius = objSizeInPoints.width / 2;
        }
        auto tileSize = _mapInfo->getTileSize();
        float rotateDegree = atan(tileSize.width / tileSize.height);
        radius /= sin(rotateDegree);
        ellipseNode->drawCircle(center, radius, 0, 50, false, scaleX, scaleY, _groupColor);
        
        // should rotate the ellipse
        ellipseNode->setRotationSkewX(CC_RADIANS_TO_DEGREES(rotateDegree));
        ellipseNode->setRotationSkewY(90 - CC_RADIANS_TO_DEGREES(rotateDegree));
    }
}

void TMXObjectShape::_drawPoly(const ValueMap& objectInfo, const Vec2& originPos, bool isPolygon)
{
    // parse the data
    ValueVector pointsData;
    if (isPolygon)
        pointsData = objectInfo.at("points").asValueVector();
    else
        pointsData = objectInfo.at("polylinePoints").asValueVector();
    
    Vec2 * points = new Vec2[pointsData.size()];
    int idx = 0;
    float minX = 0, minY = 0, maxX = 0, maxY = 0;
    for (auto& pointData : pointsData) {
        auto pointDict = pointData.asValueMap();
        float x = pointDict["x"].asFloat();
        float y = pointDict["y"].asFloat();
        points[idx] = Vec2(x, y);
        minX = MIN(minX, points[idx].x);
        minY = MIN(minY, points[idx].y);
        maxX = MAX(maxX, points[idx].x);
        maxY = MAX(maxY, points[idx].y);
        idx++;
    }

    if (TMXOrientationIso != _mapOrientation) {
        // set the content size & anchor point
        float width = maxX - minX, height = maxY - minY;
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(Size(width, height)));
        setAnchorPoint(Vec2(-minX / width, maxY / height));

        // correct the points data
        for (int i = 0; i < idx; i++) {
            points[i] = CC_POINT_PIXELS_TO_POINTS(Vec2(points[i].x - minX, -points[i].y + maxY));
        }
        drawPoly(points, idx, isPolygon, _groupColor);
    } else {
        Vec2 bl = _getPosByOffset(Vec2(minX, maxY));
        Vec2 tr = _getPosByOffset(Vec2(maxX, minY));
        Vec2 origin = _getPosByOffset(Vec2(0 ,0));
        float width = tr.x - bl.x, height = tr.y - bl.y;
        setContentSize(Size(width, height));
        setAnchorPoint(Vec2((origin.x - bl.x) / width, (origin.y - bl.y) / height));
        
        // correct the points data
        for (int i = 0; i < idx; i++) {
            Vec2 tempPoint = _getPosByOffset(points[i]);
            points[i] = Vec2(tempPoint.x - bl.x, tempPoint.y - bl.y);
        }
        drawPoly(points, idx, isPolygon, _groupColor);
    }
    delete [] points;
}

//implementation TMXObjectGroup

TMXObjectGroup::TMXObjectGroup(TMXObjectGroupInfo* groupInfo, TMXMapInfo* mapInfo)
    : _groupName("")
{
    _initGroup(groupInfo, mapInfo);
}

TMXObjectGroup::~TMXObjectGroup()
{
    CCLOGINFO("deallocing TMXObjectGroup: %p", this);
}

TMXObject* TMXObjectGroup::getObject(const std::string& objectName)
{
    CCASSERT(objectName.size() > 0, "Invalid object name!");
    
    for (auto& child : _children)
    {
        TMXObject* object = dynamic_cast<TMXObject*>(child);
        if(object)
        {
            if(objectName.compare( object->getObjectName()) == 0)
            {
                return object;
            }
        }
    }
    
    // object not found
    return nullptr;
}

Vector<TMXObject*> TMXObjectGroup::getObjects()
{
    Vector<TMXObject*> objects;
    for (auto& child : _children)
    {
        TMXObject* object = dynamic_cast<TMXObject*>(child);
        if(object)
        {
            objects.pushBack(object);
        }
    }

    return objects;
}

Value TMXObjectGroup::getProperty(const std::string& propertyName) const
{
    if (_properties.find(propertyName) != _properties.end())
        return _properties.at(propertyName);

    return Value();
}

void TMXObjectGroup::_initGroup(TMXObjectGroupInfo* groupInfo, TMXMapInfo* mapInfo)
{
    _groupName = groupInfo->_groupName;
    _positionOffset = groupInfo->_positionOffset;
    _properties = groupInfo->getProperties();
    auto mapSize = mapInfo->getMapSize();
    auto tileSize = mapInfo->getTileSize();
    if (mapInfo->getOrientation() == TMXOrientationHex) {
        float width, height = 0;
        if (mapInfo->getStaggerAxis() == TMXStaggerAxis_X) {
            height = tileSize.height * (mapSize.height + 0.5);
            width = (tileSize.width + mapInfo->getHexSideLength()) * (int)(mapSize.width / 2) + tileSize.width * ((int)mapSize.width % 2);
        } else {
            width = tileSize.width * (mapSize.width + 0.5);
            height = (tileSize.height + mapInfo->getHexSideLength()) * (int)(mapSize.height / 2) + tileSize.height * ((int)mapSize.height % 2);
        }
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(Size(width, height)));
    }
    else if (mapInfo->getOrientation() == TMXOrientationStaggered)
    {
        float width, height = 0;
        if (mapInfo->getStaggerAxis() == TMXStaggerAxis_X) {
            height = tileSize.height * (mapSize.height + 0.5);
            width = (tileSize.width / 2) * (mapSize.width + 1);
        } else {
            width = tileSize.width * (mapSize.width + 0.5);
            height = (tileSize.height / 2) * (mapSize.height + 1);
        }
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(Size(width, height)));
    }
    else {
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(Size(mapSize.width * tileSize.width, mapSize.height * tileSize.height)));
    }
    setAnchorPoint(Vec2(0, 0));
    setPosition(Vec2(_positionOffset.x, -_positionOffset.y));
    setVisible(groupInfo->_visible);
    
    // create objects children
    auto objects = groupInfo->_objects;
    int idx = 0;
    for (auto& object : objects) {
        auto objectInfo = object.asValueMap();
        Node* obj = nullptr;
        if (objectInfo["type"].asInt() == static_cast<int>(TMXObjectType::IMAGE)) {
            obj = new TMXObjectImage(objectInfo, mapInfo, getContentSize());
        } else {
            obj = new TMXObjectShape(objectInfo, mapInfo, getContentSize(), groupInfo->_color);
        }
        obj->autorelease();
        obj->setOpacity(groupInfo->_opacity);
        addChild(obj, idx, idx);
        idx++;
    }
}

NS_CC_END
