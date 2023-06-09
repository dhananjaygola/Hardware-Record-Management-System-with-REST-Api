
#include "Methods.h"

// ********************************************** Main **********************************************
int main()
{
	crow::SimpleApp app; //define your crow application
	set_global_base("."); //search for the files in current dir.
	mongocxx::instance inst{};
	string mongoConnect = std::string(mongoURIStr);
	mongocxx::client conn{ mongocxx::uri{mongoConnect} };
	auto collection = conn["HardwareRecords"]["HardwareCollection"];//get collection from database


	//endpoint to search using invoive number
	CROW_ROUTE(app, "/hardware/<string>")
		([&collection](string invoiceNo) {
		auto doc = collection.find_one(make_document(kvp("Invoice No", invoiceNo)));
		crow::json::wvalue dto;//json to string
		dto["hardware"] = json::load(bsoncxx::to_json(doc.value().view()));
		return getView("hardware", dto);
			});


	//endpoint to display all documents
	CROW_ROUTE(app, "/hardwares")([&collection]() {
		mongocxx::options::find opts;
		auto docs = collection.find({}, opts);
		
		crow::json::wvalue dto;
		vector<crow::json::rvalue> hardwares;//string to json
		

		for (auto doc : docs) {
			hardwares.push_back(json::load(bsoncxx::to_json(doc)));
		}
		dto["hardwares"] = hardwares;

		return getView("hardwares", dto);
		});


	//API endpoint to read all documents
	CROW_ROUTE(app, "/api/hardwares")
		([&collection](const request& req) {
		mongocxx::options::find opts;
		auto docs = collection.find({}, opts);
		vector<crow::json::rvalue> hardwares;

		for (auto doc : docs) {
			hardwares.push_back(json::load(bsoncxx::to_json(doc)));
		}
		crow::json::wvalue dto;
		dto["hardwares"] = hardwares;
		return crow::response{ dto };
			});


	//API endpoint to update document from the given key-value pairs.
	CROW_ROUTE(app, "/api/update/<string>/<string>").methods(HTTPMethod::POST)
		([&collection](const request& req, const string& key, const string& value) {
		crow::json::rvalue request_body = json::load(req.body);
		bool update_successful;
		if (key == "brand") {
			update_successful = updateDocument(collection, "Invoice No", std::string(request_body["Invoice No"]), "Brand", value);
		}
		else if (key == "dateOfPurchase") {
			update_successful = updateDocument(collection, "Invoice No", std::string(request_body["Invoice No"]), "Date Of Purchase", value);
		}
		else if (key == "unitsSold") {
			update_successful = updateDocument(collection, "Invoice No", std::string(request_body["Invoice No"]), "Units Sold", value);
		}
		return (update_successful ? crow::response(200) : crow::response(400));
		});


	//API endpoint to delete document from the given json body
	CROW_ROUTE(app, "/api/delete").methods(HTTPMethod::POST)
		([&collection](const request& req) {
		crow::json::rvalue request_body = json::load(req.body);

		bool delete_successful = deleteDocument(collection, createDocument({ {"Invoice No", std::string(request_body["Invoice No"])} }));

		return (delete_successful ? crow::response(200) : crow::response(400));
			});


	//API endpoint to insert document from the given json body
	CROW_ROUTE(app, "/api/addNew").methods(HTTPMethod::POST)
		([&collection](const request& req) {
		crow::json::rvalue request_body = json::load(req.body);

		bool find_successful = findHardwareRecord(collection, std::string(request_body["Invoice No"]));

		if (find_successful) {
			return crow::response(400);
		}
		else if(!find_successful)
		{
			insertDocument(collection, createDocument({ {"Product Name", std::string(request_body["Product Name"])}, {"Invoice No", std::string(request_body["Invoice No"])}, {"Brand", std::string(request_body["Brand"])},{"Units Sold", std::string(request_body["Units Sold"])}, {"Date Of Purchase", std::string(request_body["Date Of Purchase"])} }));
			return crow::response(200);
		}
		});


	//API endpoint to find document from the given json body
	CROW_ROUTE(app, "/api/find").methods(HTTPMethod::GET)
		([&collection](const request& req) {
		crow::json::rvalue request_body = json::load(req.body);

		auto doc = collection.find_one(make_document(kvp("Invoice No", std::string(request_body["Invoice No"]))));
		crow::json::wvalue dto;
		dto["hardware"] = json::load(bsoncxx::to_json(doc.value().view()));
		return crow::response{ dto };
			});

	//set the port, set the app to run on multiple threads, and run the app
	app.port(18080).multithreaded().run();
	
}


