#include "AppDelegate.h"

#include "SimpleAudioEngine.h"
#include "jsb_cocos2dx_auto.hpp"
#include "jsb_cocos2dx_ui_auto.hpp"
#include "jsb_cocos2dx_studio_auto.hpp"
#include "jsb_cocos2dx_builder_auto.hpp"
#include "jsb_cocos2dx_spine_auto.hpp"
#include "jsb_cocos2dx_extension_auto.hpp"
#include "ui/jsb_cocos2dx_ui_manual.h"
#include "cocostudio/jsb_cocos2dx_studio_manual.h"
#include "cocosbuilder/js_bindings_ccbreader.h"
#include "spine/jsb_cocos2dx_spine_manual.h"
#include "extension/jsb_cocos2dx_extension_manual.h"
#include "localstorage/js_bindings_system_registration.h"
#include "chipmunk/js_bindings_chipmunk_registration.h"
#include "jsb_opengl_registration.h"
#include "network/XMLHTTPRequest.h"
#include "network/jsb_websocket.h"
#include "network/jsb_socketio.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/CCJavascriptJavaBridge.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#include "platform/ios/JavaScriptObjCBridge.h"
#endif

#include "CodeIDESupport.h"
#include "Runtime.h"
#include "ConfigParser.h"

#include "cocostudio/CocoStudio.h"

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC))
#include "service/DeviceEx.h"
#include "network/CCHTTPRequest.h"
#endif

using namespace CocosDenshion;

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
	SimpleAudioEngine::end();
    ScriptEngineManager::destroyInstance();
#if (CC_CODE_IDE_DEBUG_SUPPORT > 0)
	// NOTE:Please don't remove this call if you want to debug with Cocos Code IDE
	endRuntime();
#endif

	ConfigParser::purge();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    //
#if ((CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC) && (CC_CODE_IDE_DEBUG_SUPPORT > 0))
    _project.setDebuggerType(kCCRuntimeDebuggerCodeIDE);
    
#endif
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();    
    if(!glview) {
        Size viewSize = ConfigParser::getInstance()->getInitViewSize();
        string title = ConfigParser::getInstance()->getInitViewName();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC) && (CC_CODE_IDE_DEBUG_SUPPORT > 0)
#else
        glview = cocos2d::GLViewImpl::createWithRect(title.c_str(), Rect(0, 0, viewSize.width, viewSize.height));
        director->setOpenGLView(glview);
#endif
    }

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    ScriptingCore* sc = ScriptingCore::getInstance();
    sc->addRegisterCallback(register_all_cocos2dx);
    sc->addRegisterCallback(register_cocos2dx_js_core);
    sc->addRegisterCallback(register_cocos2dx_js_extensions);
    sc->addRegisterCallback(jsb_register_system);

    // extension can be commented out to reduce the package
    sc->addRegisterCallback(register_all_cocos2dx_extension);
    sc->addRegisterCallback(register_all_cocos2dx_extension_manual);

    // chipmunk can be commented out to reduce the package
    sc->addRegisterCallback(jsb_register_chipmunk);
    // opengl can be commented out to reduce the package
    sc->addRegisterCallback(JSB_register_opengl);
    
    // builder can be commented out to reduce the package
    sc->addRegisterCallback(register_all_cocos2dx_builder);
    sc->addRegisterCallback(register_CCBuilderReader);
    
    // ui can be commented out to reduce the package, attension studio need ui module
    sc->addRegisterCallback(register_all_cocos2dx_ui);
    sc->addRegisterCallback(register_all_cocos2dx_ui_manual);

    // studio can be commented out to reduce the package, 
    sc->addRegisterCallback(register_all_cocos2dx_studio);
    sc->addRegisterCallback(register_all_cocos2dx_studio_manual);
    
    // spine can be commented out to reduce the package
    sc->addRegisterCallback(register_all_cocos2dx_spine);
    sc->addRegisterCallback(register_all_cocos2dx_spine_manual);
    
    // XmlHttpRequest can be commented out to reduce the package
    sc->addRegisterCallback(MinXmlHttpRequest::_js_register);
    // websocket can be commented out to reduce the package
    sc->addRegisterCallback(register_jsb_websocket);
    // sokcet io can be commented out to reduce the package
    sc->addRegisterCallback(register_jsb_socketio);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    sc->addRegisterCallback(JavascriptJavaBridge::_js_register);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS|| CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    sc->addRegisterCallback(JavaScriptObjCBridge::_js_register);
