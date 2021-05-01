/*
** Top Class Uniforms Login Implementation (tcu_login.cpp)
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

#include "tcu_login.h"
#include "../resource.h"

void tcu_login::on_caption() {
	gui::prompt_params params;
	params.type = gui::prompt_type::ok;
	prompt(params, "", app_state_.get_credits());
}

void tcu_login::on_stop() {
	close();
}

void tcu_login::on_shutdown() {
}

void tcu_login::on_login() {
	// get username and password
	std::string username, password, error;
	get_editbox_text(home_page_name_ + "/username", username, error);
	get_editbox_text(home_page_name_ + "/password", password, error);

	if (username.empty() || password.empty()) return;

	if (!app_state_.get_db().get_user(username, password, app_state_.get_user(), error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	app_state_.loggedin(true);
	stop();
}

tcu_login::tcu_login(const std::string& guid, state& app_state) :
	gui::gui(guid),
	app_state_(app_state),
	home_page_name_("TCU Login") {
}

tcu_login::~tcu_login() {
}

bool tcu_login::layout(gui::page& persistent_page,
	gui::page& home_page,
	std::string& error) {
	// set caption
	home_page.set_name(home_page_name_);

	// set form parameters
	prevent_resizing();

	// set form dimensions
	set_width(350);
	set_min_width(350);
	set_height(400);
	set_min_height(400);

	// set up form icons
	set_icons(ico_resource,
		gui::caption_icon_png{ icon_16, icon_20, icon_24, icon_28, icon_32 });

	// add image
	widgets::image image;
	image.rect = { 105, (long)width() - 105, 30, 160 };
	image.png_resource = png_user;

	home_page.add_image(image);

	// add caption
	widgets::text caption;
	caption.rect = { 50, (long)width() - 50, 210, 230 };
	caption.text_value = "Enter your log-in credentials below";
	caption.alignment = widgets::text_alignment::center;
	caption.color = { 180, 180, 180 };

	home_page.add_text(caption);

	// add username edit box
	widgets::editbox username;
	username.alias = "username";
	username.cue_banner = "Username";
	username.rect = { 50, (long)width() - 50, 235, 255 };

	home_page.add_editbox(username);

	// add password edit box
	widgets::editbox password;
	password.alias = "password";
	password.cue_banner = "Password";
	password.password = true;
	password.rect = { 50, (long)width() - 50, 265, 285 };

	home_page.add_editbox(password);

	// add log-in button
	widgets::button login;
	login.alias = "login";
	login.caption = "Log In";
	login.rect = { 120, (long)width() - 120, 300, 325 };
	login.on_click = [&]() { on_login(); };

	home_page.add_button(login);

	return true;
}