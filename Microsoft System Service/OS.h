#pragma once

#include <stdint.h>
#include <vector>
#include <string>

struct OS{
	static void Shutdown();

	static void GetPCName(char* buffer, unsigned long* read_size);

	static void ExportInExecutable(const char* path, const void* data, size_t size);

	static uint64_t GetBootupTime();

	static void CreateMessageBox(const char* title, const char* text);

	static std::string GetCurrentProcessExecutablePath();

	static void NewProcess(char* path);

	static void Exit();

	static void Sleep(unsigned long milliseconds);

	static bool Delete(const char* filepath);

	static bool Move(const char* source, const char* destination);

	static std::vector<std::string> GetActiveWindowList();
};

