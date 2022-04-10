#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include <memory>
#include "ray.h"
#include "observables.h"


class Scene{
    public:
        std::vector<std::shared_ptr<Observable>> objects;
        Vec3 light;
        Vec3 camera;

        Scene(){}

        void addObject(std::shared_ptr<Observable> object){
            objects.push_back(object);
        }

        void clear(){
            objects.clear();
        }

        bool intersection(Ray& ray, Intersection& inter) const{
            Intersection temp;
            bool contact = false;
            for (const auto& object: objects){
                if (object->intersection(ray, temp)){
                    contact = true;
                    inter = temp;
                }
            }
            return contact;
        }
};