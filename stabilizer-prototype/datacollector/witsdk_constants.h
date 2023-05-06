#pragma once

namespace witsdk {

    inline const int REGSIZE = 0x90;

    inline const int SAVE = 0x00;
    inline const int CALSW = 0x01;
    inline const int RSW = 0x02;
    inline const int RRATE = 0x03;
    inline const int BAUD = 0x04;
    inline const int AXOFFSET = 0x05;
    inline const int AYOFFSET = 0x06;
    inline const int AZOFFSET = 0x07;
    inline const int GXOFFSET = 0x08;
    inline const int GYOFFSET = 0x09;
    inline const int GZOFFSET = 0x0a;
    inline const int HXOFFSET = 0x0b;
    inline const int HYOFFSET = 0x0c;
    inline const int HZOFFSET = 0x0d;
    inline const int D0MODE = 0x0e;
    inline const int D1MODE = 0x0f;
    inline const int D2MODE = 0x10;
    inline const int D3MODE = 0x11;
    inline const int D0PWMH = 0x12;
    inline const int D1PWMH = 0x13;
    inline const int D2PWMH = 0x14;
    inline const int D3PWMH = 0x15;
    inline const int D0PWMT = 0x16;
    inline const int D1PWMT = 0x17;
    inline const int D2PWMT = 0x18;
    inline const int D3PWMT = 0x19;
    inline const int IICADDR = 0x1a;
    inline const int LEDOFF = 0x1b;
    inline const int MAGRANGX = 0x1c;
    inline const int MAGRANGY = 0x1d;
    inline const int MAGRANGZ = 0x1e;
    inline const int BANDWIDTH = 0x1f;
    inline const int GYRORANGE = 0x20;
    inline const int ACCRANGE = 0x21;
    inline const int SLEEP = 0x22;
    inline const int ORIENT = 0x23;
    inline const int AXIS6 = 0x24;
    inline const int FILTK = 0x25;
    inline const int GPSBAUD = 0x26;
    inline const int READADDR = 0x27;
    inline const int BWSCALE = 0x28;
    inline const int MOVETHR = 0x28;
    inline const int MOVESTA = 0x29;
    inline const int ACCFILT = 0x2A;
    inline const int GYROFILT = 0x2b;
    inline const int MAGFILT = 0x2c;
    inline const int POWONSEND = 0x2d;
    inline const int VERSION = 0x2e;
    inline const int CCBW = 0x2f;
    inline const int YYMM = 0x30;
    inline const int DDHH = 0x31;
    inline const int MMSS = 0x32;
    inline const int MS = 0x33;
    inline const int AX = 0x34;
    inline const int AY = 0x35;
    inline const int AZ = 0x36;
    inline const int GX = 0x37;
    inline const int GY = 0x38;
    inline const int GZ = 0x39;
    inline const int HX = 0x3a;
    inline const int HY = 0x3b;
    inline const int HZ = 0x3c;
    inline const int Roll = 0x3d;
    inline const int Pitch = 0x3e;
    inline const int Yaw = 0x3f;
    inline const int TEMP = 0x40;

/* High precision */
    inline const int LRoll = 0x3d;
    inline const int HRoll = 0x3e;
    inline const int LPitch = 0x3f;
    inline const int HPitch = 0x40;
    inline const int LYaw = 0x41;
    inline const int HYaw = 0x42;
    inline const int TEMP905x = 0x43;

