#pragma once
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include "crow.h"
using namespace std;
using namespace crow;
using namespace crow::mustache;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;


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


//Load the HTML content and render it using mustache
string getView(const string& filename, context& x) {
	auto page = load(filename + ".html");
	return page.render_string(x);
}


// Update a document from the given key-value pairs.
bool updateDocument(mongocxx::collection& collection, const string& key, const string& value, const string& newKey, const string& newValue)
{
	bsoncxx::stdx::optional<mongocxx::result::update> maybe_result = collection.update_one(bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize,
		bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << newKey << newValue << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
	if (maybe_result) {
		return maybe_result->modified_count() == 1;
	}
	return false;
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


// Delete a document from the given collection and document value returned from createDocument function.
bool deleteDocument(mongocxx::collection& collection, const bsoncxx::document::value& document)
{
	bsoncxx::stdx::optional<mongocxx::result::delete_result> maybe_result = collection.delete_one(document.view());
	if (maybe_result) {
		return maybe_result->deleted_count() == 1;
	}
	return false;
}


// Add the document to the given collection.
void insertDocument(mongocxx::collection& collection, const bsoncxx::document::value& document)
{
	collection.insert_one(document.view());
}


// Find a document from the given key-value pairs and return true if found.
bool findDocument(mongocxx::collection& collection, const string& key, const string& value)
{
	// Create the query filter
	auto filter = bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize;
	//Add query filter argument in find
	auto cursor = collection.find({ filter });
	auto count = std::distance(cursor.begin(), cursor.end());
	if (count != 0L) {
		return true;
	}
	return false;
}

//Pass the given collection and key-value pairs.
bool findHardwareRecord(mongocxx::collection& collection, const string& invoiceNo)
{
	return findDocument(collection, "Invoice No", invoiceNo);
}