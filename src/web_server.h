#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>

// Global web server object
extern WebServer server;

// Web server functions
void setupWebServer();
void handleWebClients();

// HTTP route handlers
void handleRoot();
void handleCSS();
void handleJS();
void handleUpdate();
void handleToggle();
void handleTestColor();
void handleGetConfig();

#endif