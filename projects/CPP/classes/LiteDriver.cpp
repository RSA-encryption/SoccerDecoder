#include "../headers/LiteDriver.hpp"

namespace DB {

	LiteDriver::LiteDriver(std::string& url) {
		this->loc.assign(url);
		this->code = sqlite3_open(this->loc.c_str(), &db);
		if (this->code != SQLITE_OK) { // If this fails we just commit suicide
			std::cerr << "Cannot open database: " << sqlite3_errmsg(this->db);
			this->~LiteDriver();
			std::exit(EXIT_FAILURE);
		}
		this->storage = new Storage();
	}

	void LiteDriver::execSingleStmt(const std::string& stmt, bool out) {
		if (out) {
			this->code = sqlite3_exec(this->db, stmt.c_str(), &this->callback, reinterpret_cast<void**>(&this->storage), &this->msg);
		}
		else {
			this->code = sqlite3_exec(this->db, stmt.c_str(), 0, 0, &this->msg);
		}
		if (this->code != SQLITE_OK) {
			std::cerr << "Database error: " << this->msg;
			exit(EXIT_FAILURE);
		}
	}

	const DB::LiteDriver::Storage& LiteDriver::getStorageRefference() {
		return *this->storage;
	}

	void LiteDriver::getRawStoragePtr(const DB::LiteDriver::Storage* storage, bool nullStoragePtr) {
		storage = this->storage;
		if (nullStoragePtr) this->storage = nullptr;
	}

	const double LiteDriver::GetDoubleFromRow(const Storage& vec, const std::string s) {
		for (auto& tmp : vec)
			for (auto& j : tmp) if (strcmp(s.c_str(), j.name.get()) == 0) {
				if (isDouble(j.value.get())) return std::stod(j.value.get());
				throw new std::exception("Unable to convert string to numeral value");
			}
		throw new std::exception("Column doesn't exist");
	}

	const std::string LiteDriver::GetStringFromRow(const Storage& vec, const std::string s) {
		for (auto& tmp : vec)
			for (auto& j : tmp) if (strcmp(s.c_str(), j.name.get()) == 0) return std::string(j.value.get());
		throw new std::exception("Column doesn't exist");
	}

	LiteDriver::~LiteDriver() {
		sqlite3_free(msg);
		sqlite3_close(this->db);
		delete this->storage;

		this->db = nullptr;
		this->storage = nullptr;
	}

	int LiteDriver::callback(void** NotUsed, int argc, char** argv, char** azColName) {
		Storage* storage = (Storage*)*NotUsed;
		std::vector<Column> vec;
		for (int i = 0; i < argc; ++i) {
			vec.push_back(std::move(Column(azColName[i], argv[i])));
		}
		storage->push_back(std::move(vec));
		return 0;
	}

	bool LiteDriver::isDouble(const char* c) {
		double f;
		std::string s(c);
		std::istringstream iss(s);
		iss >> std::noskipws >> f;
		return iss.eof() && !iss.fail();
	}
}