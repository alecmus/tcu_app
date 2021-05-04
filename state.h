/*
** Top Class Uniforms Application State Interface (state.h)
**
*************************************************************************
** MIT License
**
** Copyright (c) 2019 Mthandazo J.J. Ncube, Ropafadzo Siziba, Bruce Chimoyo, Kennedy J.J. Maturure,  Alec Musasa, Wallis T. Makurumidze
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this softwareand associated documentation files(the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions :
**
** The above copyright noticeand this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**
*************************************************************************
** Project Details:
**
** National University of Science and Technology
** SCS2104 Structured System Analysis and Design 2019
** Group 4: Mthandazo, Ropafadzo, Bruce, Kennedy, Alec, Wallis
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
		"SCS2104 Group 4, 2019: Mthandazo, Ropafadzo, Bruce,\nKennedy, Alec, Wallis.\n\n"
		"All rights reserved."; };
	bool loggedin() { return loggedin_; }
	void loggedin(bool loggedin) { loggedin_ = loggedin; }
	tcu_db& get_db() { return database; }
	tcu_db::credentials& get_user() { return user_credentials; }
	int bottom_margin() { return bottom_margin_; };
	void bottom_margin(const int bottom_margin) { bottom_margin_ = bottom_margin; }
};
