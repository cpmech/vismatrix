#ifndef MATRIX_DATA_PANEL_HPP
#define MATRIX_DATA_PANEL_HPP

/**
 * @file matrix_data_panel.hpp
 * The definition file for the matrix_data_panel class.
 */

/*
 * David Gleich
 * 22 November 2006
 * Copyright, Stanford University
 */

#include "glut_window.hpp"

#include <string>
#include <sstream>

/**
 * The matrix_data_panel class manages the display of the data window 
 * showing additional information about the current cursor position
 * of the matrix.
 */
class matrix_data_panel
    : public glut_window<matrix_data_panel>
{
private:
    typedef glut_window<matrix_data_panel> super;
	
public:
    matrix_data_panel(int parent_glut_id, 
        int w, int h, int x, int y);

    // this is the function called when the matrix canvas updates
    // the cursor
    // void position(int x, int y);
    
    // this draws the window
    void display();
    void reshape(int w, int h);

    void update(int in_row, int in_col, float in_val,
        const std::string& in_rlabel = "",
        const std::string& in_clabel = "")
    {
        row = in_row; col = in_col;
        val = in_val;
        rlabel = in_rlabel; clabel = in_clabel;

        int old_glut_win = glutGetWindow();
        glutSetWindow(super::get_glut_window_id());
        glutPostRedisplay();
        glutSetWindow(old_glut_win);
    }

    void set_background_color(float r, float g, float b)
    { background_color[0]=r; background_color[1]=g; background_color[2]=b; }

    void set_border_color(float r, float g, float b)
    { border_color[0]=r; border_color[1]=g; border_color[2]=b; }

    void set_text_color(float r, float g, float b)
    { text_color[0]=r; text_color[1]=g; text_color[2]=b; }

private:
    int parent_glut_id;
    int width, height;

    int row, col;
    float val;

    std::string rlabel, clabel;
    float background_color[3];
    float border_color[3];
    float text_color[3];
};


#endif // MATRIX_DATA_PANEL_HPP


