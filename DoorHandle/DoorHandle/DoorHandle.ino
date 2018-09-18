/*
 * DoorHandle.ino
 *
 * Created: 7/31/2018 12:29:17 PM
 * Author: David
 */ 

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>

char* ssid     = "Marcus SacAPus";
char* password = "chenapan";
#define TEMPS_UPLOAD_OTA 10000
#define DOOR_PIN 2
#define LED_BUILTIN 1
//Server and FileSystem object
ESP8266WebServer server(302);
ESP8266WebServer httpServer(301);
ESP8266HTTPUpdateServer httpUpdater;
File fsUploadFile;

//Liste des fonctions
void connectWiFi();
void beginServer();
void handleSubmit();
String formatBytes(size_t bytes);
String getContentType(String filename);
bool handleFileRead(String path);
void handleFileUpload();
void handleFileDelete();
void handleFileCreate();
void handleFileList();
void handleON();


void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(DOOR_PIN,OUTPUT);
  digitalWrite(DOOR_PIN, HIGH);
  delay(2000);
  digitalWrite(DOOR_PIN,LOW);
  Serial.begin(115200);
  SPIFFS.begin();
    {
	    Dir dir = SPIFFS.openDir("/");
	    while (dir.next()) {
		    String fileName = dir.fileName();
		    size_t fileSize = dir.fileSize();
		    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
	    }
	    Serial.printf("\n");
    }
  
  
  connectWiFi();
  digitalWrite(DOOR_PIN, HIGH);
  delay(2000);
  digitalWrite(DOOR_PIN,LOW);
  beginServer();
  
}



void loop() {


  server.handleClient();  
  httpServer.handleClient();

}

void connectWiFi(){
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
	
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void handleLogin() {                         // If a POST request is made to URI /login
	if( ! server.hasArg("username") || ! server.hasArg("password") || server.arg("username") == NULL || server.arg("password") == NULL) { // If the POST request doesn't have username and password data
		server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
		return;
	}
	if(server.arg("username") == "Jo" && server.arg("password") == "Ma") { // If both the username and the password are correct
		
		if (!handleFileRead("/edit.htm")) {
			server.send(404, "text/plain", "FileNotFound");
		}
	} else {                                                                              // Username and password don't match
		server.send(401, "text/plain", "401: Unauthorized");
	}
}


void beginServer(){
	server.on("/login", HTTP_POST, handleLogin);
	server.on("/ON", handleON);
    server.on("/edit", HTTP_PUT, handleFileCreate);
    //delete file
    server.on("/edit", HTTP_DELETE, handleFileDelete);
    //first callback is called after the request has ended with all parsed arguments
    //second callback handles file uploads at that location
    server.on("/edit", HTTP_POST, []() {
		server.send(200, "text/plain", "");
     }, handleFileUpload);
   	
	
	server.on("/", HTTP_GET, []() {
		if (!handleFileRead("/index.htm")) {
			server.send(404, "text/plain", "FileNotFound");
	    }
	   });
  
  
  
	server.on("/Controleur", HTTP_GET, []() {
		if (!handleFileRead("/Controleur.htm")) {
			server.send(404, "text/plain", "FileNotFound");
			}
		});
	   
	server.on("/SourceDC", HTTP_GET, []() {
		if (!handleFileRead("/SourceDC.htm")) {
			server.send(404, "text/plain", "FileNotFound");
		    }
		});
		   
	server.on("/ServeurHTTP", HTTP_GET, []() {
		if (!handleFileRead("/ServeurHTTP.htm")) {
			server.send(404, "text/plain", "FileNotFound");
			}
		});
  
  
	server.on("/Tetris", HTTP_GET, []() {
		if (!handleFileRead("/Tetris.htm")) {
			server.send(404, "text/plain", "FileNotFound");
			}
		});
  
   server.on("/list", HTTP_GET, handleFileList);
   //load editor
   //server.on("/edit", HTTP_GET, []() {
	//   if (!handleFileRead("/edit.htm")) {
	//	   server.send(404, "text/plain", "FileNotFound");
	//   }
   //});
   //create file


   //called when the url is not defined here
   //use it to load content from SPIFFS
   server.onNotFound([]() {
	   if (!handleFileRead(server.uri())) {
		   server.send(404, "text/plain", "FileNotFound");
	   }
   });

   //get heap status, analog input value and all GPIO statuses in one json call
   server.on("/all", HTTP_GET, []() {
	   String json = "{";
		   json += "\"heap\":" + String(ESP.getFreeHeap());
		   json += ", \"analog\":" + String(analogRead(A0));
		   json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
	   json += "}";
	   server.send(200, "text/json", json);
	   json = String();
   });

  server.begin();
  Serial.println ( "HTTP server started" );
}



void handleON() {
 
  digitalWrite(DOOR_PIN, HIGH);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(3000);
  digitalWrite(DOOR_PIN,LOW);
  digitalWrite(LED_BUILTIN,LOW);
  
  
  if (!handleFileRead("/index.htm")) {
	server.send(404, "text/plain", "FileNotFound");
  }
 
}





String formatBytes(size_t bytes) {
	if (bytes < 1024) {
		return String(bytes) + "B";
		} else if (bytes < (1024 * 1024)) {
		return String(bytes / 1024.0) + "KB";
		} else if (bytes < (1024 * 1024 * 1024)) {
		return String(bytes / 1024.0 / 1024.0) + "MB";
		} else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	}
}

