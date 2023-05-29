
#include "Methods.h"

// ********************************************** Main **********************************************
int main()
{
	if (mongoURI.to_string().empty())
	{
		cout << "URI is empty";
		return 0;
	}
	// Create an instance.
	mongocxx::instance inst{};
	mongocxx::options::client client_options;
	auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
	client_options.server_api_opts(api);
	mongocxx::client conn{ mongoURI, client_options };
	const string dbName = "HardwareRecords";
	const string collName = "HardwareCollection";
	auto dbs = getDatabases(conn);
	// Check if database already exists.
	if (!(std::find(dbs.begin(), dbs.end(), dbName) != dbs.end()))
	{
		// Create a new database & collection for hardwares.
		conn[dbName];
	}
	auto hardwareDB = conn.database(dbName);
	auto allCollections = hardwareDB.list_collection_names();
	// Check if collection already exists.
	if (!(std::find(allCollections.begin(), allCollections.end(), collName) != allCollections.end()))
	{
		createCollection(hardwareDB, collName);
	}
	auto hardwareCollection = hardwareDB.collection(collName);
	// Create a menu for user interaction
	int choice = -1;
	do while (choice != 0)
	{
		//system("cls");
		cout << endl << "**************************************************************************************************************" << endl;
		cout << "Enter 1 to add hardware record" << endl;
		cout << "Enter 2 to update hardware record" << endl;
		cout << "Enter 3 to find hardware record" << endl;
		cout << "Enter 4 to delete hardware record" << endl;
		cout << "Enter 5 to print all hardware records" << endl;
		cout << "Enter 0 to exit" << endl;
		cout << "Enter Choice : ";
		cin >> choice;
		cout << endl;
		switch (choice)
		{
		case 1:
			addHardwareRecord(hardwareCollection);
			break;
		case 2:
			updateHardwareRecord(hardwareCollection);
			break;
		case 3:
			findHardwareRecord(hardwareCollection);
			break;
		case 4:
			deleteHardwareRecord(hardwareCollection);
			break;
		case 5:
			printHardwareRecords(hardwareCollection);
			break;
		case 0:
			break;
		default:
			cout << "Invalid choice" << endl;
			break;
		}
	} while (choice != 0);
	return 0;
}