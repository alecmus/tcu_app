/*
** Top Class Uniforms Login Interface (tcu_login.h)
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
class tcu_login : public gui {
	state& app_state_;
	const std::string home_page_name_;

	void on_caption() override;
	void on_stop() override;
	void on_shutdown() override;

	void on_login();

public:
	tcu_login(const std::string& guid, state& app_state);
	~tcu_login();

	bool layout(gui::page& persistent_page,
		gui::page& home_page,
		std::string& error) override;
};
