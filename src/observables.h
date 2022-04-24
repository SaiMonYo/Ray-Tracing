#pragma once

#include "ray.h"
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>


class Material;

struct Intersection{
    Vec3 point;
    Vec3 normal;
    Vec3 colour;
    float timestep;
    uint32_t sceneIndex;
    std::shared_ptr<Material> material;
    bool inside;

    Intersection(){
        timestep = finf;
    }
};


class Observable{
    public:
        std::shared_ptr<Material> material;
        Observable(){}
        virtual bool intersection(Ray& ray, Intersection& inter) = 0;
};


bool AABBIntersection(const Vec3& min, const Vec3& max, const Ray& ray){
    // ray AABB intersection using Nvidia's ray slab intersection algorithm
    Vec3 t0 = (min - ray.origin) * ray.invDirection;
    Vec3 t1 = (max - ray.origin) * ray.invDirection;
    Vec3 tmin = t0.min(t1);
    Vec3 tmax = t0.max(t1);

    return (tmin.maxComponent() <= tmax.minComponent());
}

bool AABBIntersection(const Vec3& min, const Vec3& max, Vec3 v0, Vec3 v1, Vec3 v2){
    // crude AABB-triangle intersection
    // creates a bounding box around triangle and checks for intersection with AABB
    // quick but quite a few false positives
    // speeds up octree creation but slows down raytracing
    Vec3 center = (max + min) / 2.0f;
    Vec3 e = (max - center);

    Vec3 top = v0.max(v1.max(v2));
    Vec3 bottom = v0.min(v1.min(v2));
    Vec3 center1 = (top + bottom) / 2.0f;
    Vec3 e1 = (top - center1);

    if (std::fabs(center.x - center1.x) > e.x + e1.x) return false;
    if (std::fabs(center.y - center1.y) > e.y + e1.y) return false;
    if (std::fabs(center.z - center1.z) > e.z + e1.z) return false;
    return true;
}
