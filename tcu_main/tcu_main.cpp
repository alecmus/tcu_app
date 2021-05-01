/*
** Top Class Uniforms Dashboard Implementation (tcu_main.cpp)
**
*************************************************************************
** MIT License
**
** Copyright (c) 2019 Mthandazo J.J. Ncube, Ropafadzo Siziba, Takura Chimoyo, Kennedy J.J. Maturure,  Alec T. Musasa, Wallis T. Makurumidze
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
** Group 4: Mthandazo, Ropafadzo, Takura, Kennedy, Alec, Wallis
*/

#include "tcu_main.h"
#include "../resource.h"
#include "people/people.h"
#include "orders/orders.h"

void tcu_gui::on_caption() {
	gui::prompt_params params;
	params.type = gui::prompt_type::ok;
	prompt(params, "", app_state_.get_credits());
}

void tcu_gui::on_stop() {
	gui::prompt_params params;
	params.type = gui::prompt_type::yes_no;
	if (prompt(params, "", "Are you sure to close?"))
		close();
}

void tcu_gui::on_shutdown() {
}

void tcu_gui::on_people() {
	if (!page_exists("People")) {
		page page("People");

		// add back icon
		widgets::image back;
		back.png_resource = png_back;
		back.rect = { margin, margin + app_state_.previous_icon_size.width, margin, margin + app_state_.previous_icon_size.height };
		back.on_click = [&]() { show_previous_page(); };

		page.add_image(back);

		// add people icon
		widgets::image people;
		people.png_resource = png_people;
		people.rect = { back.rect.right, back.rect.right + back.rect.width(), back.rect.top, back.rect.top + back.rect.height() };

		page.add_image(people);

		// add title
		widgets::text title;
		title.text_value = "People";
		title.font_size = app_state_.title_font_size;
		title.rect = { people.rect.right + margin, people.rect.right + margin + app_state_.page_title_width, people.rect.top, people.rect.top + (app_state_.previous_icon_size.height / 2) };
		
		page.add_text(title);

		// add description
		widgets::text description;
		description.text_value = "Manage contact information";
		description.color = app_state_.color_caption;
		description.rect = { people.rect.right + margin, people.rect.right + margin + app_state_.page_title_width, title.rect.bottom, people.rect.bottom };

		page.add_text(description);

		// add 'add person' icon
		widgets::image add;
		add.alias = "add";
		add.tooltip = "Add a new person";
		add.png_resource = png_add;
		add.change_color = true;
		add.color.color = app_state_.color_menu_add;
		add.color.color_hot = app_state_.color_hot;
		add.color.color_border_hot = add.color.color_border;
		add.color_background_hot = add.color_background;
		add.tight_fit = true;
		add.rect = { margin, margin + app_state_.menu_icon_size.width, people.rect.bottom + margin, people.rect.bottom + margin + app_state_.menu_icon_size.height };
		add.on_click = [&]() { on_add_person(); };

		page.add_image(add);

		// add 'edit person' icon
		widgets::image edit;
		edit.alias = "edit";
		edit.tooltip = "Edit a person's information";
		edit.png_resource = png_edit;
		edit.change_color = true;
		edit.color.color = app_state_.color_menu_edit;
		edit.color.color_hot = app_state_.color_hot;
		edit.color.color_border_hot = edit.color.color_border;
		edit.color_background_hot = edit.color_background;
		edit.tight_fit = true;
		edit.rect = { add.rect.right + 2 * margin, add.rect.right + 2 * margin + app_state_.menu_icon_size.width, add.rect.top, add.rect.bottom };

		page.add_image(edit);

		// add 'delete person' icon
		widgets::image del;
		del.alias = "del";
		del.tooltip = "Delete a person's information";
		del.png_resource = png_del;
		del.change_color = true;
		del.color.color = app_state_.color_menu_del;
		del.color.color_hot = app_state_.color_hot;
		del.color.color_border_hot = del.color.color_border;
		del.color_background_hot = del.color_background;
		del.tight_fit = true;
		del.rect = { edit.rect.right + 2 * margin, edit.rect.right + 2 * margin + app_state_.menu_icon_size.width, edit.rect.top, edit.rect.bottom };

		page.add_image(del);

		// add status text
		widgets::text status;
		status.alias = "status";
		status.color = app_state_.color_status;
		status.rect = { margin, margin + 200, (long)height() - app_state_.bottom_margin() - margin - 20, (long)height() - app_state_.bottom_margin() - margin };
		status.on_resize.perc_v = 100;

		// add people list
		widgets::listview people_list;
		people_list.alias = "people_list";
		people_list.rect = { margin, margin + 200, del.rect.bottom + margin, status.rect.top - margin };
		people_list.on_resize = { 0, 0, 0, 100 };
		people_list.border = false;
		people_list.on_selection = [&]() { on_people_list(); };

		people_list.columns = {
			{ "#", 35, widgets::listview_column_type::int_ },
			{ "Name", 170, widgets::listview_column_type::string_ },
			{ "ID", 100, widgets::listview_column_type::string_ }
		};

		people_list.unique_column_name = "ID";

		{
			std::vector<tcu_db::person_details> people;
			std::string error;

			if (app_state_.get_db().get_people(people, error)) {
				int i = 0;
				for (const auto& person : people) {
					widgets::listview_row row;

					row.items.push_back({ "#", std::to_string(i + 1) });
					row.items.push_back({ "Name", person.title + " " + person.name + " " + person.surname });
					row.items.push_back({ "ID", person.id });
					people_list.data.push_back(row);
					i++;
				}
			}

			std::string status_text = std::to_string(people_list.data.size());
			if (people_list.data.size() == 1) {
				status_text += " person";
			}
			else {
				status_text += " people";
			}

			status.text_value = status_text;
		}

		page.add_text(status);

		page.add_listview(people_list);

		// add name
		widgets::text name_caption;
		name_caption.alias = "name_caption";
		name_caption.text_value = "Name";
		name_caption.color = app_state_.color_caption;
		name_caption.rect = { people_list.rect.right + 4 * margin, (long)width() - margin, people_list.rect.top, people_list.rect.top + 20 };
		name_caption.on_resize.perc_width = 100;

		page.add_text(name_caption);
	
		widgets::text name;
		name.alias = "name";
		name.rect = { name_caption.rect.left, name_caption.rect.right, name_caption.rect.bottom, name_caption.rect.bottom + 20 };
		name.on_resize = name_caption.on_resize;

		page.add_text(name);

		// add address
		widgets::text address_caption;
		address_caption.alias = "address_caption";
		address_caption.text_value = "Address";
		address_caption.color = app_state_.color_caption;
		address_caption.rect = { name.rect.left, name.rect.right, name.rect.bottom + margin, name.rect.bottom + margin + 20 };
		address_caption.on_resize = name.on_resize;

		page.add_text(address_caption);

		widgets::text address;
		address.alias = "address";
		address.rect = { address_caption.rect.left, address_caption.rect.right, address_caption.rect.bottom, address_caption.rect.bottom + 20 };
		address.on_resize = address_caption.on_resize;

		page.add_text(address);

		// add occupation
		widgets::text occupation_caption;
		occupation_caption.alias = "occupation_caption";
		occupation_caption.text_value = "Occupation";
		occupation_caption.color = app_state_.color_caption;
		occupation_caption.rect = { address.rect.left, address.rect.right, address.rect.bottom + margin, address.rect.bottom + margin + 20 };
		occupation_caption.on_resize = address.on_resize;

		page.add_text(occupation_caption);

		widgets::text occupation;
		occupation.alias = "occupation";
		occupation.rect = { occupation_caption.rect.left, occupation_caption.rect.right, occupation_caption.rect.bottom, occupation_caption.rect.bottom + 20 };
		occupation.on_resize = occupation_caption.on_resize;

		page.add_text(occupation);

		// add organization
		widgets::text organization_caption;
		organization_caption.alias = "organization_caption";
		organization_caption.text_value = "Organization";
		organization_caption.color = app_state_.color_caption;
		organization_caption.rect = { occupation.rect.left, occupation.rect.right, occupation.rect.bottom + margin, occupation.rect.bottom + margin + 20 };
		organization_caption.on_resize = occupation.on_resize;

		page.add_text(organization_caption);

		widgets::text organization;
		organization.alias = "organization";
		organization.rect = { organization_caption.rect.left, organization_caption.rect.right, organization_caption.rect.bottom, organization_caption.rect.bottom + 20 };
		organization.on_resize = organization_caption.on_resize;

		page.add_text(organization);

		// add phones
		widgets::text phones_caption;
		phones_caption.alias = "phones_caption";
		phones_caption.text_value = "Phones";
		phones_caption.color = app_state_.color_caption;
		phones_caption.rect = { organization.rect.left, organization.rect.right, organization.rect.bottom + margin, organization.rect.bottom + margin + 20 };
		phones_caption.on_resize = organization.on_resize;

		page.add_text(phones_caption);

		widgets::listview phones;
		phones.alias = "phones";
		phones.border = false;
		phones.gridlines = false;
		phones.rect = { phones_caption.rect.left, phones_caption.rect.right, phones_caption.rect.bottom, people_list.rect.bottom };
		phones.on_resize = phones_caption.on_resize;
		phones.on_resize.perc_height = 100;

		phones.columns = {
			{ "Type", 130, widgets::listview_column_type::string_ },
			{ "Number", 300, widgets::listview_column_type::string_}
		};

		phones.unique_column_name = "Number";

		page.add_listview(phones);

		add_page(page);

		hide_personview();
	}

	show_page("People");
}

