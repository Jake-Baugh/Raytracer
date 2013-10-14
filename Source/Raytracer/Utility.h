#ifndef RAYTRACER_UTILITY_H
#define RAYTRACER_UTILITY_H

#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16

#define SAFE_RELEASE(com) if(com != nullptr){ com->Release(); com = nullptr;}
#define SAFE_DELETE(obj) if(obj != nullptr){delete obj; obj = nullptr;}

#endif //RAYTRACER_UTILITY_H