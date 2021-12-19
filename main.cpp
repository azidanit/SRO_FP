/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <cstdio>
#include <vector>

// Serial pc(USBTX, USBRX);

PinName pin_motors_pwm[] = {PB_1, PB_15, PB_14, PB_13} ;
PinName pin_motors_switcher[] = {PA_0, PA_1, PA_4, PB_0, PC_1, PC_0, PC_3, PC_2} ;
enum motor_position {Front_Left, Front_Right, Rear_Left, Rear_Right};
vector<PwmOut*> motors_pwm;
vector<DigitalOut*> motors_switch;

DigitalOut led1(LED1);

void initMotorsL298NPin(){
    for(int i = 0; i < sizeof(pin_motors_pwm)/sizeof(pin_motors_pwm[0]); i++){
        motors_pwm.push_back( (new PwmOut(pin_motors_pwm[i])) );
        motors_pwm[i]->period_ms(20);
        motors_pwm[i]->write(0);

        motors_switch.push_back( new DigitalOut(pin_motors_switcher[i*2]));
        motors_switch.push_back( new DigitalOut(pin_motors_switcher[i*2 + 1]));
        motors_switch[i*2]->write(0);
        motors_switch[i*2 + 1]->write(1);
    }
}

void setMotorDirection(int pos_motor, bool is_forward){
    motors_switch[pos_motor*2]->write(!is_forward);
    motors_switch[pos_motor*2 + 1]->write(is_forward);
}

int main()
{

    initMotorsL298NPin();

    int counter = 0;
    short pengali = 1;
    bool is_forward = true;
    while (1) {
        counter += 10 * pengali;

        if (counter >= 400){
            pengali *= -1;
            
        }else if(counter <= 0){
            pengali *= -1;
            is_forward = !is_forward;
        }

        
        printf("CYCLE MOTOR %d is for %d\n", counter, is_forward);

        for(int i = 0; i < sizeof(pin_motors_pwm)/sizeof(pin_motors_pwm[0]); i++){
            motors_pwm[i]->write(float(counter) / 1000);
            setMotorDirection(i, is_forward);
        }
        led1.write(!led1.read());
        ThisThread::sleep_for(100ms);
    }

}
