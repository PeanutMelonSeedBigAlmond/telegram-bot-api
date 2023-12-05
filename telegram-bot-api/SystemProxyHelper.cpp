#include "SystemProxyHelper.h"
#include <cstdarg>
#include <cstdlib>

namespace peanut {
using std::string;

string& trim(string& s) {
  if (s.empty()) {
    return s;
  }

  s.erase(0, s.find_first_not_of(" \t\r\n"));
  s.erase(s.find_last_not_of(" \t\r\n") + 1);
  return s;
}

string getSystemEnvironmentVariables(int count, ...) {
  va_list keys;
  va_start(keys, count);

  for (int i = 0; i < count; i++) {
    char* key = va_arg(keys, char*);
    char* valuePtr=getenv(key);
    if (!valuePtr){
      continue;
    }
    string value = string(valuePtr);
    value = trim(value);

    if (!value.empty()) {
      return value;
    }
  }

  return "";
}

std::shared_ptr<Proxy> parseUrl(const std::string& url) {
  string copy = url;
  int indexOfSchemeSpilter = copy.find("://");
  if (indexOfSchemeSpilter == -1) {
    return nullptr;
  }

  string scheme = url.substr(0, indexOfSchemeSpilter);
  scheme = trim(scheme);
  if (scheme.empty()) {
    return nullptr;
  }
  copy = copy.substr(indexOfSchemeSpilter + 3);

  string username;
  string password;
  int port = 0;
  string host;

  int indexOfHostSplit = copy.find('@', indexOfSchemeSpilter + 3);
  if (indexOfHostSplit != -1) {
    string t = copy.substr(0, indexOfHostSplit);
    t = trim(t);
    if (!t.empty()) {
      int index = t.find(':');
      if (index == -1) {
        return nullptr;
      }
      username = t.substr(0, index);
      password = t.substr(index + 1);
      copy = copy.substr(indexOfHostSplit + 1);
    }
  }

  int index = copy.rfind(':');
  if (index != -1) {
    port = atoi(copy.substr(index + 1).c_str());
    copy = copy.substr(0, index);
  }

  host = copy;

  return std::make_shared<Proxy>(Proxy{scheme, host, port, username, password});
}

string getSystemNonProxy() {
  return getSystemEnvironmentVariables(2, "NO_PROXY", "no_proxy");
}

std::shared_ptr<Proxy> getSystemHttpsProxy() {
  auto ptr = parseUrl(getSystemEnvironmentVariables(2, "HTTPS_PROXY", "https_proxy"));
  if (ptr==nullptr) return nullptr;

  Proxy* proxy = ptr.get();
  if (proxy->port == 0) {
    if (proxy->type == "http") {
      proxy->port = 80;
    } else if (proxy->type == "https") {
      proxy->port = 443;
    } else if (proxy->type == "socks5") {
      proxy->port = 1080;
    } else {
      return nullptr;
    }
  }

  if (proxy->port <= 0 || proxy->port >= 65536) {
    return nullptr;
  } else {
    return ptr;
  }
}

std::shared_ptr<Proxy> getSystemHttpProxy() {
  auto ptr = parseUrl(getSystemEnvironmentVariables(2, "HTTP_PROXY", "http_proxy"));
  if (ptr==nullptr) return nullptr;

  Proxy* proxy = ptr.get();
  if (proxy->port == 0) {
    if (proxy->type == "http") {
      proxy->port = 80;
    } else if (proxy->type == "https") {
      proxy->port = 443;
    } else if (proxy->type == "socks5") {
      proxy->port = 1080;
    } else {
      return nullptr;
    }
  }

  if (proxy->port <= 0 || proxy->port >= 65536) {
    return nullptr;
  } else {
    return ptr;
  }
}

std::shared_ptr<Proxy> getProxy() {
  auto httpProxy = getSystemHttpProxy();
  auto httpsProxy = getSystemHttpsProxy();
  if (httpProxy != nullptr) {
    return httpProxy;
  } else if (httpsProxy != nullptr) {
    return httpsProxy;
  } else {
    return nullptr;
  }
}
}  // namespace peanut