void tcu_gui::on_add_person() {
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

		std::vector<widgets::listview_row> data;

		int i = 0;
		for (const auto& person : people) {
			widgets::listview_row row;
			row.items.push_back({ "#", std::to_string(i + 1) });
			row.items.push_back({ "Name", person.title + " " + person.name + " " + person.surname });
			row.items.push_back({ "ID", person.id });
			data.push_back(row);
			i++;
		}

		repopulate_listview("People/people_list", data, error);

		std::string status_text = std::to_string(data.size());
		if (data.size() == 1) {
			status_text += " person";
		}
		else {
			status_text += " people";
		}

		set_text("People/status", status_text, error);
		hide_personview();
	}
}

void tcu_gui::on_people_list() {
	std::vector<widgets::listview_row> rows;
	std::string error;
	get_listview_selected("People/people_list", rows, error);

	if (rows.size() == 1) {
		std::string person_id;
		for (auto& item : rows[0].items) {
			if (item.column_name == "ID") {
				person_id = item.item_data;
				break;
			}
		}

		if (!person_id.empty()) {
			tcu_db::person_details person;
			if (!app_state_.get_db().get_person(person_id, person, error)) {
				gui::prompt_params params;
				params.type = gui::prompt_type::ok;
				params.png_icon_resource = png_error;
				prompt(params, "Error", error);
				return;
			}

			set_text("People/name", person.title + " " + person.name + " " + person.surname, error);
			set_text("People/address", person.address, error);
			set_text("People/occupation", person.occupation, error);

			if (!person.organization.id.empty())
				set_text("People/organization", person.organization.name + ", " + person.organization.address, error);

			std::vector<widgets::listview_row> data;

			for (const auto& phone : person.phones) {
				widgets::listview_row row;
				row.items.push_back({ "Number", phone.number });
				row.items.push_back({ "Type", phone.type });
				data.push_back(row);
			}

			repopulate_listview("People/phones", data, error);

			show_personview();
		}
	}
	else
		hide_personview();
}

