#pragma once
#include <string>

class DbResult {
public:
    enum Severity { None, Warning, Error };

    DbResult() : severity(None), code(0), message("") {}

    void clear() {
        severity = None;
        code = 0;
        message.clear();
    }

    void setError(int c, const std::string& msg) {
        severity = Error;
        code = c;
        message = msg;
    }

    void setWarning(int c, const std::string& msg) {
        severity = Warning;
        code = c;
        message = msg;
    }

    bool hasError() const { return severity == Error; }
    bool hasWarning() const { return severity == Warning; }
    bool ok() const { return severity == None; }

    std::string toString() const { return message; }

    Severity severity;
    int code;
    std::string message;
};
