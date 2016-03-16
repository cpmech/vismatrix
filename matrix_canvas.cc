/**
 * @file matrix_canvas.cc
 * An implementation of a matrix canvas class to display a sparse matrix
 * for the vismatrix program.  
 */

/*
 * David Gleich
 * 21 November 2006
 * Copyright, Stanford University
 */

#include <stdlib.h>

#include <string>
#include <limits>
#include <cmath>

#include <algorithm>
#include <utility>

// the yasmic files have to be included before anything
// with OpenGL
#define YASMIC_UTIL_LOAD_GZIP
#include <yasmic/compressed_row_matrix.hpp>
#include <yasmic/util/load_crm_matrix.hpp>
#include <yasmic/transpose_matrix.hpp>
#include <yasmic/nonzero_union.hpp>
#include <yasmic/iterator_utility.hpp>

#include <boost/timer.hpp>

#include "glut_2d_canvas.h"
#include "matrix_data_panel.hpp"
#include "matrix_data_cursor.hpp"

#include "matrix_canvas.hpp"

#include "colormaps.hpp"

#include "util/file.hpp"
#include "util/array.hpp"

matrix_canvas::matrix_canvas(int w, int h)
: 
  glut_2d_canvas(w,h,"vismatrix"),  // initialize the window first
      // the order of the rest is the order in which the variables are
      // declared in the class declaration
  	  p_offset(panel_offset), p_height(panel_height),
      data_panel(glut_id, width-2*p_offset, std::max(height/2,p_height), p_offset, p_offset),
            matrix_display_list(0),
            data_panel_visible(false),
            data_cursor(0,0),
            matrix_filename(""),
            matrix_loaded(false),
            point_alpha(0.5f), 
            normalization_state(no_normalization),
            permutation_state(no_permutation),
            colormap_state(rainbow_colormap),
            colormap((float *)spring_color_map, 
                sizeof(spring_color_map)/sizeof(spring_color_map[0])),
            colormap_invert(false),
            rperm_loaded(false),
            cperm_loaded(false)
{
    border_color[0]=1.0f; border_color[1]=1.0f; border_color[2]=1.0f;
}


void matrix_canvas::post_constructor()
{
    init_window();
    init_menu();
    init_display_list();

    set_zoom(0.95f);

    show_data_panel();
}

/**
 * This function draws the matrix.
 */
void matrix_canvas::draw()
{
    using namespace std;
    if (!matrix_loaded) { return; }

    int m = _m.nrows;
    int n = _m.ncols;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // border
	glPointSize(1.0);
    glColor3f(border_color[0], border_color[1], border_color[2]);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(-0.5f, m-0.5f);
    glVertex2f(n-0.5f, m-0.5f);
    glVertex2f(n-0.5f, -0.5f);
    glEnd();

    glPointSize(zoom / (virtual_width*aspect/(float)width));

    float x1,y1,x2,y2;
    world_extents(x1,y1,x2,y2);

    int r1=(int)floor(y1),r2=(int)floor(y2);
    int c1=(int)floor(x1),c2=(int)floor(x2);

    if (std::max(x2-x1,y2-y1) < 16384)
    {
        cout << "drawing partial matrix (" << r1 << ", " << c1 << ") - ("
            << r2 << ", " << c2 << ")" << endl;

        draw_partial_matrix(r1,c1,r2,c2);
    }
    else if (_m.nrows < 32768) {
        cout << "drawing full matrix" << endl;
        // only draw the full matrix if there isn't a very small portion
	    draw_full_matrix();
    }
    else {
        r1=(std::max)(r1,0);
        int r2end=(std::min)(r2,_m.nrows);

        r1=r1+16384*frame;
        r2=(std::min)(r1+16384,r2end);
        cout << "drawing matrix iteratively " << frame << " (" << r1 << ", " << c1 << ") - ("
            << r2 << ", " << c2 << ")" << " " << r2end << endl;

        draw_partial_matrix(r1,c1,r2,c2);

        if (r2 != r2end) {
            display_finished = false;
        }
    }

    if (data_panel_visible)
    {
        int r = data_cursor.get_y();
        int c = data_cursor.get_x();

        if (permutation_state == row_permutation ||
            permutation_state == row_column_permutation) {
            r = irperm[r];
        }
        if (permutation_state == column_permutation ||
            permutation_state == row_column_permutation) {
            c = icperm[c];
        }

        // workaround for stupid VC++ bug
        //data_panel.update(r, c, yasmic::value(r, c, 
        //    static_cast<const Matrix&>(_m)));
        data_panel.update(r,c,(float)matrix_value(r,c),
            row_label(r), column_label(c));

        data_cursor.draw();   
    }
}

