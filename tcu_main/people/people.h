/*
** Top Class Uniforms People Interface (people.h)
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

#include "../../state.h"
#include "../../tcu_db/tcu_db.h"

class people : public gui {
	state& app_state_;
	const std::string home_page_name_;
	const int margin = 10;
	bool saved_ = false;

	void on_caption() override;
	void on_stop() override;
	void on_shutdown() override;

	void on_add_phone();
	void on_add_organization();
	void on_organization();
	void on_save();

public:
	people(state& app_state);
	~people();

	bool layout(gui::page& persistent_page,
		gui::page& home_page,
		std::string& error) override;

	bool saved();
	const tcu_db::person_details& get_details();

private:
	tcu_db::person_details details_;
};
