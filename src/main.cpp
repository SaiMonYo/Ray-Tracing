#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include "vector.h"
#include "triangle.h"
#include "ray.h"
#include "trianglemesh.h"
#include "scene.h"


const int WIDTH = 1280;
const int HEIGHT = 720;
const float fov = M_PI / 4;


void drawPixel(SDL_Renderer *renderer, int x, int y, Vec3 colour){
    SDL_SetRenderDrawColor(renderer, colour.x, colour.y, colour.z, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}


Vec3 trace(Ray &ray, Scene world){
    float tnear = finf;
    Intersection inter;
    if (world.intersection(ray, inter)){
        float dotted = inter.normal.dot((world.light - inter.point).normalise());
        return Vec3(255,255,255) * std::max(0.1f, dotted);
    }
    return Vec3(0,0,0);
}


void render(SDL_Renderer *renderer, Scene world){
    float invWidth = 1/(WIDTH + 0.0);
    float invHeight = 1/(HEIGHT + 0.0);
    float ratio = WIDTH / (HEIGHT + 0.0);
    float angle = tan(fov);
    Vec3 colour;
    std::ofstream file("images/imgascii.ollo", std::ios::out|std::ios::binary);
    file << WIDTH << "\n" << HEIGHT << "\n\n";
    for (int y = 0; y < HEIGHT; y++){
        for (int x = 0; x < WIDTH; x++){
            float xd = (2 * ((x+0.5) * invWidth) - 1) * angle * ratio;
            float yd = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            Ray ray = Ray(Vec3(0,0.5,1), Vec3(xd, yd, -1).normalise());
            colour = trace(ray, world).clamp(0, 255);
            drawPixel(renderer, x, y, colour);
            int xp = static_cast<int>(colour.x);
            int yp = static_cast<int>(colour.y);
            int zp = static_cast<int>(colour.z);
            file << (unsigned char)xp;
            file << (unsigned char)yp;
            file << (unsigned char)zp;
        }
        if (y % (HEIGHT / 10) == 0){
            std::cout << "Rendering: " << (y * 100) / HEIGHT << "%" << std::endl;
            SDL_RenderPresent(renderer);
        }
    }
    file.close();
}


int main(int argc, char** argv){  
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL test", 0, 20, WIDTH, HEIGHT, 0);
    if(!window){
        printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    Scene world;
    world.addObject(std::make_shared<TriangleMesh>("Objects/bunny.obj"));
    world.light = Vec3(0, 10, 7);
    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        std::cout << "loading..." << std::endl;
        render(renderer, world);
        SDL_RenderPresent(renderer);
        std::cout << "loaded" << std::endl;
        running = false;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}