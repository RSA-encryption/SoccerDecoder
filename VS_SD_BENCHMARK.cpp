#include <iostream>
#include <cstdlib>
#include <direct.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <processenv.h>
#include <fstream>
#include <filesystem>
#include <random>
#include "headers/json.hpp"
#include "headers/LiteDriver.hpp"
#include "headers/mysql_driver.h"
#include "headers/cppconn/statement.h"
#include "headers/mysql_error.h"
#include "headers/mysql_connection.h"
#include "headers/cppconn/resultset.h"
#include <thread>

#define CONFIG "config.json"
#define TEAM_COUNT 2
#define NUMBER_OF_ENGINES 3
#define POSITION_TYPE_COUNT 4
#define JSON_PRETTY_PRINT 4
#define POSITION_PLAYER_COUNT 5
#define SIMULATION_COUNT 10000
#define BUFSIZE 4096

using JSON = nlohmann::json;

std::string spaceString(std::string& rStr, std::string rProp = std::string(""));
DWORD runTerminal(const char* pArg, bool bStandardOutput, bool useIndexedHandle = false, int index = -1);
JSON runAll();

bool checkBuildLog(const std::string& rLogPath);
void setupConfig();
void processConfig();
void fatalExit(std::string str);
std::vector<int> chunks(const int count);
void replaceAll(std::string& rStr, const std::string& rFrom, const std::string& rTo);
const std::string& addQuotes(std::string& rStr, bool bAppendComma = true);
void prepareAllDatabases();

HANDLE g_hStdin_r = NULL;
HANDLE g_hStdout_w = NULL;
HANDLE* g_StdinArray;
HANDLE* g_StdoutArray;

JSON g_cfg = NULL;
int simulationCount = NULL;
auto threadCount = std::thread::hardware_concurrency();
std::unordered_map<std::string, std::pair<std::string, std::string>> g_umExecuteables;
std::unordered_map<std::string, bool> g_umExecuteableFakeThreading; // Quick fix 
std::unordered_map<int, std::string> g_umNames = {std::make_pair(0,"SQLITE"), std::make_pair(1,"MYSQL"), std::make_pair(2,"MARIADB"), std::make_pair(3,"ORACLE") /* I might add oracle if I have enough time */};

namespace fs = std::filesystem;

int main()
{
	g_StdinArray = new HANDLE[threadCount];
	g_StdoutArray = new HANDLE[threadCount];

	prepareAllDatabases();
	setupConfig();
	processConfig();

	std::string file = fs::current_path().string().append("\\output.json");
	remove(file.c_str()); // No real need to check for return value since either it gets deleted or it didn't cuz it wasn't even present in the directory
	if (!std::filesystem::exists(file)) { // Sanity check
		std::string str = runAll().dump(JSON_PRETTY_PRINT);
		std::fstream fs;
		std::cout << std::endl << "\x1B[93mDumping results to file... \033[0m\t\t" << std::endl;
		fs.open(file, std::ios::out);
		fs.write(str.c_str(), str.length());
		fs.close();
		std::cout << "\x1B[36mDumping results successfully finished\033[0m\t\t" << std::endl;
	}

	delete g_StdoutArray;
	delete g_StdinArray;
	return EXIT_SUCCESS;
}

