
#if !defined(USE_TEENSY) && !defined(USE_PICO)
void usbMidiSendTwoByteMessage(uint8_t b1, uint8_t b2) {};
void usbMidiSendThreeByteMessage(uint8_t b1, uint8_t b2, uint8_t b3) {};
void usbMidiSendRTMessage(uint8_t b) {};
void usbMidiHandleSysEx(const uint8_t *data, uint16_t length, bool complete) {};
void usbMidiInit() {};
void usbMidiUpdate() {};
#else

void usbMidiSendTwoByteMessage(uint8_t b1, uint8_t b2)
{
    uint8_t stat = b1 & 0xf0;
    uint8_t chan = (b1 & 0x0f) + 1;
    if (stat == 0xC0)
    {
        usbMIDI.sendProgramChange(b2, chan);
    }
    else if (stat == 0xD0)
    {
        usbMIDI.sendAfterTouch(b2, chan);
    }
}

void usbMidiSendThreeByteMessage(uint8_t b1, uint8_t b2, uint8_t b3)
{
    uint8_t channel = (b1 & 0x0F) + 1;

    switch (midiData[0] & 0xF0)
    {
    case 0x80:
        usbMIDI.sendNoteOff(b2, b3, channel);
#ifndef USE_PICO
        usbMIDI.send_now();
#endif
        break;
    case 0x90:
        usbMIDI.sendNoteOn(b2, b3, channel);
#ifndef USE_PICO
        usbMIDI.send_now();
#endif
        break;
    case 0xA0:
#ifdef USE_TEENSY
        usbMIDI.sendPolyPressure(b2, b3, channel);
#else
        usbMIDI.sendAfterTouch((midi::DataByte)b2, (midi::DataByte)b3, (midi::Channel)channel);
#endif
        break;
    case 0xB0:
        usbMIDI.sendControlChange(b2, b3, channel);
#ifndef USE_PICO
        usbMIDI.send_now();
#endif
        break;
    case 0xE0:
        unsigned short v = (unsigned short)b3;
        v <<= 7;
        v |= (unsigned short)b2;
        usbMIDI.sendPitchBend(v, channel);
        break;
    }
}

void usbMidiSendRTMessage(uint8_t b)
{
#ifdef USE_PICO
    usbMIDI.sendRealTime((midi::MidiType)b);
#else
    usbMIDI.sendRealTime(b);
#endif
}

void usbMidiUpdate()
{
    usbMIDI.read();
}

void usbMidiHandleSysEx(const uint8_t *data, uint16_t length, bool complete)
{
    if (sysexPosition + length >= longestSysexMessage || (length < 3 && complete))
    {
        // wrapped!
        sysexPosition = 0;
        return;
    }

    if (sysexPosition == 0 && complete)
    {
        memcpy(&sysexData[0], &data[1], length - 2);
        sysexPosition += length - 2;
    }
    else if (sysexPosition == 0 && !complete)
    {
        memcpy(&sysexData[0], &data[1], length - 1);
        sysexPosition += length - 1;
    }
    else if (!complete)
    {
        memcpy(&sysexData[sysexPosition], &data[0], length);
        sysexPosition += length;
    }
    else
    {
        memcpy(&sysexData[sysexPosition], &data[0], length - 1);
        sysexPosition += length - 1;
    }

    if (complete)
    {
        getSysexData();
    }
}

void usbMidiInit()
{
#ifdef USE_PICO
    usbMIDI.setHandleSystemExclusive((midi::SystemExclusiveCallback)usbMidiHandleSysEx);
#else
    usbMIDI.setHandleSysEx(usbMidiHandleSysEx);
#endif
}

#endif
