#ifndef REQUEST_H
#define REQUEST_H

#include <map>
#include <sstream>
#include <string>

class Request {
public:
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;

    static Request parse(const std::string& rawRequest) {
        Request req;
        std::istringstream stream(rawRequest);
        std::string line;

        // First line: GET /path HTTP/1.1 (example)
        if (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            std::istringstream lineStream(line);
            lineStream >> req.method >> req.path >> req.httpVersion;
        }

        // TODO: Parse headers and body if needed
    }
};

#endif
