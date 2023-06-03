/*******************************************************************************
* INCLUDED FILES
******************************************************************************/
#include "obstacleDetection.h"


/*******************************************************************************
* API
******************************************************************************/
/*
* @param *this:             "Object" - which side of the Rover is directed
* @param thisSensorAddr[] : Addresses from Sensors
* @param size:              size of this SensorAddr[] which is the the number
                            of Sensors
* @param range:             range in cm
*/
void S_obstacleDetec_init(S_obstacleDetec *this, uint8_t thisSensorAddr[],int
                                size, int range){
    int i;
    // number of Sensor that are used at 'this' side of the Rover
    this -> numOfSensors = size;
    // Initialize the dynamic array AddrAndRecivedData from S_obstacleDetec
    arrayInit(this);
    // Initialize the dynamic array arrayMap from the S_obstacleDetec
    arrayMapInit(this);
    // put all addresses in the first column of the array AddrAndRecivedData
    for (i=0; i<(this -> numOfSensors); i++){
    this -> AddrAndRecivedData[i][0] = thisSensorAddr[i];
    }
    /*
    // check if all addresses arrived
    for (int i=0; i<(this -> numOfSensors); i++){
        sprintf(charToPrint, "Sensor %i: %x\n", i, this ->
        AddrAndRecivedData[i][0]);
        toPrint(charToPrint);
    }
    */
    setRange(this, range);
}
void S_obstacleDetec_delete(S_obstacleDetec *this){
    free(this);
}


/*
* @param oldAddress :   the current address from the Sensor in Hex
* @param newAdress :    the new address that the Sensor should get
*
* When changing the address, only one sensor may be connected to the bus
*/
void changeI2Caddress(uint8_t oldAddress, uint8_t newAddress){
    uint8_t byteSequence [4] = {0xA0, 0xAA, 0xA5, newAddress};
    sendBuf[0] = REGISTER_0;
    for(int i = 0; i < 4; i++){
        sendBuf[1] = byteSequence[i];
        ret = HAL_I2C_Master_Transmit(&hi2c1, oldAddress, sendBuf, 2,
        HAL_MAX_DELAY);
        if ( ret != HAL_OK ) {
            toPrint( "Error Tx\r\n");
        }
        HAL_Delay(50);
    }
}

/*
* @param *this :    "Object" - which side of the Rover is directed
*/
void triggerMessuarments(S_obstacleDetec *this){
    sendData(this, REGISTER_0, READ_IN_CM);
    HAL_Delay(650);
    readData(this, REGISTER_1, 33);
    analyzeData(this);
    printArray(this);
    HAL_Delay(1000);
}

bool **getLastMessurments(S_obstacleDetec *this){
    return this->arrayMap;
}


/******************************** ***********************************************
* METHOD
******************************************************************************/

/*
* @ param *this : "Object" - which side of the Rover is directed
* @ param toRegister : register in which is written
* @ param data : data to be transmitted
*/
void sendData(S_obstacleDetec *this, uint8_t toRegister, uint8_t data){
    sendBuf[0] = toRegister;
    sendBuf[1] = data;
    for(int i = 0; i < (this -> numOfSensors); i++){
        ret = HAL_I2C_Master_Transmit(&hi2c1, this ->
        AddrAndRecivedData[i][0], sendBuf, 2, HAL_MAX_DELAY);
        if ( ret != HAL_OK ) {
            sprintf(charToPrint, "Error send Tx to %x\r\n", this ->
            AddrAndRecivedData[i][0]);
            toPrint(charToPrint);
        }else{
            /*
            sprintf(charToPrint, "send %i to %x \n", data, this ->
            AddrAndRecivedData[i][0]);
            toPrint(charToPrint);
            */
        }
    }
}

/*
* @ param *this : "Object" - which side of the Rover is directed
* @ param toRegister : from which register should be read
* Never call up the register zero in this function,
* since the column zero would be overwritten by AddrAndRecivedData,
* in which the addresses of the sensors are stored.
* To read register zero, write your own function
* @ param num : how many registers are read out
*/
void readData(S_obstacleDetec *this, uint8_t toRegister, int num){
    sendBuf[0] = toRegister;
    for(int i = 0; i < (this -> numOfSensors); i++){
        // send request
        ret = HAL_I2C_Master_Transmit(&hi2c1, this ->
        AddrAndRecivedData[i][0], sendBuf, 1,
        HAL_MAX_DELAY);
        if ( ret != HAL_OK ) {
            toPrint("Error read Tx\r\n");
        } else {
            // received data in recivedBuf
            ret = HAL_I2C_Master_Receive(&hi2c1, this ->
            AddrAndRecivedData[i][0], recivedBuf, num ,
            HAL_MAX_DELAY);
            if ( ret != HAL_OK ) {
                toPrint("Error read Rx\r\n");
            } else {
                /*
                sprintf(charToPrint, "read sensor %x \n", this ->
                AddrAndRecivedData[i][0]);
                toPrint(charToPrint);
                */
                // transmit received data in the right element of
                // <AddrAndRecivedData>
                if((int)toRegister != 0){
                    for(int b = (int)toRegister; b < (num + (int)toRegister); b++){
                        this -> AddrAndRecivedData[i][b] =
                        recivedBuf[b - (int)toRegister];
                        /*
                        sprintf(charToPrint, "Sensor % x Register
                        %i : %i \n",this ->
                        AddrAndRecivedData[i][0], b,
                        this ->
                        AddrAndRecivedData[i][b]);
                        toPrint(charToPrint);
                        */
                    }
                }else{ toPrint("Error");}
            }
        }
    }
}

