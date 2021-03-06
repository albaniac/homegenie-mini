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

#ifndef HOMEGENIE_MINI_APIHANDLER_H
#define HOMEGENIE_MINI_APIHANDLER_H

#include <Arduino.h>

#include <io/IOEvent.h>
#include <service/HomeGenie.h>

namespace Service { namespace API {

    using namespace IO;
    using namespace Service;

    class OutputStreamCallback {
    public:
        virtual void write(String &s) = 0;
    };

    class APIHandler {
        virtual bool canHandleDomain(String &domain) = 0;
        virtual bool handleRequest(HomeGenie &homeGenie, APIRequest *request, ESP8266WebServer &server) = 0;
        virtual bool handleEvent(HomeGenie &homeGenie, IIOEventSender *sender, const unsigned char *eventPath, void *eventData, IOEventDataType dataType) = 0;
        virtual void getModuleJSON(OutputStreamCallback *outputCallback, String &domain, String &address) = 0;
        virtual void getModuleListJSON(OutputStreamCallback *outputCallback) = 0;
    };

}}

#endif //HOMEGENIE_MINI_APIHANDLER_H