template <bool partial, class NRMap, class NCMap, class PRMap, class PCMap>
void matrix_canvas::draw_matrix(int r1, int c1, int r2, int c2,
    value_type min_val, value_type inv_val_range, float alpha,
    NRMap nrv, NCMap ncv, PRMap iprm, PCMap pcm)
{
    int colormap_entry;
    value_type v;

    glBegin( GL_POINTS );
	{
        for (int pi = std::max(0,r1); pi < std::min(r2, _m.nrows); ++pi)
        {
            // i is the real row in the matrix for the pith row
            // of the display
            int i = iprm[pi];

            for (index_type ri = _m.ai[i]; ri < _m.ai[i+1]; ++ri)
            {
                // j is the real column in the matrix for the pjth
                // column of the display
                int j = _m.aj[ri];
                int pj = pcm[j];

                // skip all the columns outside
                if (partial && (pj < c1 || pj > c2)) { continue; }

                v = _m.a[ri]*nrv[i]*ncv[j];

                // scale v to the range [0,1]
                v = v - min_val;
                v = v*inv_val_range;

                if (!colormap_invert) { 
                    colormap_entry = (int)(v*(colormap.size-1)); 
                } else { 
                    colormap_entry = (int)(v*(colormap.size-1)); 
                    colormap_entry=colormap.size-1-colormap_entry;
                }

                glColor4f(colormap.map[colormap_entry*3],
                          colormap.map[colormap_entry*3+1],
                          colormap.map[colormap_entry*3+2],
                          alpha);
                glVertex2f((GLfloat)pj, (GLfloat)pi);
            }
        }
	}
	glEnd();
}

template <bool partial, class NRMap, class NCMap>
void matrix_canvas::draw_matrix_dispatch(int r1, int c1, int r2, int c2,
    value_type min_val, value_type inv_val_range, float alpha,
    NRMap nrv, NCMap ncv)
{
    switch (permutation_state) {
        case no_permutation:
            draw_matrix<partial>(r1,c1,r2,c2,min_val,inv_val_range,alpha,nrv,ncv,
                util::identity_array(),util::identity_array());
            break;

        case row_permutation: 
            draw_matrix<partial>(r1,c1,r2,c2,min_val,inv_val_range,alpha,nrv,ncv,
                &irperm[0],util::identity_array());
            break;

        case column_permutation:
            draw_matrix<partial>(r1,c1,r2,c2,min_val,inv_val_range,alpha,nrv,ncv,
                util::identity_array(),&cperm[0]);
            break;

        case row_column_permutation:
            draw_matrix<partial>(r1,c1,r2,c2,min_val,inv_val_range,alpha,nrv,ncv,
                &irperm[0],&cperm[0]);
            break;
    }
}

