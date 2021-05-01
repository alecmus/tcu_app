/*
** Top Class Uniforms Dashboard Interface (tcu_main.h)
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

// gui class
class tcu_gui : public gui {
	state& app_state_;
	const std::string home_page_name_;
	const int margin = 10;
	bool update_ = false;

	void on_caption() override;
	void on_stop() override;
	void on_shutdown() override;

	void on_people();
	void on_add_person();
	void on_people_list();
	void hide_personview();
	void show_personview();

	void on_orders();
	void on_add_order();
	void on_order_list();
	void on_order_pending();
	void on_order_completed();

	void update_home_page();

public:
	tcu_gui(const std::string& guid, state& app_state);
	~tcu_gui();

	bool layout(gui::page& persistent_page,
		gui::page& home_page,
		std::string& error) override;
};
