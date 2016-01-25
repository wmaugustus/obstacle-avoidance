//
// Created by paul on 17/01/16.
//

#ifndef OBSTACLE_DETECTION_ORIENTATION_H
#define OBSTACLE_DETECTION_ORIENTATION_H

#include <sstream>
#include <math.h>

struct Orientation {
    Orientation(double rollRad, double pitchRad, double yawRad);

public:
    const double rollRadians;
    const double pitchRadians;
    const double yawRadians;

    const double rollDegrees;
    const double pitchDegrees;
    const double yawDegrees;

    /**
     * Returns a relatively nicely-formatted string with roll, pitch, and yaw in degrees.
     */
    std::string toPrettyString();

    /**
     * Returns a space-efficient string with roll, pitch, and yaw in radians.
     */
    std::string toDataString();

    /**
     * Returns true if the roll, pitch, and yaw angles are the same.
     */
    bool operator==(const Orientation &other) const;

private:
    const double RAD_TO_DEG = 180 / M_PI;

    double _EPSILON = 0.001;
};

#endif //OBSTACLE_DETECTION_ORIENTATION_H