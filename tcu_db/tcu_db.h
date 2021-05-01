/*
** Top Class Uniforms Database Interface (tcu_db.h)
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

#include <string>
#include <vector>
#include <map>

class tcu_db {
public:
	enum class clearance { administrator, regular };

	struct credentials {
		std::string user_name;
		clearance user_position = clearance::regular;
	};

	struct organization_info {
		std::string id;
		std::string name;
		std::string address;
	};

	struct phone {
		std::string number;
		std::string type;
	};

	struct person_details {
		std::string id;
		std::string title;
		std::string name;
		std::string surname;
		std::string address;
		std::vector<phone> phones;
		std::string occupation;
		organization_info organization;
	};

	struct item_info {
		std::string id;
		std::string name;
		std::string description;
	};

	struct order_item {
		item_info item;
		int quantity = 0;
	};

	struct order_details {
		std::string id;
		std::string date;
		std::string time;
		credentials attendant;
		person_details client;
		std::string status;
		std::vector<order_item> order_items;
	};

	tcu_db();
	~tcu_db();

	bool connect(const std::string& file_name, const std::string& password, std::string& error);

	// users
	bool new_user(const credentials& user, const std::string& password, std::string& error);
	bool get_users(std::vector<credentials>& users, std::string& error);
	bool get_user(const std::string& user_name, const std::string& password,
		credentials& user, std::string& error);
	bool get_user(const std::string& user_name, credentials& user, std::string& error);

	// organizations
	bool new_organization(const organization_info& organization, std::string& error);
	bool get_organization(const std::string& organization_id,
		organization_info& organization, std::string& error);
	bool get_organizations(std::vector<organization_info>& organizations, std::string& error);

	// people
	bool new_person(const person_details& person_info, std::string& error);
	bool get_person(const std::string& person_id, person_details& person, std::string& error);
	bool get_people(std::vector<person_details>& people, std::string& error);

	// phones
	bool get_personalphones(const std::string& person_id, std::vector<phone>& phones,
		std::string& error);

	// items
	bool new_item(const item_info& item, std::string& error);
	bool get_items(std::vector<item_info>& items, std::string& error);
	bool get_item(const std::string& item_id, item_info& item, std::string& error);
	bool get_order_items(const std::string& order_id, std::vector<order_item>& order_items,
		std::string& error);

	// order
	bool new_order(const order_details& order, std::string& error);
	bool get_orders(std::vector<order_details>& orders, std::string& error);
	bool get_orders(const std::string& date_start, const std::string& date_end,
		std::vector<order_details>& orders, std::string& error);
	bool order_status_update(const std::string& order_id, const std::string& status,
		std::string& error);

private:
	class tcu_db_impl;
	tcu_db_impl& d_;
};
