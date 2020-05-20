#include "Arduino.h"
#include "IoBoard.h"

byte lightPin[] = {LT_START, LT_A, LT_B, LT_C, LT_D, LT_FXL, LT_FXR};

//contructor
IoBoard::IoBoard(char* rCode)
{
    byte rType[] = {0x09, 0x06, 0x00, 0x00};
    byte rVersion[] = {0x01, 0x01, 0x00};
    setVersion(rType, 0x00, rVersion, rCode);


    //pins for pushbuttons
    //fx R
    pinMode(BT_FXR,INPUT);
    digitalWrite(BT_FXR, HIGH);
    //fx L
    pinMode(BT_FXL,INPUT);
    digitalWrite(BT_FXL, HIGH);
    //A
    pinMode(BT_A,INPUT);
    digitalWrite(BT_A, HIGH);
    //B
    pinMode(BT_B,INPUT);
    digitalWrite(BT_B, HIGH);
    //C
    pinMode(BT_C,INPUT);
    digitalWrite(BT_C, HIGH);
    //D
    pinMode(BT_D,INPUT);
    digitalWrite(BT_D, HIGH);
    //Start
    pinMode(BT_START,INPUT);
    digitalWrite(BT_START, HIGH);
    //test
    pinMode(BT_TEST, INPUT);
    digitalWrite(BT_TEST,HIGH);
    //service
    pinMode(BT_SVC, INPUT);
    digitalWrite(BT_SVC,HIGH);

    //for key lights
    for (int i = 0;i<7; i++) {
        pinMode(lightPin[i], OUTPUT);
    }

    //for rgb leds
    pinMode(LED1_R, OUTPUT);
    pinMode(LED1_G, OUTPUT);
    pinMode(LED1_B, OUTPUT);

}

void IoBoard::init()
{
    keys= 0x00;
    keysLights = 0x00;
    test =0;

    //for volume encoders
    volR = 0;
    volRState = 0;
    volL = 0;
    volLState = 0;
}

void IoBoard::update()
{
    keys = 0x00;

    if (!digitalRead(BT_FXR))
        keys |= 0x01;
    if (!digitalRead(BT_FXL))
        keys |= 0x02;
    if (!digitalRead(BT_A))
        keys |= 0x20;
    if (!digitalRead(BT_B))
        keys |= 0x10;
    if (!digitalRead(BT_C))
        keys |= 0x08;
    if (!digitalRead(BT_D))
        keys |= 0x04;
    if (!digitalRead(BT_START))
        keys |= 0x40;

    test= !digitalRead(BT_TEST);
    svc= !digitalRead(BT_SVC);
    
}


short IoBoard::processRequest(byte* request, byte* answer)
{
    answer[0] = request[0] | 0x80;        // node id
    answer[1] = request[1];               // type
    answer[2] = request[2];               // command
    answer[3] = request[3];               // paquet id
    answer[4] = 0;                        // data length

    switch (answer[2])                   // switch on the command
    {
        //
        // get version
    case 0x02:
        answer[4] = 0x2C;
        memcpy(answer+5, getVersion(), 0x2C);
        break;

        //
        // init?
    case 0x00:
    case 0x03:
    case 0x16:
    case 0x20:
    case 0x30:
        answer[4] = 1;
        answer[5] = 0x00;
        break;

    case 0x13:  //set outputs and get inputs

        /*   outputs lights format:
        byte 2 :    10 : start
                    20 : A
                    40 : B
                    80 : C
        byte 3 :    01 : D
                    02 : FX L
                    04 : FX R
        byte 4-6 : wing left up RGB
        byte 7-9 : wing right up RGB
        byte 10-12 : wing left low RGB
        byte 13-15 : wing right low RGB
        byte 16-18 : woofer RGB
        byte 19-21 : controller RGB
        */


//keys lights
        keysLights = (request[5+3] & 0x07) << 4 | (request[5+2] & 0xF0) >> 4;


        for (int i = 0;i<7; i++) {
            digitalWrite(lightPin[i], (keysLights >> i) & 1);
        }


//rgb leds

        analogWrite(LED1_R, 255-(request[5+4]<<1));
        analogWrite(LED1_G, 255-(request[5+5]<<1));
        analogWrite(LED1_B, 255-(request[5+6]<<1));

        /*   input control format:
        byte 0 : 8 high bits of L vol
        byte 1 : 2 higher bits : lower bits of L VOL
                    20 :  test
                    10 : service
                    04 : coin
        byte 2 : 8 high bits of R vol
        byte 3 : 2 higher bits : lower bits of R VOL
        byte 4 : 8 high bits of ex analog1
        byte 5 : 2 higher bits : lower bits of ex analog1
        byte 6 : 8 high bits of ex analog2
        byte 7 : 2 higher bits : lower bits of ex analog2
        byte 8 : nothing ?
        byte 9 : 20 : headphone
                    10 : recorder
                    08 : start
                    04 : A
                    02 : B
                    01 : C
        byte 10 : nothing ?
        byte 11 : 20 : D
                      10 : fx L
                      08 : fx R
                      04 : ex bt1
                      02 : ex bt2
        byte 12 : 80 : ex bt3 (not)
        byte 13 : 80 : ex bt4 (not)
        */

        answer[4] = 0x10;

        memset(answer+5, 0x00, 0x10);

        answer[9+5] = keys >> 3;
        answer[11+5] = keys << 3;

        unsigned int vl = analogRead(VOL_L);
        unsigned int vr = analogRead(VOL_R);

        answer[0+5] = vl>>2;
        answer[1+5] = vl<<6;

        answer[2+5] = vr>>2;
        answer[3+5] = vr<<6;

        if (test)
            answer[1+5] |= 0x20;
        if (svc)
            answer[1+5] |= 0x10;

        answer[12+5] = 0x80;
        answer[13+5] = 0x80;

        break;

    }

}
