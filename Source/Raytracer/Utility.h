#ifndef RAYTRACER_UTILITY_H
#define RAYTRACER_UTILITY_H

#define SAFE_RELEASE(com) if(com != nullptr){ com->Release(); com = nullptr;}
#define SAFE_DELETE(obj) if(obj != nullptr){delete obj; obj = nullptr;}

#endif //RAYTRACER_UTILITY_H