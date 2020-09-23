/**
 * @file vismatrix.cc
 * The main file for the vismatrix application
 */

/*
 * David Gleich
 * 6 April 2006
 */

// setup YASMIC to define the global verbose variable
#define YASMIC_VERBOSE_UTIL_DEFINE

#include "xplat_gl.h"

#if _MSC_VER >= 1400
	// disable the warning for ifstream::read
	#pragma warning( disable : 4996 )
#endif // _MSC_VER >= 1400

#include "gl_util.hpp"

#include "glut_window.hpp"
#include "matrix_data_panel.hpp"
#include "matrix_canvas.hpp"
#include "matrix_data_panel.hpp"

#include <tclap/CmdLine.h>

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <iterator>
#include <fstream>

#include <boost/timer.hpp>

/*
 * TODO:
 *
 * -Implement storing the matrix as a quad-tree "too" (for now...)
 * -Implement a two-level drawing scheme: level 1 and 2 are large 1024x1024 textures...
 * -Implement colors
 * -Implement SVG output
 * -Implement menu's for different viewing information (view->white background; view->black background)
 * -Implement permutations
 */

void print_header()
{
    std::cerr 
        << "------------------------------------------------------------" << std::endl
        << "vismatrix 2.0 " << std::endl
        << "------------------------------------------------------------" << std::endl
        << "Copyright David Gleich, Leonid Zhukov, 2006-2007.\n"
        << std::endl
        << "For questions, contact David Gleich, dgleich@stanford.edu." << std::endl
        << "------------------------------------------------------------" << std::endl
        << std::endl;
}

template <class RAI>
bool read_ascii_or_binary(const std::string& filename, RAI v)
{
    using namespace std;
    typedef typename iterator_traits<RAI>::value_type Index;

    ifstream test(filename.c_str());
    if (test.is_open())
	{
		// we test for a binary file in the following way...
		// 1.  If the filesize is exactly as predicted (sizeof(Index)*nrows)
		// 2.  If the filesize is at least as big as predicted and there 
		//     are binary bytes in the first 100 characters
		ifstream::off_type begin = test.tellg();
		test.seekg (0, ios::end);
		ifstream::off_type end = test.tellg();

		bool binary2 = false;
		//if (len && (unsigned)(end-begin) >= sizeof(Index)*len)
		{
            test.seekg (0, ios::beg);
			// look for any non ASCII data (it would have to be a list of
			// of ints, so ...)
			for (int i = 0; i < 100; i++)
			{
				char c;
				test.get(c);
				// isspace takes care of newline, tabs, etc.
				if (!std::isdigit(c) && !std::isspace(c))
				{
					binary2 = true;
				}
			}
		}

		test.close();

		if (binary2)
		{
			YASMIC_VERBOSE ( std::cerr << "reading binary file..." << std::endl; )

			// the file is binary
			std::ifstream file(filename.c_str(), ios::binary);

			while (file)
			{
				Index ci;
				file.read((char *)&ci, sizeof(Index));
				*v = ci;
				++v;
			}

		}
		else
		{
			YASMIC_VERBOSE (  std::cerr << "reading ASCII file..." << std::endl; )

			// the degrees file is text
			std::ifstream file(filename.c_str());

			copy(istream_iterator<Index>(file), 
				 istream_iterator<Index>(), v);
		}

        return (true);
	}
    else
    {
        return (false);
    }
}

struct glui_control_info {
    matrix_canvas *wind;
    GLUI_Spinner *alpha_spinner;

    int norm_rows;
    int norm_columns;

    int perm_rows;
    int perm_columns;
};

// allocate a global glui_control object
glui_control_info glui_control;

const static int glui_spinner_alpha_id = 101;

const static int glui_normalization_id = 102;

const static int glui_permutation_id = 104;

