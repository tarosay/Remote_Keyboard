#ifndef MAKE_HTML_HPP
#define MAKE_HTML_HPP
#include <Arduino.h>

#include "html_response.hpp"

String RootHtml(String ssid, String serverIP, String staIP);
String Root_SetHtml(String ssid);

#endif  // MAKE_HTML_HPP