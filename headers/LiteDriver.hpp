#ifndef C79B92EF_2EA5_43C7_A33B_1420690D1E
#define C79B92EF_2EA5_43C7_A33B_1420690D1E

#include <iostream>
#include <vector>
#include <assert.h>
#include <direct.h>
#include <string>
#include <sstream>
#include <filesystem>
#include "../headers/sqlite3.h"

namespace DB {

	class LiteDriver {
	public:
		LiteDriver() = default;
		LiteDriver(std::string& url);
		~LiteDriver();

		char* msg = nullptr;
		struct Column {
			Column(Column& c) {}
			Column(Column&& c) noexcept : name(std::move(c.name)),
				value(std::move(c.value)) {
			}
			Column(const char* x, const char* y) {
				this->name = std::make_unique<char[]>(std::strlen(x) + 1);
				this->value = std::make_unique<char[]>(std::strlen(y) + 1);
				strcpy_s(this->name.get(), std::strlen(x) + 1, x);
				strcpy_s(this->value.get(), std::strlen(y) + 1, y);
			}
			std::unique_ptr<char[]> name;
			std::unique_ptr<char[]> value;
			~Column() {}
		};
		using Storage = std::vector<std::vector<Column>>;

		void execSingleStmt(const std::string& stmt, bool out = false);
		void getRawStoragePtr(const Storage* storage, bool nullStoragePtr = true);
		const Storage& getStorageRefference();
		static const std::string GetStringFromRow(const Storage& vec, const std::string s);
		static const double GetDoubleFromRow(const Storage& vec, const std::string s);
	private:
		int code;
		std::string loc;
		sqlite3* db;
		Storage* storage;

		static int callback(void** NotUsed, int argc, char** argv, char** azColName);
		static bool isDouble(const char* c);
	};

}


#endif