void tcu_gui::hide_personview() {
	hide(std::string("People/name_caption"));
	hide(std::string("People/name"));
	hide(std::string("People/address_caption"));
	hide(std::string("People/address"));
	hide(std::string("People/occupation_caption"));
	hide(std::string("People/occupation"));
	hide(std::string("People/organization_caption"));
	hide(std::string("People/organization"));
	hide(std::string("People/phones_caption"));
	hide(std::string("People/phones"));
}

void tcu_gui::show_personview() {
	show(std::string("People/name_caption"));
	show(std::string("People/name"));
	show(std::string("People/address_caption"));
	show(std::string("People/address"));
	show(std::string("People/occupation_caption"));
	show(std::string("People/occupation"));
	show(std::string("People/organization_caption"));
	show(std::string("People/organization"));
	show(std::string("People/phones_caption"));
	show(std::string("People/phones"));
}

void tcu_gui::on_orders() {
	if (!page_exists("Orders")) {
		page page("Orders");

		// add back icon
		widgets::image back;
		back.png_resource = png_back;
		back.rect = { margin, margin + app_state_.previous_icon_size.width, margin, margin + app_state_.previous_icon_size.height };
		back.on_click = [&]() {
			show_previous_page();
			if (update_) update_home_page();
			update_ = false;
		};

		page.add_image(back);

		// add people icon
		widgets::image people;
		people.png_resource = png_orders;
		people.rect = { back.rect.right, back.rect.right + back.rect.width(), back.rect.top, back.rect.top + back.rect.height() };

		page.add_image(people);

		// add title
		widgets::text title;
		title.text_value = "Orders";
		title.font_size = app_state_.title_font_size;
		title.rect = { people.rect.right + margin, people.rect.right + margin + app_state_.page_title_width, people.rect.top, people.rect.top + (app_state_.previous_icon_size.height / 2) };

		page.add_text(title);

		// add description
		widgets::text description;
		description.text_value = "View and manage orders";
		description.color = app_state_.color_caption;
		description.rect = { people.rect.right + margin, people.rect.right + margin + app_state_.page_title_width, title.rect.bottom, people.rect.bottom };

		page.add_text(description);

		// add 'add order' icon
		widgets::image add;
		add.alias = "add";
		add.tooltip = "Add a new order";
		add.png_resource = png_add;
		add.change_color = true;
		add.color.color = app_state_.color_menu_add;
		add.color.color_hot = app_state_.color_hot;
		add.color.color_border_hot = add.color.color_border;
		add.color_background_hot = add.color_background;
		add.tight_fit = true;
		add.rect = { margin, margin + app_state_.menu_icon_size.width, people.rect.bottom + margin, people.rect.bottom + margin + app_state_.menu_icon_size.height };
		add.on_click = [&]() { on_add_order(); };

		page.add_image(add);

		// add 'edit order' icon
		widgets::image edit;
		edit.alias = "edit";
		edit.tooltip = "Edit an order";
		edit.png_resource = png_edit;
		edit.change_color = true;
		edit.color.color = app_state_.color_menu_edit;
		edit.color.color_hot = app_state_.color_hot;
		edit.color.color_border_hot = edit.color.color_border;
		edit.color_background_hot = edit.color_background;
		edit.tight_fit = true;
		edit.rect = { add.rect.right + 2 * margin, add.rect.right + 2 * margin + app_state_.menu_icon_size.width, add.rect.top, add.rect.bottom };

		page.add_image(edit);

		// add 'delete order' icon
		widgets::image del;
		del.alias = "del";
		del.tooltip = "Delete an order";
		del.png_resource = png_del;
		del.change_color = true;
		del.color.color = app_state_.color_menu_del;
		del.color.color_hot = app_state_.color_hot;
		del.color.color_border_hot = del.color.color_border;
		del.color_background_hot = del.color_background;
		del.tight_fit = true;
		del.rect = { edit.rect.right + 2 * margin, edit.rect.right + 2 * margin + app_state_.menu_icon_size.width, edit.rect.top, edit.rect.bottom };

		page.add_image(del);

		// add status text
		widgets::text status;
		status.alias = "status";
		status.color = app_state_.color_status;
		status.rect = { margin, margin + 200, (long)height() - app_state_.bottom_margin() - margin - 20, (long)height() - app_state_.bottom_margin() - margin };
		status.on_resize.perc_v = 100;

		// add people list
		widgets::listview order_list;
		order_list.alias = "order_list";
		order_list.rect = { margin, (long)width() - margin, del.rect.bottom + margin, status.rect.top - margin };
		order_list.on_resize = { 0, 0, 100, 100 };
		order_list.border = false;
		order_list.on_selection = [&]() { on_order_list(); };
		order_list.context_menu = {
			{ "Mark pending", false, [&]() { on_order_pending(); } },
			{ "Mark completed", false, [&]() { on_order_completed(); } }
		};

		order_list.columns = {
			{ "ID", 100, widgets::listview_column_type::string_ },
			{ "Date", 170, widgets::listview_column_type::int_ },
			{ "Time", 100, widgets::listview_column_type::string_ },
			{ "Client", 170, widgets::listview_column_type::string_ },
			{ "Attendant", 110, widgets::listview_column_type::string_ },
			{ "Items", 110, widgets::listview_column_type::int_ },
			{ "Status", 100, widgets::listview_column_type::string_ }
		};

		order_list.unique_column_name = "ID";

		{
			std::vector<tcu_db::order_details> orders;
			std::string error;

			if (app_state_.get_db().get_orders(orders, error)) {
				for (const auto& order : orders) {
					widgets::listview_row row;

					row.items.push_back({ "ID", order.id });
					row.items.push_back({ "Date", order.date });
					row.items.push_back({ "Time", order.time });
					row.items.push_back({ "Client", order.client.title + " " + order.client.name + " " + order.client.surname });
					row.items.push_back({ "Attendant", order.attendant.user_name });
					row.items.push_back({ "Items", std::to_string(order.order_items.size()) });
					row.items.push_back({ "Status", order.status, true, 
						(order.status == "Completed") ? color{ 0, 150, 0 } : color{ 255, 0, 0 } });
					order_list.data.push_back(row);
				}
			}

			std::string status_text = std::to_string(order_list.data.size());
			if (order_list.data.size() == 1) {
				status_text += " order";
			}
			else {
				status_text += " orders";
			}

			status.text_value = status_text;
		}

		page.add_text(status);

		page.add_listview(order_list);

		add_page(page);
	}

	show_page("Orders");
}

