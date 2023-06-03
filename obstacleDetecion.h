/*
 * File Name: obstacleDetecion.h
 * 
 * Created on: 21.04.2020
 * Author: MiriamNold
 * Description : This file provides code for the configuration
 * of the UltrasonicSensors SRF08.
 *  
 */

#ifndef OBSTACLEDETECTION_H_
#define OBSTACLEDETECTION_H_

/*******************************************************************************
* INCLUDED FILES
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <i2c.h>
#include <stdbool.h>

/*******************************************************************************
* I2C Objects
******************************************************************************/
I2C_HandleTypeDef hi2cl;
HAL_StatusTypeDef ret;

/*******************************************************************************
* TYPES
******************************************************************************/
static const uint8_t REGISTER_0 = 0x00;
static const uint8_t REGISTER_1 = 0x01;
static const uint8_t REGISTER_2 = 0x02;
static const uint8_t READ_IN_CM = 0x51;
static const uint8_t READ_ANN_CM = 0x54;
// in how many steps should the area be divided?
static const int amountOfMapDivision = 5;
uint8_t sendBuf[2];
uint8_t recivedBuf[32];
char charToPrint[40];

/*******************************************************************************
* STRUCT
******************************************************************************/
typedef struct S_obstacleDetec {
    int numOfSensors;
    float finalRange; //final calculated range in mm
    uint8_t ** AddrAndRecivedData; // Pointer to Pointer
    // Column 0 =I2c-Addresses from Sensors
    //Column 1...32 = received Values from
    Register 1-32
    bool ** arrayMap;
}S_obstacleDetec;

/******************************** ***********************************************
* API
******************************************************************************/
void S_obstacleDetec_init(S_obstacleDetec *this, uint8_t thisSensorAddr[],int
size, int range);
void S_obstacleDetec_delete(S_obstacleDetec *this);
void changeI2Caddress(uint8_t oldAdrdess, uint8_t newAddress);
void triggerMessuarments(S_obstacleDetec *this);
bool **getLastMessurments(S_obstacleDetec *this);

/******************************** ***********************************************
* METHOD
******************************************************************************/
void sendData(S_obstacleDetec *this, uint8_t toRegister, uint8_t data);
void readData(S_obstacleDetec *this, uint8_t toRegister, int num);
void setRange(S_obstacleDetec *this, int range);
void arrayInit(S_obstacleDetec *this);
void arrayMapInit(S_obstacleDetec *this);
void analyzeData(S_obstacleDetec *this);
void printArray(S_obstacleDetec *this);
void toPrint(char toPrint[]);

/*******************************************************************************
* end of file
******************************************************************************/
#endif /* OBSTACLEDETECTION_H_ */