#endif
    
    StartupCall *call = StartupCall::create(this);
    call->startup();
    
    cocos2d::log("iShow!");
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    auto director = Director::getInstance();
    director->stopAnimation();
    director->getEventDispatcher()->dispatchCustomEvent("game_on_hide");
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();    
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    auto director = Director::getInstance();
    director->startAnimation();
    director->getEventDispatcher()->dispatchCustomEvent("game_on_show");
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
}

void AppDelegate::setProjectConfig(const ProjectConfig& project)
{
    _project = project;
}

void AppDelegate::reopenProject()
{
    auto fileUtils = FileUtils::getInstance();
    
    //
    // set root path
    // set search root **MUST** before set search paths
    //
    fileUtils->setDefaultResourceRootPath(_project.getProjectDir());
    
    // clean
    Director::getInstance()->getTextureCache()->removeAllTextures();
    Director::getInstance()->purgeCachedData();
    SimpleAudioEngine::getInstance()->stopAllEffects();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);
    vector<string> searchPaths;
    fileUtils->setSearchPaths(searchPaths);
    
    const string writablePath = _project.getWritableRealPath();
    if (writablePath.length())
    {
        FileUtils::getInstance()->setWritablePath(writablePath.c_str());
    }
    
    resetDesignResolution();
    
    StartupCall *call = StartupCall::create(this);
    call->startup();
}

// ----------------------------------------

StartupCall *StartupCall::create(AppDelegate *app)
{
    StartupCall *call = new StartupCall();
    call->_app = app;
    call->autorelease();
    return call;
}

StartupCall::StartupCall()
: _launchEvent("empty")
{
}

static bool endWithString(const std::string &buf, const std::string &suffix)
{
    return ((buf.find(suffix) + suffix.length()) == buf.length());
}

void StartupCall::startup()
{
    const ProjectConfig &project = _app->_project;
    
    // set search path
    string path = FileUtils::getInstance()->fullPathForFilename(project.getScriptFileRealPath().c_str());
    size_t pos;
    while ((pos = path.find_first_of("\\")) != std::string::npos)
    {
        path.replace(pos, 1, "/");
    }
    size_t p = path.find_last_of("/");
    string workdir;
    if (p != path.npos)
    {
        workdir = path.substr(0, p);
        FileUtils::getInstance()->addSearchPath(workdir);
    }
    
    // update search pathes
    FileUtils::getInstance()->addSearchPath(project.getProjectDir());
    auto &customizedPathes = project.getSearchPath();
    for (auto &path : customizedPathes)
    {
        FileUtils::getInstance()->addSearchPath(path);
    }
    
    updateConfigParser(project);
    if (FileUtils::getInstance()->isFileExist(path))
    {
        updatePreviewFuncForPath(path);
        
        // launch
        if (project.getDebuggerType() == kCCRuntimeDebuggerNone)
        {
            _previewFunc(path);
        }
        else
        {
            // NOTE:Please don't remove this call if you want to debug with Cocos Code IDE
            initRuntime(project.getProjectDir());
            startRuntime();
        }
    }
    else
    {
        CCLOG("[ERROR]: %s is not exist.", path.c_str());
    }
    
    // track start event
    trackLaunchEvent();
}

// *NOTE*
// track event on windows / mac platform
//
void StartupCall::trackEvent(const char *eventName)
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC))
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    const char *platform = "win";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    const char *platform = "mac";
#else
    const char *platform = "UNKNOWN";
#endif
    
    auto request = extra::HTTPRequest::createWithUrl(NULL,
                                                     "http://www.google-analytics.com/collect",
                                                     kCCHTTPRequestMethodPOST);
    request->addPOSTValue("v", "1");
    request->addPOSTValue("tid", "UA-55061270-1");
    request->addPOSTValue("cid", player::DeviceEx::getInstance()->getUserGUID().c_str());
    request->addPOSTValue("t", "event");
    
    request->addPOSTValue("an", "simulator");
    request->addPOSTValue("av", cocos2dVersion());
    
    request->addPOSTValue("ec", platform);
    request->addPOSTValue("ea", eventName);
    
    request->start();
    
