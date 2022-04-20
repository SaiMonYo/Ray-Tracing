#include <iostream>
#include <vector>
#include <memory>
#include "plane.h"
#include "trianglemesh.h"
#include "scene.h"
#include <chrono>


const int WIDTH = 1280;
const int HEIGHT = 640;
const float fov = M_PI / 4;

const int QOI_OP_RUN   = 0xc0;
const int QOI_OP_INDEX = 0x00;
const int QOI_OP_DIFF  = 0x40;
const int QOI_OP_LUMA  = 0x80;
const int QOI_OP_RGB   = 0xfe;
const int QOI_OP_RGBA  = 0xff;


Vec3 trace(Ray &ray, Scene world){
    float tnear = finf;
    Intersection inter;
    if (world.intersection(ray, inter)){
        //working but takes x4 longer
        //get shadows
        Vec3 pointToLight = (world.light - inter.point).normalise();
        Ray shadowRay(inter.point + pointToLight * 0.0001f, pointToLight);
        Intersection shadowInter;
        if (world.intersection(shadowRay, shadowInter)){
            return Vec3(0, 0, 0);
        }

        float dotted = inter.normal.dot(pointToLight);
        Vec3 colour = inter.colour * dotted;

        Vec3 v = (pointToLight - ray.direction).normalise();
        float phong = pow(inter.normal.dot(v), 64);
        colour += Vec3(255, 255, 255) * phong * 0.4;
        return colour;
    }
    return Vec3(0,0,0);
}


int getKey(Vec3 colour){
    return ((int)(colour.x * 3 + colour.y * 5 + colour.z * 7) % 64);
}

void write32(std::ofstream& file, long value){
    file << (unsigned char) ((value & 0xff000000) >> 24);
    file << (unsigned char) ((value & 0x00ff0000) >> 16);
    file << (unsigned char) ((value & 0x0000ff00) >> 8);
    file << (unsigned char) ((value & 0x000000ff));
}

void render(Scene world){
    float invWidth = 1/(WIDTH + 0.0);
    float invHeight = 1/(HEIGHT + 0.0);
    float ratio = WIDTH/(HEIGHT + 0.0);
    float angle = tan(fov);
    Vec3 colour;
    std::ofstream qoi("images/result.qoi", std::ios::out|std::ios::binary);
    write32(qoi, 0x716f6966);
    write32(qoi, WIDTH);
    write32(qoi, HEIGHT);
    qoi << (unsigned char) 3;
    qoi << (unsigned char) 1;
    Vec3 lookup[64] = {};
    int run_length = 0;
    Vec3 previous = Vec3(0,0,0);
    for (int y = 0; y < HEIGHT; y++){
        for (int x = 0; x < WIDTH; x++){
            // calculate ray colour
            float xd = (2 * ((x+0.5) * invWidth) - 1) * angle * ratio;
            float yd = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            Ray ray = Ray(world.camera, Vec3(xd, yd, -1).normalise());
            colour = trace(ray, world).clamp(0, 255);
            colour.toFloor();
            // write to qoi
            if (colour == previous){
                // run length encoding
                run_length += 1;
                if (run_length == 62){
                    qoi << (unsigned char) (QOI_OP_RUN | (run_length - 1));
                    run_length = 0;
                }
            }
            else{
                // write previous run length encoding
                if (run_length > 0){
                    qoi << (unsigned char) (QOI_OP_RUN | (run_length - 1));
                    run_length = 0;
                }
                // check if we've already seen pixel
                int key = getKey(colour);
                if (colour == lookup[key]){
                    qoi << (unsigned char) (QOI_OP_INDEX | key);
                }
                else{
                    // get difference of colour
                    lookup[key] = colour;
                    Vec3 diff = colour - previous;
                    int dr_dg = diff.x - diff.y;
                    int db_dg = diff.z - diff.y;
                    // small difference of -2 to 1 in each colour channel
                    if ((-2 <= diff.x && diff.x <= 1) && (-2 <= diff.y && diff.y <= 1) && (-2 <= diff.z && diff.z <= 1)){
                        qoi << (unsigned char) (QOI_OP_DIFF | (((int)diff.x + 2)) << 4 | (((int)diff.y + 2) << 2) | ((int)diff.z + 2));
                    }
                    // larger differeence of -32 to 31 in green channel and -8 to 7 in red and blue channels
                    else if (-32 <= diff.y && diff.y <= 31 && -8 <= dr_dg && dr_dg <= 7 && -8 <= db_dg && db_dg <= 7){
                        qoi << (unsigned char) (QOI_OP_LUMA | ((int)diff.y + 32));
                        qoi << (unsigned char) (((dr_dg + 8) << 4) | (db_dg + 8));
                    }
                    else{
                        // no encoding possible so write RGB header and RGB values
                        qoi << (unsigned char) QOI_OP_RGB;
                        qoi << (unsigned char) (int)colour.x;
                        qoi << (unsigned char) (int)colour.y;
                        qoi << (unsigned char) (int)colour.z;
                    }
                }
            previous = colour;
            }
        }
        if (y % (HEIGHT / 10) == 0){
            std::cout << "Rendering: " << (y * 100) / HEIGHT << "%" << std::endl;
        }
    }
    std::cout << "Rendered: 100%" << std::endl;
    qoi.close();
}


int main(){ 
    //time the render
    Scene world;
    // lady lucy
    std::shared_ptr<TriangleMesh> lucy = std::make_shared<TriangleMesh>("Objects/lucy.obj", Vec3(78,117,102));
    lucy->rescale(0.01);
    lucy->rotate(0, M_PI/2, M_PI);
    lucy->center();
    lucy->translate(Vec3(-10, 0, 0));
    world.addObject(lucy);

    // buddha
    std::shared_ptr<TriangleMesh> buddha = std::make_shared<TriangleMesh>("Objects/Happy.obj", Vec3(252,140,92));
    buddha->rescale(60);
    buddha->center();
    buddha->translate(Vec3(10, 0, 0));
    world.addObject(buddha);

    // bunny
    std::shared_ptr<TriangleMesh> bunny = std::make_shared<TriangleMesh>("Objects/bigbunny.obj", Vec3(255,255,255));
    bunny->rescale(50);
    bunny->center();
    world.addObject(bunny);

    //planes
    world.addObject(std::make_shared<Plane>(Vec3(0,-6,0), Vec3(0, 1, 0), Vec3(255,255,255), true));
    world.addObject(std::make_shared<Plane>(Vec3(0,0,-20), Vec3(0, 0, 1), Vec3(64,224,208), false));


    world.light = Vec3(0, 50, 10);
    world.camera = Vec3(0,1,10);
    auto start = std::chrono::high_resolution_clock::now();
    render(world);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Render time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}