String getContentType(String filename) {
	if (server.hasArg("download")) {
		return "application/octet-stream";
		} else if (filename.endsWith(".htm")) {
		return "text/html";
		} else if (filename.endsWith(".html")) {
		return "text/html";
		} else if (filename.endsWith(".css")) {
		return "text/css";
		} else if (filename.endsWith(".js")) {
		return "application/javascript";
		} else if (filename.endsWith(".png")) {
		return "image/png";
		} else if (filename.endsWith(".gif")) {
		return "image/gif";
		} else if (filename.endsWith(".jpg")) {
		return "image/jpeg";
		} else if (filename.endsWith(".ico")) {
		return "image/x-icon";
		} else if (filename.endsWith(".xml")) {
		return "text/xml";
		} else if (filename.endsWith(".pdf")) {
		return "application/x-pdf";
		} else if (filename.endsWith(".zip")) {
		return "application/x-zip";
		} else if (filename.endsWith(".gz")) {
		return "application/x-gzip";
	}
	return "text/plain";
}

bool handleFileRead(String path) {
	Serial.println("handleFileRead: " + path);
	if (path.endsWith("/")) {
		path += "index.htm";
	}
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz)) {
			path += ".gz";
		}
		File file = SPIFFS.open(path, "r");
		server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

void handleFileUpload() {
	if (server.uri() != "/edit") {
		return;
	}
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) {
			filename = "/" + filename;
		}
		Serial.print("handleFileUpload Name: "); Serial.println(filename);
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
		} else if (upload.status == UPLOAD_FILE_WRITE) {
		//Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
		if (fsUploadFile) {
			fsUploadFile.write(upload.buf, upload.currentSize);
		}
		} else if (upload.status == UPLOAD_FILE_END) {
		if (fsUploadFile) {
			fsUploadFile.close();
		}
		Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
	}
}

void handleFileDelete() {
	if (server.args() == 0) {
		return server.send(500, "text/plain", "BAD ARGS");
	}
	String path = server.arg(0);
	Serial.println("handleFileDelete: " + path);
	if (path == "/") {
		return server.send(500, "text/plain", "BAD PATH");
	}
	if (!SPIFFS.exists(path)) {
		return server.send(404, "text/plain", "FileNotFound");
	}
	SPIFFS.remove(path);
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileCreate() {
	if (server.args() == 0) {
		return server.send(500, "text/plain", "BAD ARGS");
	}
	String path = server.arg(0);
	Serial.println("handleFileCreate: " + path);
	if (path == "/") {
		return server.send(500, "text/plain", "BAD PATH");
	}
	if (SPIFFS.exists(path)) {
		return server.send(500, "text/plain", "FILE EXISTS");
	}
	File file = SPIFFS.open(path, "w");
	if (file) {
		file.close();
		} else {
		return server.send(500, "text/plain", "CREATE FAILED");
	}
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileList() {
	if (!server.hasArg("dir")) {
		server.send(500, "text/plain", "BAD ARGS");
		return;
	}

	String path = server.arg("dir");
	Serial.println("handleFileList: " + path);
	Dir dir = SPIFFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next()) {
		File entry = dir.openFile("r");
		if (output != "[") {
			output += ',';
		}
		bool isDir = false;
		output += "{\"type\":\"";
			output += (isDir) ? "dir" : "file";
			output += "\",\"name\":\"";
			output += String(entry.name()).substring(1);
		output += "\"}";
		entry.close();
	}

	output += "]";
	server.send(200, "text/json", output);
}