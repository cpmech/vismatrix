#ifndef MATRIX_CANVAS_HPP
#define MATRIX_CANVAS_HPP

#include <string>
#include <limits>
#include <cmath>
#include <vector>

#include "xplat_gl.h"

#include "glut_2d_canvas.h"
#include "matrix_data_panel.hpp"
#include "matrix_data_cursor.hpp"

/**
 * A lightweight wrapper class to implement a sparse matrix as
 * a small set of variables.
 */
template <class index_type, class value_type>
struct sparse_matrix 
{
    std::vector<index_type> ai;
    std::vector<index_type> aj;
    std::vector<value_type> a;
    index_type nrows;
    index_type ncols;
    index_type nnz;
};


/**
 * The matrix_canvas assumes that the matrix
 * can be stored in memory.  It loads a matrix_data_cursor and a 
 * matrix_data_panel to handle other details of the implementation.
 */
class matrix_canvas
    : public glut_2d_canvas
{
protected:
    typedef int index_type;
    typedef double value_type;
    typedef glut_2d_canvas super;

    // large_scale_nz controls when we build the matrix _m_fast to draw
    // quickly and then ``fill in'' later
    const static int large_scale_nz = 524288;

    sparse_matrix<index_type, value_type> _m;

    std::vector<index_type> irperm;
    std::vector<index_type> cperm;
    std::vector<index_type> icperm;
    bool rperm_loaded;
    bool cperm_loaded;

    std::vector<std::string> rlabel;
    std::vector<std::string> clabel;
    std::vector<value_type> rnorm;
    std::vector<value_type> cnorm;

    struct {
        value_type min_val;
        value_type max_val;
        index_type max_degree;
        index_type min_degree;
    } matrix_stats;

    std::string matrix_filename;
    bool matrix_loaded;

    GLuint matrix_display_list;

public:
    matrix_canvas(int w, int h);

    void post_constructor();

    // glut functions
    virtual void draw();
    virtual void reshape(int w, int h);

    void motion(int w, int h);
    void mouse_click(int button, int state, int x, int y);

    void menu(int value);

    virtual void special_key(int key, int x, int y);
    virtual void key(unsigned char key, int x, int y);

    //
    // control functions
    //

    enum permutation_state_type {
        no_permutation=0,
        row_permutation=1,
        column_permutation=2,
        row_column_permutation=3
    };

    enum normalization_state_type {
        no_normalization=0,
        row_normalization=1,
        column_normalization=2,
        row_column_normalization=3
    };

    enum colormap_state_type {
        first_colormap=1,
        user_colormap=1,
        rainbow_colormap=2,
        bone_colormap=3,
        spring_colormap=4,
        last_colormap,
    };

    void show_data_panel();
    void hide_data_panel();

    void set_point_alpha(float a) { if (a >= 0. && a <= 1.) point_alpha = a; }
    float get_point_alpha() { return (point_alpha); }

    float get_aspect() { return aspect; }
    void set_aspect(float r) { if (r > 0) aspect = r; }

    void home();
    
    void set_permutation(permutation_state_type p) { permutation_state = p; }
    void set_normalization(normalization_state_type n) { normalization_state = n; }
       
    colormap_state_type get_colormap();
    void set_colormap(colormap_state_type c);
    void set_next_colormap();

    void set_border_color(float r, float g, float b) 
    { border_color[0]=r; border_color[1]=g; border_color[2]=b; }

    // data loading
    bool load_matrix(const std::string& filename,
        bool symmetrize = false);
    bool load_permutations(const std::string& rperm_filename,
        const std::string& cperm_filename);
    bool load_labels(const std::string& rlabel_filename,
        const std::string& clabel_filename);

protected:
    // matrix drawing
    void draw_full_matrix();
    void draw_partial_matrix(int r1, int c1, int r2, int c2);

    template <bool partial, class NRMap, class NCMap, class PRMap, class PCMap>
    void draw_matrix(int r1, int c1, int r2, int c2,
        value_type min, value_type inv_val_range, float alpha,
        NRMap nrv, NCMap ncv, PRMap iprm, PCMap pcm);

    template <bool partial, class NRMap, class NCMap>
    void draw_matrix_dispatch(int r1, int c1, int r2, int c2,
        value_type min, value_type inv_val_range, float alpha,
        NRMap nrv, NCMap ncv);

    template <bool partial>
    void draw_matrix_dispatch(int r1, int c1, int r2, int c2);

    void write_svg();

    float alpha_from_zoom();

    // control variables
    float point_alpha;
    
    bool data_panel_visible;
    bool control_visible;

    permutation_state_type permutation_state;
    normalization_state_type normalization_state;
    colormap_state_type colormap_state;
    bool colormap_invert;

    struct colormap_type {
        float *map;
        int size;

        colormap_type(float *m, int s) : map(m), size(s) {}
    } colormap;

    float border_color[3];

    // internal functions
    void init_window();
    void init_display_list();   
    void init_menu();

    value_type matrix_value(index_type r, index_type c);
    const std::string& row_label(index_type r);
    const std::string& column_label(index_type r);

    const std::string empty_label;

    const static int menu_file_id = 1;
    const static int menu_exit_id = 2;
    const static int menu_toggle_cursor_id = 3;

    const static int menu_aspect_normal = 4;
    const static int menu_aspect_wide_12 = 5;
    const static int menu_aspect_wide_14 = 6;
    const static int menu_aspect_tall_21 = 7;
    const static int menu_aspect_tall_41 = 8;

    const static int menu_colormap_rainbow = 1001;
    const static int menu_colormap_bone = 1002;
    const static int menu_colormap_spring = 1003;
    const static int menu_colormap_invert = 1101;

    const static int menu_colors_white_bkg = 2001;
    const static int menu_colors_black_bkg = 2002;
    

    const static int panel_offset = 5;
    const static int panel_height = 50;
    
    // workaround for stupid gcc bug
    int p_offset;
    int p_height;

    // put everything I want constructed last here
    matrix_data_panel data_panel;
    matrix_data_cursor data_cursor;
};

#endif // MATRIX_CANVAS_HPP




