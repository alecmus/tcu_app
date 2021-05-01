/*
** Top Class Uniforms Orders Implementation (orders.cpp)
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

#include "orders.h"
#include "../../resource.h"
#include "../organization/organization.h"
#include "../people/people.h"
#include "../item/item.h"

void orders::on_caption() {
	gui::prompt_params params;
	params.type = gui::prompt_type::ok;
	prompt(params, "", app_state_.get_credits());
}

void orders::on_stop() {
	close();
}

void orders::on_shutdown() {
}

void orders::on_new_person() {
	people people(app_state_);
	people.modal(*this);

	std::string error;
	if (!people.run(error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	if (people.saved()) {
		const auto& person_info = people.get_details();

		std::vector<tcu_db::person_details> people;
		if (!app_state_.get_db().get_people(people, error)) {
			gui::prompt_params params;
			params.type = gui::prompt_type::ok;
			params.png_icon_resource = png_error;
			prompt(params, "Error", error);
			return;
		}

		std::vector<std::string> items;

		for (const auto& it : people) {
			items.push_back(it.title + " " + it.name + " " + it.surname + " (" + it.id + ")");
		}

		repopulate_combobox("TCU: Add Order/client", items, error);
		select_combobox_item("TCU: Add Order/client",
			person_info.title + " " + person_info.name + " " + person_info.surname + " (" + person_info.id + ")", error);
	}
}

void orders::on_new_item() {
	item item(app_state_);
	item.modal(*this);

	std::string error;
	if (!item.run(error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	if (item.saved()) {
		const auto& item_info = item.get_details();

		std::vector<std::string> items;

		std::vector<tcu_db::item_info> items_info;
		app_state_.get_db().get_items(items_info, error);

		for (const auto& item : items_info) {
			items.push_back(item.name + " (" + item.id + ")");
		}

		repopulate_combobox(home_page_name_ + "/item", items, error);

		select_combobox_item(home_page_name_ + "/item", item_info.name + " (" + item_info.id + ")", error);
	}
}

void orders::on_add_item() {
	std::string item_name_id, item_id, quantity, error;
	get_combobox_text(home_page_name_ + "/item", item_name_id, error);
	get_editbox_text(home_page_name_ + "/quantity", quantity, error);

	if (quantity.empty()) return;

	if (!item_name_id.empty()) {
		auto start = item_name_id.find('(');
		auto end = item_name_id.find(')');

		if (start != std::string::npos && end != std::string::npos) {
			start++;
			item_id = item_name_id.substr(start, end - start);
		}
	}
	else
		return;

	std::vector<widgets::listview_column> columns;
	std::vector<widgets::listview_row> data;
	get_listview(home_page_name_ + "/item_list", columns, data, error);

	for (auto& row : data) {
		for (auto& item : row.items) {
			if (item.column_name == "ItemID")
				if (item_id == item.item_data)
					return;
		}
	}

	tcu_db::item_info item_info;
	if (!app_state_.get_db().get_item(item_id, item_info, error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	widgets::listview_row row =
	{ { { {"Name"}, {item_info.name} },
		{ {"Quantity"}, {quantity} },
		{ {"Description"}, {item_info.description} },
		{ {"ItemID"}, {item_info.id} } } };

	add_listview_row(home_page_name_ + "/item_list", row, true, error);
	set_editbox_text(home_page_name_ + "/quantity", "", error);
	set_combobox_text(home_page_name_ + "/item_list", "", error);
}

void orders::on_save() {
	tcu_db::order_details order;
	std::string client_name_id, client_id, error;
	get_combobox_text(home_page_name_ + "/client", client_name_id, error);
	get_combobox_text(home_page_name_ + "/status", order.status, error);

	if (order.status.empty()) return;

	std::vector<widgets::listview_column> columns;
	std::vector<widgets::listview_row> data;
	get_listview(home_page_name_ + "/item_list", columns, data, error);

	for (auto& row : data) {
		tcu_db::order_item order_item;
		for (auto& item : row.items) {
			if (item.column_name == "Name")
				order_item.item.name = item.item_data;
			else
				if (item.column_name == "Quantity")
					order_item.quantity = atoi(item.item_data.c_str());
				else
					if (item.column_name == "Description")
						order_item.item.description = item.item_data;
					else
						if (item.column_name == "ItemID")
							order_item.item.id = item.item_data;
		}

		order.order_items.push_back(order_item);
	}

	if (order.order_items.empty()) return;

	if (!client_name_id.empty()) {
		auto start = client_name_id.find('(');
		auto end = client_name_id.find(')');

		if (start != std::string::npos && end != std::string::npos) {
			start++;
			client_id = client_name_id.substr(start, end - start);
		}
	}
	else
		return;

	order.id = liblec::cui::unique_string_short();

	if (!app_state_.get_db().get_person(client_id, order.client, error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	order.attendant = app_state_.get_user();

	const auto time_stamp = liblec::cui::date_gen::time_stamp();

	auto idx = time_stamp.find(' ');
	
	if (idx != std::string::npos) {
		order.date = time_stamp.substr(0, idx);
		order.time = time_stamp;
		order.time.erase(0, idx + 1);
	}

	if (!app_state_.get_db().new_order(order, error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	saved_ = true;
	details_ = order;
	stop();
}

orders::orders(state& app_state) :
	app_state_(app_state),
	home_page_name_("TCU: Add Order") {
}

orders::~orders() {
}

bool orders::layout(gui::page& persistent_page, gui::page& home_page, std::string& error) {
	// set caption
	home_page.set_name(home_page_name_);

	// set form parameters
	prevent_resizing();

	// set up form dimensions
	set_width(350);
	set_min_width(350);
	set_height(500);
	set_min_height(500);

	// set up form icons
	set_icons(ico_resource,
		gui::caption_icon_png{ icon_16, icon_20, icon_24, icon_28, icon_32 });

	// add client
	widgets::text client_caption;
	client_caption.text_value = "Client";
	client_caption.color = app_state_.color_caption;
	client_caption.rect = { margin, (long)width() - margin, margin, margin + 20 };
	
	home_page.add_text(client_caption);

	widgets::combobox client;
	client.alias = "client";
	client.auto_complete = true;
	client.read_only = true;
	client.rect = { client_caption.rect.left, client_caption.rect.right - margin - 20, client_caption.rect.bottom, client_caption.rect.bottom + 20 };

	std::vector<tcu_db::person_details> people;
	app_state_.get_db().get_people(people, error);

	for (const auto& it : people) {
		client.items.push_back(it.title + " " + it.name + " " + it.surname + " (" + it.id + ")");
	}

	home_page.add_combobox(client);

	// add new person
	widgets::image new_person;
	new_person.alias = "new_person";
	new_person.tooltip = "Add a new person";
	new_person.png_resource = png_add;
	new_person.change_color = true;
	new_person.color.color = app_state_.color_menu_add;
	new_person.color.color_hot = app_state_.color_hot;
	new_person.color.color_border_hot = new_person.color.color_border;
	new_person.color_background_hot = new_person.color_background;
	new_person.tight_fit = true;
	new_person.rect = { (long)width() - margin - 20, (long)width() - margin, client.rect.top, client.rect.bottom };
	new_person.on_click = [&]() { on_new_person(); };

	home_page.add_image(new_person);

	// add order items
	widgets::text order_items_caption;
	order_items_caption.text_value = "Order Items";
	order_items_caption.color = app_state_.color_caption;
	order_items_caption.rect = { client.rect.left, client_caption.rect.right, client.rect.bottom + margin, client.rect.bottom + margin + 20 };

	home_page.add_text(order_items_caption);

	widgets::text item_caption;
	item_caption.text_value = "Item";
	item_caption.color = app_state_.color_caption;
	item_caption.rect = { order_items_caption.rect.left, order_items_caption.rect.left + 225, order_items_caption.rect.bottom + (2 * margin), order_items_caption.rect.bottom + (2 * margin) + 20 };

	home_page.add_text(item_caption);

	widgets::combobox item;
	item.alias = "item";
	item.read_only = true;
	item.auto_complete = true;
	item.rect = { item_caption.rect.left, item_caption.rect.right, item_caption.rect.bottom, item_caption.rect.bottom + 20 };

	std::vector<tcu_db::item_info> items_info;
	app_state_.get_db().get_items(items_info, error);

	for (const auto& it : items_info) {
		item.items.push_back(it.name + " (" + it.id + ")");
	}

	home_page.add_combobox(item);

	widgets::text quantity_caption;
	quantity_caption.text_value = "Quantity";
	quantity_caption.color = app_state_.color_caption;
	quantity_caption.rect = { item.rect.right + margin, (long)width() - margin - 20 - margin, item_caption.rect.top, item_caption.rect.bottom };

	home_page.add_text(quantity_caption);

	widgets::editbox quantity;
	quantity.alias = "quantity";
	quantity.allowed_set = "0123456789";
	quantity.limit = 5;
	quantity.rect = { quantity_caption.rect.left, quantity_caption.rect.right, quantity_caption.rect.bottom, quantity_caption.rect.bottom + 20 };
	
	home_page.add_editbox(quantity);

	// add new item
	widgets::image new_item;
	new_item.alias = "new_item";
	new_item.tooltip = "Add a new item";
	new_item.png_resource = png_add;
	new_item.change_color = true;
	new_item.color.color = app_state_.color_menu_add;
	new_item.color.color_hot = app_state_.color_hot;
	new_item.color.color_border_hot = new_item.color.color_border;
	new_item.color_background_hot = new_item.color_background;
	new_item.tight_fit = true;
	new_item.rect = { (long)width() - margin - 20, (long)width() - margin, quantity.rect.top, quantity.rect.bottom };
	new_item.on_click = [&]() { on_new_item(); };

	home_page.add_image(new_item);

	// add 'add' item
	widgets::button add_item;
	add_item.caption = "Add";
	add_item.rect = { quantity.rect.left, (long)width() - margin, quantity.rect.bottom + margin, quantity.rect.bottom + margin + 25 };
	add_item.on_click = [&]() { on_add_item(); };

	home_page.add_button(add_item);

	// add group box
	widgets::groupbox group;
	group.rects = { item_caption.rect, item.rect, quantity_caption.rect, quantity.rect, new_item.rect, add_item.rect };

	home_page.add_groupbox(group);

	widgets::listview item_list;
	item_list.alias = "item_list";
	item_list.rect = { margin, (long)width() - margin, add_item.rect.bottom + (2 * margin), (long)height() - margin - 25 - margin - 20 - margin - 20 - margin };
	item_list.border = false;

	item_list.columns = {
		{ "Name", 180, widgets::listview_column_type::string_ },
		{ "Quantity", 60, widgets::listview_column_type::string_ },
		{ "Description", 200, widgets::listview_column_type::string_ },
		{ "ItemID", 100, widgets::listview_column_type::string_ }
	};

	item_list.unique_column_name = "ItemID";

	home_page.add_listview(item_list);

	// add order status
	widgets::text status_caption;
	status_caption.text_value = "Order Status";
	status_caption.color = app_state_.color_caption;
	status_caption.rect = { margin, (long)width() - margin, item_list.rect.bottom + margin, item_list.rect.bottom + margin + 20 };
	
	home_page.add_text(status_caption);

	widgets::combobox status;
	status.alias = "status";
	status.read_only = true;
	status.auto_complete = true;
	status.rect = { status_caption.rect.left, status_caption.rect.right, status_caption.rect.bottom, status_caption.rect.bottom + 20 };

	status.items = { "Pending", "Completed", "Aborted", "Unspecified" };
	status.selected_item = "Pending";

	home_page.add_combobox(status);

	// add save button
	widgets::button save;
	save.alias = "save";
	save.caption = "Save";
	save.rect = { 120, (long)width() - 120, (long)height() - margin - 25, (long)height() - margin };
	save.on_click = [&]() { on_save(); };

	home_page.add_button(save);

	return true;
}

bool orders::saved() {
	return saved_;
}

const tcu_db::order_details& orders::get_details() {
	return details_;
}
