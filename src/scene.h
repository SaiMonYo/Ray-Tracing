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
            temp.timestep = inter.timestep;
            bool contact = false;
            for (int i = 0; i < objects.size(); i++){
                if (objects[i]->intersection(ray, temp)){
                    contact = true;
                    inter = temp;
                    inter.sceneIndex = i;
                    inter.material = objects[i]->material;
                }
            }
            return contact;
        }
};