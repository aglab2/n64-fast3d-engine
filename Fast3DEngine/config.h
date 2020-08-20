#pragma once

#include <string>

enum class VsyncMode
{
    DISABLED = 0,
    V1 = 1,
    V2 = 2,
    V3 = 3,
    V4 = 4,
    V5 = 5,

    AUTOMATIC = 100,
};

class Config
{
public:
    Config();

    int width()           const { return width_;     }
    int height()          const { return height_;    }
    int fullScreenWidth()  const { return fullScreenWidth_; }
    int fullScreenHeight() const { return fullScreenHeight_; }
    VsyncMode vsyncMode() const { return vsyncMode_; }
    int reducedLatency()  const { return reducedLatency_; }
    float nerfFogFactor() const { return nerfFogFactor_; }
    float shadowBias()    const { return shadowBias_; }
    bool captureFrames()  const { return captureFrames_; }

    const std::string& configPath() const { return configPath_; }

private:
    int width_ = 640;
    int height_ = 480;
    int fullScreenWidth_ = 1920;
    int fullScreenHeight_ = 1080;
    bool reducedLatency_ = true;
    VsyncMode vsyncMode_ = VsyncMode::DISABLED;
    float nerfFogFactor_ = 0;
    float shadowBias_ = 2.f;
    bool captureFrames_ = false;
    std::string configPath_;

    static VsyncMode toVsyncMode(const std::string&);

    bool read(const std::string&);
};