template <bool partial>
void matrix_canvas::draw_matrix_dispatch(int r1, int c1, int r2, int c2)
{
    value_type max_val = matrix_stats.max_val;
    value_type min_val = matrix_stats.min_val;

    if (max_val - min_val <= 0) 
    {
        // this sets min_val to something reasonable, and 
        // shows the high end of the colormap if the values
        // are all equal
        min_val = max_val - 1.0;
    }
    value_type inv_val_range = 1.0/(max_val - min_val);

    float alpha = alpha_from_zoom();

    switch (normalization_state) {
        case no_normalization:
            draw_matrix_dispatch<partial>(r1,c1,r2,c2,min_val,inv_val_range,alpha,
                util::constant_array<value_type>(1),util::constant_array<value_type>(1));
            break;

        case row_normalization:
            draw_matrix_dispatch<partial>(r1,c1,r2,c2,0.0,1.0,alpha,
                &rnorm[0],util::constant_array<value_type>(1));
            break;

        case column_normalization:
            draw_matrix_dispatch<partial>(r1,c1,r2,c2,0.0,1.0,alpha,
                util::constant_array<value_type>(1),&cnorm[0]);
            break;

        case row_column_normalization:
            draw_matrix_dispatch<partial>(r1,c1,r2,c2,0.0,1.0,alpha,&rnorm[0],&cnorm[0]);
            break;
    }
}

void matrix_canvas::draw_full_matrix()
{
    draw_matrix_dispatch<false>(0,0,_m.nrows,_m.ncols);
}

void matrix_canvas::draw_partial_matrix(int r1, int c1, int r2, int c2)
{
    draw_matrix_dispatch<true>(r1,c1,r2,c2);
}

/**
 * draw a subset of the matrix parameterized on a few types of modifications
 *
 * Each matrix is drawn with value
 * v*nrv[i]*ncv[j]
 * so that nrv and ncv are the row and column normalization vectors
 *
 * Further, each row and columns are permuted based on prm and pcm, which are
 * the row and column permutation maps, respectively.
 *
 * point i,j in the matrix is drawn as point prm[i], pcm[i]
 *
 * This function always draws the full matrix, and never the 
 * smaller, faster to display version.
 *
 * @param r1 the first row to draw
 * @param r2 the last row to draw
 * @param c1 the first column to draw
 * @param c2 the last column to draw
 * @param nrv a normalization vector for the rows of the matrix.  
 * @param ncv a normalization vector for the columns of the matrix.
 * @param iprm a inverse permutation map for the rows of the matrix.
 * @param pcm a permutation map for the columns of the matrix.
 */
/*template <class NRMap, class NCMap, class PRMap, class PCMap>
void matrix_canvas::draw_partial_matrix(int r1, int c1, int r2, int c2,
        NRMap nrv, NCMap ncv, PRMap iprm, PCMap pcm)
{
    //
    // todo, fix max/min based on the normalization of the matrix
    //
    value_type max_val = matrix_stats.max_val;
    value_type min_val = matrix_stats.min_val;

    if (max_val - min_val <= 0) 
    {
        // this sets min_val to something reasonable, and 
        // shows the high end of the colormap if the values
        // are all equal
        min_val = max_val - 1.0;
    }

    value_type v;
    value_type inv_val_range = 1.0/(max_val - min_val);

    float alpha = alpha_from_zoom();
    int colormap_entry;

    glBegin( GL_POINTS );
	{
        for (int pi = std::max(0,r1); pi < std::min(r2, _m.nrows); ++pi)
        {
            // i is the real row in the matrix for the pith row
            // of the display
            int i = iprm[pi];

            for (index_type ri = _m.ai[i]; ri < _m.ai[i+1]; ++ri)
            {
                // j is the real column in the matrix for the pjth
                // column of the display
                int j = _m.aj[ri];
                int pj = pcm[j];

                // skip all the columns outside
                if (pj < c1 || pj > c2) { continue; }

                v = _m.a[ri]*nrv[i]*ncv[j];

                // scale v to the range [0 ,1]
                v = v - min_val;
                v = v*inv_val_range;

                colormap_entry = (int)(v*(colormap.size-1));

                glColor4f(colormap.map[colormap_entry*3],
                          colormap.map[colormap_entry*3+1],
                          colormap.map[colormap_entry*3+2],
                          alpha);
                glVertex2f(pj, pi);
            }
        }
	}
	glEnd();
}*/

/**
 * Compute a point_alpha from the current zoom level.
 *
 * zoom < 1: alpha = point_alpha
 * zoom s.t. point_size >= 2.0 => alpha = 1
 */
