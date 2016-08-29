/*******************************************************************************
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef _DRAGONBONES_TEST_H_
#define _DRAGONBONES_TEST_H_

#include "cocos2d.h"
#include "../BaseTest.h"

DEFINE_TEST_SUITE(DragonBonesTests);

class DragonBonesTestBase : public TestCase
{
public:
    virtual std::string title() const override
    {
        return "DragonBones Test";
    }
    virtual bool init () override;
    
    virtual void addTestLayer() = 0;
};

class HelloDragonLayer : public DragonBonesTestBase
{
public:
    virtual std::string subtitle() const override
    {
        return "Hello DragonBones";
    }
    virtual void addTestLayer() override;

    CREATE_FUNC (HelloDragonLayer);
};

class AnimationBaseLayer : public DragonBonesTestBase
{
public:
    virtual std::string subtitle() const override
    {
        return "Animation Base";
    }
    virtual void addTestLayer() override;
    
    CREATE_FUNC (AnimationBaseLayer);
};

class KnightLayer : public DragonBonesTestBase
{
public:
    virtual std::string subtitle() const override
    {
        return "Knight";
    }
    virtual void addTestLayer() override;
    
    CREATE_FUNC (KnightLayer);
};

class CoreElementLayer : public DragonBonesTestBase
{
public:
    virtual std::string subtitle() const override
    {
        return "Core Element";
    }
    virtual void addTestLayer() override;
    
    CREATE_FUNC (CoreElementLayer);
};

class DragonPerformanceLayer : public DragonBonesTestBase
{
public:
    virtual std::string subtitle() const override
    {
        return "Performance Test";
    }
    virtual void addTestLayer() override;

    CREATE_FUNC (DragonPerformanceLayer);
};

#endif // _DRAGONBONES_TEST_H_
