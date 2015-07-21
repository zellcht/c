#include <SDL.h>
#include <math.h>

#define WWIDTH 800
#define WHEIGHT 600

#define SDL_8BITCOLOUR 256

#define FONTCHARS 256
#define FONTBYTESCHAR 8
#define FNT_FONTHEIGHT 8
#define FNT_FONTWIDTH 8

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

// All info required for windows / renderer & event loop
struct SDL_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
};
typedef struct SDL_Simplewin SDL_Simplewin;

void Neill_SDL_Init(SDL_Simplewin *sw);
void Neill_SDL_Events(SDL_Simplewin *sw);
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b);
void Neill_SDL_RenderFillCircle(SDL_Renderer *rend, int cx, int cy, int r);
void Neill_SDL_RenderDrawCircle(SDL_Renderer *rend, int cx, int cy, int r);

void Neill_SDL_DrawText(SDL_Simplewin *sw, const char* text, int x, int y);
unsigned char* Neill_SDL_GetFont();