float matrix_canvas::alpha_from_zoom()
{
    float point_size = zoom / (virtual_width*aspect/(float)width);
    if (zoom < 1) { return (point_alpha); }
    else if (point_size >= 2.0f) { return (1.0f); }
    else
    {
        float zoom_1 = 1.0;
        float zoom_2 = 2.0*(virtual_width*aspect/(float)width);

        return 1.0f - (zoom_2 - zoom)/(zoom_2 - zoom_1)*(1.0f-point_alpha);
    }
}

void matrix_canvas::reshape(int w, int h)
{
    super::reshape(w,h);

    // handle the subwindow...
    glutSetWindow(data_panel.get_glut_window_id());
    if (data_panel_visible && w < 4*p_offset || h < 4*p_offset)
    {
        // if the window is too small, then hide it!
        hide_data_panel();
    }
    else if (data_panel_visible)
    {
        glutPositionWindow(p_offset, p_offset);
        glutReshapeWindow(w-2*p_offset, std::min(h/2,p_height));
    }

    glutSetWindow(get_glut_window_id());
    
    int x,y,w_glui,h_glui;
    GLUI_Master.get_viewport_area(&x,&y,&w_glui,&h_glui);
    if (data_panel_visible)
    {
        //y += panel_height;
        h_glui -= p_height;
    }

    super::reshape(w_glui,h_glui);
    glViewport(x,y,w_glui,h_glui);    
}

void matrix_canvas::motion(int x, int y)
{
    display_finished = true;

    // rescale the mouse click 
    int move_x = x-mouse_begin_x;
    int move_y = y-mouse_begin_y;

    double scaled_x, scaled_y;
    scaled_x = scale_to_world((float)move_x);
    scaled_y = scale_to_world((float)move_y);

    double norm_x=scaled_x, norm_y=scaled_y;
    norm_x /= aspect;
    
    if (data_cursor.scaled_motion(norm_x, norm_y))
    {
        begin_mouse_click(x,y);
    }
    else
    {
        super::motion(x, y);
    }
}

void matrix_canvas::mouse_click(int button, int state, int x, int y)
{
    if (data_cursor.mouse_click(button, state, x, y))
    {
        begin_mouse_click(x,y);
    }
    else
    {
        super::mouse_click(button, state, x, y);
    }
}

void matrix_canvas::key(unsigned char key, int x, int y)
{
    switch (key) {
        case 'o':
        case 'O':
            write_svg();
            break;
    }
}

void matrix_canvas::special_key(int key, int x, int y) 
{
    int m = _m.nrows;
    int n = _m.ncols;

    switch (key) {
    case GLUT_KEY_UP:
        data_cursor.set_position(data_cursor.get_x(),data_cursor.get_y()-1);
        glutPostRedisplay();
        break;

    case GLUT_KEY_DOWN:
        data_cursor.set_position(data_cursor.get_x(),data_cursor.get_y()+1);
        glutPostRedisplay();
        break;

    case GLUT_KEY_LEFT:
        data_cursor.set_position(data_cursor.get_x()-1,data_cursor.get_y());
        glutPostRedisplay();
        break;

    case GLUT_KEY_RIGHT:
        data_cursor.set_position(data_cursor.get_x()+1,data_cursor.get_y());
        glutPostRedisplay();
        break;
    }
}


/**
 * This function initializes the window and assures it is
 * sized correctly, etc.
 */
void matrix_canvas::init_window()
{
    // make sure we are the current window
    glutSetWindow(get_glut_window_id());

    set_center(_m.ncols/2, _m.nrows/2);
    set_natural_size(_m.ncols, _m.nrows);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
#if !defined (__APPLE__)
    // POINT_SMOOTHING doesn't work quite right on OS X when I tried it
    glEnable(GL_POINT_SMOOTH);
#endif
}

/**
 * This function creates the display list for the matrix.
 */
void matrix_canvas::init_display_list()
{
    glutSetWindow(get_glut_window_id());

    /*if (glIsList(matrix_display_list)) 
    {
        glDeleteLists(matrix_display_list, 1);
    }

    matrix_display_list = glGenLists(1);

    glNewList(matrix_display_list,GL_COMPILE);
    glEndList();*/    
}

