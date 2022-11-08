////////////////////////////////////////////////////////////////////////////////////////////
//    Demo code for the protocentral-ads1262 sensor breakout board
//
//    Copyright (c) 2013 ProtoCentral
//
//    This example measures raw capacitance across CHANNEL0 and Gnd and
//    prints on serial terminal
//    
//    this example gives differential voltage across the AN0 And AN1 in mV
//    Hooking-up with the Arduino
//    
//    |ads1262 pin label| Pin Function         |Arduino Connection|
//    |-----------------|:--------------------:|-----------------:|
//    | DRDY            | Data ready Output pin|  D6              |             
//    | MISO            | Slave Out            |  D12             |
//    | MOSI            | Slave In             |  D11             |
//    | SCLK            | Serial Clock         |  D13             |
//    | CS              | Chip Select          |  D7              |
//    | START           | Start Conversion     |  D5              | 
//    | PWDN            | Power Down/Reset     |  D4              |
//    | DVDD            | Digital VDD          |  +5V             |
//    | DGND            | Digital Gnd          |  Gnd             |
//    | AN0-AN9         | Analog Input         |  Analog Input    |
//    | AINCOM          | Analog input common  |                  |
//    | AVDD            | Analog VDD           |  -               |
//    | AGND            | Analog Gnd           |  -               |
//    -------------------------------------------------------------
//
//   This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//   For information on how to use, visit https://github.com/Protocentral/ProtoCentral_ads1262
//
//////////////////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <ads1262.h>

#define PGA 1                     // Programmable Gain = 1
#define VREF 2.50                 // Internal reference of 2.5V
#define VFSR VREF/PGA             
#define FSR (((long int)1<<23)-1)  

ads1262 PC_ADS1262;                     // class

float volt_V=0;
float volt_mV=0;
volatile int i;
volatile char SPI_RX_Buff[10];
volatile long ads1262_rx_Data[10];
volatile static int SPI_RX_Buff_Count = 0;
volatile char *SPI_RX_Buff_Ptr;
volatile int Responsebyte = false;
volatile signed long sads1262Count = 0;
volatile signed long uads1262Count=0;
double resolution;

void setup() 
{
  // initalize the  data ready and chip select pins:
  pinMode(ADS1262_DRDY_PIN, INPUT);                  //data ready input line
  pinMode(ADS1262_CS_PIN, OUTPUT);                   //chip enable output line
  pinMode(ADS1262_START_PIN, OUTPUT);               // start 
  pinMode(ADS1262_PWDN_PIN, OUTPUT);                // Power down output   

  Serial.begin(9600);
  PC_ADS1262.ads1262_Init();                      // initialise ads1262
  Serial.println("ads1262 Initialised successfully....");
}

void loop() 
{
  volatile int i,data;

  if((digitalRead(ADS1262_DRDY_PIN)) == LOW)               // monitor Data ready(DRDY pin)
  {  
    SPI_RX_Buff_Ptr = PC_ADS1262.ads1262_Read_Data();      // read 6 bytes conversion register
    Responsebyte = true ; 
  }

  if(Responsebyte == true)
  {
    for(i = 0; i <5; i++)
    {
      SPI_RX_Buff[SPI_RX_Buff_Count++] = *(SPI_RX_Buff_Ptr + i);              
    }
    Responsebyte = false;
  }
  
  if(SPI_RX_Buff_Count >= 5)
  {     
    ads1262_rx_Data[0]= (unsigned char)SPI_RX_Buff[1];  // read 4 bytes adc count
    ads1262_rx_Data[1]= (unsigned char)SPI_RX_Buff[2];
    ads1262_rx_Data[2]= (unsigned char)SPI_RX_Buff[3];
    ads1262_rx_Data[3]= (unsigned char)SPI_RX_Buff[4];
 
    uads1262Count = (signed long) (((unsigned long)ads1262_rx_Data[0]<<24)|((unsigned long)ads1262_rx_Data[1]<<16)|(ads1262_rx_Data[2]<<8)|ads1262_rx_Data[3]);//get the raw 32-bit adc count out by shifting
    sads1262Count = (signed long) (uads1262Count);      // get signed value
    resolution = (double)((double)VREF/pow(2,31));       //resolution= Vref/(2^n-1) , Vref=2.5, n=no of bits
    volt_V      = (resolution)*(float)sads1262Count;     // voltage = resolution * adc count
    volt_mV   =   volt_V*1000;                           // voltage in mV
    Serial.print("Readout voltage");
    Serial.print(" : ");
    Serial.print(volt_V);
    Serial.print(" V ,");
    Serial.print(volt_mV);
    Serial.println(" mV");
   }
    
  SPI_RX_Buff_Count = 0;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
