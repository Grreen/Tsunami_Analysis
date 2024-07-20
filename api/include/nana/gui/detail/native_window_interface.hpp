/*
 *	Platform Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2015 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/native_window_interface.hpp
 */

#ifndef NANA_GUI_DETAIL_NATIVE_WINDOW_INTERFACE_HPP
#define NANA_GUI_DETAIL_NATIVE_WINDOW_INTERFACE_HPP

#include "../basis.hpp"
#include <nana/paint/image.hpp>

namespace nana
{
namespace detail
{

	struct native_interface
	{
		struct window_result
		{
			native_window_type native_handle;

			unsigned width;		//client size
			unsigned height;	//client size

			unsigned extra_width;	//extra border size, it is useful in Windows, ignore in X11 always 0
			unsigned extra_height;	//extra border size, it is useful in Windows, ignore in X11 always 0
		};

		static nana::size __cdecl	primary_monitor_size();
		static rectangle __cdecl screen_area_from_point(const point&);
		static window_result __cdecl create_window(native_window_type, bool nested, const rectangle&, const appearance&);
		static native_window_type __cdecl create_child_window(native_window_type, const rectangle&);

#if defined(NANA_X11)
		static void set_modal(native_window_type);
#endif
		static void __cdecl enable_dropfiles(native_window_type, bool);
		static void __cdecl enable_window(native_window_type, bool);
		// (On Windows) The system displays the large icon in the ALT+TAB dialog box, and the small icon in the window caption.
		static bool __cdecl window_icon(native_window_type, const paint::image& big_icon, const paint::image& small_icon);
		static void __cdecl activate_owner(native_window_type);
		static void __cdecl activate_window(native_window_type);
		static void __cdecl close_window(native_window_type);
		static void __cdecl show_window(native_window_type, bool show, bool active);
		static void __cdecl restore_window(native_window_type);
		static void __cdecl zoom_window(native_window_type, bool ask_for_max);
		static void __cdecl	refresh_window(native_window_type);
		static bool __cdecl is_window(native_window_type);
		static bool __cdecl	is_window_visible(native_window_type);
		static bool __cdecl is_window_zoomed(native_window_type, bool ask_for_max);

		static nana::point __cdecl	window_position(native_window_type);
		static void __cdecl	move_window(native_window_type, int x, int y);
		static void __cdecl	move_window(native_window_type, const rectangle&);
		static void __cdecl bring_top(native_window_type, bool activated);
		static void __cdecl	set_window_z_order(native_window_type, native_window_type wd_after, z_order_action action_if_no_wd_after);

		static void __cdecl	window_size(native_window_type, const size&);
		static void __cdecl	get_window_rect(native_window_type, rectangle&);
		static void __cdecl	window_caption(native_window_type, const nana::string&);
		static nana::string __cdecl	window_caption(native_window_type);
		static void __cdecl	capture_window(native_window_type, bool);
		static nana::point __cdecl	cursor_position();
		static native_window_type __cdecl get_owner_window(native_window_type);
		//For Caret
		static void __cdecl	caret_create(native_window_type, const ::nana::size&);
		static void __cdecl caret_destroy(native_window_type);
		static void __cdecl	caret_pos(native_window_type, const ::nana::point&);
		static void __cdecl caret_visible(native_window_type, bool);

		static void __cdecl	set_focus(native_window_type);
		static native_window_type __cdecl get_focus_window();
		static bool __cdecl calc_screen_point(native_window_type, nana::point&);
		static bool __cdecl calc_window_point(native_window_type, nana::point&);

		static native_window_type __cdecl find_window(int x, int y);
		static nana::size __cdecl check_track_size(nana::size sz, unsigned extra_width, unsigned extra_height, bool true_for_max);
	};


}//end namespace detail
}//end namespace nana
#endif