/*
 * =================
 * matrix functions
 * =================
 */

matrix_canvas::value_type matrix_canvas::matrix_value(
    matrix_canvas::index_type r,
    matrix_canvas::index_type c)
{
    if (r < 0 || r >= _m.nrows || c < 0 || c >= _m.ncols) 
    {
        return (value_type)0;
    }

    index_type ri,riend;
    ri = _m.ai[r];
    riend = _m.ai[r+1];

    while (ri < riend) 
    {
        if (c == _m.aj[ri]) { return _m.a[ri]; }
        ++ri;
    }
    return ((value_type)0);
}

const std::string& matrix_canvas::row_label(index_type r)
{
    if (r >= 0 && r < rlabel.size()) {
        return rlabel[r];
    } else {
        return empty_label;
    }
}

const std::string& matrix_canvas::column_label(index_type c)
{
    if (c >= 0 && c < clabel.size()) {
        return clabel[c];
    } else {
        return empty_label;
    }
}

bool matrix_canvas::load_matrix(const std::string& filename,
    bool symmetrize)
{
    using namespace std;
    matrix_loaded = false;

    matrix_filename = filename;

    bool rval;
    boost::timer t0;

    t0.restart();
    cerr << "loading " << matrix_filename << "..." << endl;

    ifstream t(matrix_filename.c_str());
    t.close();
    if (t.fail()) 
    { 
        cerr << matrix_filename << " does not exist." << endl;
        return false; 
    }

	rval = load_crm_matrix(matrix_filename, _m.ai, _m.aj, _m.a, 
        _m.nrows, _m.ncols, _m.nnz);
	if (!rval)
	{
        std::cerr << "error loading matrix: cannot proceed!\n" << std::endl;
		return false;
	}

	std::cerr << "read matrix in " << t0.elapsed() <<  std::endl;

	using namespace yasmic;

	// for operations that need value information, use this type
	typedef compressed_row_matrix<
		vector<index_type>::iterator, vector<index_type>::iterator,
        vector<value_type>::iterator  >
        crs_matrix;  

    if (symmetrize)
	{
		t0.restart();

		_m.nnz = (int)(2*_m.aj.size());

		vector<int> rows_temp(_m.ai);
		vector<int> cols_temp(_m.aj);
		vector<value_type> vals_temp(_m.a);

		std::fill(_m.ai.begin(), _m.ai.end(), 0);
		_m.aj.resize(_m.nnz);
		_m.a.resize(_m.nnz);

		typedef transpose_matrix<crs_matrix> t_matrix;
		typedef nonzero_union<crs_matrix, t_matrix> nzu_matrix;

		crs_matrix m(rows_temp.begin(), rows_temp.end(), cols_temp.begin(), cols_temp.end(), 
					vals_temp.begin(), vals_temp.end(), 
                    _m.nrows, _m.ncols, _m.nnz/2);

		t_matrix mt(m);
		nzu_matrix nzu(m, mt);

		_m.nrows = nrows(nzu);
		_m.ncols = ncols(nzu);

		// load the matrix
		load_matrix_to_crm(nzu, _m.ai.begin(), _m.aj.begin(), _m.a.begin());

		std::cerr << "symmetrized matrix in " << t0.elapsed() <<  std::endl;
	}

	{
		index_type nzstart = _m.nnz;

		t0.restart();

		crs_matrix mlarge(_m.ai.begin(), _m.ai.end(), _m.aj.begin(), _m.aj.end(), 
					_m.a.begin(), _m.a.end(), _m.nrows, _m.ncols, _m.nnz);

        pack_storage(mlarge, std::plus<value_type>());
		sort_storage(mlarge);

		_m.nnz = _m.ai.back();

		std::cerr << "packed matrix in " << t0.elapsed() <<  std::endl;
		std::cerr << "removed " << nzstart - _m.nnz << " nzs" << std::endl;
	}

    {
		std::cerr << "matrix: " << matrix_filename << std::endl;
		std::cerr << "nrows: " << _m.nrows << std::endl;
		std::cerr << "ncols: " << _m.ncols << std::endl;
		std::cerr << "nnz: " << _m.nnz << std::endl;
	}

    matrix_loaded = true;
    init_window();
    data_cursor.set_matrix_size(_m.nrows, _m.ncols);

    //
    // compute matrix stats
    //

    matrix_stats.max_degree = 0;
    matrix_stats.min_degree = std::numeric_limits<index_type>::max();
    matrix_stats.max_val = std::numeric_limits<value_type>::min();
    matrix_stats.min_val = std::numeric_limits<value_type>::max();

    rnorm.resize(_m.nrows);
    cnorm.resize(_m.ncols);

    for (index_type r = 0; r < _m.nrows; ++r)
    {
        index_type deg = _m.ai[r+1] - _m.ai[r];
        matrix_stats.max_degree = std::max(matrix_stats.max_degree, deg);
        matrix_stats.min_degree = std::min(matrix_stats.min_degree, deg);

        for (index_type ri = _m.ai[r]; ri < _m.ai[r+1]; ++ri) 
        {
            value_type val = _m.a[ri];
            matrix_stats.max_val = std::max(matrix_stats.max_val, val);
            matrix_stats.min_val = std::min(matrix_stats.min_val, val);

            rnorm[r] += val*val;
            cnorm[_m.aj[ri]] += val*val;
        }
    }
    for (index_type r=0; r<_m.nrows; ++r) { rnorm[r]=1.0/sqrt(rnorm[r]); }
    for (index_type r=0; r<_m.ncols; ++r) { cnorm[r]=1.0/sqrt(cnorm[r]); }

    return (true);
}

