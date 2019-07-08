/*
  xsns_91_tc74.ino - TC74 temperature sensor support for Sonoff-Tasmota
  Adapted from code for HTU21 --- Ivan Smyth

  Copyright (C) 2019  Heiko Krupp and Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef USE_I2C
#ifdef USE_TC74
/*********************************************************************************************\
 * TC74 - Temperature
 *
 * Source: Ivan Smyth
 *
 * I2C Address: 0x48
\*********************************************************************************************/

#define XSNS_91             91

#define TC74_ADDR          0x48 //This is the i2c address for TC74A0


 


uint8_t tc74_type=0;
char tc74_types[]="TC74A0";
uint8_t tc74_valid=0;
int8_t tc74_temperature=0;


void Tc74Detect(void)
{
  if (tc74_type){
    return;
  }
  Wire.beginTransmission(TC74_ADDR);
  Wire.write(0);
  if (!Wire.endTransmission()){
    tc74_type = 1;
    AddLog_P2(LOG_LEVEL_DEBUG, S_LOG_I2C_FOUND_AT, tc74_types, TC74_ADDR);
  }
  
}

bool Tc74Read(void)
{
  if(tc74_valid){tc74_valid--;}
  Wire.beginTransmission(TC74_ADDR);
  Wire.write(0);
  Wire.requestFrom(TC74_ADDR,1);
  if(Wire.available()){
    tc74_temperature = Wire.read();
    tc74_valid = SENSOR_MAX_MISS;
    return true;
  }
  else{return false;}
}

bool Tc74EverySecond(void)
{
  if(35==(uptime%100)){
    Tc74Detect();
  }
  else {
    if (tc74_type){
      if(!Tc74Read()){
        AddLogMissed(tc74_types,tc74_valid);
      }
    }

  }
}

void Tc74Show(bool json)
{
  if (tc74_valid){
    char str_temperature[33];
    dtostrfd(tc74_temperature, Settings.flag2.temperature_resolution, str_temperature); //Convert decimal temp to string
    if(json){
      ResponseAppend_P(JSON_SNS_TEMP, tc74_types,str_temperature);
#ifdef USE_DOMOTICZ
      if(0==tele_period){
        DomoticzSensor(DZ_TEMP,str_temperature);
      }
#endif //USE_DOMOTICZ
    } else {
#ifdef USE_WEBSERVER
      WSContentSend_PD(HTTP_SNS_TEMP,tc74_types,str_temperature,TempUnit());
#endif //USE_WEBSERVER
    } 
  }
} 


/*********************************************************************************************\
 * Interface
\*********************************************************************************************/


bool Xsns91(uint8_t function)
{
  bool result =false;
  if(i2c_flg){
    switch (function){
      case FUNC_INIT:
        Tc74Detect();
        break;
      case FUNC_EVERY_SECOND:
        Tc74EverySecond();
        break;
      case FUNC_JSON_APPEND:
       Tc74Show(1);
       break;
#ifdef USE_WEBSERVER
      case FUNC_WEB_SENSOR:
        Tc74Show(0);
        break;
#endif //USE_WEBSERVER
    }
  }
  return result;
}



#endif  //USE_TC74
#endif  //USE_I2C
