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
 * - 2019-01-28 Initial release
 *
 */

#ifndef HOMEGENIE_MINI_HOMEGENIEHANDLER_H
#define HOMEGENIE_MINI_HOMEGENIEHANDLER_H

#include <Utility.h>
#include <io/IOEventDomains.h>
#include <service/HomeGenie.h>
#include <service/api/APIHandler.h>

namespace Service { namespace API {

    class APIHandlerOutputCallback : public OutputStreamCallback {
        ESP8266WebServer *server;
    public:
        int outputLength = 0;
        APIHandlerOutputCallback(ESP8266WebServer *server) {
            this->server = server;
        }
        void write(String &s) {
            outputLength += s.length();
            if (server != NULL) {
                server->sendContent(s);
            }
        }
    };

    class HomeGenieHandler : public APIHandler {
    public:
        bool canHandleDomain(String &domain);
        bool handleRequest(HomeGenie &homeGenie, APIRequest *request, ESP8266WebServer &server);
        bool handleEvent(HomeGenie &homeGenie, IIOEventSender *sender, const unsigned char *eventPath, void *eventData,
                    IOEventDataType dataType);
        void getModuleJSON(OutputStreamCallback *outputCallback, String &domain, String &address);
        void getModuleListJSON(OutputStreamCallback *outputCallback);
    };

}}

#endif //HOMEGENIE_MINI_HOMEGENIEHANDLER_H