void glui_callback(int id)
{
    bool redisplay = false;

    switch(id)
    {
    case glui_spinner_alpha_id:
        glui_control.wind->set_point_alpha(
            glui_control.alpha_spinner->get_float_val());
        redisplay = true;
        break;

    case glui_normalization_id:
        if (glui_control.norm_rows) {
            if (glui_control.norm_columns) {
                glui_control.wind->set_normalization(
                    glui_control.wind->row_column_normalization);
            } else {
                glui_control.wind->set_normalization(
                    glui_control.wind->row_normalization);
            }
        } else {
            if (glui_control.norm_columns) {
                glui_control.wind->set_normalization(
                    glui_control.wind->column_normalization);
            } else {
                glui_control.wind->set_normalization(
                    glui_control.wind->no_normalization);
            }
        }
        break;

    case glui_permutation_id:
        if (glui_control.perm_rows) {
            if (glui_control.perm_columns) {
                glui_control.wind->set_permutation(
                    glui_control.wind->row_column_permutation);
            } else {
                glui_control.wind->set_permutation(
                    glui_control.wind->row_permutation);
            }
        } else {
            if (glui_control.perm_columns) {
                glui_control.wind->set_permutation(
                    glui_control.wind->column_permutation);
            } else {
                glui_control.wind->set_permutation(
                    glui_control.wind->no_permutation);
            }
        }
        break;
    }

    if (redisplay) {
        glutSetWindow(glui_control.wind->get_glut_window_id());
        glutPostRedisplay();
    }

}

