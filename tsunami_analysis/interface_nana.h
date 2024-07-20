#ifndef INTERFACE_NANA
#define INTERFACE_NANA
#include "declaration.h"
#include <nana/gui/wvl.hpp>
#include "displaying.h"

extern nana::drawing* pMainPicture;

extern nana::string p_dat;
extern nana::string p_barchart;
extern nana::string p_brick;


void fm_param_nana();
void fm_param_gl();
void fm_main();
void fm_calc();
void fm_gist();
void fm_select_area();
void load_position(nana::string name, bool full);
void handle_resize(GLFWwindow* window, int width, int height);
void handle_keypress(GLFWwindow* window, int key, int scancodes, int action, int modes);
void handle_cursor_pos(GLFWwindow * window, double x, double y);





#endif