JSON runAll() {
	JSON data;

	std::cout << std::endl << "\x1B[93mRunning all processes... \033[0m\t\t" << std::endl;
	for (auto& exec : g_umExecuteables) {
		std::string cmd;

		std::cout << std::endl << "\x1B[93mProcess: " << exec.first << " ----- Status:\033[0m\t\t \x1B[36mrunning\033[0m\t\t" << std::endl;

		if (exec.second.second.size() != 0) {
			cmd = spaceString(exec.second.second, exec.second.first);
		} else {
			cmd = exec.second.first;
		}
		for (int i = 0; i < NUMBER_OF_ENGINES; ++i) {
			std::string tempCmd(cmd);
			if (g_umExecuteableFakeThreading.at(exec.first)) {
				std::vector<std::thread> threads;
				std::atomic<int> aw = 0, bw = 0, dw = 0;
				auto t = std::chrono::high_resolution_clock::now();
				auto vec = chunks(simulationCount);
				for (int j = 0; j < vec.size(); ++j) {
					threads.push_back(std::thread([&](int j)
						{
							std::string result;
							CHAR chBuf[BUFSIZE];
							DWORD dwRead = NULL;

							if (i == 0) { // Sqlite has an exception because I feel like hard coding the file path is bad idea ( no shit ), the other database systems will be run on localhost and I can't imagine anyone wanting to test this somewhere else except their own pc, worst case scenario they will do a little bit of editing
								tempCmd.append((std::stringstream() << " " << vec[j] << " " << std::to_string(i) << " " << fs::current_path().string().append("\\data.sqlite")).str());
							} else {
								tempCmd.append((std::stringstream() << " " << vec[j] << " " << std::to_string(i)).str());
							}

							if (runTerminal(tempCmd.c_str(), true, true, j) == 0) {
								if (!ReadFile(g_StdinArray[j], chBuf, BUFSIZE, &dwRead, NULL)) {
									fatalExit("Failed to read data from pipe");
								}
								result.resize(static_cast<int>(dwRead)); // Why not account for nullbyte ? Because it would have to get removed anyway
								memcpy_s(result.data(), static_cast<int>(dwRead), chBuf, static_cast<int>(dwRead));
								result = result.substr(0, result.find("<!-!>", 0)); // Used as marker for cleanup
								auto data = JSON::parse(result);
								aw += data.find("a").value().get<int>();
								bw += data.find("b").value().get<int>();
								dw += data.find("draws").value().get<int>();
							} else {
								throw new std::exception("Fatal error during batch job execution..");
							}
						}, j));
					std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Prevent collisions
				}
				try{
					for (auto& t : threads)
						if (t.joinable())
							t.join();
				} catch (std::exception& e) {
					fatalExit(e.what());
				}

				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count();

				if (g_umNames.at(i) == "MYSQL") {
					std::cout << "\x1B[93mProcess: " << exec.first << " " << g_umNames.at(i) << " ------ Status:\033[0m\t \x1B[32mfinished\033[0m\t\t" << std::endl;
				}
				else std::cout << "\x1B[93mProcess: " << exec.first << " " << g_umNames.at(i) << " ----- Status:\033[0m\t \x1B[32mfinished\033[0m\t\t" << std::endl; // Why ? Simple answear hard coded pretty print. I may clean it up later
				
				data[g_umNames.at(i)][exec.first] = JSON::parse((std::stringstream() << "{\"a\":" << aw << "," << "\"b\":" << bw << "," << "\"draws\":" << dw << "," << "\"duration\":" << duration << "}"));
			}
			else {
				CHAR chBuf[BUFSIZE];
				DWORD dwRead = NULL;
				std::string result;

				if (i == 0) { // Sqlite has an exception because I feel like hard coding the file path is bad idea ( no shit ), the other database systems will be run on localhost and I can't imagine anyone wanting to test this somewhere else except their own pc, worst case scenario they will do a little bit of editing
					tempCmd.append((std::stringstream() << " " << simulationCount << " " << std::to_string(i) << " " << fs::current_path().string().append("\\data.sqlite")).str());
				}
				else {
					tempCmd.append((std::stringstream() << " " << simulationCount << " " << std::to_string(i)).str());
				}
				if (runTerminal(tempCmd.c_str(), true) == 0) {
					if (g_umNames.at(i) == "MYSQL") {
						std::cout << "\x1B[93mProcess: " << exec.first << " " << g_umNames.at(i) << " ------ Status:\033[0m\t \x1B[32mfinished\033[0m\t\t" << std::endl;
					}
					else std::cout << "\x1B[93mProcess: " << exec.first << " " << g_umNames.at(i) << " ----- Status:\033[0m\t \x1B[32mfinished\033[0m\t\t" << std::endl; // Why ? Simple answear hard coded pretty print. I may clean it up later
					if (!ReadFile(g_hStdin_r, chBuf, BUFSIZE, &dwRead, NULL)) {
						fatalExit("Failed to read data from pipe");
					}
					result.resize(static_cast<int>(dwRead)); // Why not account for nullbyte ? Because it would have to get removed anyway
					memcpy_s(result.data(), static_cast<int>(dwRead), chBuf, static_cast<int>(dwRead));
					result = result.substr(0, result.find("<!-!>", 0)); // Used as marker for cleanup
					data[g_umNames.at(i)][exec.first] = JSON::parse(result);
				}
				else {
					fatalExit("Process exited with unexcepted value.. Aborting all actions..");
				}
			}
		}
	}
	return data;
}

