#pragma once

#include "ray.h"
#include "observables.h"
#include "spacetree.h"
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
        TriangleMesh(const std::string& filename, const Vec3& colour_){
            std::ifstream file(filename);
            if(!file.is_open()){
                std::cerr << "Could not open file " << filename << std::endl;
                return;
            }
            colour = colour_;
            std::string line;
            while(std::getline(file, line)){
                if(line.empty()) continue;
                if(line[0] == 'v'){
                    std::istringstream iss(line);
                    std::string type;
                    iss >> type;
                    // vertex
                    if(type == "v"){
                        Vec3 v;
                        iss >> v.x >> v.y >> v.z;
                        vertices.push_back(v);
                    }
                    // vertex normal
                    else if(type == "vn"){
                        Vec3 vn;
                        iss >> vn.x >> vn.y >> vn.z;                     
                        normals.push_back(vn);
                    }
                    // vertex texture
                    else if(type == "vt"){
                        Vec3 vt;
                        iss >> vt.x >> vt.y;
                        texcoords.push_back(vt);
                    }
                }
                // face
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
            // calculate normals if they are not given
            if (normals.size() == 0){
                recalcNormals();
            }
            recalcBoundingBox();
        }

        void rescale(float factor){
            Vec3 centroid = getCentroid();
            for (int i = 0; i < vertices.size(); i++){
                vertices[i] = (vertices[i] - centroid) * factor + centroid;
            }
            recalcBoundingBox();
        }

        void recalcNormals(){
            for (int i = 0; i < faces.size(); i++){
                normals.push_back(Vec3(0,0,0));
            }
            for (auto face: faces){
                Vec3 v0 = vertices[face[0] - 1];
                Vec3 v1 = vertices[face[1] - 1];
                Vec3 v2 = vertices[face[2] - 1];
                Vec3 normal = (v1 - v0).cross(v2 - v0).normalise();
                normals[face[0] - 1] += normal;
                normals[face[1] - 1] += normal;
                normals[face[2] - 1] += normal;
            }
            for (int i = 0; i < normals.size(); i++){
                normals[i] = normals[i].normalise();
            }
        }

        void recalcBoundingBox(){
            // calculate bounding box for quick intersection testing
            Vec3 vmax = Vec3(-finf);
            Vec3 vmin = Vec3(finf);
            for(const auto& v : vertices){
                vmax = vmax.max(v);
                vmin = vmin.min(v);
            }
            boundingBox[0] = vmin;
            boundingBox[1] = vmax;
            tree = Octree(boundingBox, faces, vertices, 6);
        }

        Vec3 getCentroid(){
            Vec3 centroid = Vec3(0,0,0);
            for (const auto& v : vertices){
                centroid += v;
            }
            centroid / vertices.size();
            return centroid / vertices.size();
        }

        void rotate(float pitch, float roll, float yaw){
            float cosa = cos(yaw);
            float sina = sin(yaw);

            float cosb = cos(pitch);
            float sinb = sin(pitch);

            float cosc = cos(roll);
            float sinc = sin(roll);

            float Axx = cosa * cosb;
            float Axy = cosa * sinb * sinc - sina * cosc;
            float Axz = cosa * sinb * cosc + sina * sinc;

            float Ayx = sina * cosb;
            float Ayy = sina * sinb * sinc + cosa * cosc;
            float Ayz = sina * sinb * cosc - cosa * sinc;

            float Azx = -sinb;
            float Azy = cosb * sinc;
            float Azz = cosb * cosc;

            for (int i = 0; i < vertices.size(); i++){
                Vec3 v = vertices[i];
                vertices[i] = Vec3(
                    v.x * Axx + v.y * Ayx + v.z * Azx,
                    v.x * Axy + v.y * Ayy + v.z * Azy,
                    v.x * Axz + v.y * Ayz + v.z * Azz
                );
            }
            recalcNormals();
            recalcBoundingBox();
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

        bool intersection(Ray& ray, Intersection& inter){
            int index = -1;
            float pu = 0;
            float pv = 0;
            std::vector<std::vector<int>> possibleFaces = tree.intersection(ray);
            // check if all elements in possibleFaces are in faces

            for(int i = 0; i < possibleFaces.size(); i++){
                // check if ray intersects face by using moller-trumbore algorithm
                auto face = possibleFaces[i];
                Vec3 v0 = vertices[face[0] - 1];
                Vec3 v1 = vertices[face[1] - 1];
                Vec3 v2 = vertices[face[2] - 1];
                Vec3 v0v1 = v1 - v0;
                Vec3 v0v2 = v2 - v0;
                Vec3 pvec = ray.direction.cross(v0v2);
                float det = v0v1.dot(pvec);
                if (det < EPSILLON){
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

                float t = v0v2.dot(qvec) * invdet;
                if (t > EPSILLON && t < inter.timestep){
                    pu = u;
                    pv = v;
                    inter.timestep = t;
                    index = i;
                }
            }
            // index is greater than -1 if there is an intersection
            if (index > -1){
                inter.point = ray.attime(inter.timestep);
                auto face = possibleFaces[index];
                Vec3 n0 = normals[face[0] - 1];
                Vec3 n1 = normals[face[1] - 1];
                Vec3 n2 = normals[face[2] - 1];
                float n0n1 = n1.dot(n0);
                float n0n2 = n2.dot(n0);
                float n1n2 = n2.dot(n1);
                inter.normal = normals[face[0] - 1] * (1 - pu - pv) + normals[face[1] - 1] * pu + normals[face[2] - 1] * pv;
                inter.colour = colour;
                return true;
            }
            return false;
        }

    public:
        std::vector<Vec3> vertices;
        std::vector<Vec3> normals;
        std::vector<Vec3> texcoords;
        std::vector<Vec3> faceNormals;
        std::vector<std::vector<int>> faces;
        Vec3 boundingBox[2];
        Octree tree;
        Vec3 colour;
};
