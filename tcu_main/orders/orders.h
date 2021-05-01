/*
** Top Class Uniforms Orders Interface (orders.h)
**
*************************************************************************
** MIT License
**
** Copyright (c) 2019 Mthandazo J.J. Ncube, Ropafadzo Siziba, Bruce Chimoyo, Kennedy J.J. Maturure,  Alec T. Musasa, Wallis T. Makurumidze
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

#include "../../state.h"
#include "../../tcu_db/tcu_db.h"

class orders : public gui {
	state& app_state_;
	const std::string home_page_name_;
	const int margin = 10;
	bool saved_ = false;

	void on_caption() override;
	void on_stop() override;
	void on_shutdown() override;

	void on_new_person();
	void on_new_item();
	void on_add_item();

	void on_save();

public:
	orders(state& app_state);
	~orders();

	bool layout(gui::page& persistent_page,
		gui::page& home_page,
		std::string& error) override;

	bool saved();
	const tcu_db::order_details& get_details();

private:
	tcu_db::order_details details_;
};
