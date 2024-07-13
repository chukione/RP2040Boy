/**************************************************************************
 * Name:    Timothy Lamb                                                  *
 * Email:   trash80@gmail.com                                             *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

void modeMidiGbSetup()
{
#ifndef USE_PICO
  digitalWrite(pinStatusLed, LOW);
#endif
  pinMode(pinGBClock, OUTPUT);
  digitalWrite(pinGBClock, HIGH);

#if defined(USE_TEENSY)
  usbMIDI.setHandleRealTimeSystem(NULL);
#endif

  blinkMaxCount = 1000;
  modeMidiGb();
}

void modeMidiGb()
{
  boolean sendByte = false;
  byte message_type;
  byte channel;
  byte first_byte;
  byte second_byte;
  while (1)
  { // Loop foreverrrr
    modeMidiGbUsbMidiReceive();

    if (serial->available())
    {
      midi_mode = 0;                     // If MIDI is sending
      incomingMidiByte = serial->read(); // Get the byte sent from MIDI

      if (!checkForProgrammerSysex(incomingMidiByte) && !usbMode)
        serial->write(incomingMidiByte); // Echo the Byte to MIDI Output

      if (incomingMidiByte & 0x80)
      {
        switch (incomingMidiByte & 0xF0)
        {
        case 0xF0: // system message type
          midiValueMode = false;
          break;
        default:
          sendByte = false;
          midiStatusChannel = incomingMidiByte & 0x0F; // channel
          midiStatusType = incomingMidiByte & 0xF0;
          if (midiStatusChannel == memory[MEM_MGB_CH])
          {
            midiData[0] = midiStatusType;
            sendByte = true;
          }
          else if (midiStatusChannel == memory[MEM_MGB_CH + 1])
          {
            midiData[0] = midiStatusType + 1;
            sendByte = true;
          }
          else if (midiStatusChannel == memory[MEM_MGB_CH + 2])
          {
            midiData[0] = midiStatusType + 2;
            sendByte = true;
          }
          else if (midiStatusChannel == memory[MEM_MGB_CH + 3])
          {
            midiData[0] = midiStatusType + 3;
            sendByte = true;
          }
          else if (midiStatusChannel == memory[MEM_MGB_CH + 4])
          {
            midiData[0] = midiStatusType + 4;
            sendByte = true;
          }
          else
          {
            midiValueMode = false;
            midiAddressMode = false;
          }
          if (sendByte)
          {
#ifndef USE_PICO
            statusLedOn();
#else
            message_type = midiStatusType;
            channel = midiStatusChannel;
#endif
            sendByteToGameboy(midiData[0]);
            delayMicroseconds(GB_MIDI_DELAY);
            midiValueMode = false;
            midiAddressMode = true;
          }
          break;
        }
      }
      else if (midiAddressMode)
      {
        midiAddressMode = false;
        midiValueMode = true;
        midiData[1] = incomingMidiByte;
        first_byte = midiData[1];
        sendByteToGameboy(midiData[1]);
        delayMicroseconds(GB_MIDI_DELAY);
      }
      else if (midiValueMode)
      {
        midiData[2] = incomingMidiByte;
        second_byte = midiData[2];
        midiAddressMode = true;
        midiValueMode = false;

        sendByteToGameboy(midiData[2]);
        delayMicroseconds(GB_MIDI_DELAY);
#ifndef USE_PICO
        statusLedOn();
        blinkLight(midiData[0], midiData[2]);
#else
        uint32_t mididata = (uint32_t)message_type | ((uint32_t)channel << 8) | ((uint32_t)first_byte << 16) | ((uint32_t)second_byte << 24);
        rp2040.fifo.push_nb(mididata);
#endif
      }
    }
    else
    {
      setMode(); // Check if mode button was depressed
#ifndef USE_PICO
      updateBlinkLights();
      updateStatusLed();
#endif
    }
  }
}

/*
sendByteToGameboy does what it says. yay magic
*/
void sendByteToGameboy(byte send_byte)
{
  for (countLSDJTicks = 0; countLSDJTicks != 8; countLSDJTicks++)
  { // we are going to send 8 bits, so do a loop 8 times
    if (send_byte & 0x80)
    {
      GB_SET(0, 1, 0);
#ifdef USE_PICO
      delayMicroseconds(1);
#endif
      GB_SET(1, 1, 0);
    }
    else
    {
      GB_SET(0, 0, 0);
#ifdef USE_PICO
      delayMicroseconds(1);
#endif
      GB_SET(1, 0, 0);
    }

#if defined(F_CPU) && (F_CPU > 24000000)
    // Delays for Teensy etc where CPU speed might be clocked too fast for cable & shift register on gameboy.
    delayMicroseconds(1);
#endif
    send_byte <<= 1;
  }
}

