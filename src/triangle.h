#pragma once

#include "ray.h"
#include "observables.h"



float intersectTriangle(Ray ray, Vec3& v0, Vec3& v1, Vec3& v2){
    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    Vec3 pvec = ray.direction.cross(v0v2);
    float det = v0v1.dot(pvec);

    if (det < 0.000001){
        return -1;
    }

    float invdet = 1.0 / det;
    Vec3 tvec = ray.origin - v0;
    float u = tvec.dot(pvec) * invdet;
    
    if (u < 0 || u > 1){
        return -1;
    }

    Vec3 qvec = tvec.cross(v0v1);
    float v = ray.direction.dot(qvec) * invdet;

    if (v < 0 || u + v > 1){
        return -1;
    }

    return v0v2.dot(qvec) * invdet;
}


class Triangle{
    public:
        Vec3 v0, v1, v2, normal, colour;

        Triangle(Vec3 V0_, Vec3 V1_, Vec3 V2_, Vec3 Colour_){
            v0 = V0_;
            v1 = V1_;
            v2 = V2_;
            normal = (v1 - v0).cross(v2 - v0).normalise();
            colour = Colour_;
        }

        Vec3 normalAt(const Vec3 point){
            return normal;
        }

        // get time step of ray intersecting this triangle using Möller-Trumbore algorithm
        float intersection(const Ray &ray){
            Vec3 v0v1 = v1 - v0;
            Vec3 v0v2 = v2 - v0;
            Vec3 pvec = ray.direction.cross(v0v2);
            float det = v0v1.dot(pvec);

            if (det < 0.000001){
                return -INFINITY;
            }

            float invdet = 1.0 / det;
            Vec3 tvec = ray.origin - v0;
            float u = tvec.dot(pvec) * invdet;
            
            if (u < 0 || u > 1){
                return -INFINITY;
            }

            Vec3 qvec = tvec.cross(v0v1);
            float v = ray.direction.dot(qvec) * invdet;

            if (v < 0 || u + v > 1){
                return -INFINITY;
            }

            return v0v2.dot(qvec) * invdet;
        }
};