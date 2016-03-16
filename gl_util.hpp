#ifndef VISMATRIX_GL_UTIL_HPP
#define VISMATRIX_GL_UTIL_HPP

/**
 * 
 */

#include "xplat_gl.h"

void world_to_screen(float wx, float wy, int *x, int *y);
void screen_to_world(int x, int y, double *wx, double *wy);
float scale_to_world(float val);

void world_extents(float &x1, float &y, float &x2, float &y2);

#ifndef GL_UTIL_GLUT_TEXT_FONT
#define GL_UTIL_GLUT_TEXT_FONT GLUT_BITMAP_HELVETICA_18
#endif // GL_UTIL_GLUT_TEXT_FONT

void output_text_string(float x, float y, const char *string, void* font = GL_UTIL_GLUT_TEXT_FONT);
void text_extends(int *width, int *height, const char *string, void* font = GL_UTIL_GLUT_TEXT_FONT);
void text_extends_world(float *width, float *height, const char *string, void* font = GL_UTIL_GLUT_TEXT_FONT);

const unsigned int GL_U_TEXT_WORLD_COORDS = 0;  // default option
const unsigned int GL_U_TEXT_SCREEN_COORDS = 1;

const unsigned int GL_U_TEXT_LEFT_X = 0;        // default option
const unsigned int GL_U_TEXT_CENTER_X = 2;
const unsigned int GL_U_TEXT_RIGHT_X = 4;

const unsigned int GL_U_TEXT_BOTTOM_Y = 0;
const unsigned int GL_U_TEXT_CENTER_Y = 8;
const unsigned int GL_U_TEXT_TOP_Y = 16;

void draw_text(float x, float y, const char *string, 
               unsigned int flags = 0, 
               void* font = GL_UTIL_GLUT_TEXT_FONT);

void* get_window_data(int window_id);
void set_window_data(int window_id, void* data);

#endif // VISMATRIX_GL_UTIL_HPP


