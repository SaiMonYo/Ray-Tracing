#pragma once


#include "ray.h"
#include "observables.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "scene.h"

// WORK IN PROGRESS

float random_float(){
    return rand() / (RAND_MAX + 1.0);
}

class Material{
    public:
        Material(){}
        virtual bool transmit(const Ray& ray, Intersection& inter, Vec3& colour, Ray& transmissionRay, Scene& world) const = 0;
        std::string name;
};


class Phong: public Material{
    public:
        Phong(){
        }

        bool inline transmit(const Ray& ray, Intersection& inter, Vec3& colour, Ray& transmissionRay, Scene& world) const{
            Vec3 pointToLight = (world.light - inter.point).normalise();
            // comment out for glass material testing
            Ray shadowRay(inter.point + pointToLight * 0.0001f, pointToLight);
            Intersection shadowInter;
            if (world.intersection(shadowRay, shadowInter)){
                colour = Vec3(0, 0, 0);
                return false;
            }

            float dotted = inter.normal.dot(pointToLight);
            Vec3 pixelColour = inter.colour * dotted;

            Vec3 v = (pointToLight - ray.direction).normalise();
            float phong = pow(inter.normal.dot(v), 64);
            colour = pixelColour + Vec3(255, 255, 255) * phong * 0.4;
            return false;
        }

        std::string name = "Phong";
};


class Glass: public Material{
    public:
        Glass(float refractionIndex_){
            refractionIndex = refractionIndex_;
            refractionIndexLookup[0] = refractionIndex;
            refractionIndexLookup[1] = 1.0f / refractionIndex;
        }

        virtual bool inline transmit(const Ray& ray, Intersection& inter, Vec3& colour, Ray& transmissionRay, Scene& world) const{
            colour = Vec3(255);
            Vec3 direction;
            direction = ray.direction.refract(inter.normal, refractionIndexLookup[inter.inside]);
            transmissionRay = Ray(inter.point - inter.normal * 0.01, direction);
            return true;
        }

        float refractionIndexLookup[2];
        float refractionIndex;
        std::string name = "Glass";
};