int main(int argc, char **argv)
{
    print_header();

    using namespace std;

    string matrix_filename;

    string rperm_filename;
    string cperm_filename;

    string rlabel_filename;
    string clabel_filename;

	bool symmetrize;
    bool nocontrols=true;

    bool rval = false;

	try
	{
		using namespace TCLAP;
		CmdLine cmd("vismatrix", ' ', "2.0");
		UnlabeledValueArg<string> matrix_name("filename", "matrix/graph file name", "", "string");
		cmd.add(matrix_name);

		SwitchArg verbose_arg(
			"", /* short tag */ "verbose", /* long tag */
			"print all information", /* description */ 
			false /* default option */);
		cmd.add(verbose_arg);

		SwitchArg symmetrize_arg(
			"", /* short tag */ "symmetrize", /* long tag */
			"symmetrize an unsymmetric matrix", /* description */ 
			false /* default option */);
		cmd.add(symmetrize_arg);

        SwitchArg nocontrols_arg(
			"", /* short tag */ "nocontrols", /* long tag */
			"do not display the control panel at the bottom", /* description */ 
			false /* default option */);
		cmd.add(nocontrols_arg);

        ValueArg<std::string> rperm_arg(
            "", /* short tag */ "rperm", /* long tag */
            "indices from PERMFILE reorder the rows of the matrix", /* description */
            false, /* not required */ "", /* default option */
            "PERMFILE" /* type descrption*/);
        cmd.add(rperm_arg);

        ValueArg<std::string> cperm_arg(
            "", /* short tag */ "cperm", /* long tag */
            "indices from PERMFILE reorder the columns of the matrix", /* description */
            false, /* not required */ "", /* default option */ 
            "PERMFILE" /* type descrption*/ );
        cmd.add(cperm_arg);

        ValueArg<std::string> rcperm_arg(
            "", /* short tag */ "rcperm", /* long tag */
            "indices from PERMFILE reorder the rows and columns of the matrix", /* description */
            false, /* not required */ "", /* default option */
            "PERMFILE" /* type descrption*/);
        cmd.add(rcperm_arg);

        ValueArg<std::string> rlabel_arg(
            "", /* short tag */ "rlabel", /* long tag */
            "strings from LABELFILE are the row labels of the matrix", /* description */
            false, /* not required */ "", /* default option */
            "LABELFILE" /* type descrption*/);
        cmd.add(rlabel_arg);

        ValueArg<std::string> clabel_arg(
            "", /* short tag */ "clabel", /* long tag */
            "strings from LABELFILE are the column labels of the matrix", /* description */
            false, /* not required */ "", /* default option */ 
            "LABELFILE" /* type descrption*/ );
        cmd.add(clabel_arg);

        ValueArg<std::string> rclabel_arg(
            "", /* short tag */ "rclabel", /* long tag */
            "strings from LABELFILE are the row and column labels of the matrix", /* description */
            false, /* not required */ "", /* default option */
            "LABELFILE" /* type descrption*/);
        cmd.add(rclabel_arg);

		cmd.parse(argc, argv);

        yasmic::yasmic_verbose = verbose_arg.getValue();
		matrix_filename = matrix_name.getValue();

		symmetrize = symmetrize_arg.getValue();

        nocontrols = nocontrols_arg.getValue();

        rperm_filename = rperm_arg.getValue();
        cperm_filename = cperm_arg.getValue();
        if (rcperm_arg.isSet()) {
            rperm_filename = rcperm_arg.getValue();
            cperm_filename = rcperm_arg.getValue();
        }

        rlabel_filename = rlabel_arg.getValue();
        clabel_filename = clabel_arg.getValue();
        if (rclabel_arg.isSet()) {
            rlabel_filename = rclabel_arg.getValue();
            clabel_filename = rclabel_arg.getValue();
        }
	}
	catch (TCLAP::ArgException &e)
	{
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		return (-1);
	}
    
    yasmic::yasmic_verbose = 1;
    //nocontrols = true;

    // don't allow GLUT any parameters :-)
    argc = 1;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);

    // create the window
    matrix_canvas wind(800,600);

    // begin the data loading process
    rval = wind.load_matrix(matrix_filename, symmetrize);
    if (!rval) 
    {
        cerr << "vismatrix : error loading matrix, terminating..." << endl;
        return (-1);
    }

    
    rval = wind.load_permutations(rperm_filename, cperm_filename);
    if (!rval)
    {
        cerr << "vismatrix : error loading permutations, terminating..." << endl;
        return (-1);
    }


    
    rval = wind.load_labels(rlabel_filename, clabel_filename);
    if (!rval)
    {
        cerr << "vismatrix : error loading labels, terminating..." << endl;
        return (-1);
    }

    // this is a work around for a bug with visual c++ where the GLUT
    // calls will crash if we compile in release mode with optimizations
    // enabled
    wind.post_constructor();

    wind.register_with_glui();

    if (!nocontrols)
    {
        glui_control.wind = &wind;

        GLUI* glui_subwin = GLUI_Master.create_glui_subwindow(
        wind.get_glut_window_id(), GLUI_SUBWINDOW_BOTTOM);

        glui_subwin->set_main_gfx_window(wind.get_glut_window_id());
        glui_control.alpha_spinner = glui_subwin->add_spinner("alpha:",
            GLUI_SPINNER_FLOAT, NULL, glui_spinner_alpha_id, glui_callback);
        glui_control.alpha_spinner->set_float_val(wind.get_point_alpha());

        glui_subwin->add_column(false);

        GLUI_Panel *panel_normalization = glui_subwin->add_panel("Normalization");
        glui_subwin->add_checkbox_to_panel(panel_normalization, "Rows", 
            &glui_control.norm_rows, glui_normalization_id, glui_callback);
        glui_subwin->add_column_to_panel(panel_normalization, false);
        glui_subwin->add_checkbox_to_panel(panel_normalization, "Columns",
            &glui_control.norm_columns, glui_normalization_id, glui_callback);

        glui_subwin->add_column(false);

        if (!rperm_filename.empty() || !cperm_filename.empty()) {

            GLUI_Panel *panel_permutations = glui_subwin->add_panel("Permutations");
            if (!rperm_filename.empty()) {
                glui_subwin->add_checkbox_to_panel(panel_permutations,"Rows",
                    &glui_control.perm_rows, glui_permutation_id, glui_callback);
            }
            
            if (!rperm_filename.empty() && !cperm_filename.empty()) {
                glui_subwin->add_column_to_panel(panel_permutations, false);
            }

            if (!cperm_filename.empty()) {
                glui_subwin->add_checkbox_to_panel(panel_permutations,"Columns",
                    &glui_control.perm_columns, glui_permutation_id, glui_callback);
            }

        }

        glui_subwin->add_column(false);

        glui_subwin->add_edittext("Search:",
						GLUI_EDITTEXT_TEXT, NULL)->set_w(150);
    }
    
    // hacky fix around a GLUI bug
    glui_reshape_func(600,400);

    glutMainLoop();

    return (0);
}




