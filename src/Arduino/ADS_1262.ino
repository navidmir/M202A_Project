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

// #define PGA 1 // Programmable Gain = 1
#define VREF 2.50 // Internal reference of 2.5V
// #define VFSR 2*VREF/PGA // full-scale range   
#define TOTAL_GAIN 10 // (0.1 V/A)(100V/V)(1V/V) = (Res)(Amp)(PGA)    
#define LED_PIN 2

ads1262 PC_ADS1262;                     // class

float volt_V=0;
volatile int i;
volatile char SPI_RX_Buff[10];
volatile long ads1262_rx_Data[10];
volatile static int SPI_RX_Buff_Count = 0;
volatile char *SPI_RX_Buff_Ptr;
volatile int Responsebyte = false;
volatile signed long sads1262Count = 0;
volatile signed long uads1262Count=0;
double resolution = (double)((double)VREF/pow(2,31)); //resolution= Vref/(2^n-1) , Vref=2.5, n=no of bits
float averageVal; // to calibrate

// Leave power channel unconnected on startup and wait until LED turns on to 
// indicate calibration has completed
void setup() 
{
  // initalize the ADC
  pinMode(ADS1262_DRDY_PIN, INPUT); //data ready input line
  pinMode(ADS1262_CS_PIN, OUTPUT); //chip enable output line
  pinMode(ADS1262_START_PIN, OUTPUT); // start 
  pinMode(ADS1262_PWDN_PIN, OUTPUT); // Power down output   
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  PC_ADS1262.ads1262_Init(); // initialise ads1262

  averageVal = calcAverage(); // calibrate
  digitalWrite(LED_PIN, HIGH); // LED on after calibration
}

void loop() 
{
  Serial.print("Sample [uA]: ");
  Serial.println((receive32BitSample() - averageVal)*1000000.0/TOTAL_GAIN);
}

float receive32BitSample() {
  if((digitalRead(ADS1262_DRDY_PIN)) == LOW) {               // monitor Data ready(DRDY pin)
    SPI_RX_Buff_Ptr = PC_ADS1262.ads1262_Read_Data();      // read 6 bytes conversion register
    Responsebyte = true ; 
  }

  if(Responsebyte == true) {
    for(int i = 0; i < 5; i++) {
      SPI_RX_Buff[SPI_RX_Buff_Count++] = *(SPI_RX_Buff_Ptr + i);              
    }
    Responsebyte = false;
  }
  
  if(SPI_RX_Buff_Count >= 5) {
    ads1262_rx_Data[0] = (unsigned char)SPI_RX_Buff[1];  // read 4 bytes adc count
    ads1262_rx_Data[1] = (unsigned char)SPI_RX_Buff[2];
    ads1262_rx_Data[2] = (unsigned char)SPI_RX_Buff[3];
    ads1262_rx_Data[3] = (unsigned char)SPI_RX_Buff[4];
 
    uads1262Count = (signed long) (((unsigned long)ads1262_rx_Data[0]<<24)|((unsigned long)ads1262_rx_Data[1]<<16)|(ads1262_rx_Data[2]<<8)|ads1262_rx_Data[3]);//get the raw 32-bit adc count out by shifting
    sads1262Count = (signed long) (uads1262Count);      // get signed value
    volt_V = (resolution)*(float)sads1262Count;     // voltage = resolution * adc count
   }
    
  SPI_RX_Buff_Count = 0;
  return volt_V;
}

// take 1000 samples and find the average for calibration
float calcAverage() {
  float avg = 0;
  // discard first 1000 samples
  for (int i = 0; i < 1000; i++) {
    receive32BitSample();
  }

  // use next 10000 samples for averaging
  for (int i = 0; i < 1000; i++) {
    avg += receive32BitSample();
  }
  return avg / 1000;
}
