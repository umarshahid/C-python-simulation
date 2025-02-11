#pragma once
#include <iostream>
#include "utils.cpp"
#include <cmath>
#include <string>


class Missile {
public:
    std::string force;
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Vector3 _targetPos = Vector3(0, 0, 0);
    double speed;
    double maxAcceleration;
    bool active;
    bool hit = false;
    float heading;

    Missile(float heading, std::string force, Vector3 launchPos, Vector3 launchVel, double missileSpeed, double maxAccel);
    void update(Vector3 targetPos, Vector3 targetVel, double dt);
    Vector3 get_position3() const;
    double getHeading();
    float get_heading();
    void set_heading(float new_heading);
    void updatePosition(const Vector3& targetPos, const Vector3& targetVel, double dt);
};
