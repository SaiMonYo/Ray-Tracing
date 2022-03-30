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
        virtual bool intersection(Ray& ray, Intersection& inter) const = 0;
};


bool AABBIntersection(const Vec3& min, const Vec3& max, const Ray& ray){
    float tmin = (min.x - ray.origin.x) / ray.direction.x; 
    float tmax = (max.x - ray.origin.x) / ray.direction.x; 
 
    if (tmin > tmax) std::swap(tmin, tmax); 
 
    float tymin = (min.y - ray.origin.y) / ray.direction.y; 
    float tymax = (max.y - ray.origin.y) / ray.direction.y; 
 
    if (tymin > tymax) std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (min.z - ray.origin.z) / ray.direction.z; 
    float tzmax = (max.z - ray.origin.z) / ray.direction.z; 
 
    if (tzmin > tzmax) std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
    return true; 
}