/*
** Top Class Uniforms application entry point (main.cpp)
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

#include "tcu_newuser/tcu_newuser.h"
#include "tcu_login/tcu_login.h"
#include "tcu_main/tcu_main.h"
#include "tcu_db/tcu_db.h"
#include "resource.h"

// gui app using main
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main() {
	// constants used by the app
	const std::string guid_app = "{9D4B5BF2-EA54-4BD3-8DC8-C779568F8A86}";	// allow only one instance of the app
	const std::string guid_form = "{667CA548-0583-460F-A775-9772299955C5}";	// to bring existing form to the foreground if a new instance is attempted

	auto on_error = [&](gui& ui, const std::string& error) {
		gui::prompt_params params;
		params.type = gui::prompt_type::ok;
		params.png_icon_resource = png_error;
		ui.prompt(params, "Error", error);
	};

	// application state object
	state app_state;

	// login form
	tcu_login login(guid_app, app_state);

	// connect to the database
	std::string error;
	if (!app_state.get_db().connect("database.dbs", "ah&Nns56b", error)) {
		on_error(login, error);
		return 1;
	}

	// check if there are any users in the database
	std::vector<tcu_db::credentials> users;
	if (app_state.get_db().get_users(users, error)) {
		on_error(login, error);
		return 1;
	}
	
	if (users.empty()) {
		// new user form
		tcu_newuser newuser(guid_app, app_state);

		// run new user
		if (!newuser.run(error)) {
			on_error(newuser, error);
			return 1;
		}

		if (app_state.get_db().get_users(users, error)) {
			on_error(login, error);
			return 1;
		}

		if (users.empty()) return 0;
	}

	// run login
	if (!login.run(guid_form, error)) {
		on_error(login, error);
		return 1;
	}

	if (app_state.loggedin()) {
		// main form
		tcu_gui app(guid_app, app_state);

		// run form
		if (!app.run(guid_form, error)) {
			on_error(app, error);
			return 1;
		}
	}

	return 0;
}