bool matrix_canvas::load_permutations(const std::string& rperm_filename,
        const std::string& cperm_filename)
{
    using namespace std;

    if (!rperm_filename.empty() && !util::file_exists(rperm_filename)) {
        cerr << rperm_filename << " does not exist." << endl;
        return (false);
    }

    if (!cperm_filename.empty() && !util::file_exists(cperm_filename)) {
        cerr << cperm_filename << " does not exist." << endl;
        return (false);
    }

    // use all the boost iostreams stuff loaded from the load_crm_matrix.hpp
    // header file
    typedef boost::iostreams::filtering_stream<
                boost::iostreams::input_seekable> 
        filtered_ifstream;

    if (!rperm_filename.empty())
    {
        YASMIC_VERBOSE( cerr << "reading " << rperm_filename << endl; )
        filtered_ifstream ios_fifs;
        ifstream ifs(rperm_filename.c_str());

        if (util::gzip_header(ifs))
        {
            ifs.seekg(0, ios::beg);
            ios_fifs.push(boost::iostreams::gzip_decompressor());
            YASMIC_VERBOSE(  cerr << "detected gzip" << endl; )
        }

        ios_fifs.push(ifs);

        string line;

        irperm.resize(_m.nrows);

        index_type r;
        for (r=0; r<_m.nrows && !ios_fifs.eof(); ++r) {
            index_type p;
            ifs >> p;
            irperm[r]=p;
        }

        if (r!=_m.nrows) {
            cerr << rperm_filename << " only contains " << r << " entries, " 
                 << " not " << _m.nrows << std::endl;
            return (false);
        }

        rperm_loaded = true;
    }

    if (!cperm_filename.empty())
    {
        YASMIC_VERBOSE( cerr << "reading " << cperm_filename << endl; )
        filtered_ifstream ios_fifs;
        ifstream ifs(cperm_filename.c_str());

        if (util::gzip_header(ifs))
        {
            ifs.seekg(0, ios::beg);
            ios_fifs.push(boost::iostreams::gzip_decompressor());
            YASMIC_VERBOSE(  cerr << "detected gzip" << endl; )
        }

        ios_fifs.push(ifs);

        string line;

        cperm.resize(_m.ncols);
        icperm.resize(_m.ncols);

        index_type c;
        for (c=0; c<_m.ncols && !ios_fifs.eof(); ++c) {
            index_type p;
            ifs >> p;
            icperm[c]=p;
            cperm[p]=c;
        }

        if (c!=_m.ncols) {
            cerr << cperm_filename << " only contains " << c << " entries, " 
                 << " not " << _m.ncols << std::endl;
            return (false);
        }

        cperm_loaded = true;
    }

    return (true);
}
 
