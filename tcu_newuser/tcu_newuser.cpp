/*
** Top Class Uniforms New User Implementation (tcu_newuser.cpp)
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

#include "tcu_newuser.h"
#include "../resource.h"

void tcu_newuser::on_caption() {
	gui::prompt_params params;
	params.type = gui::prompt_type::ok;
	prompt(params, "", app_state_.get_credits());
}

void tcu_newuser::on_stop() {
	close();
}

void tcu_newuser::on_shutdown() {
}

void tcu_newuser::on_save() {
	// get username and password
	std::string username, password, confirm_password, position, error;
	get_editbox_text(home_page_name_ + "/username", username, error);
	get_editbox_text(home_page_name_ + "/password", password, error);
	get_editbox_text(home_page_name_ + "/confirm_password", confirm_password, error);
	get_combobox_text(home_page_name_ + "/position", position, error);

	if (username.empty() || password.empty()) return;

	if (password != confirm_password) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", "Password mis-match");
		return;
	}

	tcu_db::credentials user;
	user.user_name = username;
	user.user_position = (position == "Administrator") ? tcu_db::clearance::administrator : tcu_db::clearance::regular;

	if (!app_state_.get_db().new_user(user, password, error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
	}

	stop();
}

void tcu_newuser::on_password() {
	// get username and password
	std::string username, password, error;
	get_editbox_text(home_page_name_ + "/username", username, error);
	get_editbox_text(home_page_name_ + "/password", password, error);

	// get password rating and set password strength bar accordingly
	const auto rating = liblec::cui::password_rating(username, password);
	set_password_strength_bar(home_page_name_ + "/strength", rating.strength, error);
	set_text(home_page_name_ + "/rating", rating.rating_summary, error);
}

tcu_newuser::tcu_newuser(const std::string& guid, state& app_state) :
	gui::gui(guid),
	app_state_(app_state),
	home_page_name_("TCU New User") {
}

tcu_newuser::~tcu_newuser() {
}

bool tcu_newuser::layout(gui::page& persistent_page,
	gui::page& home_page,
	std::string& error) {
	// set caption
	home_page.set_name(home_page_name_);

	// set form parameters
	prevent_resizing();

	// set form dimensions
	set_width(350);
	set_min_width(350);
	set_height(480);
	set_min_height(480);

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
	caption.text_value = "Add a new user to the database";
	caption.alignment = widgets::text_alignment::center;
	caption.color = { 180, 180, 180 };

	home_page.add_text(caption);

	// add username edit box
	widgets::editbox username;
	username.alias = "username";
	username.cue_banner = "Username";
	username.rect = { 50, (long)width() - 50, 235, 255 };

	home_page.add_editbox(username);

	// add position combobox
	widgets::combobox position;
	position.alias = "position";
	position.read_only = true;

	position.rect = { 50, (long)width() - 50, 265, 285 };

	position.items.push_back("Administrator");
	position.selected_item = "Administrator";

	home_page.add_combobox(position);

	// add password edit box
	widgets::editbox password;
	password.alias = "password";
	password.cue_banner = "Password";
	password.password = true;
	password.rect = { 50, (long)width() - 50, 300, 320 };
	password.on_type = [&]() { on_password(); };

	home_page.add_editbox(password);

	// add confirm password edit box
	widgets::editbox confirm_password;
	confirm_password.alias = "confirm_password";
	confirm_password.cue_banner = "Confirm Password";
	confirm_password.password = true;
	confirm_password.rect = { 50, (long)width() - 50, 330, 350 };

	home_page.add_editbox(confirm_password);

	// add password strength bar
	widgets::password_strength_bar password_strength;
	password_strength.alias = "strength";
	password_strength.rect = { 50, (long)width() - 50, 355, 360 };

	home_page.add_password_strength_bar(password_strength);

	// add password strength summary caption
	widgets::text rating;
	rating.alias = "rating";
	rating.rect = { 50, (long)width() - 50, 360, 380 };
	rating.alignment = widgets::text_alignment::center;
	rating.color = { 180, 180, 180 };

	home_page.add_text(rating);

	// add groupbox control
	widgets::groupbox group;
	group.rects = {
		password.rect,
		confirm_password.rect,
		password_strength.rect,
		rating.rect
	};

	home_page.add_groupbox(group);

	// add save button
	widgets::button save;
	save.alias = "login";
	save.caption = "Save";
	save.rect = { 120, (long)width() - 120, 400, 425 };
	save.on_click = [&]() { on_save(); };

	home_page.add_button(save);

	return true;
}