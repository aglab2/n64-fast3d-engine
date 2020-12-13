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

static VsyncMode toVsyncMode(const std::string& mode)
{
    try
    {
        return (VsyncMode)std::stoul(mode);
    }
    catch (...) { }

    if ("auto" == mode)
        return VsyncMode::AUTOMATIC;

    if ("perfect" == mode)
        return VsyncMode::PERFECT;
    
    return VsyncMode::DISABLED;
}

static RenderingAPI toRenderingApi(std::string api)
{
    std::transform(api.begin(), api.end(), api.begin(), [](unsigned char c) { return std::tolower(c); });
    if ("opengl" == api)
        return RenderingAPI::OPENGL;

    return RenderingAPI::D3D11;
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
    try
    {
        renderingApi_ = toRenderingApi(config["api"].as<std::string>());
    }
    catch (...)
    {
        renderingApi_ = RenderingAPI::D3D11;
    }

    try
    {
        recognizeResets_ = config["recognizeResets"].as<bool>();
    }
    catch (...)
    {
        recognizeResets_ = false;
    }

    try
    {
        traceDeinitStack_ = config["traceDeinitStack"].as<bool>();
    }
    catch (...)
    {
        traceDeinitStack_ = false;
    }

    try
    {
        sampleCount_ = config["sampleCount"].as<int>();
    }
    catch (...)
    {
        sampleCount_ = 1;
    }

    if (RenderingAPI::OPENGL != renderingApi_)
    {
        try
        {
            rspThread_ = config["rspThread"].as<bool>();
        }
        catch (...)
        {
            rspThread_ = false;
        }
    }
    else
    {
        rspThread_ = true;
    }

    return true;
}
catch (...)
{
    MessageBox(nullptr, "Failed to read 'f3d.yaml' config file, using default", "Config Reader", MB_OK | MB_ICONERROR);
    return false;
}
