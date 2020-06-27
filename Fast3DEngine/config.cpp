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
    sampleCount_ = config["sample"].as<int>();
    if (0 == sampleCount_)
        sampleCount_ = 1;

    sampleQuality_ = config["sampleQuality"].as<int>();
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
        deinitAllowed_ = config["deinitAllowed"].as<bool>();
    }
    catch (...) {}
    return true;
}
catch (...)
{
    MessageBox(nullptr, "Failed to read 'f3d.yaml' config file, using default", "Config Reader", MB_OK | MB_ICONERROR);
    return false;
}


void Config::write(const std::string& p)
try
{
    YAML::Node config;
    config["width"] = width_;
    config["height"] = height_;
    config["vsync"] = "disabled";
    config["sample"] = sampleCount_;
    config["sampleQuality"] = sampleQuality_;
    config["reducedLatency"] = reducedLatency_ ? "allow" : "disallow";
    config["nerfFog"] = nerfFogFactor_;
    config["shadowBias"] = shadowBias_;
    config["deinitAllowed"] = deinitAllowed_;

    std::ofstream fout(p);
    fout << config;
}
catch (...)
{
    MessageBox(nullptr, "Failed to write 'f3d.yaml' config file", "Config Reader", MB_OK | MB_ICONERROR);
}