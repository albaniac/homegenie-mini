/*
 * HomeGenie-Mini (c) 2018-2019 G-Labs
 *
 *
 * This file is part of HomeGenie-Mini (HGM).
 *
 *  HomeGenie-Mini is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HomeGenie-Mini is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HomeGenie-Mini.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Authors:
 * - Generoso Martello <gene@homegenie.it>
 *
 *
 * Releases:
 * - 2019-01-10 Initial release
 *
 */

#include "HomeGenie.h"

#include <service/api/HomeGenieHandler.h>
#include <service/api/X10Handler.h>

namespace Service {

    API::HomeGenieHandler homeGenieHandler;
    API::X10Handler x10Handler;

    HomeGenie::HomeGenie() {
        setLoopInterval(20L);
        eventRouter.withNetManager(netManager);
    }

    void HomeGenie::begin() {
        netManager.begin();
        netManager.getHttpServer().addHandler(this);
        ioManager.begin();
        ioManager.setOnEventCallback(this);
    }

    void HomeGenie::loop() {
        Logger::verbose(":%s loop() >> BEGIN", HOMEGENIEMINI_NS_PREFIX);

        // HomeGenie-Mini Terminal CLI
        if(Serial.available() > 0) {
            String cmd = Serial.readStringUntil('\n');
            auto apiCommand = APIRequest::parse(cmd);
            // TODO: implement API commands from console input as well
            //      - see `HomeGenie::api(...)` method
        }

        Logger::verbose(":%s loop() << END", HOMEGENIEMINI_NS_PREFIX);
    }

    NetManager& HomeGenie::getNetManager() {
        return netManager;
    }
    IOManager& HomeGenie::getIOManager() {
        return ioManager;
    }
    EventRouter &HomeGenie::getEventRouter() {
        return eventRouter;
    }


    // BEGIN IIOEventSender interface methods
    void HomeGenie::onIOEvent(IIOEventSender *sender, const unsigned char *eventPath, void *eventData, IOEventDataType dataType) {
        String domain = String((char*)sender->getDomain());
        String address = String((char*)sender->getAddress());
        String event = String((char*)eventPath);
        Logger::trace(":%s [IOManager::IOEvent] >> [domain '%s' address '%s' event '%s']", HOMEGENIEMINI_NS_PREFIX, domain.c_str(), address.c_str(), event.c_str());
        if (domain == (IOEventDomains::HomeAutomation_HomeGenie)) {

            homeGenieHandler.handleEvent(*this, sender, eventPath, eventData, dataType);

        } else if (domain == (IOEventDomains::HomeAutomation_X10)) {

            x10Handler.handleEvent(*this, sender, eventPath, eventData, dataType);

        }
    }
    // END IIOEventSender

    // BEGIN RequestHandler interface methods
    bool HomeGenie::canHandle(HTTPMethod method, String uri) {
        return uri != NULL && uri.startsWith("/api/");
    }
    bool HomeGenie::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
        auto command = APIRequest::parse(requestUri);
        if (api(&command, server)) {
            server.send(200, "application/json", command.Response);
        } else {
            server.send(400, "application/json", command.Response);
        }
        return true;
    }
    // END RequestHandler interface methods


    String HomeGenie::createModuleParameter(const char* name, const char *value, const char *timestamp) {
        static const char *parameterTemplate = R"({
    "Name": "%s",
    "Value": "%s",
    "Description": "%s",
    "FieldType": "%s",
    "UpdateTime": "%s"
  })";
        ssize_t size = snprintf(NULL, 0, parameterTemplate,
                                name, value, "", "", timestamp
                                )+1;
        char* parameterJson = (char*)malloc(size);
        snprintf(parameterJson, size, parameterTemplate,
                 name, value, "", "", timestamp
        );
        auto p = String(parameterJson);
        free(parameterJson);
        return p;
    }

    String HomeGenie::createModule(const char* domain, const char *address, const char *name, const char* description, const char *deviceType, const char *parameters) {
        static const char* moduleTemplate = R"({
  "Name": "%s",
  "Description": "%s",
  "DeviceType": "%s",
  "Domain": "%s",
  "Address": "%s",
  "Properties": [%s],
  "RoutingNode": ""
})";
        // TODO: WARNING removing "RoutingNode" property will break HomeGenie plus client compatibility

        ssize_t size = snprintf(NULL, 0, moduleTemplate,
                                name, description, deviceType,
                                domain, address,
                                parameters
                                )+1;
        char* moduleJson = (char*)malloc(size);
        snprintf(moduleJson, size, moduleTemplate,
                 name, description, deviceType,
                 domain, address,
                 parameters
        );
        auto m = String(moduleJson);
        free(moduleJson);
        return m;
    }

    String HomeGenie::getBuiltinModuleJSON() {
        auto currentTime = NetManager::getTimeClient().getFormattedDate();
        auto lightSensor = getIOManager().getLightSensor();
        auto temperatureSensor = getIOManager().getTemperatureSensor();
        auto paramLuminance = HomeGenie::createModuleParameter("Sensor.Luminance", String(lightSensor.getLightLevel()).c_str(), currentTime.c_str());
        auto paramTemperature = HomeGenie::createModuleParameter("Sensor.Temperature", String(temperatureSensor.getTemperature()).c_str(), currentTime.c_str());
        return String(HomeGenie::createModule(IOEventDomains::HomeAutomation_HomeGenie, HOMEGENIE_BUILTIN_MODULE_ADDRESS,
                                              "HG-Mini", "HomeGenie Mini node", "Sensor",
                                              (paramLuminance+","+paramTemperature).c_str()));
    }

    bool HomeGenie::api(APIRequest *request, ESP8266WebServer &server) {
        if (request->Domain == (IOEventDomains::HomeAutomation_X10)) {

            return x10Handler.handleRequest(*this, request, server);

        } else if (request->Domain == (IOEventDomains::HomeAutomation_HomeGenie)) {

            return homeGenieHandler.handleRequest(*this, request, server);

        } else return false;
    }

    int HomeGenie::writeModuleJSON(ESP8266WebServer *server, String &domain, String &address) {
        auto outputCallback = APIHandlerOutputCallback(server);
        if (domain == (IOEventDomains::HomeAutomation_HomeGenie) && address == HOMEGENIE_BUILTIN_MODULE_ADDRESS) {
            auto module = getBuiltinModuleJSON();
            outputCallback.write(module);
            // TODO: check out if `module` gets actually disposed
        } else if (domain == (IOEventDomains::HomeAutomation_X10)) {
            x10Handler.getModuleJSON(&outputCallback, domain, address);
        }
        return outputCallback.outputLength;
    }

    int HomeGenie::writeModuleListJSON(ESP8266WebServer *server) {
        auto outputCallback = APIHandlerOutputCallback(server);
        String line = "[";
        outputCallback.write(line);
        // HomeAutomation.HomeGenie
        line = getBuiltinModuleJSON();
        outputCallback.write(line);
        line = ",\n";
        outputCallback.write(line);
        // HomeGenie Mini P1Port modules (GPIO D5, D6, D6, D8)
        homeGenieHandler.getModuleListJSON(&outputCallback);
        line = ",\n";
        outputCallback.write(line);
        // HomeAutomation.X10
        x10Handler.getModuleListJSON(&outputCallback);
        line = "]\n";
        outputCallback.write(line);
        return outputCallback.outputLength;
    }

    int HomeGenie::writeGroupListJSON(ESP8266WebServer *server) {
        auto outputCallback = APIHandlerOutputCallback(server);
        x10Handler.getGroupListJSON(&outputCallback);
        return outputCallback.outputLength;
    }

}