/*
* The range will be set for all Sensors that are in this Object.
* - the next possible value of the desired value is calculated
* - the next possible Range will be saved in "finalRange"
* - the information will be send to all connected Sensors
*/
void setRange(S_obstacleDetec *this, int range){
    int value = (range*10 - 43) / 43;
    this -> finalRange = value*43+43;
    sendData(this, REGISTER_2, value);
}

/*
* the Array-size for the "AddrAndRecivedData" Pointer will be allocated
* the size will be [number of Sensors ][number of Register +1]
*/
void arrayInit(S_obstacleDetec *this){
    //matrix = malloc(zeile * sizeof(int *));
    //uint8_t ** matrix = this->AddrAndRecivedData;
    this->AddrAndRecivedData = malloc(this -> numOfSensors * sizeof(uint8_t*));
    // reserve memory (=row)
    if(NULL == this ->AddrAndRecivedData){
        toPrint("Kein virtueller RAM mehr vorhanden ... !");
        //return EXIT_FAILURE //TODO
    }
    for(int i = 0; i < this->numOfSensors; i++) {
        // reserve memory (=column)
        this->AddrAndRecivedData[i] = malloc(33 * sizeof(uint8_t));
        if(NULL == this->AddrAndRecivedData[i]) {
            toPrint("Kein virtueller RAM mehr vorhanden ... !");
            //return EXIT_FAILURE;
        }
    }
}
/*
* the Array-size for the "arrayMap" Pointer will be allocated
* the size will be [number of Map Division][number of Sensors]
*/
void arrayMapInit(S_obstacleDetec *this){
    this->arrayMap = malloc(amountOfMapDivision * sizeof(bool*));
    // reserve memory (=row)
    if(NULL == this ->arrayMap){
        toPrint("Kein virtueller RAM mehr vorhanden ... !");
        //return EXIT_FAILURE //TODO
    }
    for(int i = 0; i < amountOfMapDivision; i++) {
        // reserve memory (=column)
        this->arrayMap[i] = malloc(this->numOfSensors * sizeof(bool));
        if(NULL == this->arrayMap[i]) {
            toPrint("Kein virtueller RAM mehr vorhanden ... !");
            //return EXIT_FAILURE;
        }
    }
    for(int i = 0; i < amountOfMapDivision; i++){
        for(int a = 0; a < this->numOfSensors; a++){
            this->arrayMap[i][a] = false;
        }
    }
}

/*
* from the data stored in the "AddrAndRecivedData" array,
* the position of possible obstacles is analyzed and saved in the array map
*/
void analyzeData(S_obstacleDetec *this){
    uint16_t temp;
    float steps = (this->finalRange/amountOfMapDivision)/10;
    int i, b, a, x;
    for (i = 0; i < this->numOfSensors; i++ ){
        // if Register_1 = 0 than there are no measurements
        if(this->AddrAndRecivedData[i][1] == '\0'){
            sprintf(charToPrint, "no measurements for Sensor %x \n",
            this-> AddrAndRecivedData[i][0]);
            toPrint(charToPrint);
        }else{
            // sets all rows to false, before saving new values
            for(b= 0; b< amountOfMapDivision; b++){
                this-> arrayMap[b][i]= false;
            }
            // from register 2, values of the distance measurement are saved
            a = 2;
            while(a < 33){
                temp = ((uint16_t)this->AddrAndRecivedData[i][a] << 8) |
                                (uint16_t)this->AddrAndRecivedData[i][a+1];
                if (temp == 0){
                    break;
                }
                a = a + 2;
                x = 0;
                // Compares the value obtained (temp) with the value
                // range of the individual fields of the arrayMap
                do{
                    if((steps * x) < temp && temp <(steps * (x+1))){
                        this-> arrayMap[x][i]= true;
                        break;
                    }
                    x++;
                }while(x < amountOfMapDivision);
            }
        }
    }
}

/*
* arrayMap will be printed as an array
*/
void printArray(S_obstacleDetec *this){
    for(int i = 0; i < this->numOfSensors; i++){
        printf("%x ",this -> AddrAndRecivedData[i][0]);
    }
    printf("\n");
    for(int i = 0; i < amountOfMapDivision; i++){
        for(int a = 0; a < this->numOfSensors; a++){
            printf("%d ",this->arrayMap[i][a]);
        }
        printf("\n");
    }
    printf("\n");
}
/*
*To deactivate the output as quickly as possible,
*all outputs are routed via this function
*/
void toPrint(char toPrint[]){
    printf("%s \n", toPrint);
}