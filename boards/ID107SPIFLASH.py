#!/bin/false
# This file is part of Espruino, a JavaScript interpreter for Microcontrollers
#
# Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# ----------------------------------------------------------------------------------------
# This file contains information for a specific board - the available pins, and where LEDs,
# Buttons, and other in-built peripherals are. It is used to build documentation as well
# as various source and header files for Espruino.
# ----------------------------------------------------------------------------------------

import pinutils;

info = {
 'name' : "ID107",
 'link' :  [ "https://github.com/abhigkar/ID107-HR-Plus-Reverse-Engineering" ],
 'espruino_page_link' : 'MDBT42Q',
  # This is the PCA10036
 'default_console' : "EV_BLUETOOTH",
# 'default_console_tx' : "D6",
 #'default_console_rx' : "D8",
 'default_console_baudrate' : "9600",
 'variables' : 2550, # How many variables are allocated for Espruino to use. RAM will be overflowed if this number is too high and code won't compile.
 'bootloader' : 1, 
 'binary_name' : 'espruino_%v_ID107.hex',
 'build' : {
   'optimizeflags' : '-Os',
   'libraries' : [
     'BLUETOOTH',
     'GRAPHICS',
   ],
   'makefile' : [
     #'DEFINES+=-DCONFIG_GPIO_AS_PINRESET', # Allow the reset pin to work
     'DEFINES+=-DBLUETOOTH_NAME_PREFIX=\'"Espruino-107"\'',
     'DEFINES+=-DBLE_HIDS_ENABLED=1',
     'DFU_SETTINGS=--sd-req 129,136 --dev-type 572 --dev-revision 0x64', ## TODO change for ID107 HR plus
     'JSMODULESOURCES+=libs/js/id107/Font6x8.min.js',
     'JSMODULESOURCES+=libs/js/id107/Font6x12.min.js',
     'JSMODULESOURCES+=libs/js/id107/Font8x12.min.js',
     'JSMODULESOURCES+=libs/js/id107/Font8x16.min.js',
     'JSMODULESOURCES+=libs/js/id107/GoodTime18x19.min.js',
     'JSMODULESOURCES+=libs/js/id107/iqs263v2.js',
     'JSMODULESOURCES+=libs/js/id107/oled.js',
     'JSMODULESOURCES+=libs/js/id107/si1142v4.js',
   ]
 }
};


chip = {
  'part' : "NRF52832",
  'family' : "NRF52",
  'package' : "QFN48",
  'ram' : 64,
  'flash' : 512,
  'speed' : 64,
  'usart' : 1,
  'spi' : 1,
  'i2c' : 1,
  'adc' : 1,
  'dac' : 0,
  'saved_code' : {
    'address' : 0x60000000, # put this in external flash
    'page_size' : 4096,
    'pages' : 256, # 1024kb - still loads left
    'flash_available' : 512 - ((28 + 8 + 2)*4) # Softdevice uses 28 pages of flash, bootloader 8, FS 2. Each page is 4 kb.
  },
};

devices = {
  'SPIFLASH' : {
            'pin_cs' : 'D28',
            'pin_sck' : 'D30',
            'pin_mosi' : 'D31', # D0
            'pin_miso' : 'D27', # D1
            'size' : 1024*1024, # 1MB
            'memmap_base' : 0x60000000 # map into the address space (in software)
          }
};

# left-right, or top-bottom order
board = {
  'left' : [ 'VDD', 'VDD', 'RESET', 'VDD','5V','GND','GND','','','D3','D4','D28','D29','D30','D31'],
  'right' : [
     'D27', 'D26', 'D2', 'GND', 'D25','D24','D23', 'D22','D20','D19','',
     'D18','D17','D16','D15','D14','D13','D12','D11','',
     'D10','D9','D8','D7','D6','D5','D21','D1','D0'],
  '_notes' : {
    'D6' : "Serial console RX",
    'D8' : "Serial console TX"
  }
};
board["_css"] = """
""";

def get_pins():
  pins = pinutils.generate_pins(0,31) # 32 General Purpose I/O Pins.
  pinutils.findpin(pins, "PD0", True)["functions"]["XL1"]=0;
  pinutils.findpin(pins, "PD1", True)["functions"]["XL2"]=0;
  pinutils.findpin(pins, "PD2", True)["functions"]["ADC1_IN0"]=0;
  pinutils.findpin(pins, "PD3", True)["functions"]["ADC1_IN1"]=0;
  pinutils.findpin(pins, "PD4", True)["functions"]["ADC1_IN2"]=0;
  pinutils.findpin(pins, "PD5", True)["functions"]["ADC1_IN3"]=0;
  pinutils.findpin(pins, "PD28", True)["functions"]["ADC1_IN4"]=0;
  pinutils.findpin(pins, "PD29", True)["functions"]["ADC1_IN5"]=0;
  pinutils.findpin(pins, "PD30", True)["functions"]["ADC1_IN6"]=0;
  pinutils.findpin(pins, "PD31", True)["functions"]["ADC1_IN7"]=0;

  # everything is non-5v tolerant
  for pin in pins:
    pin["functions"]["3.3"]=0;
  #The boot/reset button will function as a reset button in normal operation. Pin reset on PD21 needs to be enabled on the nRF52832 device for this to work.
  return pins