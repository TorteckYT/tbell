#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

#define ssid_addr 0
#define password_addr 50

#define ap_ssid "tbell"
#define ap_password "tbell_password"

AsyncWebServer server(80);
const char wifi_html[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=ru><meta charset=UTF-8><meta content="IE=edge"http-equiv=X-UA-Compatible><meta content="width=device-width,initial-scale=1"name=viewport><title>Document</title><header class=header><h1 class=title>Tbell</h1></header><div class=content><h3 class=action>Для начала работы подключитесь к wifi</h3><form action=/connect class=form><input class=input name=ssid placeholder=Ssid required> <input class=input name=password placeholder=Пароль required> <button class=submit type=submit>Подключиться</button></form></div><style>@import url(https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;800&display=swap);*{font-family:Montserrat,sans-serif;font-weight:400}body,html{margin:0;padding:0}.header{display:flex;justify-content:center;background-color:#f29327}.title{color:#734612;font-weight:800;font-size:2em;margin:17px 0}.content{display:flex;text-align:center;flex-direction:column;align-items:center;margin-top:6em;width:100%}.action{margin:0;font-weight:600;font-size:1.8rem;font-weight:600}.form{display:flex;flex-direction:column;align-items:center;margin-top:3em;max-width:90%}.input{width:20em;margin-bottom:1em;text-align:center;background-color:#ccc;border:none;padding:.3em;border-radius:10px;color:#5c5c5c;font-size:1.8em;max-width:95%}.submit{width:20em;max-width:90%;border:none;background-color:#f29327;color:#734612;font-size:2em;padding:6px 0;border-radius:10px}</style>)rawliteral";
const char reboot_html[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=ru><meta charset=UTF-8><meta content="IE=edge"http-equiv=X-UA-Compatible><meta content="width=device-width,initial-scale=1"name=viewport><title>Document</title><header class=header><h1 class=title>Tbell</h1></header><div class=content><h3 class=action>Сейчас tbell перезагрузится и подключится к wifi</h3></div><style>@import url(https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;800&display=swap);*{font-family:Montserrat,sans-serif;font-weight:400}body,html{margin:0;padding:0}.header{display:flex;justify-content:center;background-color:#f29327}.title{color:#734612;font-weight:800;font-size:2em;margin:17px 0}.content{display:flex;text-align:center;flex-direction:column;align-items:center;margin-top:6em;width:100%}.action{margin:0;font-weight:600;font-size:1.8rem;font-weight:600}</style>)rawliteral";

bool reboot_render = false;

String eeprom_read(int addr) {
	String str;

	int len = EEPROM.read(addr);
	str.reserve(len);
	for (int i=0;i<len;i++) {
		str += (char)EEPROM.read(addr+1+i);
	}
	return str;
}

void eeprom_write(int addr, String str) {
	int len = str.length();
	EEPROM.write(addr, len);

	for (int i=0; i<len; i++) {
		EEPROM.write(addr+1+i, str[i]);
		EEPROM.commit();
    }
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
	Serial.begin(115200);
	EEPROM.begin(4096);

	String ssid = eeprom_read(ssid_addr);
	String password = eeprom_read(password_addr);

	Serial.print("EEPROM ssid: ");Serial.println(ssid);
	Serial.print("EEPROM password: ");Serial.println(password);

	WiFi.begin(ssid, password);
	for(int i=0;i<10;i++) {
		delay(1000);
		Serial.println(".");
	}

	if(WiFi.status() != WL_CONNECTED) {
		WiFi.softAP(ap_ssid, ap_password);
		
		server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
			request->send_P(200, "text/html", wifi_html);
		});

		server.on("/connect", HTTP_GET, [] (AsyncWebServerRequest *request) {
			String input_ssid = request->getParam("ssid")->value();
			String input_password = request->getParam("password")->value();
			
			Serial.println(input_ssid);
			Serial.println(input_password);
			
			eeprom_write(ssid_addr, input_ssid);
			eeprom_write(password_addr, input_password);

			request->redirect("/reboot");
		});

		server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
			request->send_P(200, "text/html", reboot_html);
			reboot_render = true;
		});
		server.onNotFound(notFound);
		server.begin();
	}
	else {
		Serial.println(WiFi.localIP());
	}
}

void loop() {
	if(reboot_render==true) {
		delay(10000);
		ESP.restart();
	}

}