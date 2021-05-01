/*
** Top Class Uniforms New User Interface (tcu_newuser.h)
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

#include "../state.h"

// gui login class
class tcu_newuser : public gui {
	state& app_state_;
	const std::string home_page_name_;

	void on_caption() override;
	void on_stop() override;
	void on_shutdown() override;

	void on_save();
	void on_password();

public:
	tcu_newuser(const std::string& guid, state& app_state);
	~tcu_newuser();

	bool layout(gui::page& persistent_page,
		gui::page& home_page,
		std::string& error) override;
};