void tcu_gui::on_add_order() {
	orders order(app_state_);
	order.modal(*this);

	std::string error;
	if (!order.run(error)) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		prompt(params, "Error", error);
		return;
	}

	if (order.saved()) {
		const auto& order_info = order.get_details();

		std::vector<tcu_db::order_details> orders;
		std::string error;

		std::vector<widgets::listview_row> data;

		if (app_state_.get_db().get_orders(orders, error)) {
			for (const auto& order : orders) {
				widgets::listview_row row;

				row.items.push_back({ "ID", order.id });
				row.items.push_back({ "Date", order.date });
				row.items.push_back({ "Time", order.time });
				row.items.push_back({ "Client", order.client.title + " " + order.client.name + " " + order.client.surname });
				row.items.push_back({ "Attendant", order.attendant.user_name });
				row.items.push_back({ "Items", std::to_string(order.order_items.size()) });
				row.items.push_back({ "Status", order.status, true,
						(order.status == "Completed") ? color{ 0, 150, 0 } : color{ 255, 0, 0 } });
				data.push_back(row);
			}
		}

		std::string status_text = std::to_string(data.size());
		if (data.size() == 1) {
			status_text += " order";
		}
		else {
			status_text += " orders";
		}

		set_text("Orders/status", status_text, error);
		repopulate_listview("Orders/order_list", data, error);
		update_home_page();
	}
}