    inline const int D0Status = 0x41;
    inline const int D1Status = 0x42;
    inline const int D2Status = 0x43;
    inline const int D3Status = 0x44;
    inline const int PressureL = 0x45;
    inline const int PressureH = 0x46;
    inline const int HeightL = 0x47;
    inline const int HeightH = 0x48;
    inline const int LonL = 0x49;
    inline const int LonH = 0x4a;
    inline const int LatL = 0x4b;
    inline const int LatH = 0x4c;
    inline const int GPSHeight = 0x4d;
    inline const int GPSYAW = 0x4e;
    inline const int GPSVL = 0x4f;
    inline const int GPSVH = 0x50;
    inline const int q0 = 0x51;
    inline const int q1 = 0x52;
    inline const int q2 = 0x53;
    inline const int q3 = 0x54;
    inline const int SVNUM = 0x55;
    inline const int PDOP = 0x56;
    inline const int HDOP = 0x57;
    inline const int VDOP = 0x58;
    inline const int DELAYT = 0x59;
    inline const int XMIN = 0x5a;
    inline const int XMAX = 0x5b;
    inline const int BATVAL = 0x5c;
    inline const int ALARMPIN = 0x5d;
    inline const int YMIN = 0x5e;
    inline const int YMAX = 0x5f;
    inline const int GYROZSCALE = 0x60;
    inline const int GYROCALITHR = 0x61;
    inline const int ALARMLEVEL = 0x62;
    inline const int GYROCALTIME = 0x63;
    inline const int REFROLL = 0x64;
    inline const int REFPITCH = 0x65;
    inline const int REFYAW = 0x66;
    inline const int GPSTYPE = 0x67;
    inline const int TRIGTIME = 0x68;
    inline const int KEY = 0x69;
    inline const int WERROR = 0x6a;
    inline const int TIMEZONE = 0x6b;
    inline const int CALICNT = 0x6c;
    inline const int WZCNT = 0x6d;
    inline const int WZTIME = 0x6e;
    inline const int WZSTATIC = 0x6f;
    inline const int ACCSENSOR = 0x70;
    inline const int GYROSENSOR = 0x71;
    inline const int MAGSENSOR = 0x72;
    inline const int PRESSENSOR = 0x73;
    inline const int MODDELAY = 0x74;

    inline const int ANGLEAXIS = 0x75;
    inline const int XRSCALE = 0x76;
    inline const int YRSCALE = 0x77;
    inline const int ZRSCALE = 0x78;

    inline const int XREFROLL = 0x79;
    inline const int YREFPITCH = 0x7a;
    inline const int ZREFYAW = 0x7b;

    inline const int ANGXOFFSET = 0x7c;
    inline const int ANGYOFFSET = 0x7d;
    inline const int ANGZOFFSET = 0x7e;

    inline const int NUMBERID1 = 0x7f;
    inline const int NUMBERID2 = 0x80;
    inline const int NUMBERID3 = 0x81;
    inline const int NUMBERID4 = 0x82;
    inline const int NUMBERID5 = 0x83;
    inline const int NUMBERID6 = 0x84;

    inline const int XA85PSCALE = 0x85;
    inline const int XA85NSCALE = 0x86;
    inline const int YA85PSCALE = 0x87;
    inline const int YA85NSCALE = 0x88;
    inline const int XA30PSCALE = 0x89;
    inline const int XA30NSCALE = 0x8a;
    inline const int YA30PSCALE = 0x8b;
    inline const int YA30NSCALE = 0x8c;

