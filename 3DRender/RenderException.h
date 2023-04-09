#pragma once
#include <exception>
#include <string>

class RenderException : std::exception{
public:
	RenderException(int line, const char* file);
	const char* what();
	virtual const char* GetType();
	int GetLine();
	const std::string& GetFile();
	std::string GetOriginString();

private:
	int line;
	std::string file;

protected:
	mutable std::string whatBuffer;
};