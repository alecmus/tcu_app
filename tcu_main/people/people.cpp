/*
** Top Class Uniforms People Implementation (people.cpp)
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

#include "people.h"
#include "../../resource.h"
#include "../organization/organization.h"

void people::on_caption() {
	gui::prompt_params params;
	params.type = gui::prompt_type::ok;
	prompt(params, "", app_state_.get_credits());
}

void people::on_stop() {
	close();
}

void people::on_shutdown() {
}

void people::on_add_phone() {
	std::string number, number_type, error;

	get_editbox_text(home_page_name_ + "/number", number, error);
	get_combobox_text(home_page_name_ + "/number_type", number_type, error);

	if (number.empty() || number_type.empty()) return;

	std::vector<widgets::listview_column> columns;
	std::vector<widgets::listview_row> data;
	get_listview(home_page_name_ + "/phone_list", columns, data, error);

	for (auto& row : data) {
		for (auto& item : row.items) {
			if (item.column_name == "Number")
				if (number == item.item_data)
					return;
		}
	}

	widgets::listview_row row =
	{ { { {"Number"}, {number} },
			  { {"Type"}, {number_type} } } };

	add_listview_row(home_page_name_ + "/phone_list", row, true, error);
	set_editbox_text(home_page_name_ + "/number", "", error);
	set_combobox_text(home_page_name_ + "/number_type", "", error);
}

void people::on_add_organization() {
	organization organization(app_state_);
	organization.modal(*this);

	std::string error;
	if (!organization.run(error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	if (organization.saved()) {
		const auto& organization_info = organization.get_details();

		std::vector<std::string> items;

		std::vector<tcu_db::organization_info> organizations;
		app_state_.get_db().get_organizations(organizations, error);

		for (const auto& organization : organizations) {
			items.push_back(organization.name + " (" + organization.id + ")");
		}

		repopulate_combobox(home_page_name_ + "/organization_name", items, error);

		select_combobox_item(home_page_name_ + "/organization_name", organization_info.name + " (" + organization_info.id + ")", error);
		set_editbox_text(home_page_name_ + "/organization_address", organization_info.address, error);
	}
}

void people::on_organization() {
	std::string organization_name_id, organization_id, error;
	set_editbox_text(home_page_name_ + "/organization_address", "", error);

	get_combobox_text(home_page_name_ + "/organization_name", organization_name_id, error);

	if (!organization_name_id.empty()) {
		auto start = organization_name_id.find('(');
		auto end = organization_name_id.find(')');

		if (start != std::string::npos && end != std::string::npos) {
			start++;
			organization_id = organization_name_id.substr(start, end - start);
		}
	}
	else
		return;

	tcu_db::organization_info organization_info;
	if (!app_state_.get_db().get_organization(organization_id, organization_info, error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	set_editbox_text(home_page_name_ + "/organization_address", organization_info.address, error);
}

void people::on_save() {
	tcu_db::person_details details;

	std::string error, organization_name_id;
	get_combobox_text(home_page_name_ + "/title", details.title, error);
	get_editbox_text(home_page_name_ + "/name", details.name, error);
	get_editbox_text(home_page_name_ + "/surname", details.surname, error);
	get_editbox_text(home_page_name_ + "/address", details.address, error);
	get_editbox_text(home_page_name_ + "/occupation", details.occupation, error);
	get_combobox_text(home_page_name_ + "/organization_name", organization_name_id, error);

	std::vector<widgets::listview_column> columns;
	std::vector<widgets::listview_row> data;
	get_listview(home_page_name_ + "/phone_list", columns, data, error);

	if (details.name.empty() || details.surname.empty()) return;

	if (!organization_name_id.empty()) {
		auto start = organization_name_id.find('(');
		auto end = organization_name_id.find(')');

		if (start != std::string::npos && end != std::string::npos) {
			start++;
			const auto organization_id = organization_name_id.substr(start, end - start);
			app_state_.get_db().get_organization(organization_id, details.organization, error);
		}
	}

	for (auto& row : data) {
		tcu_db::phone phone_info;
		for (auto &item : row.items) {
			if (item.column_name == "Number")
				phone_info.number = item.item_data;
			else
				if (item.column_name == "Type")
					phone_info.type = item.item_data;
		}

		details.phones.push_back(phone_info);
	}

	details.id = liblec::cui::unique_string_short();

	if (!app_state_.get_db().new_person(details, error)) {
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

people::people(state& app_state) :
	app_state_(app_state),
	home_page_name_("TCU: Add Person") {
}

people::~people() {
}

bool people::layout(gui::page& persistent_page, gui::page& home_page, std::string& error) {
	// set caption
	home_page.set_name(home_page_name_);

	// set form parameters
	prevent_resizing();

	// set up form dimensions
	set_width(540);
	set_min_width(540);
	set_height(420);
	set_min_height(420);

	// set up form icons
	set_icons(ico_resource,
		gui::caption_icon_png{ icon_16, icon_20, icon_24, icon_28, icon_32 });

	// add title
	widgets::text title_caption;
	title_caption.text_value = "Title";
	title_caption.color = app_state_.color_caption;
	title_caption.rect = { margin, margin + 250, margin, margin + 20 };

	home_page.add_text(title_caption);

	widgets::combobox title;
	title.alias = "title";
	title.read_only = false;
	title.auto_complete = true;
	title.rect = { margin, margin + 250, title_caption.rect.bottom, title_caption.rect.bottom + 20 };

	title.items = { "Dr.", "Miss.", "Mr.", "Mrs.", "Ms." };
	
	home_page.add_combobox(title);

	// add name
	widgets::text name_caption;
	name_caption.text_value = "Name";
	name_caption.color = app_state_.color_caption;
	name_caption.rect = { margin, margin + 250, title.rect.bottom + margin, title.rect.bottom + margin + 20 };

	home_page.add_text(name_caption);

	widgets::editbox name;
	name.alias = "name";
	name.cue_banner = "Enter name";
	name.rect = { margin, margin + 250, name_caption.rect.bottom, name_caption.rect.bottom + 20 };

	home_page.add_editbox(name);

	// add surname
	widgets::text surname_caption;
	surname_caption.text_value = "Surname";
	surname_caption.color = app_state_.color_caption;
	surname_caption.rect = { name_caption.rect.right + (2 * margin), name_caption.rect.right + (2 * margin) + 250, name_caption.rect.top, name_caption.rect.bottom };

	home_page.add_text(surname_caption);

	widgets::editbox surname;
	surname.alias = "surname";
	surname.cue_banner = "Enter surname";
	surname.rect = { surname_caption.rect.left, surname_caption.rect.right, surname_caption.rect.bottom, surname_caption.rect.bottom + 20 };

	home_page.add_editbox(surname);

	// add address
	widgets::text address_caption;
	address_caption.text_value = "Address";
	address_caption.color = app_state_.color_caption;
	address_caption.rect = { name.rect.left, name.rect.right, name.rect.bottom + margin, name.rect.bottom + margin + 20 };

	home_page.add_text(address_caption);

	widgets::editbox address;
	address.alias = "address";
	address.cue_banner = "Enter address";
	address.rect = { address_caption.rect.left, address_caption.rect.right, address_caption.rect.bottom, address_caption.rect.bottom + 40 };
	address.multiline = true;
	address.scrollbar = true;

	home_page.add_editbox(address);

	// add phones
	widgets::text phones_caption;
	phones_caption.text_value = "Phones";
	phones_caption.color = app_state_.color_caption;
	phones_caption.rect = { address.rect.left, address.rect.right, address.rect.bottom + margin, address.rect.bottom + margin + 20 };

	home_page.add_text(phones_caption);

	widgets::editbox number;
	number.alias = "number";
	number.cue_banner = "Enter phone number";
	number.allowed_set = "+0123456789";
	number.rect = { phones_caption.rect.left, phones_caption.rect.left + 120, phones_caption.rect.bottom + margin, phones_caption.rect.bottom + margin + 20 };

	home_page.add_editbox(number);

	widgets::combobox number_type;
	number_type.alias = "number_type";
	number_type.read_only = false;
	number_type.auto_complete = true;
	number_type.rect = { number.rect.right + margin, number.rect.right + margin + 80, number.rect.top, number.rect.bottom };

	number_type.items = { "Cell", "Home", "Work" };

	home_page.add_combobox(number_type);

	widgets::image add;
	add.alias = "add";
	add.tooltip = "Add phone number to list";
	add.png_resource = png_add;
	add.change_color = true;
	add.color.color = app_state_.color_menu_add;
	add.color.color_hot = app_state_.color_hot;
	add.color.color_border_hot = add.color.color_border;
	add.color_background_hot = add.color_background;
	add.tight_fit = true;
	add.rect = { phones_caption.rect.right - 20, phones_caption.rect.right, number_type.rect.top, number_type.rect.bottom };
	add.on_click = [&]() { on_add_phone(); };

	home_page.add_image(add);

	widgets::listview phone_list;
	phone_list.alias = "phone_list";
	phone_list.rect = { number.rect.left, add.rect.right, add.rect.bottom + margin, (long)height() - margin - 25 - margin };
	phone_list.border = false;

	phone_list.columns = {
		{ "Number", 130, widgets::listview_column_type::string_ },
		{ "Type", 80, widgets::listview_column_type::string_ }
	};

	phone_list.unique_column_name = "Number";

	home_page.add_listview(phone_list);

	widgets::groupbox group;
	group.rects = { number.rect, number_type.rect, add.rect, phone_list.rect };

	home_page.add_groupbox(group);

	// add occupation
	widgets::text occupation_caption;
	occupation_caption.text_value = "Occupation";
	occupation_caption.color = app_state_.color_caption;
	occupation_caption.rect = { surname.rect.left, surname.rect.right, surname.rect.bottom + (2 * margin), surname.rect.bottom + (2 * margin) + 20 };
	
	home_page.add_text(occupation_caption);

	widgets::editbox occupation;
	occupation.alias = "occupation";
	occupation.cue_banner = "Enter occupation";
	occupation.rect = { occupation_caption.rect.left, occupation_caption.rect.right, occupation_caption.rect.bottom, occupation_caption.rect.bottom + 20 };

	home_page.add_editbox(occupation);

	// add organization
	widgets::text organization_caption;
	organization_caption.text_value = "Organization";
	organization_caption.rect = { occupation.rect.left, occupation.rect.right, occupation.rect.bottom + 2 * margin, occupation.rect.bottom + 2 * margin + 20 };
	organization_caption.color = app_state_.color_caption;

	home_page.add_text(organization_caption);

	widgets::text organization_name_caption;
	organization_name_caption.text_value = "Name";
	organization_name_caption.rect = { organization_caption.rect.left, organization_caption.rect.right, organization_caption.rect.bottom + margin, organization_caption.rect.bottom + margin + 20 };
	organization_name_caption.color = app_state_.color_caption;

	home_page.add_text(organization_name_caption);

	widgets::combobox organization_name;
	organization_name.alias = "organization_name";
	organization_name.auto_complete = true;
	organization_name.read_only = true;
	organization_name.rect = { organization_name_caption.rect.left, organization_name_caption.rect.right - 20 - margin, organization_name_caption.rect.bottom, organization_name_caption.rect.bottom + 20 };

	std::vector<tcu_db::organization_info> organizations;
	app_state_.get_db().get_organizations(organizations, error);

	for (const auto& organization : organizations) {
		organization_name.items.push_back(organization.name + " (" + organization.id + ")");
	}

	organization_name.on_selection = [&]() { on_organization(); };

	home_page.add_combobox(organization_name);

	widgets::image add_organization;
	add_organization.alias = "add_organization";
	add_organization.tooltip = "Add a new organization";
	add_organization.png_resource = png_add;
	add_organization.change_color = true;
	add_organization.color.color = app_state_.color_menu_add;
	add_organization.color.color_hot = app_state_.color_hot;
	add_organization.color.color_border_hot = add_organization.color.color_border;
	add_organization.color_background_hot = add_organization.color_background;
	add_organization.tight_fit = true;
	add_organization.rect = { organization_caption.rect.right - 20, organization_caption.rect.right, organization_name.rect.top, organization_name.rect.bottom };
	add_organization.on_click = [&]() { on_add_organization(); };

	home_page.add_image(add_organization);

	// add organization address
	widgets::text organization_address_caption;
	organization_address_caption.text_value = "Address";
	organization_address_caption.color = app_state_.color_caption;
	organization_address_caption.rect = { organization_name_caption.rect.left, organization_name_caption.rect.right, organization_name.rect.bottom + margin, organization_name.rect.bottom + margin + 20 };

	home_page.add_text(organization_address_caption);

	widgets::editbox organization_address;
	organization_address.alias = "organization_address";
	organization_address.cue_banner = "Enter organization address";
	organization_address.rect = { organization_address_caption.rect.left, organization_address_caption.rect.right, organization_address_caption.rect.bottom, organization_address_caption.rect.bottom + 40 };
	organization_address.multiline = true;
	organization_address.scrollbar = true;
	organization_address.read_only = true;

	home_page.add_editbox(organization_address);

	widgets::groupbox group_2;
	group_2.rects = { organization_name_caption.rect, organization_name.rect, organization_address_caption.rect, organization_address.rect };

	home_page.add_groupbox(group_2);

	widgets::button save;
	save.alias = "save";
	save.caption = "Save";
	save.rect = { (long)width() - margin - 110, (long)width() - margin, phone_list.rect.bottom + margin, phone_list.rect.bottom + margin + 25 };
	save.on_click = [&]() { on_save(); };

	home_page.add_button(save);

	return true;
}

bool people::saved() {
	return saved_;
}

const tcu_db::person_details& people::get_details() {
	return details_;
}
