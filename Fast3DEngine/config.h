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

enum class RenderingAPI
{
    D3D11,
    OPENGL,
};

class Config
{
public:
    Config();

    auto width()            const { return width_;     }
    auto height()           const { return height_;    }
    auto fullScreenWidth()  const { return fullScreenWidth_; }
    auto fullScreenHeight() const { return fullScreenHeight_; }
    auto vsyncMode()        const { return vsyncMode_; }
    auto reducedLatency()   const { return reducedLatency_; }
    auto nerfFogFactor()    const { return nerfFogFactor_; }
    auto shadowBias()       const { return shadowBias_; }
    auto captureFrames()    const { return captureFrames_; }
    auto renderingApi()     const { return renderingApi_; }

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
    RenderingAPI renderingApi_ = RenderingAPI::D3D11;
    std::string configPath_;

    bool read(const std::string&);
};