bool matrix_canvas::load_labels(const std::string& rlabel_filename,
        const std::string& clabel_filename)
{
    using namespace std;

    if (!rlabel_filename.empty() && !util::file_exists(rlabel_filename)) {
        cerr << rlabel_filename << " does not exist." << endl;
        return (false);
    }

    if (!clabel_filename.empty() && !util::file_exists(clabel_filename)) {
        cerr << clabel_filename << " does not exist." << endl;
        return (false);
    }

    // use all the boost iostreams stuff loaded from the load_crm_matrix.hpp
    // header file
    typedef boost::iostreams::filtering_stream<
                boost::iostreams::input_seekable> 
        filtered_ifstream;

    if (!rlabel_filename.empty())
    {
        YASMIC_VERBOSE( cerr << "reading " << rlabel_filename << endl; )
        filtered_ifstream ios_fifs;
        ifstream ifs(rlabel_filename.c_str());

        if (util::gzip_header(ifs))
        {
            ifs.seekg(0, ios::beg);
            ios_fifs.push(boost::iostreams::gzip_decompressor());
            YASMIC_VERBOSE(  cerr << "detected gzip" << endl; )
        }

        ios_fifs.push(ifs);

        string line;

        while (!ios_fifs.eof()) {
            getline(ios_fifs, line);
            rlabel.push_back(line);
        }
    }

    if (!clabel_filename.empty())
    {
        YASMIC_VERBOSE(  cerr << "reading " << clabel_filename << endl; )
        filtered_ifstream ios_fifs;
        ifstream ifs(clabel_filename.c_str());

        if (util::gzip_header(ifs))
        {
            ifs.seekg(0, ios::beg);
            ios_fifs.push(boost::iostreams::gzip_decompressor());
            YASMIC_VERBOSE(  cerr << "detected gzip" << endl; )
        }

        ios_fifs.push(ifs);

        string line;

        while (!ios_fifs.eof()) {
            getline(ios_fifs, line);
            clabel.push_back(line);
        }
    }

    return (true);
}

/*
 * =================
 * control functions
 * =================
 */

void matrix_canvas::show_data_panel()
{
    glutSetWindow(data_panel.get_glut_window_id());
    glutShowWindow();
    glutPostRedisplay();
    data_panel_visible = true;
    reshape(width, height);
}

void matrix_canvas::hide_data_panel()
{
    glutSetWindow(data_panel.get_glut_window_id());
    glutHideWindow();
    data_panel_visible = false;
    reshape(width, height);
}

void matrix_canvas::set_colormap(colormap_state_type c)
{
    // at the moment, we don't have any support for a user colormap
    if (c == user_colormap) { c = rainbow_colormap; }
    switch (c)
    {
    case rainbow_colormap:
        colormap.map = (float*)rainbow_color_map;
        colormap.size = sizeof(rainbow_color_map)/sizeof(rainbow_color_map[0]);
        break;

    case bone_colormap:
        colormap.map = (float*)bone_color_map;
        colormap.size = sizeof(bone_color_map)/sizeof(bone_color_map[0]);
        break;

    case spring_colormap:
        colormap.map = (float*)spring_color_map;
        colormap.size = sizeof(spring_color_map)/sizeof(spring_color_map[0]);
        break;
    }

    glutPostRedisplay();
}

void matrix_canvas::set_next_colormap()
{
    int next = colormap_state+1;
    if (next == last_colormap) { next = first_colormap; }
    set_colormap((colormap_state_type)next);
}
 
/*
 * =================
 * menu functions
 * =================
 */

