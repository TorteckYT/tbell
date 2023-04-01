#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

#define ssid_addr 0
#define password_addr 50

#define ap_ssid "tbell"
#define ap_password "tbell_password"

#define enc_addr 150

#define change_password "tbell_password"

#define rellay 2

String lastzv = "incorrect:time";

String daysOfTheWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DynamicJsonDocument enc_json(3072);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

AsyncWebServer server(5000);
const char wifi_html[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=ru><meta charset=UTF-8><meta content="IE=edge"http-equiv=X-UA-Compatible><meta content="width=device-width,initial-scale=1"name=viewport><title>Tbell</title><header class=header><h1 class=title>Tbell</h1></header><div class=content><h3 class=action>Для начала работы подключитесь к wifi</h3><form action=/connect class=form><input class=input name=ssid placeholder=Ssid required> <input class=input name=password placeholder=Пароль required> <button class=submit type=submit>Подключиться</button></form></div><style>@import url(https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;800&display=swap);*{font-family:Montserrat,sans-serif;font-weight:400}body,html{margin:0;padding:0}.header{display:flex;justify-content:center;background-color:#f29327}.title{color:#734612;font-weight:800;font-size:2em;margin:17px 0}.content{display:flex;text-align:center;flex-direction:column;align-items:center;margin-top:6em;width:100%}.action{margin:0;font-weight:600;font-size:1.8rem;font-weight:600}.form{display:flex;flex-direction:column;align-items:center;margin-top:3em;max-width:90%}.input{width:20em;margin-bottom:1em;text-align:center;background-color:#ccc;border:none;padding:.3em;border-radius:10px;color:#5c5c5c;font-size:1.8em;max-width:95%}.submit{width:20em;max-width:90%;border:none;background-color:#f29327;color:#734612;font-size:2em;padding:6px 0;border-radius:10px}</style>)rawliteral";
const char reboot_html[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=ru><meta charset=UTF-8><meta content="IE=edge"http-equiv=X-UA-Compatible><meta content="width=device-width,initial-scale=1"name=viewport><title>Tbell</title><header class=header><h1 class=title>Tbell</h1></header><div class=content><h3 class=action>Сейчас tbell перезагрузится и подключится к wifi</h3></div><style>@import url(https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;800&display=swap);*{font-family:Montserrat,sans-serif;font-weight:400}body,html{margin:0;padding:0}.header{display:flex;justify-content:center;background-color:#f29327}.title{color:#734612;font-weight:800;font-size:2em;margin:17px 0}.content{display:flex;text-align:center;flex-direction:column;align-items:center;margin-top:6em;width:100%}.action{margin:0;font-weight:600;font-size:1.8rem;font-weight:600}</style>)rawliteral";
const char schedule_html[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=ru><meta charset=UTF-8><meta content="IE=edge"http-equiv=X-UA-Compatible><meta content="width=device-width,initial-scale=1"name=viewport><title>Tbell</title><header class=header><h1 class=title>Tbell</h1></header><div class=content><h3 class=action>Редактирование расписания</h3><form action=/change class=form><input class=input name=enc placeholder=Строка-расписание required> <input class=input name=password placeholder=Пароль required> <button class=submit type=submit>Сохранить</button></form></div><style>@import url(https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;800&display=swap);*{font-family:Montserrat,sans-serif;font-weight:400}body,html{margin:0;padding:0}.header{display:flex;justify-content:center;background-color:#f29327}.title{color:#734612;font-weight:800;font-size:2em;margin:17px 0}.content{display:flex;text-align:center;flex-direction:column;align-items:center;margin-top:6em;width:100%}.action{margin:0;font-weight:600;font-size:1.8rem;font-weight:600}.form{display:flex;flex-direction:column;align-items:center;margin-top:3em;max-width:90%}.input{width:20em;margin-bottom:1em;text-align:center;background-color:#ccc;border:none;padding:.3em;border-radius:10px;color:#5c5c5c;font-size:1.8em;max-width:95%}.submit{width:20em;max-width:90%;border:none;background-color:#f29327;color:#734612;font-size:2em;padding:6px 0;border-radius:10px}</style>)rawliteral";
const char success[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=ru><meta charset=UTF-8><meta content="IE=edge"http-equiv=X-UA-Compatible><meta content="width=device-width,initial-scale=1"name=viewport><title>Document</title><header class=header><h1 class=title>Tbell</h1></header><div class=content><h3 class=action>Успешно! Сейчас tbell перезагрузится и начнёт работу по заданному расписанию.</h3></div><style>@import url(https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;800&display=swap);*{font-family:Montserrat,sans-serif;font-weight:400}body,html{margin:0;padding:0}.header{display:flex;justify-content:center;background-color:#f29327}.title{color:#734612;font-weight:800;font-size:2em;margin:17px 0}.content{display:flex;text-align:center;flex-direction:column;align-items:center;margin-top:6em;width:100%}.action{margin:0;font-weight:600;font-size:1.8rem;font-weight:600}</style>)rawliteral";
const char unsuccess[] PROGMEM = R"rawliteral(<!doctypehtml><html lang=ru><meta charset=UTF-8><meta content="IE=edge"http-equiv=X-UA-Compatible><meta content="width=device-width,initial-scale=1"name=viewport><title>Document</title><header class=header><h1 class=title>Tbell</h1></header><div class=content><h3 class=action>Пароль не верен! Попробуйте ещё раз.</h3></div><style>@import url(https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;800&display=swap);*{font-family:Montserrat,sans-serif;font-weight:400}body,html{margin:0;padding:0}.header{display:flex;justify-content:center;background-color:#f29327}.title{color:#734612;font-weight:800;font-size:2em;margin:17px 0}.content{display:flex;text-align:center;flex-direction:column;align-items:center;margin-top:6em;width:100%}.action{margin:0;font-weight:600;font-size:1.8rem;font-weight:600}</style>)rawliteral";

bool rbt = false;

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
	pinMode(rellay, OUTPUT);
	digitalWrite(rellay, LOW);

	Serial.begin(115200);
	EEPROM.begin(4096);

	String ssid = eeprom_read(ssid_addr);
	String password = eeprom_read(password_addr);
	
	EepromStream eepromStream(enc_addr, 3072);
	deserializeJson(enc_json, eepromStream);

	String senc_json = enc_json.as<String>();

	Serial.print("EEPROM ssid: ");Serial.println(ssid);
	Serial.print("EEPROM password: ");Serial.println(password);
	Serial.print("EEPROM enc: ");Serial.println(senc_json.c_str());

	WiFi.begin(ssid, password);
	for(int i=0;i<10;i++) {
		delay(1000);
		Serial.println(".");
	}

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		if(WiFi.status() != WL_CONNECTED) {request->send_P(200, "text/html", wifi_html);}
		else {request->send_P(200, "text/html", schedule_html);}
	});

	server.on("/change", HTTP_GET, [] (AsyncWebServerRequest *request) {
		if(WiFi.status() != WL_CONNECTED) {request->redirect("/");}
		else {
			String input_enc = request->getParam("enc")->value();
			String input_password = request->getParam("password")->value();
			
			Serial.println(input_enc);
			Serial.println(input_password);
			
			if(input_password == change_password) {
				EepromStream eepromStream(enc_addr, 3072);
				deserializeJson(enc_json, input_enc);
				serializeJson(enc_json, eepromStream);
				EEPROM.commit();

				request->send_P(200, "text/html", success);
				rbt = true;
			}
			else {
				request->send_P(200, "text/html", unsuccess);
			}
			
		}
	});

	server.begin();

	if(WiFi.status() != WL_CONNECTED) {
		WiFi.softAP(ap_ssid, ap_password);

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
			rbt = true;
		});
		server.onNotFound(notFound);
		server.begin();
	}
	else {

		Serial.println(WiFi.localIP());
		timeClient.begin();
		timeClient.setTimeOffset(10800);
	}
}

void loop() {
	if(rbt==true) {
		delay(10000);
		ESP.restart();
	}

	if(WiFi.status() == WL_CONNECTED and enc_json != NULL) {
		timeClient.update();
		
		String cur = timeClient.getFormattedTime(); 
		cur.remove(cur.length()-1);
		cur.remove(cur.length()-1);
		cur.remove(cur.length()-1);

		if(cur[0]=='0') {cur.remove(0);}

		Serial.println(cur);

		for(int i=0;i<(int)(enc_json[daysOfTheWeek[timeClient.getDay()]].size());i++) {
			if(((String)(enc_json[daysOfTheWeek[timeClient.getDay()]])[i])==cur and ((String)(enc_json[daysOfTheWeek[timeClient.getDay()]])[i])!=lastzv) {
				lastzv = cur;
				Serial.write("bzbzbzbzb");
				digitalWrite(rellay, HIGH);
				delay(1000);
				digitalWrite(rellay, LOW);
			}
		}
		delay(1000);
	}
	else {
		ESP.restart();
	}
}