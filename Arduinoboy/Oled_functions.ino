void ShowSelectedModeOled()
{
        switch (memory[MEM_MODE])
        {
        case 0:
#ifndef OLED_GFX // simple no buffer text display
                u8x8.clearLine(5);
                u8x8.clearLine(6);
                u8x8.drawString(1, 5, "LSDJ SlaveSync");
#else
#ifndef USE_PICO // 32u4 not enough flash/ram to display the logos
                u8x8_ClearLine(u8g2.getU8x8(), 5);
                u8x8_ClearLine(u8g2.getU8x8(), 6);
                u8x8_DrawString(u8g2.getU8x8(), 1, 5, "LSDJ SlaveSync");
#else            // rp2024 display the logo with text on full buffer.
                u8g2.clearBuffer();
                u8g2.drawXBMP(17, 0, lsdj_width, lsdj_height, lsdj_bits);
                u8g2.setFont(u8g2_font_profont22_mf);
                u8g2.drawStr(10, 62, "SlaveSync");
                u8g2.sendBuffer();
#endif
#endif

                break;
        case 1:
#ifndef OLED_GFX
                u8x8.clearLine(5);
                u8x8.clearLine(6);
                u8x8.drawString(1, 5, "LSDJ MasterSync");
#else
#ifndef USE_PICO
                u8x8_ClearLine(u8g2.getU8x8(), 5);
                u8x8_ClearLine(u8g2.getU8x8(), 6);
                u8x8_DrawString(u8g2.getU8x8(), 1, 5, "LSDJ MasterSync");
#else
                u8g2.clearBuffer();
                u8g2.drawXBMP(17, 0, lsdj_width, lsdj_height, lsdj_bits);
                u8g2.setFont(u8g2_font_profont22_mf);
                u8g2.drawStr(5, 62, "MasterSync");
                u8g2.sendBuffer();
#endif
#endif
                break;
        case 2:
#ifndef OLED_GFX
                u8x8.clearLine(5);
                u8x8.clearLine(6);
                u8x8.drawString(1, 5, "LSDJ Keyboard");
#else
#ifndef USE_PICO
                u8x8_ClearLine(u8g2.getU8x8(), 5);
                u8x8_ClearLine(u8g2.getU8x8(), 6);
                u8x8_DrawString(u8g2.getU8x8(), 1, 5, "LSDJ Keyboard");
#else
                u8g2.clearBuffer();
                u8g2.drawXBMP(17, 0, lsdj_width, lsdj_height, lsdj_bits);
                u8g2.setFont(u8g2_font_profont22_mf);
                u8g2.drawStr(15, 62, "Keyboard");
                u8g2.sendBuffer();
#endif
#endif
                break;
        case 3:
#ifndef OLED_GFX
                u8x8.clearLine(5);
                u8x8.clearLine(6);
                u8x8.drawString(4, 5, "NANOLOOP");
#else
#ifndef USE_PICO
                u8x8_ClearLine(u8g2.getU8x8(), 5);
                u8x8_ClearLine(u8g2.getU8x8(), 6);
                u8x8_DrawString(u8g2.getU8x8(), 4, 5, "NANOLOOP");
#else
                u8g2.clearBuffer();
                u8g2.drawXBMP(28, 5, nanoloop_width, nanoloop_height, nanoloop_bits);
                u8g2.setFont(u8g2_font_profont22_mf);
                u8g2.drawStr(16, 62, "nanoloop");
                u8g2.sendBuffer();

#endif
#endif
                break;
        case 4:
#ifndef OLED_GFX
                u8x8.clearLine(5);
                u8x8.clearLine(6);
                u8x8.drawString(6, 5, "mGB");
#else
#ifndef USE_PICO
                u8x8_ClearLine(u8g2.getU8x8(), 5);
                u8x8_ClearLine(u8g2.getU8x8(), 6);
                u8x8_DrawString(u8g2.getU8x8(), 6, 5, "mGB");
#else
                u8g2.clearBuffer();
                // u8g2.setFont(u8g2_font_callite24_tr);
                // u8g2.drawStr(40,20,"mGB");
                u8g2.drawXBMP(9, 10, mgb_width, mgb_height, mgb_bits);
                // u8g2.setFont(u8g2_font_profont22_mf);
                // u8g2.drawStr(10, 62, "mG");
                u8g2.sendBuffer();
#endif
#endif
                break;
        case 5:
#ifndef OLED_GFX
                u8x8.clearLine(5);
                u8x8.clearLine(6);
                u8x8.drawString(4, 5, "LSDJ Map");
#else
#ifndef USE_PICO
                u8x8_ClearLine(u8g2.getU8x8(), 5);
                u8x8_ClearLine(u8g2.getU8x8(), 6);
                u8x8_DrawString(u8g2.getU8x8(), 4, 5, "LSDJ Map");
#else
                u8g2.clearBuffer();
                u8g2.drawXBMP(17, 0, lsdj_width, lsdj_height, lsdj_bits);
                u8g2.setFont(u8g2_font_profont22_mf);
                u8g2.drawStr(45, 62, "Map");
                u8g2.sendBuffer();
#endif
#endif
                break;
        case 6:
#ifndef OLED_GFX
                u8x8.clearLine(5);
                u8x8.clearLine(6);
                u8x8.drawString(4, 5, "MIDI OUT");
#else
#ifndef USE_PICO
                u8x8_ClearLine(u8g2.getU8x8(), 5);
                u8x8_ClearLine(u8g2.getU8x8(), 6);
                u8x8_DrawString(u8g2.getU8x8(), 4, 5, "MIDI OUT");
#else
                u8g2.clearBuffer();
                u8g2.drawXBMP(17, 0, lsdj_width, lsdj_height, lsdj_bits);
                u8g2.setFont(u8g2_font_profont22_mf);
                u8g2.drawStr(17, 62, "MIDI Out");
                u8g2.sendBuffer();
#endif
#endif
                break;
        }
        lastMode = memory[MEM_MODE];
        delay(600);
}