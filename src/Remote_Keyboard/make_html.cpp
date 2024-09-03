#include "make_html.hpp"

String RootHtml(String ssid, String serverIP, String staIP) {
  String htmlResponse = "<!DOCTYPE html><html>";
  htmlResponse += root_html_header;
  htmlResponse += "<body>";
  htmlResponse += "<h2>Remote USB Keyboard by M5StampS3</h2>";

  htmlResponse += "<textarea id=\"keyCapture\" style=\"width:100%; height:200px;\" autofocus></textarea>";
  htmlResponse += "<hr>";

  if (staIP != "0.0.0.0") {
    htmlResponse += "<p>The station is connected to SSID:" + ssid + ". The station's IP address is " + staIP + ".</p>";
    htmlResponse += "<p>If you want to access via the station, please visit <a href=\"http://" + staIP + "\">http://" + staIP + "</a>.</p>";
  }

  htmlResponse += R"rawliteral(
    <div id="description">
      <p>At the next startup, the WiFi station will connect using the SSID and password set here. </p>
      <p>If you want to switch to a new WiFi station, please enter the details and press the 'Set' button. </p>
    </div>
    <!-- WiFi Configuration Form -->
    <form id="wifiForm">
      <label for="ssid">SSID:</label>
      <input type="text" id="ssid" name="ssid">
      <label for="password">Password:</label>
      <input type="password" id="password" name="password">
      <button type="button" onclick="setWiFi()">Set</button>
    </form>
  )rawliteral";

  htmlResponse += root_body_script;
  htmlResponse += "</body></html>";
  return htmlResponse;
}

String Root_SetHtml(String ssid) {
  String htmlResponse = "<!DOCTYPE html><html>";
  htmlResponse += "<head><title>WiFi Setting Parameters</title></head>";
  htmlResponse += "<body>";
  htmlResponse += "<h3>The SSID:" + ssid + " and password have been saved for station connection on the next startup.</h3>";
  htmlResponse += R"rawliteral(
    <button onclick="window.location.href='/'">Go back to Remote USB Keyboard</button>
  )rawliteral";
  htmlResponse += "</body></html>";
  return htmlResponse;
}
