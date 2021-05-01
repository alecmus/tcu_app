/*
** Top Class Uniforms Organization Implementation (organization.cpp)
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

#include "organization.h"
#include "../../resource.h"

void organization::on_caption() {
	gui::prompt_params params;
	params.type = gui::prompt_type::ok;
	prompt(params, "", app_state_.get_credits());
}

void organization::on_stop() {
	close();
}

void organization::on_shutdown() {
}

void organization::on_save() {
	tcu_db::organization_info details;

	std::string error;
	get_editbox_text(home_page_name_ + "/name", details.name, error);
	get_editbox_text(home_page_name_ + "/address", details.address, error);

	if (details.name.empty()) return;

	details.id = liblec::cui::unique_string_short();

	if (!app_state_.get_db().new_organization(details, error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	saved_ = true;
	details_ = details;
	stop();
}

organization::organization(state& app_state) :
	app_state_(app_state),
	home_page_name_("TCU: Add Organization") {
}

organization::~organization() {
}

bool organization::layout(gui::page& persistent_page, gui::page& home_page, std::string& error) {
	// set caption
	home_page.set_name(home_page_name_);

	// set form parameters
	prevent_resizing();

	// set form dimensions
	set_width(350);
	set_min_width(350);
	set_height(145);
	set_min_height(145);

	// set up form icons
	set_icons(ico_resource,
		gui::caption_icon_png{ icon_16, icon_20, icon_24, icon_28, icon_32 });

	// add name edit box
	widgets::editbox name;
	name.alias = "name";
	name.cue_banner = "Enter Organization Name";
	name.rect = { 50, (long)width() - 50, margin, margin + 20 };

	home_page.add_editbox(name);

	// add address edit box
	widgets::editbox address;
	address.alias = "address";
	address.cue_banner = "Enter Address";
	address.rect = { name.rect.left, name.rect.right, name.rect.bottom + margin, name.rect.bottom + margin + 20 };

	home_page.add_editbox(address);

	// add save button
	widgets::button save;
	save.alias = "save";
	save.caption = "Save";
	save.rect = { 120, (long)width() - 120, address.rect.bottom + (2 * margin), address.rect.bottom + (2 * margin) + 25 };
	save.on_click = [&]() { on_save(); };

	home_page.add_button(save);

	return true;
}

bool organization::saved() {
	return saved_;
}

const tcu_db::organization_info& organization::get_details() {
	return details_;
}
