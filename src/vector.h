#pragma once

#include <math.h>


const float finf  = 1e8;
const float EPSILLON = 0.000000001f;


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
        Vec3 normalise(){
            float l = dot(*this);
            if (l > 0){
                float inv = 1 / sqrt(l);
                x *= inv;
                y *= inv;
                z *= inv;
            }
            return *this;
        }

        float lengthsquared() const{
            return x * x + y * y + z * z;
        }

        // get length or magnitude of vector
        float length() const{
            return sqrt(x * x + y * y + z * z);
        }

        // the dot product of two vectors
        float dot(const Vec3 &other) const{
            return x * other.x + y * other.y + z * other.z;
        }

        // the cross product of the two vectors
        Vec3 cross(const Vec3 &other) const{
            return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
        }

        Vec3 rotate(float yaw, float pitch, float roll) { //X Y Z Rotation
            float cosa = cos(yaw);
            float cosb = cos(pitch);
            float cosc = cos(roll);
            float sina = sin(yaw);
            float sinb = sin(pitch);
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

            float px = x; float py = y; float pz = z;
            x = Axx * px + Axy * py + Axz * pz;
            y = Ayx * px + Ayy * py + Ayz * pz;
            z = Azx * px + Azy * py + Azz * pz;
            return *this;
        }

        Vec3 reflect(const Vec3 &normal) const{
            return *this - normal * dot(normal) * 2;
        }

        Vec3 refract(const Vec3 &normal, float n) const{
            float cosTheta = std::fmin(-dot(normal), 1.0f);
            Vec3 perp = (*this + normal * cosTheta) * n;
            Vec3 parralel = normal * -sqrt(std::fabs(1.0f - perp.lengthsquared()));
            return perp + parralel;
        }

        Vec3 multiplyColour(const Vec3 &other) const{
            return Vec3(x * other.x * 0.00392156862f, y * other.y * 0.00392156862f, z * other.z * 0.00392156862);
        }

        Vec3 max(Vec3 other) const{
            return Vec3(std::fmax(x, other.x), std::fmax(y, other.y), std::fmax(z, other.z));
        }

        Vec3 min(Vec3 other) const{
            return Vec3(std::fmin(x, other.x), std::fmin(y, other.y), std::fmin(z, other.z));
        }

        Vec3 clamp(float min, float max) const{
            return Vec3(std::fmax(min, std::fmin(max, x)), std::fmax(min, std::fmin(max, y)), std::fmax(min, std::fmin(max, z)));
        }

        Vec3 rcp() const{
            return Vec3(1.0f / x, 1.0f / y, 1.0f / z);
        }

        float maxComponent() const{
            return std::fmax(x, std::fmax(y, z));
        }

        float minComponent() const{
            return std::fmin(x, std::fmin(y, z));
        }

        bool near(const Vec3 &other, float epsilon) const{
            return (std::fabs(x - other.x) < epsilon) && (std::fabs(y - other.y) < epsilon) && (std::fabs(z - other.z) < epsilon);
        }

        Vec3 toFloor(){
            x = floor(x);
            y = floor(y);
            z = floor(z);
            return *this;
        }

        Vec3 toAbs(){
            x = fabs(x);
            y = fabs(y);
            z = fabs(z);
            return *this;
        }

    
        // !!!! VECTOR OPERATORS BELOW !!!!
        Vec3 operator-() const{
            return Vec3(-x, -y, -z);
        }

        Vec3 operator-(const Vec3 &other) const{
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

        Vec3 operator-=(const Vec3 &other){
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vec3 operator+(const Vec3 &other) const{
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        Vec3 operator+=(const Vec3 &other){
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vec3 operator/(const float value) const{
            return Vec3(x / value, y / value, z / value);
        }

        Vec3 operator/=(const float value){
            x /= value;
            y /= value;
            z /= value;
            return *this;
        }

        Vec3 operator*(const float value) const{
            return Vec3(x * value, y * value, z * value);
        }

        Vec3 operator*(const Vec3 &other) const{
            return Vec3(x * other.x, y * other.y, z * other.z);
        }

        Vec3 operator*=(const float value){
            x *= value;
            y *= value;
            z *= value;
            return *this;
        }

        bool operator==(const Vec3 &other) const{
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vec3 &other) const{
            return x != other.x || y != other.y || z != other.z;
        }

        bool operator<(const Vec3 &other) const{
            return x < other.x && y < other.y && z < other.z;
        }

        bool operator>(const Vec3 &other) const{
            return x > other.x && y > other.y && z > other.z;
        }

        bool operator<=(const Vec3 &other) const{
            return x <= other.x && y <= other.y && z <= other.z;
        }

        bool operator>=(const Vec3 &other) const{
            return x >= other.x && y >= other.y && z >= other.z;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec3& v){
            os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
            return os;
        }
};

inline Vec3 operator*(const float value, const Vec3 &other){
    return Vec3(value * other.x, value * other.y, value * other.z);
}