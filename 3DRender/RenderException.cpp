#include "RenderException.h"
#include <sstream>

// This class is used to catch any exceptions that is thrown while rendering, this will help a lot later on.

RenderException::RenderException(int line, const char* file) {
	this->line = line;
	this->file = file;
}

const char* RenderException::what() {
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	// Because this method returns a char pointer if we were to just return the oss string then at the end of the method the oss will delete itself
	// and would be pointing to dead memory. That is the buffer is used
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* RenderException::GetType() {
	return "Render Exception";
}

const std::string& RenderException::GetFile() {
	return file;
}

int RenderException::GetLine() {
	return line;
}

std::string RenderException::GetOriginString() {
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
