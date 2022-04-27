#pragma once

#include "ray.h"
#include "observables.h"
#include <iostream>
#include <vector>
#include <memory>

class Sphere: public Observable{
    public:
        Sphere(Vec3 center_, float radius_, Vec3 colour_, std::shared_ptr<Material> material_){
            center = center_;
            radius = radius_;
            colour = colour_;
            material = material_;
        }

        bool intersection(Ray& ray, Intersection& inter){
            Vec3 oc = ray.origin - center;
            auto a = ray.direction.lengthsquared();
            auto half_b = oc.dot(ray.direction);
            auto c = oc.lengthsquared() - radius*radius;

            auto discriminant = half_b*half_b - a*c;
            if (discriminant < 0) return false;
            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (-half_b - sqrtd) / a;
            if (root < EPSILLON || inter.timestep < root) {
                root = (-half_b + sqrtd) / a;
                if (root < EPSILLON || inter.timestep < root)
                    return false;
            }

            inter.timestep = root;
            inter.point = ray.attime(root);
            Vec3 outward_normal = (inter.point - center) / radius;
            inter.set_face_normal(ray, outward_normal);

            return true;        
        }

        Vec3 center;
        float radius;
        Vec3 colour;
};