DWORD runTerminal(const char* pArg, bool bStandardOutput, bool useIndexedHandle, int index){
	STARTUPINFOA startInfo;
	PROCESS_INFORMATION procInfo;
	SECURITY_ATTRIBUTES securityAttr;
	DWORD dwExitCode = NULL;

	ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&startInfo, sizeof(STARTUPINFO));

	securityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttr.bInheritHandle = TRUE;
	securityAttr.lpSecurityDescriptor = NULL;
	startInfo.cb = sizeof(STARTUPINFO);

	if (!useIndexedHandle) {
		if (!CreatePipe(&g_hStdin_r, &g_hStdout_w, &securityAttr, 0))
			fatalExit("Failed to create stdout pipe");
		if (!SetHandleInformation(g_hStdin_r, HANDLE_FLAG_INHERIT, 0))
			fatalExit("Failed to set stdout handle information");

		if (bStandardOutput) {
			startInfo.hStdError = g_hStdout_w;
			startInfo.hStdOutput = g_hStdout_w;
			startInfo.hStdInput = g_hStdin_r;
			startInfo.dwFlags |= STARTF_USESTDHANDLES;
		}
	}
	else {
		if (!CreatePipe(&g_StdinArray[index], &g_StdoutArray[index], &securityAttr, 0))
			fatalExit("Failed to create stdout pipe");
		if (!SetHandleInformation(g_StdinArray[index], HANDLE_FLAG_INHERIT, 0))
			fatalExit("Failed to set stdout handle information");

		if (bStandardOutput) {
			startInfo.hStdError = g_StdoutArray[index];
			startInfo.hStdOutput = g_StdoutArray[index];
			startInfo.hStdInput = g_StdinArray[index];
			startInfo.dwFlags |= STARTF_USESTDHANDLES;
		}
	}

	if (!CreateProcessA(NULL,
		(LPSTR)pArg,
		NULL,
		NULL,
		bStandardOutput,
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
		NULL,
		NULL,
		&startInfo,
		&procInfo)) {
		fatalExit((std::stringstream() << "process encountered an error " << " Error Code: " << GetLastError()).str());
	} else {
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		GetExitCodeProcess(procInfo.hProcess, &dwExitCode);

		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		return dwExitCode;
	}
	return EXIT_FAILURE;
}

const std::string& addQuotes(const std::string& rStr, bool bAppendComma = true) {
	std::string& s = const_cast<std::string&>(rStr); // God forgive me 
	s.insert(0, "\'");
	s.append("\'");
	if (bAppendComma) {
		s.append(",");
	}
	return rStr;
}

std::vector<int> chunks(const int count)
{
	int total = 0;
	std::vector<int> chunk;
	for (size_t i = 0; i < threadCount; i++) {
		chunk.push_back(std::floor(count / (threadCount + 1)));
		total += std::floor(count / (threadCount + 1));
	}
	if (std::floor(count) - total != 0)
		chunk.push_back(count - total);
	return chunk;
}

double randomDouble(double lowerBound, double upperBound){
	std::random_device randomDevice;
	std::default_random_engine engine(randomDevice());
	std::uniform_real_distribution<double> uniformDistribution(lowerBound, upperBound);
	return uniformDistribution(engine);
}

