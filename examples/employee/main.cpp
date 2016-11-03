#include "db_employee.h"

#include <assert.h>
#include <ctime>
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace qsl;

// A simple struct to store employee data
struct Employee { string name, address, city, country; system_clock::time_point dob; };

/** A function to remove trailing newline char from a string. */
static char* trim(char *str)
{
	int len = strlen(str);
	while (str[len-1] == '\n')
		str[--len] = '\0';
	return str;
}

/** A function to convert an employee into a human-readable string. */
static string to_str(const Employee &e)
{
	time_t tt = system_clock::to_time_t(e.dob);
	return "Employee{ name=\"" + e.name + "\", address=\"" + e.address + "\", city=\""
		+ e.city + "\", country=\"" + e.country + "\", dob=\"" + trim(ctime(&tt)) + "\" }";
}

/** A class to deal with the database. */
class Employees
{
public:
	Employees(const QString &driver = "psql") : d(driver) {}
	
	// these functions controll the database connection
	void setName(const QString &name) { d.setName(name); }
	void setHost(const QString &host) { d.setHost(host); }
	void setPort(int port) { d.setPort(port); }
	void setUser(const QString &user) { d.setUser(user); }
	void setPassword(const QString &password) { d.setPassword(password); }
	bool connect() { return d.connect(); }
	void disconnect() { d.disconnect(); }
	
	// to query the employees
	vector<Employee> employees()
	{
		vector<Employee> list;
		auto r = d.employees().query();
		for (auto e : r) // iterate over the employees
			list.push_back({ e.name(), e.address(), e.city().name(), e.city().country(), e.dob() });
		return list;
	}
	
	// to query an employee by its name
	Employee employee(const string &name)
	{
		auto r = d.employees().filter("name" EQUALS name).query();
		if (r.empty())
			return { "invalid", "", "", "", system_clock::from_time_t(0) };
		return { r[0].name(), r[0].address(), r[0].city().name(), r[0].city().country(), r[0].dob() };
	}
	
	// to add an employee
	bool add(const Employee &e)
	{
		// first we have to lookup the city
		auto r = d.cities().filter("name" EQUALS e.city AND "country" EQUALS e.country).query();
		if (r.empty())
		{
			// we have to add that city
			if (!d.cities().insert({ e.city, e.country }))
				return false;
			
			// now lets lookup the newly created city
			r = d.cities().filter("name" EQUALS e.city AND "country" EQUALS e.country).query();
			assert(!r.empty()); // it should exist now
		}
		
		// now we can add the employee
		return d.employees().insert({ e.name, e.address, r[0], e.dob });
	}
	
private:
	db::employee d;
};

// just for convenience
static void usage(char *name)
{
	cout << "Usage: " << name << " <driver> <db-name> [<host> <user> <password>]" << endl;
}

int main(int argc, char **argv)
{
	if (argc < 3 || strcmp(argv[1], "--help") == 0)
	{
		usage(argv[0]);
		return (argc < 3 ? 1 : 0);
	}
	
	Employees e(argv[1]);
	e.setName(argv[2]);
	if (argc > 3)
		e.setHost(argv[3]);
	if (argc > 4)
		e.setUser(argv[4]);
	if (argc > 5)
		e.setPassword(argv[5]);
	if (!e.connect())
	{
		cerr << "Error: Failed to connect to the database" << endl;
		return 1;
	}
	
	// show all known employees
	cout << "All known employees:" << endl;
	vector<Employee> employees = e.employees();
	for (auto employee : employees)
		cout << " - " << to_str(employee) << endl;
	
	// now lets ask the user to insert a new employee
	cout << "Enter the value for a new employee:" << endl;
	Employee employee;
	cout << "Name: ";
	getline(cin, employee.name);
	cout << "Address: ";
	getline(cin, employee.address);
	cout << "City: ";
	getline(cin, employee.city);
	cout << "Country: ";
	getline(cin, employee.country);
	employee.dob = system_clock::now();
	if (!e.add(employee))
	{
		cerr << "Failed to insert the new employee :(" << endl;
		return 1;
	}
	
	// and lets query the db for the newly created employee
	cout << "Your new created employee:" << endl;
	cout << to_str(e.employee(employee.name)) << endl;
	
	return 0;
}