void tcu_gui::on_order_list() {
}

void tcu_gui::on_order_pending() {
	std::string error;
	std::vector<widgets::listview_row> rows;
	get_listview_selected("Orders/order_list", rows, error);

	for (const auto& row : rows) {
		std::string order_id, status;

		widgets::listview_item item_;

		for (const auto& item : row.items) {
			if (item.column_name == "ID")
				order_id = item.item_data;
			else
				if (item.column_name == "Status") {
					item_ = item;
					status = item.item_data;
				}
		}

		if (!order_id.empty() && status != "Pending") {
			// update to completed
			if (app_state_.get_db().order_status_update(order_id, "Pending", error))
			{
				item_.item_data = "Pending";
				item_.custom_text_color = true;
				item_.color_text = color{ 255, 0, 0 };
				update_listview_item("Orders/order_list", item_, row, error);
				update_ = true;
			}
		}
	}
}

void tcu_gui::on_order_completed() {
	std::string error;
	std::vector<widgets::listview_row> rows;
	get_listview_selected("Orders/order_list", rows, error);

	for (const auto& row : rows) {
		std::string order_id, status;

		widgets::listview_item item_;

		for (const auto& item : row.items) {
			if (item.column_name == "ID")
				order_id = item.item_data;
			else
				if (item.column_name == "Status") {
					item_ = item;
					status = item.item_data;
				}
		}

		if (!order_id.empty() && status != "Completed") {
			// update to completed
			if (app_state_.get_db().order_status_update(order_id, "Completed", error))
			{
				item_.item_data = "Completed";
				item_.custom_text_color = true;
				item_.color_text = color{ 0, 150, 0 };
				update_listview_item("Orders/order_list", item_, row, error);
				update_ = true;
			}
		}
	}
}

void tcu_gui::update_home_page() {
	std::string error;
	// update order info on home page

	// get orders
	const auto today = liblec::cui::date_gen::today();
	liblec::cui::date_gen::date week_start, week_end, month_start, month_end, year_start, year_end;
	liblec::cui::date_gen::get_week(today, week_start, week_end);
	liblec::cui::date_gen::get_month(today, month_start, month_end);
	liblec::cui::date_gen::get_year(today, year_start, year_end);

	std::vector<tcu_db::order_details> orders_week;
	app_state_.get_db().get_orders(liblec::cui::date_gen::to_string(week_start),
		liblec::cui::date_gen::to_string(week_end), orders_week, error);

	std::vector<tcu_db::order_details> orders_month;
	app_state_.get_db().get_orders(liblec::cui::date_gen::to_string(month_start),
		liblec::cui::date_gen::to_string(month_end), orders_month, error);

	std::vector<tcu_db::order_details> orders_year;
	app_state_.get_db().get_orders(liblec::cui::date_gen::to_string(year_start),
		liblec::cui::date_gen::to_string(year_end), orders_year, error);

	// set values for order summary
	double week_completed = 0;
	double week_pending = 0;

	for (const auto& it : orders_week) {
		if (it.status == "Pending") week_pending++;
		if (it.status == "Completed") week_completed++;
	}

	double month_completed = 0;
	double month_pending = 0;

	for (const auto& it : orders_month) {
		if (it.status == "Pending") month_pending++;
		if (it.status == "Completed") month_completed++;
	}

	double year_completed = 0;
	double year_pending = 0;

	for (const auto& it : orders_year) {
		if (it.status == "Pending") year_pending++;
		if (it.status == "Completed") year_completed++;
	}

	widgets::piechart_data week_data, month_data, year_data;

	week_data.caption = "Summary";
	week_data.autocolor = false;
	week_data.slices.push_back({ "Pending", week_pending, { 244, 67, 54 } });
	week_data.slices.push_back({ "Completed", week_completed, { 110, 170, 70 } });
	piechart_reload("TCU Order App/week_piechart", week_data, error);
	set_text("TCU Order App/week_pending", std::to_string((int)week_pending) + " pending", error);
	set_text("TCU Order App/week_completed", std::to_string((int)week_completed) + " completed", error);

	month_data.caption = "Summary";
	month_data.autocolor = false;
	month_data.slices.push_back({ "Pending", month_pending, { 255, 163, 0 } });
	month_data.slices.push_back({ "Completed", month_completed, { 30, 90, 145 } });
	piechart_reload("TCU Order App/month_piechart", month_data, error);
	set_text("TCU Order App/month_pending", std::to_string((int)month_pending) + " pending", error);
	set_text("TCU Order App/month_completed", std::to_string((int)month_completed) + " completed", error);

	year_data.caption = "Summary";
	year_data.autocolor = false;
	year_data.slices.push_back({ "Pending", year_pending, { 207, 216, 220 } });
	year_data.slices.push_back({ "Completed", year_completed, { 120, 105, 145 } });
	piechart_reload("TCU Order App/year_piechart", year_data, error);
	set_text("TCU Order App/year_pending", std::to_string((int)year_pending) + " pending", error);
	set_text("TCU Order App/year_completed", std::to_string((int)year_completed) + " completed", error);
}

