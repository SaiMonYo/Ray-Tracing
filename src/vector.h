#pragma once

#include <math.h>


const int finf  = 1e8;


class Vec3{
    public:
        float x, y, z;

        Vec3(const float X_, const float Y_, const float Z_){
            x = X_;
            y = Y_;
            z = Z_;
        }

        Vec3(const float &value){
            x = value;
            y = value;
            z = value;
        }

        Vec3(){
            x = 0;
            y = 0;
            z = 0;
        }
    
        // normalise vector in place and return the normalised vector
        inline Vec3 normalise(){
            float l = dot(*this);
            if (l > 0){
                float inv = 1 / sqrt(l);
                x *= inv;
                y *= inv;
                z *= inv;
            }
            return *this;
        }

        inline float lengthsquared() const{
            return x * x + y * y + z * z;
        }

        // get length or magnitude of vector
        inline float length() const{
            return sqrt(x * x + y * y + z * z);
        }

        // the dot product of two vectors
        inline float dot(const Vec3 &other) const{
            return x * other.x + y * other.y + z * other.z;
        }

        // the cross product of the two vectors
        inline Vec3 cross(const Vec3 &other) const{
            return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
        }

        inline Vec3 max(Vec3 other) const{
            return Vec3(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z));
        }

        inline Vec3 min(Vec3 other) const{
            return Vec3(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z));
        }

    
        // !!!! VECTOR OPERATORS BELOW !!!!
        inline Vec3 operator-() const{
            return Vec3(-x, -y, -z);
        }

        inline Vec3 operator-(const Vec3 &other) const{
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

        inline Vec3 operator-=(const Vec3 &other){
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        inline Vec3 operator+(const Vec3 &other) const{
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        inline Vec3 operator+=(const Vec3 &other){
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        inline Vec3 operator/(const float value) const{
            return Vec3(x / value, y / value, z / value);
        }

        inline Vec3 operator/=(const float value){
            x /= value;
            y /= value;
            z /= value;
            return *this;
        }

        inline Vec3 operator*(const float value) const{
            return Vec3(x * value, y * value, z * value);
        }

        inline Vec3 operator*=(const float value){
            x *= value;
            y *= value;
            z *= value;
            return *this;
        }

        inline bool operator==(const Vec3 &other) const{
            return x == other.x && y == other.y && z == other.z;
        }

        inline bool operator!=(const Vec3 &other) const{
            return x != other.x || y != other.y || z != other.z;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec3& v){
            os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
            return os;
        }
};