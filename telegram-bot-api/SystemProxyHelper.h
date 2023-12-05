#pragma once
#include "memory"
#include <string>

namespace peanut {
struct Proxy {
  std::string type;
  std::string host;
  int port;
  std::string username;
  std::string password;
};

std::shared_ptr<Proxy> getSystemHttpProxy();
std::shared_ptr<Proxy> getSystemHttpsProxy();
std::shared_ptr<Proxy> getProxy();
std::string getSystemNonProxy();
}  // namespace peanut