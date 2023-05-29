#pragma once
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

//Get Mongodb database URI string from system environment variables
std::string getEnvironmentVariable(std::string environmentVarKey)
{
	char* pBuffer = nullptr;
	size_t size = 0;
	auto key = environmentVarKey.c_str();
	// Use the secure version of getenv, ie. _dupenv_s to fetch environment variable.
	if (_dupenv_s(&pBuffer, &size, key) == 0 && pBuffer != nullptr)
	{
		std::string environmentVarValue(pBuffer);
		free(pBuffer);
		return environmentVarValue;
	}
	else
	{
		return "";
	}
}

//Global variable to store URI
auto mongoURIStr = getEnvironmentVariable("MONGODB_URI");
static const mongocxx::uri mongoURI = mongocxx::uri{ mongoURIStr };

// Get all the databases from a given client.
vector<string> getDatabases(mongocxx::client& client)
{
	return client.list_database_names();
}


// Create a new collection in the given database.
void createCollection(mongocxx::database& db, const string& collectionName)
{
	db.create_collection(collectionName);
}


// Create a document from the given key-value pairs.
bsoncxx::document::value createDocument(const vector<pair<string, string>>& keyValues)
{
	bsoncxx::builder::stream::document document{};
	for (auto& keyValue : keyValues)
	{
		document << keyValue.first << keyValue.second;
	}
	return document << bsoncxx::builder::stream::finalize;
}


// Insert a document into the given collection.
void insertDocument(mongocxx::collection& collection, const bsoncxx::document::value& document)
{
	collection.insert_one(document.view());
}


// Print the contents of the given collection.
void printCollection(mongocxx::collection& collection)
{
	// Check if collection is empty.
	if (collection.count_documents({}) == 0)
	{
		cout << "Collection is empty." << endl;
		return;
	}
	auto cursor = collection.find({});
	for (auto&& doc : cursor)
	{
		cout << bsoncxx::to_json(doc) << endl;
	}
}


// Find the document with given key-value pair.
void findDocument(mongocxx::collection& collection, const string& key, const string& value)
{
	// Create the query filter
	auto filter = bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize;
	//Add query filter argument in find
	auto cursor = collection.find({ filter });
	for (auto&& doc : cursor)
	{
		cout << bsoncxx::to_json(doc) << endl;
	}
}


// Update the document with given key-value pair.
void updateDocument(mongocxx::collection& collection, const string& key, const string& value, const string& newKey, const string& newValue)
{
	collection.update_one(bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize,
		bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << newKey << newValue << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
}


// Delete a document from a given collection.
void deleteDocument(mongocxx::collection& collection, const bsoncxx::document::value& document)
{
	collection.delete_one(document.view());
}


// ********************************************** I/O Methods **********************************************
// Add hardware record.
void addHardwareRecord(mongocxx::collection& collection)
{
	string ProductName, invoiceNo, brand, units, DateOfPurchase;
	cout << "Enter product name: ";
	/*cin >> ProductName;*/
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(cin, ProductName);
	cout << "Enter invoice number: ";
	cin >> invoiceNo;
	cout << "Enter brand: ";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(cin, brand);
	cout << "Enter no of units sold: ";
	cin >> units;
	cout << "Enter date of purchase: ";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(cin, DateOfPurchase);

	const string& key = "Invoice No", value = invoiceNo;
	auto filter = bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize;
	//Add query filter argument in find
	auto cursor = collection.find({ filter });
	auto count = std::distance(cursor.begin(), cursor.end());
	if (count != 0L) {
		cout << "\nItem already exist with invoice : " << invoiceNo << endl;
		return;
	}
	else
	{
		insertDocument(collection, createDocument({ {"Product Name", ProductName}, {"Invoice No", invoiceNo}, {"Brand", brand}, {"Units Sold", units}, {"Date Of Purchase", DateOfPurchase} }));
	}

}


// Update hardware record.
void updateHardwareRecord(mongocxx::collection& collection)
{
	string invoiceNo, newBrand, units, newDateOfPurchase;
	cout << "Enter invoice number: ";
	cin >> invoiceNo;
	cout << "Enter new brand: ";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(cin, newBrand);
	cout << "Enter no of units sold: ";
	cin >> units;
	cout << "Enter new date of purchase: ";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(cin, newDateOfPurchase);
	updateDocument(collection, "Invoice No", invoiceNo, "Brand", newBrand);
	updateDocument(collection, "Invoice No", invoiceNo, "Units Sold", units);
	updateDocument(collection, "Invoice No", invoiceNo, "Date Of Purchase", newDateOfPurchase);
}


// Find hardware record.
void findHardwareRecord(mongocxx::collection& collection)
{
	string invoiceNo;
	cout << "Enter invoice number: ";
	cin >> invoiceNo;
	findDocument(collection, "Invoice No", invoiceNo);
}


// Delete hardware record.
void deleteHardwareRecord(mongocxx::collection& collection)
{
	string invoiceNo;
	cout << "Enter invoice number: ";
	cin >> invoiceNo;
	deleteDocument(collection, createDocument({ {"Invoice No", invoiceNo} }));
}


// Print hardware records.
void printHardwareRecords(mongocxx::collection& collection)
{
	printCollection(collection);
}