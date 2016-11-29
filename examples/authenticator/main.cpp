#include "db_auth.h"
#include "../exampleglobal.h"

using namespace spis;

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    db::auth *a = connect<db::auth>(argc, argv);
    if (!a)
    {
	fprintf(stderr, "Failed to connect to database\n");
	return 1;
    }

    printf("Commands:\n"
	   "  new   <user> <mail> <password>   - create a new user\n"
	   "  login <user> <password>          - login\n"
	   "  quit                             - exit\n"
	   "\n");

    while (true)
    {
	printf("> ");
	string cmd;
	cin >> cmd;

	cout << "command: " << cmd << endl;

	if (cmd == "new")
	{
	    string user, mail, password;
	    cin >> user >> mail >> password;
	    a->users().insert({user, mail, password});
	}

	else if (cmd == "login")
	{
	    string user, password;
	    cin >> user >> password;
	    auto r = a->users().filter("name" EQ user).query();
	    printf("Found %d users with name %s\n", r.size(), user.data());
	    if (r.size() != 1)
		continue;
	    if (r[0].pw() == password)
		printf("login successfull\n");
	    else
		printf("login failed\n");
	}

	else if (cmd == "quit" || cmd == "q")
	{
	    break;
	}

	else
	{
	    fprintf(stderr, "Unknown command: %s\n", cmd.data());
	}
    }

    return 0;
}
