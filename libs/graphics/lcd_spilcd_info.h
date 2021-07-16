/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2019 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Device specific information for SPI LCDs
 * ----------------------------------------------------------------------------
 */

#include "platform_config.h"

#define CMDINDEX_CMD   0
#define CMDINDEX_DELAY 1
#define CMDINDEX_DATALEN  2

#ifdef LCD_CONTROLLER_ST7735
static const char SPILCD_INIT_CODE[] = {
  // CMD,DELAY,DATA_LEN,D0,D1,D2...
  // SWRESET Software reset - but we have hardware reset
  // 0x01, 20, 0,
  // SLPOUT Leave sleep mode
  0x11, 100, 0,
  // FRMCTR1 , FRMCTR2 Frame Rate configuration -- Normal mode, idle
  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
   0xB1, 0, 3,  /*data*/0x05, 0x3c, 0x3c ,
   0xB2, 0, 3,  /*data*/0x05, 0x3c, 0x3c,
  // FRMCTR3 Frame Rate configureation -- partial mode
   0xB3, 0, 6, /*data*/0x05, 0x3c, 0x3c , 0x05, 0x3c, 0x3c ,
  // INVCTR Display inversion (no inversion)
   0xB4, 0, 1, /*data*/0x03 ,
  // PWCTR1 Power control -4.6V, Auto mode
   0xC0, 0, 3,  /*data*/0xe0, 0x00, 0x07,
  // PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
   0xC1, 0, 1,  /*data*/0xC5,
  // PWCTR3 Power control , opamp current smal, boost frequency
   0xC2, 0, 2,  /*data*/0x0A, 0x00 ,
  // PWRCTR5 , VMCTR1 Power control
   0xC4, 0, 2,  /*data*/0x8D, 0xEE,
   0xC5, 0, 1,  /*data*/0x03 ,
   0xC7, 0, 1, /*data*/0x76 ,
  // MADCTL row address/col address, bottom to top refesh (10.1.27)
   0x36, 0, 1, /*data*/0xC8,
  // COLMOD, Color mode 12 bit
   0x3A, 0, 1, /*data*/0x03,
  // GMCTRP1 Gamma correction
   0xE0, 0, 16, /*data*/0x27,0xe,7,4,0x11,0xb,6,0xc,0xe,0x14,0x1b,0x3e,6,0x25,7,0x1f ,
  // GMCTRP2 Gamma Polarity correction
   0xE1, 0, 16, /*data*/0x27,0xe,7,4,0x11,0xb,6,0xc,0xe,0x14,0x1b,0x3e,0x30,0x25,7,0x1f ,
  // DISPON Display on
   0x29, 120, 0,
};
const unsigned char SPILCD_CMD_WINDOW_X = 0x2A;
const unsigned char SPILCD_CMD_WINDOW_Y = 0x2B;
const unsigned char SPILCD_CMD_DATA = 0x2C;
#endif