void modeMidiGbUsbMidiReceive()
{
#if defined(USE_TEENSY)
  // #if defined(USE_TEENSY) || defined(USE_PICO)

  while (usbMIDI.read())
  {
    uint8_t ch = usbMIDI.getChannel() - 1;
    boolean send = false;
    if (ch == memory[MEM_MGB_CH])
    {
      ch = 0;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 1])
    {
      ch = 1;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 2])
    {
      ch = 2;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 3])
    {
      ch = 3;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 4])
    {
      ch = 4;
      send = true;
    }

    if (!send)
      return;
    uint8_t s;
    switch (usbMIDI.getType())
    {
    case 0x80: // note off
    case 0x90: // note on
      s = 0x90 + ch;
      if (usbMIDI.getType() == 0x80)
      {
        s = 0x80 + ch;
      }
      sendByteToGameboy(s);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData2());
      delayMicroseconds(GB_MIDI_DELAY);
      blinkLight(s, usbMIDI.getData2());
      break;
    case 0xB0: // CC
      sendByteToGameboy(0xB0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData2());
      delayMicroseconds(GB_MIDI_DELAY);
      blinkLight(0xB0 + ch, usbMIDI.getData2());
      break;
    case 0xC0: // PG
      sendByteToGameboy(0xC0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      blinkLight(0xC0 + ch, usbMIDI.getData2());
      break;
    case 0xE0: // PB
      sendByteToGameboy(0xE0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData2());
      delayMicroseconds(GB_MIDI_DELAY);
      break;
    }
#ifndef USE_PICO
    statusLedOn();
#endif
  }
#endif

#ifdef USE_LEONARDO

  midiEventPacket_t rx;
  do
  {
    rx = MidiUSB.read();
    uint8_t ch = rx.byte1 & 0x0F;
    boolean send = false;
    if (ch == memory[MEM_MGB_CH])
    {
      ch = 0;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 1])
    {
      ch = 1;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 2])
    {
      ch = 2;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 3])
    {
      ch = 3;
      send = true;
    }
    else if (ch == memory[MEM_MGB_CH + 4])
    {
      ch = 4;
      send = true;
    }
    if (!send)
      return;
    uint8_t s;
    switch (rx.header)
    {
    case 0x08: // note off
    case 0x09: // note on
      s = 0x90 + ch;
      if (rx.header == 0x08)
      {
        s = 0x80 + ch;
      }
      sendByteToGameboy(s);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(rx.byte2);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(rx.byte3);
      delayMicroseconds(GB_MIDI_DELAY);
      blinkLight(s, rx.byte2);
      break;
    case 0x0B: // CC
      sendByteToGameboy(0xB0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(rx.byte2);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(rx.byte3);
      delayMicroseconds(GB_MIDI_DELAY);
      blinkLight(0xB0 + ch, rx.byte2);
      break;
    case 0x0C: // PG
      sendByteToGameboy(0xC0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(rx.byte2);
      delayMicroseconds(GB_MIDI_DELAY);
      blinkLight(0xC0 + ch, rx.byte2);
      break;
    case 0x0E: // PB
      sendByteToGameboy(0xE0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(rx.byte2);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(rx.byte3);
      delayMicroseconds(GB_MIDI_DELAY);
      break;
    default:
      return;
    }

    statusLedOn();
  } while (rx.header != 0);
#endif
#ifdef USE_PICO
  while (usbMIDI.read())
  {
    midi_mode = 1;
    uint8_t ch = usbMIDI.getChannel() - 1;
    boolean send = false;
    if (ch <= 5)
    {
      send = true;
      digitalWrite(PIN_LED, HIGH);
    }
    if (!send)
      return;
    uint8_t s;
    switch (usbMIDI.getType())
    {
    case 0x80: // note off
    case 0x90: // note on
      s = 0x90 + ch;
      if (usbMIDI.getType() == 0x80)
      {
        s = 0x80 + ch;
      }
      sendByteToGameboy(s);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData2());
      delayMicroseconds(GB_MIDI_DELAY);
      break;
    case 0xB0: // CC
      sendByteToGameboy(0xB0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData2());
      delayMicroseconds(GB_MIDI_DELAY);
      break;
    case 0xC0: // PG
      sendByteToGameboy(0xC0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      break;
    case 0xE0: // PB
      sendByteToGameboy(0xE0 + ch);
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData1());
      delayMicroseconds(GB_MIDI_DELAY);
      sendByteToGameboy(usbMIDI.getData2());
      delayMicroseconds(GB_MIDI_DELAY);
      break;
    }
    uint32_t mididata = (uint32_t)usbMIDI.getType() | ((uint32_t)usbMIDI.getChannel() << 8) | ((uint32_t)usbMIDI.getData1() << 16) | ((uint32_t)usbMIDI.getData2() << 24);
    rp2040.fifo.push_nb(mididata);
  }

#endif
}
