#include "ray.h"
#include "observables.h"
#include <iostream>
#include <vector>
#include <memory>


class Plane: public Observable{
    public:
        Plane(Vec3 point_, Vec3 normal_, Vec3 colour_, bool checkered_, std::shared_ptr<Material> material_){
            normal = normal_;
            point = point_;
            colour = colour_;
            checkered = checkered_;
            material = material_;
        }

        bool intersection(Ray& ray, Intersection& inter){
            float denom = normal.dot(ray.direction);
            if (std::fabs(denom) > EPSILLON){
                float t = (point - ray.origin).dot(normal) / denom;
                if (t > EPSILLON && t < inter.timestep){
                    inter.point = ray.origin + ray.direction * t;
                    inter.normal = normal;
                    inter.timestep = t;
                    if (!checkered){
                        inter.colour = colour;
                    }
                    else{
                        // checkerboard
                        inter.colour = (int(floor(inter.point.x) + int(floor(inter.point.z))) % 2 == 0) ? Vec3(255, 255, 255) : Vec3(80, 80, 80);
                    }
                    return true;
                }
            }
            return false;
        }

        bool checkered;
        Vec3 normal;
        Vec3 point;
        Vec3 colour;
};