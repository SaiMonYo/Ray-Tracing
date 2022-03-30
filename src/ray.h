#pragma once

#include "vector.h"

class Ray{
    public:
        Vec3 origin, direction;

        Ray(const Vec3 Origin_, const Vec3 Direction_){
            origin = Origin_;
            direction = Direction_;
        }

        Ray(){
            origin = Vec3(0);
            direction = Vec3(0);
        }

        Vec3 attime(float t){
            return origin + direction * t;
        }

        friend std::ostream & operator << (std::ostream &os, const Ray &ray){ 
            os << "[" << ray.origin << ", " << ray.direction << "]"; 
            return os; 
        }   
};