#pragma once

class HeadingSensor
{
public:
    virtual ~HeadingSensor() = default;

    virtual int getHeading() = 0;
    virtual void update() = 0;
};
