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
        std::vector<Vec3> faceNormals;
        std::vector<std::vector<int>> faces;
        Vec3 boundingBox[2];
};

bool TriangleMesh::intersection(Ray& ray, Intersection& inter) const{
    if (!AABBIntersection(boundingBox[0], boundingBox[1], ray)){
        return false;
    }
    std::vector<Vec3> hitface;
    int i = -1;
    int index = -1;
    float pu = 0;
    float pv = 0;
    for(const auto& face : faces){
        i++;
        Vec3 v0 = vertices[face[0] - 1];
        Vec3 v1 = vertices[face[1] - 1];
        Vec3 v2 = vertices[face[2] - 1];
        Vec3 v0v1 = v1 - v0;
        Vec3 v0v2 = v2 - v0;
        Vec3 pvec = ray.direction.cross(v0v2);
        float det = v0v1.dot(pvec);

        if (det <= 0){
            continue;
        }

        float invdet = 1.0 / det;
        Vec3 tvec = ray.origin - v0;
        float u = tvec.dot(pvec) * invdet;
        
        if (u < 0 || u > 1){
            continue;
        }

        Vec3 qvec = tvec.cross(v0v1);
        float v = ray.direction.dot(qvec) * invdet;

        if (v < 0 || u + v > 1){
            continue;
        }

        pu = u;
        pv = v;
        float t = v0v2.dot(qvec) * invdet;
        if (t > 0 && t < inter.timestep){
            inter.timestep = t;
            index = i;
        }
    }
    if (index > -1){
        inter.point = ray.attime(inter.timestep);
        auto face = faces[index];
        Vec3 v0 = vertices[face[0] - 1];
        Vec3 v1 = vertices[face[1] - 1];
        Vec3 v2 = vertices[face[2] - 1];
        float v0p = (v0 - inter.point).length();
        float v1p = (v1 - inter.point).length();
        float v2p = (v2 - inter.point).length();
        inter.normal = normals[face[0] - 1] * (1 - pu - pv) + normals[face[1] - 1] * pu + normals[face[2] - 1] * pv;
        inter.normal.normalise();
        return true;
    }
    return false;
}