#endif // ((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC))
}

void StartupCall::trackLaunchEvent()
{
    trackEvent(_launchEvent.c_str());
}

void StartupCall::onPreviewCocosCSD(const std::string &path)
{
    std::string filepath = path;
    if (filepath.empty())
    {
        filepath = ConfigParser::getInstance()->getEntryFile();
    }
    
    CCLOG("------------------------------------------------");
    CCLOG("LOAD Cocos Studio FILE (.csd): %s", filepath.c_str());
    CCLOG("------------------------------------------------");
    
    auto node = CSLoader::getInstance()->createNodeWithFlatBuffersForSimulator(filepath.c_str());
    auto action = cocostudio::timeline::ActionTimelineCache::getInstance()->createActionWithFlatBuffersForSimulator(filepath.c_str());
    if (action)
    {
        node->runAction(action);
        action->gotoFrameAndPlay(0);
    }
    
    if (node)
    {
        if (Director::getInstance()->getRunningScene())
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->replaceScene(scene);
        }
        else
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->runWithScene(scene);
        }
    }
}

void StartupCall::onPreviewCocosCSB(const std::string &path)
{
    std::string filepath = path;
    if (filepath.empty())
    {
        filepath = ConfigParser::getInstance()->getEntryFile();
    }
    CCLOG("\n------------------------------------------------\n");
    CCLOG("[WARNING]: using **SUITABLE** Cocos Studio generate csb file!!");
    CCLOG("LOAD Cocos Studio FILE (.csb): %s", filepath.c_str());
    CCLOG("\n------------------------------------------------\n");
    
    auto node = CSLoader::getInstance()->createNode(filepath);
    if (node)
    {
        if (Director::getInstance()->getRunningScene())
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->replaceScene(scene);
        }
        else
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->runWithScene(scene);
        }
    }
}

void StartupCall::onPreviewJs(const std::string &path)
{
    std::string filepath = path;
    if (filepath.empty())
    {
        filepath = ConfigParser::getInstance()->getEntryFile();
    }
    CCLOG("------------------------------------------------");
    CCLOG("LOAD Js FILE: %s", filepath.c_str());
    CCLOG("------------------------------------------------");
    
    ScriptingCore* sc = ScriptingCore::getInstance();
    sc->start();
    sc->runScript("script/jsb_boot.js");
    auto engine = ScriptingCore::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    ScriptingCore::getInstance()->runScript(filepath.c_str());
}

void StartupCall::updateConfigParser(const ProjectConfig& project)
{
    // set entry file
    auto parser = ConfigParser::getInstance();
    string entryFile(project.getScriptFileRealPath());
    if (entryFile.find(project.getProjectDir()) != string::npos)
    {
        entryFile.erase(0, project.getProjectDir().length());
    }
    entryFile = replaceAll(entryFile, "\\", "/");
    parser->setEntryFile(entryFile);
    
    parser->setBindAddress(project.getBindAddress());
}

void StartupCall::updatePreviewFuncForPath(const std::string &path)
{
    // set loader
    _previewFunc = [](const std::string &path) { CCLOG("[WARNING]: unsupport %s", path.c_str()); };

    if (!FileUtils::getInstance()->isFileExist(path))
    {
        CCLOG("[ERROR]: %s is not exist.", path.c_str());
        return ;
    }
    
    if (endWithString(path, ".csd"))
    {
        _launchEvent = "ccs";
        _previewFunc = std::bind(&StartupCall::onPreviewCocosCSD, this, std::placeholders::_1);
    }
    else if (endWithString(path, ".csb"))
    {
        _launchEvent = "ccs";
        _previewFunc = std::bind(&StartupCall::onPreviewCocosCSB, this, std::placeholders::_1);
    }
    else if (endWithString(path, ".js"))
    {
        _launchEvent = "js";
        _previewFunc = std::bind(&StartupCall::onPreviewJs, this, std::placeholders::_1);
    }
}

