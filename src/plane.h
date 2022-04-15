#include "ray.h"
#include "observables.h"
#include <iostream>
#include <vector>
#include <memory>


class Plane: public Observable{
    public:
        Plane(Vec3 point_, Vec3 normal_, Vec3 colour_){
            normal = normal_;
            point = point_;
            colour = colour_;
        }

        bool intersection(Ray& ray, Intersection& inter) const{
            float denom = normal.dot(ray.direction);
            if (std::fabs(denom) > EPSILLON){
                float t = (point - ray.origin).dot(normal) / denom;
                if (t > EPSILLON && t < inter.timestep){
                    inter.point = ray.origin + ray.direction * t;
                    inter.normal = normal;
                    inter.timestep = t;
                    inter.colour = colour;
                    return true;
                }
            }
            return false;
        }

        Vec3 normal;
        Vec3 point;
        Vec3 colour;
};