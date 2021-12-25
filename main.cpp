/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "platform/mbed_thread.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float64MultiArray.h>
#include <geometry_msgs/Twist.h>


ros::NodeHandle  nh;

std_msgs::String str_msg;
ros::Publisher *motor_fb_pub, *chatter2;
ros::Subscriber_ *vel_subs;

// ros::Publisher chatter("chatter", &str_msg);
// ros::Publisher chatter2("chatter2", &str_msg);

char hello[13] = "hello world!";

// Blinking rate in milliseconds
#define BLINKING_RATE_MS                                                    500

// Serial pc(USBTX, USBRX);

PinName pin_motors_pwm[] = {PB_5, PB_15, PB_14, PB_13} ;
PinName pin_motors_switcher[] = {PA_0, PA_1, PA_4, PB_0, PC_1, PC_0, PC_3, PC_2} ;
enum motor_position {Front_Left, Front_Right, Rear_Left, Rear_Right};
vector<PwmOut*> motors_pwm;
vector<DigitalOut*> motors_switch;

DigitalOut led1(LED1);

//kinematik
double vx, va;

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

void publishMotorCmd(float l_motor_, float r_motor_,  bool is_l, bool is_r){
    
    float arr_[4];
    
    std_msgs::Float32MultiArray msg;
    // msg.layout.dim = (std_msgs::MultiArrayDimension *)malloc(sizeof(std_msgs::MultiArrayDimension) * 2);
    // msg.layout.dim[0].size = 4;
    // msg.layout.dim[0].stride = 1*4;
    // msg.layout.dim_length = 1;
    msg.data_length = 4;
    // msg.data = (float*)malloc(sizeof(float) * 4);
    msg.data = arr_;
    // arr_[2] = is_l;
    // arr_[3] = is_r;
    msg.data[0] = l_motor_;
    msg.data[1] = r_motor_;
    msg.data[2] = is_l;
    msg.data[3] = is_r;
    motor_fb_pub->publish(&msg);
}

void driveMotorByKinematics(){
    float l_motor = vx + va;
    float r_motor = vx - va;
    bool is_l_motor_forward = true;
    bool is_r_motor_forward = true;

    if (l_motor >= 0){
        l_motor = l_motor < 1000 ? l_motor : 1000;
    }else{
        l_motor = (l_motor > -1000 ? l_motor : -1000);
        l_motor *= -1;
        is_l_motor_forward = false;
    }

    if (r_motor >= 0){
        r_motor = r_motor < 1000 ? r_motor : 1000;
    }else{
        r_motor = (r_motor > -1000 ? r_motor : -1000);
        r_motor *= -1;
        is_r_motor_forward = false;
    }

    setMotorDirection(0, is_l_motor_forward);
    setMotorDirection(2, is_l_motor_forward);
    setMotorDirection(1, is_r_motor_forward);
    setMotorDirection(3, is_r_motor_forward);
    thread_sleep_for(5);
    motors_pwm[0]->write(l_motor / 1000);
    motors_pwm[2]->write(l_motor / 1000);
    motors_pwm[1]->write(r_motor / 1000);
    motors_pwm[3]->write(r_motor / 1000);
    

    publishMotorCmd(l_motor, r_motor, is_l_motor_forward, is_r_motor_forward);
}

void VelCallback(const geometry_msgs::Twist& vel_msg){
    vx = vel_msg.linear.x;
    va = vel_msg.angular.z;

    driveMotorByKinematics();
}

int main()
{
   

    initMotorsL298NPin();

    vx = va = 0;
    // driveMotorByKinematics();

    nh.initNode();

    vel_subs = new ros::Subscriber<geometry_msgs::Twist>("/stm32/vel_cmd", &VelCallback);
    motor_fb_pub = new ros::Publisher("/stm32/motor_fb", new std_msgs::Float32MultiArray);
    // chatter2 = new ros::Publisher("chatter2", new std_msgs::String);
    
    nh.advertise(*motor_fb_pub);
    nh.subscribe(*vel_subs);
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);
    // int counter = 1;
    while (true) {
        led = !led;

        nh.spinOnce();
        // printf("HEHE %s\n", pesan.c_str());
        thread_sleep_for(25);

    }


    // int counter = 0;
    // short pengali = 1;
    // bool is_forward = true;
    // short counter_which_motor = 0;
    // while (1) {
    //     counter += 10 * pengali;

    //     if (counter >= 400){
    //         pengali *= -1;
            
    //     }else if(counter <= 0){
    //         pengali *= -1;
    //         is_forward = !is_forward;
    //     }

        
    //     printf("CYCLE MOTOR %d is for %d\n", counter, is_forward);

    //     for(int i = 0; i < sizeof(pin_motors_pwm)/sizeof(pin_motors_pwm[0]); i++){
    //         motors_pwm[i]->write(float(counter) / 1000);
    //         setMotorDirection(i, is_forward);
    //     }
    //     led1.write(!led1.read());
    //     ThisThread::sleep_for(100);
    // }
}