tcu_gui::tcu_gui(const std::string& guid, state& app_state) :
	gui::gui(guid),
	app_state_(app_state),
	home_page_name_("TCU Order App") {
}

tcu_gui::~tcu_gui() {
}

bool tcu_gui::layout(gui::page& persistent_page,
	gui::page& home_page,
	std::string& error) {
	// set caption
	home_page.set_name(home_page_name_);

	// set up form dimensions
	set_width(app_state_.default_size.width);
	set_min_width(width());
	set_height(app_state_.default_size.height);
	set_min_height(height());

	// set up form icons
	set_icons(ico_resource,
		gui::caption_icon_png{ icon_16, icon_20, icon_24, icon_28, icon_32 });

	// display logged in user at the bottom (persistent)
	widgets::text user;
	user.alias = "user";
	user.rect = { margin, (long)width() - margin, (long)height() - 21, (long)height() - 1 };
	user.alignment = widgets::text_alignment::middle;
	user.on_resize = { 0, 100, 100, 0 };
	user.text_value = app_state_.get_user().user_name + " (" + (app_state_.get_user().user_position == tcu_db::clearance::administrator ? "Administrator" : "Regular") + ")";
	user.color = { 180, 180, 180 };
	persistent_page.add_text(user);

	// capture bottom border position
	app_state_.bottom_margin(height() - user.rect.top);

	// order summary
	const int section_width = 4 * (long)width() / 5;

	int x = ((long)width() - 4 * (long)width() / 5) / 2;
	int y = ((long)height() - 4 * (long)height() / 5) / 2;

	rect rect_section = { x, x + section_width, y, y + 250 };

	const liblec::cui::size order_period_size = { (unsigned long)rect_section.width() / 3, (unsigned long)rect_section.height() };

	widgets::text order_summary_caption;
	order_summary_caption.alignment = widgets::text_alignment::middle;
	order_summary_caption.rect = { x, x + section_width, y - 20, y };
	order_summary_caption.text_value = "Order Summary";
	order_summary_caption.on_resize = { 50, ((100 / 5) / 2), 0, 0 };

	home_page.add_text(order_summary_caption);

	// get orders
	const auto today = liblec::cui::date_gen::today();
	liblec::cui::date_gen::date week_start, week_end, month_start, month_end, year_start, year_end;
	liblec::cui::date_gen::get_week(today, week_start, week_end);
	liblec::cui::date_gen::get_month(today, month_start, month_end);
	liblec::cui::date_gen::get_year(today, year_start, year_end);

	std::vector<tcu_db::order_details> orders_week;
	app_state_.get_db().get_orders(liblec::cui::date_gen::to_string(week_start),
		liblec::cui::date_gen::to_string(week_end), orders_week, error);

	std::vector<tcu_db::order_details> orders_month;
	app_state_.get_db().get_orders(liblec::cui::date_gen::to_string(month_start),
		liblec::cui::date_gen::to_string(month_end), orders_month, error);

	std::vector<tcu_db::order_details> orders_year;
	app_state_.get_db().get_orders(liblec::cui::date_gen::to_string(year_start),
		liblec::cui::date_gen::to_string(year_end), orders_year, error);

	// set values for order summary
	double week_completed = 0;
	double week_pending = 0;

	for (const auto& it : orders_week) {
		if (it.status == "Pending") week_pending++;
		if (it.status == "Completed") week_completed++;
	}

	double month_completed = 0;
	double month_pending = 0;

	for (const auto& it : orders_month) {
		if (it.status == "Pending") month_pending++;
		if (it.status == "Completed") month_completed++;
	}

	double year_completed = 0;
	double year_pending = 0;

	for (const auto& it : orders_year) {
		if (it.status == "Pending") year_pending++;
		if (it.status == "Completed") year_completed++;
	}

	// add week summary
	rect rect_week = rect_section;
	{
		rect_week.left += margin;
		rect_week.set_width(order_period_size.width - 2 * margin);
		rect_week.top += margin;
		rect_week.set_height(order_period_size.height - 2 * margin);

		// add caption
		widgets::text caption;
		caption.text_value = "This Week";
		caption.rect = { rect_week.left + margin, rect_week.right - margin, rect_week.top + margin, rect_week.top + margin + 20 };
		caption.alignment = widgets::text_alignment::middle;
		caption.on_resize = { ((100 / 5) / 2), ((100 / 5) / 2), 0, 0 };

		home_page.add_text(caption);

		// add completed
		widgets::text completed;
		completed.alias = "week_completed";
		completed.text_value = std::to_string((int)week_completed) + " completed";
		completed.rect = { rect_week.left + margin, rect_week.right - margin, rect_week.bottom - (margin + 20), rect_week.bottom - margin };
		completed.on_resize = caption.on_resize;

		home_page.add_text(completed);

		// add pending
		widgets::text pending;
		pending.alias = "week_pending";
		pending.text_value = std::to_string((int)week_pending) + " pending";
		pending.rect = { rect_week.left + margin, rect_week.right - margin, rect_week.bottom - (margin + 20) - 20, rect_week.bottom - (margin + 20) };
		pending.on_resize = caption.on_resize;

		home_page.add_text(pending);

		// add summary pie chart
		widgets::piechart piechart;
		piechart.alias = "week_piechart";
		piechart.rect = { caption.rect.left, caption.rect.right, caption.rect.bottom + margin, pending.rect.top - margin };
		piechart.on_resize = caption.on_resize;
		piechart.data.caption = "Summary";
		piechart.data.autocolor = false;

		piechart.data.slices.push_back({ "Pending", week_pending, { 244, 67, 54 } });
		piechart.data.slices.push_back({ "Completed", week_completed, { 110, 170, 70 } });

		home_page.add_piechart(piechart);

		// add groupbox
		widgets::groupbox group;
		group.on_resize = caption.on_resize;
		group.rects = {	rect_week };
		group.color = { 255, 200, 200 };

		home_page.add_groupbox(group);
	}

	// add month summary
	rect rect_month = rect_section;
	{
		rect_month.left += (order_period_size.width + margin);
		rect_month.set_width(order_period_size.width - 2 * margin);
		rect_month.top += margin;
		rect_month.set_height(order_period_size.height - 2 * margin);

		// add caption
		widgets::text caption;
		caption.text_value = "This Month";
		caption.rect = { rect_month.left + margin, rect_month.right - margin, rect_month.top + margin, rect_month.top + margin + 20 };
		caption.alignment = widgets::text_alignment::middle;
		caption.on_resize = { ((5 * 100 / 5) / 2), ((100 / 5) / 2), 0, 0 };

		home_page.add_text(caption);

		// add completed
		widgets::text completed;
		completed.alias = "month_completed";
		completed.text_value = std::to_string((int)month_completed) + " completed";
		completed.rect = { rect_month.left + margin, rect_month.right - margin, rect_month.bottom - (margin + 20), rect_month.bottom - margin };
		completed.on_resize = caption.on_resize;

		home_page.add_text(completed);

		// add pending
		widgets::text pending;
		pending.alias = "month_pending";
		pending.text_value = std::to_string((int)month_pending) + " pending";
		pending.rect = { rect_month.left + margin, rect_month.right - margin, rect_month.bottom - (margin + 20) - 20, rect_month.bottom - (margin + 20) };
		pending.on_resize = caption.on_resize;

		home_page.add_text(pending);

		// add summary pie chart
		widgets::piechart piechart;
		piechart.alias = "month_piechart";
		piechart.rect = { caption.rect.left, caption.rect.right, caption.rect.bottom + margin, pending.rect.top - margin };
		piechart.on_resize = caption.on_resize;
		piechart.data.caption = "Summary";
		piechart.data.autocolor = false;

		piechart.data.slices.push_back({ "Pending", month_pending, { 255, 163, 0 } });
		piechart.data.slices.push_back({ "Completed", month_completed, { 30, 90, 145 } });

		home_page.add_piechart(piechart);

		// add groupbox
		widgets::groupbox group;
		group.on_resize = caption.on_resize;
		group.rects = { rect_month };
		group.color = { 200, 255, 200 };

		home_page.add_groupbox(group);
	}

	// add year summary
	rect rect_year = rect_section;
	{
		rect_year.left += (order_period_size.width * 2 + margin);
		rect_year.set_width(order_period_size.width - 2 * margin);
		rect_year.top += margin;
		rect_year.set_height(order_period_size.height - 2 * margin);

		// add caption
		widgets::text caption;
		caption.text_value = "This Year";
		caption.rect = { rect_year.left + margin, rect_year.right - margin, rect_year.top + margin, rect_year.top + margin + 20 };
		caption.alignment = widgets::text_alignment::middle;
		caption.on_resize = { 100 - ((100 / 5) / 2), ((100 / 5) / 2), 0, 0 };

		home_page.add_text(caption);

		// add completed
		widgets::text completed;
		completed.alias = "year_completed";
		completed.text_value = std::to_string((int)year_completed) + " completed";
		completed.rect = { rect_year.left + margin, rect_year.right - margin, rect_year.bottom - (margin + 20), rect_year.bottom - margin };
		completed.on_resize = caption.on_resize;

		home_page.add_text(completed);

		// add pending
		widgets::text pending;
		pending.alias = "year_pending";
		pending.text_value = std::to_string((int)year_pending) + " pending";
		pending.rect = { rect_year.left + margin, rect_year.right - margin, rect_year.bottom - (margin + 20) - 20, rect_year.bottom - (margin + 20) };
		pending.on_resize = caption.on_resize;

		home_page.add_text(pending);

		// add summary pie chart
		widgets::piechart piechart;
		piechart.alias = "year_piechart";
		piechart.rect = { caption.rect.left, caption.rect.right, caption.rect.bottom + margin, pending.rect.top - margin };
		piechart.on_resize = caption.on_resize;
		piechart.data.caption = "Summary";
		piechart.data.autocolor = false;

		piechart.data.slices.push_back({ "Pending", year_pending, { 207, 216, 220 } });
		piechart.data.slices.push_back({ "Completed", year_completed, { 120, 105, 145 } });

		home_page.add_piechart(piechart);

		// add groupbox
		widgets::groupbox group;
		group.on_resize = caption.on_resize;
		group.rects = { rect_year };
		group.color = { 200, 200, 255 };

		home_page.add_groupbox(group);
	}

	// add icons
	rect rect_icons = { rect_section.left, rect_section.right, rect_section.bottom + margin, user.rect.top - margin };

	const size icon_size = { 170, 75 };
	const int number_of_icons = 4;

	const int icon_resize_y_rate = (((100 / 5) / 2) + 100) / 2;

	int top = rect_icons.top + ((rect_icons.height() - icon_size.height) / 2);
	int bottom = top + icon_size.height;

	// add orders icon
	widgets::icon orders;
	orders.alias = "orders";
	orders.png_resource = png_orders;
	orders.rect = { rect_icons.left, rect_icons.left + icon_size.width, top, bottom };
	orders.on_resize = { ((100 / 5) / 2), icon_resize_y_rate, 0, 0 };
	orders.text = "Orders";
	orders.description = "View and manage orders";
	orders.on_click = [&]() { on_orders(); };

	home_page.add_icon(orders);

	// add people icon
	widgets::icon people;
	people.alias = "people";
	people.png_resource = png_people;
	people.rect = { rect_icons.left + (rect_icons.width() / number_of_icons), rect_icons.left + (rect_icons.width() / number_of_icons) + icon_size.width, top, bottom };
	people.on_resize = { ((100 / 5) / 2) + (100 / number_of_icons), icon_resize_y_rate, 0, 0 };
	people.text = "People";
	people.description = "Manage contact information";
	people.on_click = [&]() { on_people(); };

	home_page.add_icon(people);

	// add items icon
	widgets::icon items;
	items.alias = "items";
	items.png_resource = png_items;
	items.rect = { rect_icons.left + 2 * (rect_icons.width() / number_of_icons), rect_icons.left + 2 * (rect_icons.width() / number_of_icons) + icon_size.width, top, bottom };
	items.on_resize = { ((100 / 5) / 2) + 2 * (100 / number_of_icons), icon_resize_y_rate, 0, 0 };
	items.text = "Items";
	items.description = "Manage item information";

	home_page.add_icon(items);

	// add settings icon
	widgets::icon settings;
	settings.alias = "settings";
	settings.png_resource = png_settings;
	settings.rect = { rect_icons.right - icon_size.width, rect_icons.right, top, bottom };
	settings.on_resize = { ((100 / 5) / 2) + 3 * (100 / number_of_icons), icon_resize_y_rate, 0, 0 };
	settings.text = "Settings";
	settings.description = "Change app settings";

	home_page.add_icon(settings);

	return true;
}