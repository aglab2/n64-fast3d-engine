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
    VsyncMode vsyncMode() const { return vsyncMode_; }
    int sampleCount()     const { return sampleCount_; }
    int sampleQuality()   const { return sampleQuality_; }
    int reducedLatency()  const { return reducedLatency_; }
    float nerfFogFactor() const { return nerfFogFactor_; }
    float shadowBias()    const { return shadowBias_; }

private:
    int width_ = 640;
    int height_ = 480;
    int sampleCount_ = 1;
    int sampleQuality_ = 0;
    bool reducedLatency_ = true;
    VsyncMode vsyncMode_ = VsyncMode::DISABLED;
    float nerfFogFactor_ = 0;
    float shadowBias_ = 2.f;

    static VsyncMode toVsyncMode(const std::string&);

    bool read(const std::string&);
    void write(const std::string&);
};
