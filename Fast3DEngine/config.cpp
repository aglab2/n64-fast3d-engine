#include "config.h"

#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Windows.h>

#include "os.h"
#include "plugin.h"

Config::Config()
{
    std::filesystem::path exePath(OS::ExecutablePath());
    auto exeDir = exePath.parent_path();
    
    auto configDir = exeDir / "Config";
    if (!std::filesystem::exists(configDir))
        std::filesystem::create_directory(configDir);

    auto configFile = configDir / "f3d.yaml";
    configPath_ = configFile.string();
    //if (std::filesystem::exists(configFile))
        read(configFile.u8string());
    //else
    //    write(configFile.u8string());
}

VsyncMode Config::toVsyncMode(const std::string& mode)
{
    try
    {
        return (VsyncMode)std::stoul(mode);
    }
    catch (...) { }

    if ("auto" == mode)
        return VsyncMode::AUTOMATIC;
    
    return VsyncMode::DISABLED;
}

bool Config::read(const std::string& p)
try
{
    YAML::Node config = YAML::LoadFile(p);

    width_ = config["width"].as<int>();
    height_ = config["height"].as<int>();
    vsyncMode_ = toVsyncMode(config["vsync"].as<std::string>());
    reducedLatency_ = config["reducedLatency"].as<std::string>() == "allow" ? true : false;
    try
    {
        nerfFogFactor_ = config["nerfFog"].as<float>();
    }
    catch(...) { }
    try
    {
        shadowBias_ = config["shadowBias"].as<float>();
    }
    catch (...) {}
    try
    {
        captureFrames_ = config["captureFrames"].as<bool>();
    }
    catch (...) {}
    try
    {
        fullScreenWidth_ = config["fullScreenWidth"].as<int>();
    }
    catch (...) 
    {
        fullScreenWidth_ = width_;
    }
    try
    {
        fullScreenWidth_ = config["fullScreenHeight"].as<int>();
    }
    catch (...) 
    {
        fullScreenHeight_ = height_;
    }
    return true;
}
catch (...)
{
    MessageBox(nullptr, "Failed to read 'f3d.yaml' config file, using default", "Config Reader", MB_OK | MB_ICONERROR);
    return false;
}
