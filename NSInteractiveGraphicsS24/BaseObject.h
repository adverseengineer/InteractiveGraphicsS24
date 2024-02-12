#pragma once
#include <sstream>
#include <vector>
// The base class for everything
class BaseObject {
protected:
	static std::stringstream _log;
public:
	inline const std::string GetLog() const { return _log.str(); }
	void Log(const std::string& message) { _log << message << std::endl; }
	void Log(const char* message) { _log << message << std::endl; }
	void Log(const std::vector<char>& infoLog) {
		std::copy(
			infoLog.begin(), infoLog.end(),
			std::ostream_iterator<char>(_log, "")
		);
	}
};
