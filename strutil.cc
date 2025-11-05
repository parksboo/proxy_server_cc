
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

std::vector<std::string> ExtractLinks(const std::string& contents, const std::string& host) {
  std::vector<std::string> links;
  std::regex link_pattern(R"((?:href|src)\s*=\s*["']([^"']+)["'])");
  std::smatch match;
  std::string::const_iterator search_start(contents.cbegin());

  while (std::regex_search(search_start, contents.cend(), match, link_pattern)) {
    std::string link = match[1];
    search_start = match.suffix().first;

    if (link.find("https://") == 0) continue;
    if (link.find("http://") == 0) {
      std::regex host_re(R"(^https?:\/\/([^\/]+))");
      std::smatch host_match;
      if (std::regex_search(link, host_match, host_re)) {
        std::string link_host = host_match[1];
        if (link_host != host) continue;
      }

      size_t pos = link.find('/', link.find("//") + 2);
      if (pos != std::string::npos) link = link.substr(pos);
      else link = "/";
    }

    if (link[0] != '/') {
      if (link.rfind("./", 0) == 0)
        link = link.substr(2);

      while (link.rfind("../", 0) == 0)
        link = link.substr(3);

      link = "/" + link;
    }
    links.push_back(link);
  }
  return links;
}

}  // namespace proxy_util