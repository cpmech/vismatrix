#ifndef GLUT_WINDOW_HPP
#define GLUT_WINDOW_HPP

#include "xplat_gl.h"

#include "gl_util.hpp"
#include <glui.h>

template <class Derived>
class glut_window
{
public:
    void register_with_glut()
    {
        set_window_data(glut_id, this);

        glutSetWindow(glut_id);
        glutDisplayFunc(Derived::glut_display);
        glutReshapeFunc(Derived::glut_reshape);
        glutKeyboardFunc(Derived::glut_key);
        glutSpecialFunc(Derived::glut_special_key);
        glutMotionFunc(Derived::glut_motion);
        glutMouseFunc(Derived::glut_mouse_click);
    }

    void register_with_glui()
    {
        set_window_data(glut_id, this);

        glutSetWindow(glut_id);
        GLUI_Master.set_glutKeyboardFunc(Derived::glut_key);
        GLUI_Master.set_glutSpecialFunc(Derived::glut_special_key);
        GLUI_Master.set_glutMouseFunc(Derived::glut_mouse_click);
        GLUI_Master.set_glutReshapeFunc(Derived::glut_reshape);
    }

    int get_glut_window_id()
    {
        return (glut_id);
    }

protected:
    int glut_id;

    /**
     * Empty prototypes...
     */
    void display()
    {
    }

    void reshape(int w, int h)
    {
    }

    void key(unsigned char key, int x, int y)
    {
    }

    void special_key(int key, int x, int y)
    {
    }

    void motion(int x, int y)
    {
    }

    void mouse_click(int button, int state, int x, int y)
    {
    }

    void menu(int value)
    {
    }

public:
    static void glut_display()
    {
        Derived* gw = get_class_pointer();
        if (gw)
            gw->display();
    }

    static void glut_reshape(int w, int h)
    {
        Derived* gw = get_class_pointer();
        if (gw)
            gw->reshape(w,h);
    }

    static void glut_key(unsigned char key, int x, int y)
    {
        Derived* gw = get_class_pointer();
        if (gw)
            gw->key(key,x,y);
    }
    static void glut_special_key(int key, int x, int y)
    {
        Derived* gw = get_class_pointer();
        if (gw)
            gw->special_key(key,x,y);
    }
    static void glut_motion(int x, int y)
    {
        Derived* gw = get_class_pointer();
        if (gw)
            gw->motion(x,y);
    }
    static void glut_mouse_click(int button, int state, int x, int y)
    {
        Derived* gw = get_class_pointer();
        if (gw)
            gw->mouse_click(button,state,x,y);
    }

    static void glut_menu(int value)
    {
        Derived* gw = get_class_pointer();
        if (gw)
            gw->menu(value);
    }


private:
    static Derived* get_class_pointer()
    {
        int w_id = glutGetWindow();
        void* d = get_window_data(w_id);
        glut_window<Derived>* gw = (glut_window<Derived>*)d;
        //Derived* gw = (Derived*)d;
        if (gw && gw->glut_id==w_id)
            return ((Derived*)gw);
        else
            return (0);
    }
};

#endif // GLUT_WINDOW_HPP


