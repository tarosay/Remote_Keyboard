/*
 * Remote_Keyboard.ino
 * 
 * This program allows a device (like M5StampS3) to act as a remote keyboard over a network
 * when connected via USB to a Raspberry Pi or PC. It is designed to provide network-based
 * key input functionalities for remote operations.
 * 
 * Author: tarosay
 * Date: 2024/09/04
 * Version: 1.0.0
 * 
 * License: MIT License
 * This program is free software: you can redistribute it and/or modify it under the terms 
 * of the MIT License. See the LICENSE file for more details.
 * 
 * Repository: https://github.com/tarosay/Remote_Keyboard
 * 
 * Disclaimer: This software is provided "as is", without warranty of any kind, express or 
 * implied, including but not limited to the warranties of merchantability, fitness for a 
 * particular purpose, and noninfringement. In no event shall the authors be liable for any 
 * claim, damages, or other liability, whether in an action of contract, tort, or otherwise, 
 * arising from, out of, or in connection with the software or the use or other dealings in 
 * the software.
 * 
 */
#include <hidkeyboard.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>

#include "KeyMapping.hpp"
#include "make_html.hpp"
#include "ffs.hpp"

#if CFG_TUD_HID

#define LED_PIN 21
Adafruit_NeoPixel pixels(1, LED_PIN);
uint32_t redColor = pixels.Color(255, 0, 0);
uint32_t greenColor = pixels.Color(0, 255, 0);
uint32_t blueColor = pixels.Color(0, 0, 255);
uint32_t staConnectColor = 0;

// APモードでの設定情報
const char* ap_ssid = "Remote_Keyboard_http_192.168.4.1";  // APモードのSSID
const char* ap_password = "";                              // APモードのパスワード

// Webサーバーのポート
AsyncWebServer server(80);  // HTTPサーバーのポート
AsyncWebSocket ws("/ws");   // WebSocketエンドポイント
HIDkeyboard dev;

void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_DATA) {
    pixels.setPixelColor(0, blueColor);
    pixels.show();
    delay(6);

    if (len == 0) {
      dev.sendKey(HID_KEY_0, MODIFIER_SHIFT);
    } else {
      String receivedChar = "";
      for (size_t i = 0; i < len; i++) {
        receivedChar += (char)data[i];
      }
      // キーマッピングテーブルを検索
      for (const auto& mapping : keyMappings) {
        if (String(mapping.keyString) == receivedChar) {
          dev.sendKey(mapping.hidKey, mapping.modifier);
          break;
        }
      }
    }
    pixels.setPixelColor(0, staConnectColor);
    pixels.show();
  }
}

void setup() {
  //Serial.begin(115200);  // シリアル通信を初期化

  //LED初期化
  pixels.begin();
  pixels.setPixelColor(0, redColor);
  pixels.show();

  //SPIFFS初期化
  FFS_begin();

  // WiFiモードをAP+STAに設定
  WiFi.mode(WIFI_AP_STA);
  String ssid = SSID_Read();
  String password = PASSWORD_Read();
  WiFi.begin(ssid.c_str(), password.c_str());  // WiFi接続を開始

  int cnt = 0;
  staConnectColor = greenColor;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    cnt += 1;
    if (cnt == 100) {
      staConnectColor = redColor;
      break;
    }
  }
  
  // APモードの設定
  WiFi.softAP(ap_ssid, ap_password, 1);  // 1接続のみ許可

  pixels.setPixelColor(0, staConnectColor);
  pixels.show();

  // WebSocketのイベントハンドラを設定
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // HTTPリクエストのハンドラを設定
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    // クライアントのIPアドレスを取得
    IPAddress clientIP = request->client()->remoteIP();

    // ESP32のAPモードのIPアドレスを取得
    IPAddress apIP = WiFi.softAPIP();  // 通常は192.168.4.1
    IPAddress staIP = WiFi.localIP();

    // クライアントがAP経由かSTA経由かを判定
    String serverIP = staIP.toString();
    if (clientIP[0] == apIP[0] && clientIP[1] == apIP[1] && clientIP[2] == apIP[2]) {
      serverIP = apIP.toString();
    }

    // レスポンスのHTMLを作成
    String htmlResponse = RootHtml(WiFi.SSID(), serverIP, staIP.toString());
    request->send_P(200, "text/html; charset=utf-8", htmlResponse.c_str());    
  });


  // フォームから送信されたSSIDとパスワードを受け取るエンドポイントを追加
  server.on("/set", HTTP_POST, [](AsyncWebServerRequest* request) {
    String newSSID;
    String newPassword;

    // SSIDとパスワードの取得
    if (request->hasParam("ssid", true)) {
      newSSID = request->getParam("ssid", true)->value();
    } else {
      newSSID = "x";
    }

    if (request->hasParam("password", true)) {
      newPassword = request->getParam("password", true)->value();
    } else {
      newPassword = "x";
    }

    //ファイルに保存
    SSID_Write(newSSID);
    PASSWORD_Write(newPassword);

    String htmlResponse = Root_SetHtml(newSSID);
    AsyncWebServerResponse* response = request->beginResponse(200, "text/html", htmlResponse);
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    request->send(response);
  });

  server.begin();  // HTTPサーバーを開始
  dev.begin();
}

void loop() {
  ws.cleanupClients();  // WebSocketのクライアントのクリーンアップ
}
#endif