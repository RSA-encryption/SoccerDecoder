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
#include "headers/json.hpp"
#include "headers/LiteDriver.hpp"

#define CONFIG "config.json"
#define BUFSIZE 4096

using JSON = nlohmann::json;

std::string SpaceString(std::string& s, std::string prop = std::string(""));
DWORD RunTerminal(const char* pArg, bool bHasOpenPipes);
JSON RunAll();

bool CheckBuildLog(const std::string& logPath);
void SetupConfig();
void ProcessConfig();
void FATAL_EXIT(std::string s);
void ReplaceAll(std::string& str, const std::string& from, const std::string& to);
void PrepareSQLite();

HANDLE g_hStdin_r = NULL;
HANDLE g_hStdout_w = NULL;
JSON g_cfg = NULL;
std::unordered_map<std::string, std::pair<std::string, std::string>> g_uExecuteables;

namespace fs = std::filesystem;

int main()
{
	PrepareSQLite();
	SetupConfig();
	ProcessConfig();
	std::cout << RunAll().dump(4) << std::endl;
	return EXIT_SUCCESS;
}

JSON RunAll() {
	JSON data;
	CHAR chBuf[BUFSIZE];
	DWORD dwRead = NULL;

	std::cout << std::endl << "\x1B[93mRunning all processes... \033[0m\t\t" << std::endl;
	for (auto& exec : g_uExecuteables) {
		std::string cmd;
		std::string result;

		std::cout << std::endl << "\x1B[93mProcess: " << exec.first << " ----- Status:\033[0m\t\t \x1B[36mrunning\033[0m\t\t" << std::endl;

		if (exec.second.second.size() != 0) {
			cmd = SpaceString(exec.second.first, exec.second.second);
		} else {
			cmd = exec.second.first;
		}

		if (RunTerminal(cmd.c_str(), true) == 0) {
			std::cout << "\x1B[93mProcess: " << exec.first << " ----- Status:\033[0m\t\t \x1B[32mfinished\033[0m\t\t" << std::endl;
			if (!ReadFile(g_hStdin_r, chBuf, BUFSIZE, &dwRead, NULL)) {
				FATAL_EXIT("Failed to read data from pipe");
			}
			result.resize(static_cast<int>(dwRead)); // Why not account for nullbyte ? Because it would have to get removed anyway
			memcpy_s(result.data(), static_cast<int>(dwRead), chBuf, static_cast<int>(dwRead));
			data[exec.first] = JSON::parse(result);
		} else {
			FATAL_EXIT("Process exited with unexcepted value.. Aborting all actions..");
		}
	}
	return data;
}

DWORD RunTerminal(const char* pArg, bool standardOutput) {
	STARTUPINFOA sInfo;
	PROCESS_INFORMATION pInfo;
	SECURITY_ATTRIBUTES sAttr;
	DWORD dwExitCode = NULL;

	ZeroMemory(&pInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	sAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	sAttr.bInheritHandle = TRUE;
	sAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&g_hStdin_r, &g_hStdout_w, &sAttr, 0))
		FATAL_EXIT("Failed to create stdout pipe");
	if (!SetHandleInformation(g_hStdin_r, HANDLE_FLAG_INHERIT, 0))
		FATAL_EXIT("Failed to set stdout handle information");

	sInfo.cb = sizeof(STARTUPINFO);
	if (standardOutput) {
		sInfo.hStdError = g_hStdout_w;
		sInfo.hStdOutput = g_hStdout_w;
		sInfo.hStdInput = g_hStdin_r;
		sInfo.dwFlags |= STARTF_USESTDHANDLES;
	}

	if (!CreateProcessA(NULL,
		(LPSTR)pArg,
		NULL,
		NULL,
		standardOutput,
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
		NULL,
		NULL,
		&sInfo,
		&pInfo)) {
		FATAL_EXIT((std::stringstream() << "process encountered an error " << " Error Code: " << GetLastError()).str());
	} else {
		WaitForSingleObject(pInfo.hProcess, INFINITE);
		GetExitCodeProcess(pInfo.hProcess, &dwExitCode);

		CloseHandle(pInfo.hProcess);
		CloseHandle(pInfo.hThread);
		CloseHandle(g_hStdout_w);
		return dwExitCode;
	}
	return EXIT_FAILURE;
}

