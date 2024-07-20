#ifndef INTERFACE_H
#define INTERFACE_H
#include "declaration.h"
#include "displaying.h"

void bar_chart_gl();
void handle_resize(GLFWwindow* window, int width, int height);
void handle_keypress(GLFWwindow* window, int key, int scancodes, int action, int modes);
void handle_cursor_pos(GLFWwindow * window, double x, double y);
extern GLFWwindow* windowgl;
#endif