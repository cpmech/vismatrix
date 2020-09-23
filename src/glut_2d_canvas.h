#ifndef VISMATRIX_GLUT_2D_CANVAS_H
#define VISMATRIX_GLUT_2D_CANVAS_H

/**
 * @file glut_2d_canvas.h
 *
 * The main glut_2d_canvas window handler.  This 
 * class provides some slightly higher-level functions 
 * above a GLUT window.
 */

#include "glut_window.hpp"

class glut_2d_canvas
    : public glut_window<glut_2d_canvas>
{
public:
    glut_2d_canvas(int w, int h, const char* title);

    // derived classes should override draw, not display
    virtual void draw();

    virtual void display();
    virtual void reshape(int w, int h);
    virtual void key(unsigned char key, int x, int y);
    virtual void special_key(int key, int x, int y);
    virtual void motion(int x, int y);
    virtual void mouse_click(int button, int state, int x, int y);
    virtual void menu(int value) {}; 

public:

    // window properties
    int width;
    int height;

    float natural_width;
    float natural_height;  

    float zoom;

    float trans_x;
    float trans_y;

    float center_x;
    float center_y;

    float aspect;

    void set_natural_size(float w, float h)
    {
        natural_width = w; 
        natural_height = h;
        glutPostRedisplay();
    }

    void set_center(float x, float y)
    {
        center_x = x;
        center_y = y;
        glutPostRedisplay();
    }

    void set_window_size(int w, int h)
    {
        glutSetWindow(glut_id);
        glutReshapeWindow(w,h);
    }

    void set_zoom(float z)
    {
        zoom = z;
        glutPostRedisplay();
    }

    void set_aspect(float a)
    {
        aspect = a;
    }

    void set_background_color(float r, float g, float b)
    { background_color[0]=r; background_color[1]=g; background_color[2]=b; }



protected:
    enum {
        STATE_NONE = 100,
        STATE_MOVE,
        STATE_ZOOM
    } mouse_state;

    // used by the mouse handler
    int mouse_begin_x;
    int mouse_begin_y;

    float virtual_width;
    float virtual_height;  

    void begin_mouse_click(int x, int y)
    {
        mouse_begin_x = x;
        mouse_begin_y = y;
    }

    float background_color[3];

    bool display_finished;
    int frame;
};

#endif // VISMATRIX_GLUT_2D_CANVAS_H


