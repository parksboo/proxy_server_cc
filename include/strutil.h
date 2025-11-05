
#ifndef PROXY_STRUTIL_H_
#define PROXY_STRUTIL_H_

#include <vector>
#include <string>


namespace proxy_util {

std::string Trim(const std::string& s);
std::string ToLower(std::string s);
std::string cryptic_hash(const std::string& s);
std::vector<std::string> ExtractLinks(const std::string& contents, const std::string& host);

}  // namespace proxy_util

#endif  // PROXY_STRUTIL_H_
