/*
** Top Class Uniforms Database Implementation (tcu_db.cpp)
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

#include "tcu_db.h"

// sqlcipher
#include <sqlite3.h>

#ifdef _WIN64
#pragma comment(lib, "sqlcipher64.lib")
#else
#pragma comment(lib, "sqlcipher32.lib")
#endif

using table = std::vector<std::map<std::string, std::string>>;

class tcu_db::tcu_db_impl {
	friend tcu_db;

	std::string file_name_, password_;
	bool connected_;
	sqlite3* db_;

public:
	tcu_db_impl() :
		connected_(false),
		db_(nullptr) {}

	~tcu_db_impl() {
		if (db_) {
			// close database
			sqlite3_close(db_);
			db_ = nullptr;
		}
	}

	std::string sqlite_error(int error_code) {
		std::string error = sqlite3_errstr(error_code);
		if (error == "not an error") error.clear();
		if (error.length() > 0) error[0] = toupper(error[0]);
		return error;
	}

	std::string sqlite_error() {
		if (db_) {
			std::string error = sqlite3_errmsg(db_);
			if (error == "not an error") error.clear();
			if (error.length() > 0) error[0] = toupper(error[0]);
			return error;
		}
		else {
			return "Database not open";
		}
	}

	bool sqlite_query(const std::string& query,
		table& table,
		std::string& error)
	{
		table.clear();

		if (db_) {
			sqlite3_stmt* statement = nullptr;

			if (sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, 0) == SQLITE_OK) {
				const int columns = sqlite3_column_count(statement);
				
				while (true) {
					if (sqlite3_step(statement) == SQLITE_ROW) {
						std::map<std::string, std::string> values;

						for (int column = 0; column < columns; column++) {
							std::string column_name, value;

							// get column name
							char* ccColumn = (char*)sqlite3_column_name(statement, column);

							if (ccColumn) {
								column_name = ccColumn;

								// get data
								char* ccData = (char*)sqlite3_column_text(statement, column);

								if (ccData) value = ccData;

								values.insert(std::make_pair(column_name, value));
							}
						}

						table.push_back(values);
					}
					else
						break;
				}

				sqlite3_finalize(statement);
			}
			else {
				error = sqlite_error();
				return false;
			}

			return true;
		}
		else {
			error = "Database not open";
			return false;
		}
	}
};

tcu_db::tcu_db() :
	d_(*(new tcu_db::tcu_db_impl())) {
}

tcu_db::~tcu_db() {
	delete& d_;
}

bool tcu_db::connect(const std::string& file_name,
	const std::string& password,
	std::string& error) {
	if (d_.connected_) return true;

	int error_code = 0;
	if (password.empty()) {
		error_code = sqlite3_open_v2(file_name.c_str(), &d_.db_,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
		error_code = sqlite3_exec(d_.db_, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);
	}
	else {
		error_code = sqlite3_open_v2(file_name.c_str(), &d_.db_,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);

		sqlite3_stmt* stm = nullptr;
		const char* pzTail = nullptr;

		// key the database
		auto pragma_string = "PRAGMA key = '" + password + "';";
		sqlite3_prepare(d_.db_, pragma_string.c_str(), -1, &stm, &pzTail);
		sqlite3_step(stm);
		sqlite3_finalize(stm);

		// test if key is correct
		error_code = sqlite3_exec(d_.db_, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);
	}

	if (error_code != SQLITE_OK) {
		// an error occured
		error = d_.sqlite_error(error_code);

		// close database
		sqlite3_close(d_.db_);
		d_.db_ = nullptr;
		return false;
	}

	// create tables
	table table;

	// users
	if (!d_.sqlite_query("CREATE TABLE Users ("
		"Username TEXT NOT NULL, "
		"Password TEXT NOT NULL, "
		"Position TEXT, "
		"PRIMARY KEY (Username)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// organization
	if (!d_.sqlite_query("CREATE TABLE Organizations ("
		"OrganizationID TEXT NOT NULL, "
		"Name TEXT NOT NULL, "
		"Address TEXT, "
		"PRIMARY KEY (OrganizationID)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// people
	if (!d_.sqlite_query("CREATE TABLE People ("
		"PersonID TEXT NOT NULL, "
		"Title TEXT, "
		"Name TEXT NOT NULL, "
		"Surname TEXT NOT NULL, "
		"Address TEXT, "
		"Occupation TEXT, "
		"OrganizationID TEXT, "
		"PRIMARY KEY (PersonID)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// personal phones
	if (!d_.sqlite_query("CREATE TABLE PersonalPhones ("
		"Number INT NOT NULL, "
		"Type TEXT NOT NULL, "
		"PersonID TEXT NOT NULL, "
		"PRIMARY KEY (Number, PersonID)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// organization phones
	if (!d_.sqlite_query("CREATE TABLE OrganizationPhones ("
		"Number INT NOT NULL, "
		"Location TEXT NOT NULL, "
		"OrganizationID TEXT NOT NULL, "
		"PRIMARY KEY (Number)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// items
	if (!d_.sqlite_query("CREATE TABLE Items ("
		"ItemID TEXT NOT NULL, "
		"Name TEXT NOT NULL, "
		"Description TEXT, "
		"PRIMARY KEY (ItemID)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// orders
	if (!d_.sqlite_query("CREATE TABLE Orders ("
		"OrderID TEXT NOT NULL, "
		"Date INTEGER NOT NULL, "
		"Time TEXT NOT NULL, "
		"AttendantID TEXT NOT NULL, "
		"ClientID TEXT NOT NULL, "
		"Status TEXT NOT NULL, "
		"PRIMARY KEY (OrderID)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// order items
	if (!d_.sqlite_query("CREATE TABLE OrderItems ("
		"OrderID TEXT NOT NULL, "
		"ItemID TEXT NOT NULL, "
		"Quantity INTEGER NOT NULL, "
		"PRIMARY KEY (OrderID, ItemID)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	// personal emails
	if (!d_.sqlite_query("CREATE TABLE PersonalEmails ("
		"Email TEXT NOT NULL, "
		"PersonID TEXT NOT NULL, "
		"PRIMARY KEY (Email, PersonID)"
		");",
		table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}
	
	d_.connected_ = true;
	return true;
}

bool tcu_db::new_user(const credentials& user, const std::string& password, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	std::string position;
	switch (user.user_position) {
	case clearance::administrator: position = "Administrator"; break;
	case clearance::regular:
	default:  position = "Regular"; break;
	}

	table table;
	return d_.sqlite_query("INSERT INTO Users VALUES('"
		+ user.user_name + "', '" + password + "', '" + position + "');",
		table, error);
}

bool tcu_db::get_users(std::vector<credentials>& users, std::string& error) {
	users.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM Users;", table, error)) return false;

	for (const auto& it : table) {
		credentials user;
		user.user_name = it.at("Username");
		user.user_position = it.at("Position") ==
			"Administrator" ? clearance::administrator : clearance::regular;
		users.push_back(user);
	}

	return false;
}

bool tcu_db::get_user(const std::string& user_name, const std::string& password,
	credentials& user, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	table table;
	if (!d_.sqlite_query("SELECT * FROM Users WHERE Username = '"
		+ user_name + "' AND Password = '" + password + "';",
		table, error))
		return false;

	if (table.empty()) {
		error = "Incorrect username or password";
		return false;
	}

	user.user_name = table.at(0).at("Username");

	if (table.at(0).at("Position") == "Administrator") {
		user.user_position = clearance::administrator;
	}

	return true;
}

bool tcu_db::get_user(const std::string& user_name, credentials& user, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	table table;
	if (!d_.sqlite_query("SELECT * FROM Users WHERE Username = '" + user_name + "';",
		table, error))
		return false;

	if (table.empty()) {
		error = "Incorrect username or password";
		return false;
	}

	user.user_name = table.at(0).at("Username");

	if (table.at(0).at("Position") == "Administrator") {
		user.user_position = clearance::administrator;
	}

	return true;
}

bool tcu_db::new_organization(const organization_info& organization, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	table table;
	if (!d_.sqlite_query("INSERT INTO Organizations VALUES('"
		+ organization.id + "', '" + organization.name + "', '" + organization.address + "');",
		table, error)) {
		return false;
	}

	return true;
}

bool tcu_db::get_organization(const std::string& organization_id,
	organization_info& organization, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	if (organization_id.empty()) {
		error = "Organization ID not specified";
		return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM Organizations WHERE OrganizationID = '"
		+ organization_id + "';", table, error)) return false;

	if (!table.empty()) {
		organization_info organization_;
		organization_.id = table.at(0).at("OrganizationID");
		organization_.name = table.at(0).at("Name");
		organization_.address = table.at(0).at("Address");
		
		organization = organization_;
	}

	return true;
}

bool tcu_db::get_organizations(std::vector<organization_info>& organizations, std::string& error) {
	organizations.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM Organizations;", table, error)) return false;

	for (const auto& it : table) {
		organization_info organization;
		organization.id = it.at("OrganizationID");
		organization.name = it.at("Name");
		organization.address = it.at("Address");
		organizations.push_back(organization);
	}

	return true;
}

bool tcu_db::new_person(const person_details& person_info, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	table table;
	for (const auto& phone : person_info.phones) {
		if (!d_.sqlite_query("INSERT INTO PersonalPhones VALUES('"
			+ phone.number + "', '" + phone.type + "', '" + person_info.id + "');",
			table, error)) {
			if (error.find("not unique") == std::string::npos) return false;
		}
	}

	if (!d_.sqlite_query("INSERT INTO People VALUES('"
		+ person_info.id + "', '" + person_info.title + "', '" + person_info.name + "', '" 
		+ person_info.surname + "', '" + person_info.address + +"', '" + person_info.occupation
		+ "', '" + person_info.organization.id + "');",
		table, error)) {
		return false;
	}

	return true;
}

bool tcu_db::get_person(const std::string& person_id, person_details& person, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	if (person_id.empty()) {
		error = "Person ID not specified";
		return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM People WHERE PersonID = '"
		+ person_id + "';", table, error)) return false;

	if (!table.empty()) {
		person_details person_;
		person_.id = table.at(0).at("PersonID");
		person_.title = table.at(0).at("Title");
		person_.name = table.at(0).at("Name");
		person_.surname = table.at(0).at("Surname");
		person_.address = table.at(0).at("Address");
		person_.occupation = table.at(0).at("Occupation");

		const auto organization_id = table.at(0).at("OrganizationID");

		if (!organization_id.empty())
			if (!get_organization(organization_id, person_.organization, error)) return false;

		if (!get_personalphones(person_.id, person_.phones, error)) return false;

		person = person_;
	}

	return true;
}

bool tcu_db::get_people(std::vector<person_details>& people, std::string& error) {
	people.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM People;", table, error)) return false;

	if (!table.empty()) {

		for (const auto& row : table) {
			person_details person_;
			person_.id = row.at("PersonID");
			person_.title = row.at("Title");
			person_.name = row.at("Name");
			person_.surname = row.at("Surname");
			person_.address = row.at("Address");
			person_.occupation = row.at("Occupation");

			const auto organization_id = row.at("OrganizationID");

			if (!organization_id.empty())
				if (!get_organization(organization_id, person_.organization, error)) return false;

			if (!get_personalphones(person_.id, person_.phones, error)) return false;

			people.push_back(person_);
		}
	}

	return true;
}

bool tcu_db::get_personalphones(const std::string& person_id, std::vector<phone>& phones,
	std::string& error) {
	phones.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	if (person_id.empty()) {
		error = "Person ID not specified";
		return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM PersonalPhones WHERE PersonID = '" + person_id + "';",
		table, error)) return false;

	if (!table.empty()) {
		for (const auto& row : table) {
			phone phone_;
			phone_.number = row.at("Number");
			phone_.type = row.at("Type");
			phones.push_back(phone_);
		}
	}

	return true;
}

bool tcu_db::new_item(const item_info& item, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	table table;
	if (!d_.sqlite_query("INSERT INTO Items VALUES('"
		+ item.id + "', '" + item.name + "', '" + item.description + "');",
		table, error)) {
		return false;
	}

	return true;
}

bool tcu_db::get_items(std::vector<item_info>& items, std::string& error) {
	items.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM Items;", table, error)) return false;

	for (const auto& it : table) {
		item_info item;
		item.id = it.at("ItemID");
		item.name = it.at("Name");
		item.description = it.at("Description");
		items.push_back(item);
	}

	return true;
}

bool tcu_db::get_item(const std::string& item_id, item_info& item, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	if (item_id.empty()) {
		error = "Item ID not specified";
		return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM Items WHERE ItemID = '"
		+ item_id + "';", table, error)) return false;

	if (!table.empty()) {
		item_info item_;
		item_.id = table.at(0).at("ItemID");
		item_.name = table.at(0).at("Name");
		item_.description = table.at(0).at("Description");
		item = item_;
	}

	return true;
}

bool tcu_db::get_order_items(const std::string& order_id,
	std::vector<order_item>& order_items,
	std::string& error) {
	order_items.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	if (order_id.empty()) {
		error = "Order ID not specified";
		return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM OrderItems WHERE OrderID = '" + order_id + "';",
		table, error)) return false;

	for (const auto& it : table) {
		order_item item;
		item.item.id = it.at("ItemID");
		item.quantity = atoi(it.at("Quantity").c_str());
		
		if (!get_item(item.item.id, item.item, error)) return false;

		order_items.push_back(item);
	}

	return true;
}

bool tcu_db::new_order(const order_details& order, std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	table table;
	for (const auto& order_item : order.order_items) {
		if (!d_.sqlite_query("INSERT INTO OrderItems VALUES('"
			+ order.id + "', '" + order_item.item.id + "', '" +
			std::to_string(order_item.quantity) + "');",
			table, error)) {
			if (error.find("not unique") == std::string::npos) return false;
		}
	}

	if (!d_.sqlite_query("INSERT INTO Orders VALUES('"
		+ order.id + "', '" + order.date + "', '" + order.time + "', '" +
		order.attendant.user_name + "', '" + order.client.id + +"', '" + order.status + "');",
		table, error)) {
		return false;
	}

	return true;
}

bool tcu_db::get_orders(std::vector<order_details>& orders, std::string& error) {
	orders.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM Orders;", table, error)) return false;

	for (const auto& it : table) {
		order_details order;
		order.id = it.at("OrderID");
		order.date = it.at("Date");
		order.time = it.at("Time");

		if (!get_user(it.at("AttendantID"), order.attendant, error)) return false;
		if (!get_person(it.at("ClientID"), order.client, error)) return false;

		order.status = it.at("Status");

		if (!get_order_items(order.id, order.order_items, error)) return false;

		orders.push_back(order);
	}

	return true;
}

bool tcu_db::get_orders(const std::string& period_start, const std::string& period_end,
	std::vector<order_details>& orders, std::string& error) {
	orders.clear();
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("SELECT * FROM Orders WHERE Date >= " + period_start
		+ " AND Date <= " + period_end + ";", table, error)) return false;

	for (const auto& it : table) {
		order_details order;
		order.id = it.at("OrderID");
		order.date = it.at("Date");
		order.time = it.at("Time");

		if (!get_user(it.at("AttendantID"), order.attendant, error)) return false;
		if (!get_person(it.at("ClientID"), order.client, error)) return false;

		order.status = it.at("Status");

		if (!get_order_items(order.id, order.order_items, error)) return false;

		orders.push_back(order);
	}

	return true;
}

bool tcu_db::order_status_update(const std::string& order_id, const std::string& status,
	std::string& error) {
	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	if (order_id.empty()) {
		error = "Order ID not specified";
		return false;
	}

	// read database
	table table;
	if (!d_.sqlite_query("UPDATE Orders SET Status = '" + status + "' WHERE OrderID = '" + order_id
		+ "';", table, error)) return false;

	return true;
}