void prepareAllDatabases() {
	std::string file = fs::current_path().string().append("\\data.sqlite");
	if (!std::filesystem::exists(file)) {
		std::fstream fs;
		fs.open(file, std::ios::out);
		fs.close();
	}
	std::cout << std::endl << "\x1B[93mDatabase seeder: ----- Status:\033[0m\t\t \x1B[36mstarted\033[0m\t\t" << std::endl;
	DB::LiteDriver dbSQLITE(file);
	sql::Connection* conMYSQL = nullptr;
	sql::Connection* conMARIADB = nullptr;
	sql::mysql::MySQL_Driver* driverMYSQL = nullptr;
	sql::mysql::MySQL_Driver* driverMARIADB = nullptr;
	sql::Statement* stmtMYSQL = nullptr;
	sql::Statement* stmtMARIADB = nullptr;
	driverMYSQL = sql::mysql::get_mysql_driver_instance();
	conMYSQL = driverMYSQL->connect("tcp://127.0.0.1:3306", "root", "securepassword"); // Could be in config, will probably move it later on when I have time to cleanup
	driverMARIADB = sql::mysql::get_mysql_driver_instance();
	conMARIADB = driverMARIADB->connect("tcp://127.0.0.1:4306", "root", "securepassword");
	stmtMARIADB = conMARIADB->createStatement();
	stmtMYSQL = conMYSQL->createStatement();

	try {
		stmtMYSQL->execute("CREATE DATABASE IF NOT EXISTS soccer");
		stmtMARIADB->execute("CREATE DATABASE IF NOT EXISTS soccer");
		stmtMYSQL->execute("USE soccer");
		stmtMARIADB->execute("USE soccer");
	} catch (std::exception& e) {
		fatalExit(e.what());
	}

	auto executeAll = [&](std::string sql)
	{
		try {
			dbSQLITE.execSingleStmt(sql);
			replaceAll(sql, "AUTOINCREMENT", "AUTO_INCREMENT"); // SQLite differences in syntax
			if (sql.find("CREATE TABLE") != std::string::npos) {
				sql.append(" ENGINE=InnoDB");
			}
			stmtMYSQL->execute(sql);
			stmtMARIADB->execute(sql);
		}
		catch (std::exception& e) {
			fatalExit(e.what());
		}
	};

	executeAll("DROP TABLE IF EXISTS `players`");
	executeAll("DROP TABLE IF EXISTS `teams`");
	executeAll("CREATE TABLE `teams` (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,`name` VARCHAR(50) NOT NULL,`layout` INTEGER NOT NULL)");
	executeAll("CREATE TABLE `players` (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,`team_id` INTEGER NOT NULL,`rating` DOUBLE unsigned NOT NULL,`stamina` DOUBLE NOT NULL,`age` DOUBLE NOT NULL,`experience` DOUBLE NOT NULL,`name` VARCHAR(50) NOT NULL,`position` INT unsigned NOT NULL, FOREIGN KEY(team_id) REFERENCES teams(id))");
	executeAll("INSERT INTO teams (`id`,`name`, `layout`) VALUES(1, 'A', 433)");
	executeAll("INSERT INTO teams (`id`,`name`, `layout`) VALUES(2, 'B', 433)");

	for (size_t i = 0; i < TEAM_COUNT; ++i) {
		for (size_t j = 0; j < POSITION_TYPE_COUNT; ++j) {
			for (size_t k = 0; k < POSITION_PLAYER_COUNT; ++k) {
				executeAll((std::stringstream()
					<< "INSERT INTO `players` (`team_id`, `rating`, `stamina`, `age`, `experience`, `name`, `position`) VALUES("
					<< addQuotes(std::to_string(i + 1))
					<< addQuotes(std::to_string(randomDouble(8, 10)))
					<< addQuotes(std::to_string(randomDouble(12, 20)))
					<< addQuotes(std::to_string(randomDouble(15, 40)))
					<< addQuotes(std::to_string(randomDouble(20, 100)))
					<< addQuotes("Benchmark Player " + std::to_string(i + j + k))
					<< addQuotes(std::to_string(j), false)
					<< ")").str());
			}
		}
	}

	std::cout << "\x1B[93mDatabase seeder: ----- Status:\033[0m\t\t \x1B[32mfinished\033[0m\t\t" << std::endl << std::endl;

	delete stmtMARIADB;
	delete stmtMYSQL;
	delete conMYSQL;
	delete conMARIADB;
}


std::string spaceString(std::string& rStr, std::string rProp) { // Tiny utility so I don't have to worry about random spaces
	return rProp.empty() ? std::string(" ").append(rStr) : std::string(rStr).append(" ").append(rProp);
}

void replaceAll(std::string& rStr, const std::string& rFrom, const std::string& rTo) {
	if (rFrom.empty())
		return;
	size_t pos = 0;
	while ((pos = rStr.find(rFrom, pos)) != std::string::npos) {
		rStr.replace(pos, rFrom.length(), rTo);
		pos += rTo.length();
	}
}

bool checkBuildLog(const std::string& rLogPath) {
	std::ifstream ifs(rLogPath);
	std::string line;
	while (getline(ifs, line)) {
		size_t pos = line.find("Build succeeded");
		if (pos != std::string::npos)
			return true;
	}
	return false;
}

void fatalExit(std::string s) {
	std::cerr << "\x1B[91mFatal Error: " << s << "\033[0m\n" << std::endl;
	exit(EXIT_FAILURE);
}

