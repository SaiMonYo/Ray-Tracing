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
};


class Phong: public Material{
    public:
        Phong(){
        }

        bool inline transmit(const Ray& ray, Intersection& inter, Vec3& colour, Ray& transmissionRay, Scene& world) const{
            Vec3 pointToLight = (world.light - inter.point).normalise();
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
};


class Glass: public Material{
    public:
        Glass(float refractionIndex_){
            refractionIndex = refractionIndex_;
        }

        virtual bool inline transmit(const Ray& ray, Intersection& inter, Vec3& colour, Ray& transmissionRay, Scene& world) const{
            colour = Vec3(255);
            float ratio = (inter.inside) ? 1.0f / refractionIndex : refractionIndex;
            float cosTheta = std::fmin((-ray.direction).dot(inter.normal), 1.0f);
            float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

            bool totalInternalReflection = (refractionIndex * sinTheta > 1.0f);
            Vec3 direction;

            if (totalInternalReflection || reflectance(cosTheta, ratio) > random_float()){
                direction = ray.direction.reflect(inter.normal).normalise();
            }
            else{
                direction = ray.direction.refract(inter.normal, ratio).normalise();
            }
            transmissionRay = Ray(inter.point, direction);
            return true;
        }

        float refractionIndex;

    private:
        static double reflectance(double cosine, double ref_idx) {
            // Schlick's approximation
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};