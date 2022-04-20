#pragma once

#include "ray.h"
#include <iostream>
#include <vector>
#include <fstream>


struct Intersection{
    Vec3 point;
    Vec3 normal;
    Vec3 colour;
    float timestep;

    Intersection(){
        timestep = finf;
    }
};


class Observable{
    public:
        Observable(){}
        virtual bool intersection(Ray& ray, Intersection& inter) = 0;
};


bool AABBIntersection(const Vec3& min, const Vec3& max, const Ray& ray){
    Vec3 t0 = (min - ray.origin) * ray.invDirection;
    Vec3 t1 = (max - ray.origin) * ray.invDirection;
    Vec3 tmin = t0.min(t1);
    Vec3 tmax = t0.max(t1);

    return (tmin.maxComponent() <= tmax.minComponent());
}