void setupConfig() {
	std::fstream fsCfg;
	std::stringstream ssBuffer;

	fsCfg.open(CONFIG, std::ios::in);
	if (!fsCfg) {
		fatalExit("unable to load config file");
	}
	try {
		ssBuffer << fsCfg.rdbuf();
		g_cfg = JSON::parse(ssBuffer);
	} catch (std::exception& e) {
		fatalExit(e.what());
	}

	try { // There is 100% better way of doing this but I am speedrunning it at this point
		simulationCount = g_cfg.find("simulations").value().get<int>();
	}
	catch (std::exception& e) {
		simulationCount = SIMULATION_COUNT;
	}
}

void processConfig() {
	try {
		std::string workingDirectory(fs::current_path().string().append("\\"));
		for (JSON::iterator it = g_cfg.begin(); it != g_cfg.end(); ++it) {
			if (it.key() == "simulations") continue;
			JSON jProject = JSON(*it).at("project");
			JSON jTranslator = JSON(*it).at("translator");

			std::string translatorFullPath(workingDirectory);
			std::string projectFullPath(workingDirectory);
			std::string executeable(jProject.find("projectFile").value().get<std::string>());

			bool standardOutput = jTranslator.find("standardOutput").value().get<bool>();
			bool isCompiledLang, hasSource = false;

			if (jProject.find("isRelative").value().get<bool>()) {
				projectFullPath.append(jProject.find("path").value().get<std::string>());
			}
			if (jTranslator.find("isRelative").value().get<bool>()) {
				translatorFullPath.append(jTranslator.find("path").value().get<std::string>());
			}
			if (isCompiledLang = jProject.find("isCompiledLang").value().get<bool>()) {
				std::string args = jTranslator.find("arguments").value().get<std::string>();
				std::string projectFile = std::string(projectFullPath).append("\\").append(jProject.find("projectFile").value().get<std::string>());
				std::string compileCmd;

				if (!standardOutput) {
					replaceAll(args, "%projectdir%", projectFullPath.append("\\"));
				}
				compileCmd = spaceString(std::string(translatorFullPath).append(spaceString(projectFile)), args);
				std::cout << "\x1B[93mProject: " << it.key() << " ----- Compilation:\033[0m\t\t \x1B[36mstarted\033[0m\t\t" << std::endl;
				runTerminal(compileCmd.c_str(), standardOutput);
				if (!standardOutput) {
					if (!checkBuildLog(std::string(projectFullPath).append("build.log"))) {
						std::cout << "\x1B[93mProject: " << it.key() << " ----- Compilation:\033[0m\t\t failed" << std::endl;
						fatalExit("one or more project failed to compile.. Aborting..");
					} else {
						std::cout << "\x1B[93mProject: " << it.key() << " ----- Compilation:\033[0m\t\t \x1B[32msuccessful\033[0m\t\t" << std::endl;
						replaceAll(executeable, ".sln", ".exe");
					}
				}
			}
			for (auto const& file : fs::recursive_directory_iterator(projectFullPath)) {
				std::string execPath((std::stringstream() << file).str());
				size_t index = execPath.find(executeable);

				if (index != std::string::npos && execPath.substr(index, execPath.length() - index - 1) == executeable) {
					execPath.erase(execPath.find("\""), 1); // Cleanse the path
					execPath.erase(execPath.find("\""), 1);
					replaceAll(execPath, "\\\\", "\\"); 
					if (isCompiledLang) { // Add exec path and if it's interpreted language also add interpreter path so we can work with it later on
						g_umExecuteables[it.key()] = std::move(std::pair<std::string, std::string>(execPath, std::string()));
					} else {
						g_umExecuteables[it.key()] = std::move(std::pair<std::string, std::string>(execPath, translatorFullPath));
					}
					g_umExecuteableFakeThreading[it.key()] = jProject.find("fakeThreading").value().get<bool>();
					hasSource = true; // In case someone attempts to run this on empty dir or on a directory where specified source file is not present
					std::cout << "\x1B[93mProject: " << it.key() << " ----- Executeable:\033[0m\t\t \x1B[32msuccessfully found\033[0m\t\t" << std::endl;
					break; // Sometimes finds same exe twice ?
				}
			}
			if (!hasSource) {
				fatalExit((std::stringstream() << "project " << it.key() << " doesn't seem to have source files..  Aborting all actions...").str());
			}
		}
	} catch (std::exception& e) { // What could happen is that we could try to get non-existent json prop, hence we let the user know
		fatalExit(e.what());
	}
}