void PrepareSQLite() {
	std::string file = fs::current_path().string().append("\\data.sqlite");
	if (!std::filesystem::exists(file)) {
		std::fstream fs;
		fs.open(file, std::ios::out);
		fs.close();
	}
	DB::LiteDriver db(file);
	db.execSingleStmt("CREATE TABLE IF NOT EXISTS `teams` (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,`name` INTEGER NOT NULL)");
	db.execSingleStmt("CREATE TABLE IF NOT EXISTS `players` (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,`team_id` INTEGER NOT NULL,`rating` DOUBLE unsigned NOT NULL,`stamina` DOUBLE NOT NULL,`age` DOUBLE NOT NULL,`experience` DOUBLE NOT NULL,`name` DOUBLE NOT NULL,`position` INT unsigned NOT NULL, FOREIGN KEY(team_id) REFERENCES teams(id))");
}


std::string SpaceString(std::string& s, std::string prop) { // Tiny utility so I don't have to worry about random spaces
	return prop.empty() ? std::string(" ").append(s) : std::string(s).append(" ").append(prop);
}

void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
}

bool CheckBuildLog(const std::string& logPath) {
	std::ifstream ifs(logPath);
	std::string line;
	while (getline(ifs, line)) {
		size_t pos = line.find("Build succeeded");
		if (pos != std::string::npos)
			return true;
	}
	return false;
}

void FATAL_EXIT(std::string s) {
	std::cerr << "\x1B[91mFatal Error: " << s << "\033[0m\n" << std::endl;
	exit(EXIT_FAILURE);
}

void SetupConfig() {
	std::fstream fsCfg;
	std::stringstream ssBuffer;

	fsCfg.open(CONFIG, std::ios::in);
	if (!fsCfg) {
		FATAL_EXIT("unable to load config file");
	}
	try {
		ssBuffer << fsCfg.rdbuf();
		g_cfg = JSON::parse(ssBuffer);
	} catch (std::exception& e) {
		FATAL_EXIT(e.what());
	}
}

void ProcessConfig() {
	try {
		std::string workingDirectory(fs::current_path().string().append("\\"));
		for (JSON::iterator it = g_cfg.begin(); it != g_cfg.end(); ++it) {
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
					ReplaceAll(args, "%projectdir%", projectFullPath.append("\\"));
				}
				compileCmd = SpaceString(std::string(translatorFullPath).append(SpaceString(projectFile)), args);
				std::cout << "\x1B[93mProject: " << it.key() << " ----- Compilation:\033[0m\t\t \x1B[36mstarted\033[0m\t\t" << std::endl;
				RunTerminal(compileCmd.c_str(), standardOutput);
				if (!standardOutput) {
					if (!CheckBuildLog(std::string(projectFullPath).append("build.log"))) {
						std::cout << "\x1B[93mProject: " << it.key() << " ----- Compilation:\033[0m\t\t failed" << std::endl;
						FATAL_EXIT("one or more project failed to compile.. Aborting..");
					} else {
						std::cout << "\x1B[93mProject: " << it.key() << " ----- Compilation:\033[0m\t\t \x1B[32msuccessful\033[0m\t\t" << std::endl;
						ReplaceAll(executeable, ".sln", ".exe");
					}
				}
			}
			for (auto const& file : fs::recursive_directory_iterator(projectFullPath)) {
				std::string execPath((std::stringstream() << file).str());
				size_t index = execPath.find(executeable);

				if (index != std::string::npos && execPath.substr(index, execPath.length() - index - 1) == executeable) {
					std::string arguments = jProject.find("launchArguments").value().get<std::string>();
					execPath.erase(execPath.find("\""), 1); // Cleanse the path
					execPath.erase(execPath.find("\""), 1);
					ReplaceAll(execPath, "\\\\", "\\"); 
					if (isCompiledLang) { // Add exec path and if it's interpreted language also add interpreter path so we can work with it later on
						g_uExecuteables[it.key()] = std::move(std::pair<std::string, std::string>(execPath.append(arguments.size() == 0 ? "" : SpaceString(arguments)), std::string()));
					} else {
						g_uExecuteables[it.key()] = std::move(std::pair<std::string, std::string>(execPath.append(arguments.size() == 0 ? "" : SpaceString(arguments)), translatorFullPath));
					}
					hasSource = true; // In case someone attempts to run this on empty dir or on a directory where specified source file is not present
					std::cout << "\x1B[93mProject: " << it.key() << " ----- Executeable:\033[0m\t\t \x1B[32msuccessfully found\033[0m\t\t" << std::endl;
				}
			}
			if (!hasSource) {
				FATAL_EXIT((std::stringstream() << "project " << it.key() << " doesn't seem to have source files..  Aborting all actions...").str());
			}
		}
	} catch (std::exception& e) { // What could happen is that we could try to get non-existent json prop, hence we let the user know
		FATAL_EXIT(e.what());
	}
}