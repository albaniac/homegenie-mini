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

#include "RFTransmitterConfig.h"

namespace IO { namespace X10 {

    RFTransmitterConfig::RFTransmitterConfig()
    {
        pin = D2; // 4
        sendRepeat = 3;

        startBustLong = 9000;
        startBustShort = 4500;

        bitLong = 1110;
        bitShort = 555;

        packetGap = 29000;
    }
    RFTransmitterConfig::RFTransmitterConfig(uint8_t pin) : RFTransmitterConfig()
    {
        this->pin = pin;
    }
    uint8_t RFTransmitterConfig::getPin() { return this->pin; };
    uint8_t RFTransmitterConfig::getSendRepeat() { return this->sendRepeat; };
    uint16_t RFTransmitterConfig::getStartBustLong() { return this->startBustLong; };
    uint16_t RFTransmitterConfig::getStartBustShort() { return this->startBustShort; };

    uint16_t RFTransmitterConfig::getBitLong() { return this->bitLong; };
    uint16_t RFTransmitterConfig::getBitShort() { return this->bitShort; };

    uint16_t RFTransmitterConfig::getPacketGap() { return this->packetGap; };

}} // ns