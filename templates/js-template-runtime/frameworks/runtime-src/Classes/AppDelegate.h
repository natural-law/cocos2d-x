//
//  GCTestAppDelegate.h
//  GCTest
//
//  Created by Rohan Kuruvilla on 06/08/2012.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_


#include "ProjectConfig/ProjectConfig.h"
#include "ProjectConfig/SimulatorConfig.h"
#include "platform/CCApplication.h"
/**
 @brief    The cocos2d Application.
 
 The reason for implement as private inheritance is to hide some interface call by Director.
 */
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();
    
    void initGLContextAttrs() override;
    
    /**
     @brief    Implement Director and Scene init code here.
     @return true    Initialize success, app continue.
     @return false   Initialize failed, app terminate.
     */
    virtual bool applicationDidFinishLaunching();
    
    /**
     @brief  The function be called when the application enter background
     @param  the pointer of the application
     */
    virtual void applicationDidEnterBackground();
    
    /**
     @brief  The function be called when the application enter foreground
     @param  the pointer of the application
     */
    virtual void applicationWillEnterForeground();
    
    void setProjectConfig(const ProjectConfig& project);
    
    void reopenProject();
    
private:
    ProjectConfig _project;

    friend class StartupCall;
};


class StartupCall : public cocos2d::Ref
{
public:
    static StartupCall *create(AppDelegate *app);
    void startup();
    
private:
    StartupCall();
    
    void trackEvent(const char *eventName);
    void trackLaunchEvent();
    
    void onPreviewJs(const std::string &path);
    
    void updateConfigParser(const ProjectConfig& project);
    void updatePreviewFuncForPath(const std::string &path);
    
private:
    AppDelegate *_app;
    std::function<void(const std::string &)> _previewFunc;
    std::string _launchEvent;
};

#endif  // __APP_DELEGATE_H__

