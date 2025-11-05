
// related header
#include "strutil.h"

// system header
#include <openssl/md5.h>

// C++ standard library
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <vector>
#include <regex>

// project header

namespace proxy_util {

std::string Trim(const std::string& s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  if (start == std::string::npos || end == std::string::npos) {
    return "";
  }
  return s.substr(start, end - start + 1);
}

std::string ToLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return s;
}
std::string cryptic_hash(const std::string& s) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)s.c_str(), s.size(), digest);
    std::ostringstream oss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    return oss.str();
}

std::vector<std::string> ExtractLinks(const std::string& html) {
    std::vector<std::string> links;
    std::regex url_pattern(R"((?:href|src)\s*=\s*\"([^\"]+)\")");
    std::smatch match;
    std::string::const_iterator search_start(html.cbegin());

    while (std::regex_search(search_start, html.cend(), match, url_pattern)) {
        links.push_back(match[1]);
        search_start = match.suffix().first;
    }
    return links;
}

}  // namespace proxy_util