void matrix_canvas::init_menu()
{
    int submenu_aspect_ratio = glutCreateMenu(glut_menu);
    glutAddMenuEntry("1:1",menu_aspect_normal);
    glutAddMenuEntry("4:1 (Tall)",menu_aspect_tall_41);
    glutAddMenuEntry("2:1 (Tall)",menu_aspect_tall_21);
    glutAddMenuEntry("1:2 (Wide)",menu_aspect_wide_12);
    glutAddMenuEntry("1:4 (Wide)",menu_aspect_wide_14);

    int submenu_colormap = glutCreateMenu(glut_menu);
    glutAddMenuEntry("Rainbow",menu_colormap_rainbow);
    glutAddMenuEntry("Bone",menu_colormap_bone);
    glutAddMenuEntry("Spring",menu_colormap_spring);
    glutAddMenuEntry("Invert Colormap",menu_colormap_invert);

    int submenu_colors = glutCreateMenu(glut_menu);
    glutAddMenuEntry("White Background",menu_colors_white_bkg);
    glutAddMenuEntry("Black Background",menu_colors_black_bkg);
        
    // create the main menu
    glutCreateMenu(glut_menu);

    //glutAddMenuEntry("Open File...", menu_file_id);
    glutAddMenuEntry("Toggle Cursor", menu_toggle_cursor_id);
    glutAddSubMenu("Aspect Ratio", submenu_aspect_ratio);
    glutAddSubMenu("Colormap", submenu_colormap);
    glutAddSubMenu("Colors", submenu_colors);

    glutAddMenuEntry("Exit", menu_exit_id);


    /*
    int menu,submenu;

	submenu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Red",RED);
	glutAddMenuEntry("Blue",BLUE);
	glutAddMenuEntry("Green",GREEN);

	menu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("White",WHITE);
	glutAddSubMenu("RGB Menu",submenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);*/
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void matrix_canvas::menu(int value)
{
    bool update_screen = false;
    switch (value)
    {
    case menu_file_id:
        break;

    case menu_toggle_cursor_id:
        data_panel_visible ? hide_data_panel() : show_data_panel();
        update_screen = true;
        break;

    case menu_exit_id:
        exit(0);
        break;

    case menu_aspect_normal:
        set_aspect(1.0);
        reshape(width,height);
        update_screen = true;
        break;

    case menu_aspect_wide_12:
        set_aspect(2.0);
        reshape(width,height);
        update_screen = true;
        break;

    case menu_aspect_wide_14:
        set_aspect(4.0);
        reshape(width,height);
        update_screen = true;
        break;

    case menu_aspect_tall_21:
        set_aspect(0.5);
        reshape(width,height);
        update_screen = true;
        break;

    case menu_aspect_tall_41:
        set_aspect(0.25);
        reshape(width,height);
        update_screen = true;
        break;

    case menu_colormap_rainbow:
        set_colormap(rainbow_colormap);
        break;

    case menu_colormap_bone:
        set_colormap(bone_colormap);
        break;

    case menu_colormap_spring:
        set_colormap(spring_colormap);
        break;

    case menu_colormap_invert:
        colormap_invert = !colormap_invert;
        update_screen = true;
        break;

    case menu_colors_white_bkg:
        set_background_color(1.0f,1.0f,1.0f);
        data_panel.set_background_color(0.75f,0.75f,0.75f);
        data_panel.set_text_color(0.0f,0.0f,0.0f);
        data_panel.set_border_color(0.0f,0.0f,1.0f);
        set_border_color(0.0f,0.0f,0.0f);
        data_cursor.set_color(1.0f,0.0f,0.0f);
        update_screen = true;
        break;

    case menu_colors_black_bkg:
        set_background_color(0.0f,0.0f,0.0f);
        data_panel.set_background_color(0.25f,0.25f,0.25f);
        data_panel.set_text_color(1.0f,1.0f,1.0f);
        data_panel.set_border_color(0.0f,1.0f,0.0f);
        set_border_color(1.0f,1.0f,1.0f);
        data_cursor.set_color(1.0f,1.0f,0.0f);
        update_screen = true;
        break;

    }

    if (update_screen) {
        display_finished = true; 
        glutPostRedisplay();
    }
}





