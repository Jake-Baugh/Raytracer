#ifndef RAYTRACER_UTILITY_H
#define RAYTRACER_UTILITY_H

#define BLOCK_SIZE_X 32
#define BLOCK_SIZE_Y 32

#define SAFE_RELEASE(com) if(com != nullptr){ com->Release(); com = nullptr;}
#define SAFE_DELETE(obj) if(obj != nullptr){delete obj; obj = nullptr;}

#endif //RAYTRACER_UTILITY_H