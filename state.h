/*
** Top Class Uniforms Application State Interface (state.h)
**
** This code may not be copied, modified or distributed without the
** express written permission of the author(s). Any violation shall
** be prosecuted to the maximum extent possible under law.
**
*************************************************************************
** Project Details:
**
** National University of Science and Technology
** SCS2104 Structured System Analysis and Design 2019
** Group 4: Mthandazo, Ropafadzo, Takura, Kennedy, Alec, Wallis
**
*/

#pragma once

#include <liblec/cui/gui.h>
using namespace liblec::cui;

#include "tcu_db/tcu_db.h"

// application state class
class state {
	bool loggedin_ = false;
	tcu_db::credentials user_credentials;
	tcu_db database;
	int bottom_margin_ = 0;

public:
	const size default_size = { 900, 600 };
	const size previous_icon_size = { 40, 40 };
	const int page_title_width = 200;
	const int title_font_size = 11;
	const size menu_icon_size = { 18, 18 };
	const color color_caption = { 180, 180, 180 };
	const color color_status = { 0, 120, 200 };
	const color color_hot = { 21, 79, 139 };
	const color color_menu_add = { 0, 150, 140 };
	const color color_menu_edit = { 0, 150, 140 };
	const color color_menu_del = { 255, 0, 0 };

public:
	std::string get_credits() { return "This app was designed, built and is maintained by\n\n"
		"SCS2104 Group 4, 2019: Mthandazo, Ropafadzo, Takura,\nKennedy, Alec, Wallis.\n\n"
		"All rights reserved."; };
	bool loggedin() { return loggedin_; }
	void loggedin(bool loggedin) { loggedin_ = loggedin; }
	tcu_db& get_db() { return database; }
	tcu_db::credentials& get_user() { return user_credentials; }
	int bottom_margin() { return bottom_margin_; };
	void bottom_margin(const int bottom_margin) { bottom_margin_ = bottom_margin; }
};
