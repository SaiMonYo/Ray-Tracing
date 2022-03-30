#pragma once

#include "ray.h"
#include "observables.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>


std::string replaceSlash(std::string str){
    std::string newStr = "";
    for (int i = 0; i < str.length(); i++){
        if (str[i] == '/'){
            newStr += " ";
        }
        else {
            newStr += str[i];
        }
    }
    return newStr;
}

class TriangleMesh: public Observable{
    public:
        TriangleMesh(const std::string& filename){
            std::ifstream file(filename);
            if(!file.is_open()){
                std::cerr << "Could not open file " << filename << std::endl;
                return;
            }
            std::string line;
            while(std::getline(file, line)){
                if(line.empty()) continue;
                if(line[0] == 'v'){
                    std::istringstream iss(line);
                    std::string type;
                    iss >> type;
                    if(type == "v"){
                        Vec3 v;
                        iss >> v.x >> v.y >> v.z;
                        vertices.push_back(v);
                    }
                    else if(type == "vn"){
                        Vec3 vn;
                        iss >> vn.x >> vn.y >> vn.z;
                        normals.push_back(vn);
                    }
                    else if(type == "vt"){
                        Vec3 vt;
                        iss >> vt.x >> vt.y;
                        texcoords.push_back(vt);
                    }
                }
                else if (line[0] == 'f'){
                    std::istringstream iss(line);
                    std::string type;
                    iss >> type;
                    std::string fc;
                    if(type == "f"){
                        std::vector<int> face;
                        while (iss >> fc){
                            fc = replaceSlash(fc);
                            int v;
                            std::istringstream fcss(fc);
                            fcss >> v;
                            face.push_back(v);
                        }
                        faces.push_back(face);
                    }
                }
            }
            file.close();
            recalcBoundingBox();
        }

        void recalcBoundingBox(){
            Vec3 vmax = Vec3(-finf);
            Vec3 vmin = Vec3(finf);
            for(const auto& v : vertices){
                vmax = vmax.max(v);
                vmin = vmin.min(v);
            }
            boundingBox[0] = vmin;
            boundingBox[1] = vmax;
        }

        void translate(const Vec3& translation){
            for(auto& v : vertices){
                v += translation;
            }
            recalcBoundingBox();
        }

        ~TriangleMesh(){
            vertices.clear();
            normals.clear();
            texcoords.clear();
            faces.clear();
        }

        virtual bool intersection(Ray& ray, Intersection& inter) const override;

    public:
        std::vector<Vec3> vertices;
        std::vector<Vec3> normals;
        std::vector<Vec3> texcoords;
        std::vector<std::vector<int>> faces;
        Vec3 boundingBox[2];
};

bool TriangleMesh::intersection(Ray& ray, Intersection& inter) const{
    if (!AABBIntersection(boundingBox[0], boundingBox[1], ray)){
        return false;
    }
    std::vector<Vec3> hitface;
    int i = 0;
    int index = 0;
    for(const auto& face : faces){
        Vec3 v0 = vertices[face[0] - 1];
        Vec3 v1 = vertices[face[1] - 1];
        Vec3 v2 = vertices[face[2] - 1];
        float t = intersectTriangle(ray, v0, v1, v2);
        if (t > 0 && t < inter.timestep){
            if (hitface.size() == 3){
                hitface[0] = v0;
                hitface[1] = v1;
                hitface[2] = v2;
            }
            else{
                hitface.push_back(v0);
                hitface.push_back(v1);
                hitface.push_back(v2);
            }
            inter.timestep = t;
            index = i;
        }
        i++;
    }
    if (hitface.size() == 3){
        inter.point = ray.attime(inter.timestep);
        inter.normal = (hitface[1] - hitface[0]).cross(hitface[2] - hitface[0]).normalise();
        return true;
    }
    return false;
}