    inline const int CHIPIDL = 0x8D;
    inline const int CHIPIDH = 0x8E;
    inline const int REGINITFLAG = REGSIZE - 1;


/* AXIS6 */
    inline const int ALGRITHM9 = 0;
    inline const int ALGRITHM6 = 1;

/************CALSW**************/
    inline const int NORMAL = 0x00;
    inline const int CALGYROACC = 0x01;
    inline const int CALMAG = 0x02;
    inline const int CALALTITUDE = 0x03;
    inline const int CALANGLEZ = 0x04;
    inline const int CALACCL = 0x05;
    inline const int CALACCR = 0x06;
    inline const int CALMAGMM = 0x07;
    inline const int CALREFANGLE = 0x08;
    inline const int CALMAG2STEP = 0x09;
//inline const int CALACCX = 0x09;
//inline const int ACC45PRX = 0x0A;
//inline const int ACC45NRX = 0x0B;
//inline const int CALACCY = 0x0C;
//inline const int ACC45PRY = 0x0D;
//inline const int ACC45NRY = 0x0E;
//inline const int CALREFANGLER = 0x0F;
//inline const int CALACCINIT = 0x10;
//inline const int CALREFANGLEINIT = 0x11;
    inline const int CALHEXAHEDRON = 0x12;

/************OUTPUTHEAD**************/
    inline const int WIT_TIME = 0x50;
    inline const int WIT_ACC = 0x51;
    inline const int WIT_GYRO = 0x52;
    inline const int WIT_ANGLE = 0x53;
    inline const int WIT_MAGNETIC = 0x54;
    inline const int WIT_DPORT = 0x55;
    inline const int WIT_PRESS = 0x56;
    inline const int WIT_GPS = 0x57;
    inline const int WIT_VELOCITY = 0x58;
    inline const int WIT_QUATER = 0x59;
    inline const int WIT_GSA = 0x5A;
    inline const int WIT_REGVALUE = 0x5F;

/************RSW**************/
    inline const int RSW_TIME = 0x01;
    inline const int RSW_ACC = 0x02;
    inline const int RSW_GYRO = 0x04;
    inline const int RSW_ANGLE = 0x08;
    inline const int RSW_MAG = 0x10;
    inline const int RSW_PORT = 0x20;
    inline const int RSW_PRESS = 0x40;
    inline const int RSW_GPS = 0x80;
    inline const int RSW_V = 0x100;
    inline const int RSW_Q = 0x200;
    inline const int RSW_GSA = 0x400;
    inline const int RSW_MASK = 0xfff;

/**RRATE*****/
    inline const int RRATE_NONE = 0x0d;
    inline const int RRATE_02HZ = 0x01;
    inline const int RRATE_05HZ = 0x02;
    inline const int RRATE_1HZ = 0x03;
    inline const int RRATE_2HZ = 0x04;
    inline const int RRATE_5HZ = 0x05;
    inline const int RRATE_10HZ = 0x06;
    inline const int RRATE_20HZ = 0x07;
    inline const int RRATE_50HZ = 0x08;
    inline const int RRATE_100HZ = 0x09;
    inline const int RRATE_125HZ = 0x0a;    //only WT931
    inline const int RRATE_200HZ = 0x0b;
    inline const int RRATE_ONCE = 0x0c;

/* BAUD */
    inline const int WIT_BAUD_4800 = 1;
    inline const int WIT_BAUD_9600 = 2;
    inline const int WIT_BAUD_19200 = 3;
    inline const int WIT_BAUD_38400 = 4;
    inline const int WIT_BAUD_57600 = 5;
    inline const int WIT_BAUD_115200 = 6;
    inline const int WIT_BAUD_230400 = 7;
    inline const int WIT_BAUD_460800 = 8;
    inline const int WIT_BAUD_921600 = 9;

/*CAN BAUD*/
    inline const int CAN_BAUD_1000000 = 0;
    inline const int CAN_BAUD_800000 = 1;
    inline const int CAN_BAUD_500000 = 2;
    inline const int CAN_BAUD_400000 = 3;
    inline const int CAN_BAUD_250000 = 4;
    inline const int CAN_BAUD_200000 = 5;
    inline const int CAN_BAUD_125000 = 6;
    inline const int CAN_BAUD_100000 = 7;
    inline const int CAN_BAUD_80000 = 8;
    inline const int CAN_BAUD_50000 = 9;
    inline const int CAN_BAUD_40000 = 10;
    inline const int CAN_BAUD_20000 = 11;
    inline const int CAN_BAUD_10000 = 12;
    inline const int CAN_BAUD_5000 = 13;
    inline const int CAN_BAUD_3000 = 14;

/* KEY */
    inline const int KEY_UNLOCK = 0xB588;

/* SAVE */
    inline const int SAVE_PARAM = 0x00;
    inline const int SAVE_SWRST = 0xFF;

/* ORIENT */
    inline const int ORIENT_HERIZONE = 0;
    inline const int ORIENT_VERTICLE = 1;

/* BANDWIDTH */
    inline const int BANDWIDTH_256HZ = 0;
    inline const int BANDWIDTH_184HZ = 1;
    inline const int BANDWIDTH_94HZ = 2;
    inline const int BANDWIDTH_44HZ = 3;
    inline const int BANDWIDTH_21HZ = 4;
    inline const int BANDWIDTH_10HZ = 5;
    inline const int BANDWIDTH_5HZ = 6;
}