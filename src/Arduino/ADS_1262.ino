////////////////////////////////////////////////////////////////////////////////////////////
//    Adapted from demo code for the protocentral-ads1262 sensor breakout board
//
//    Copyright (c) 2013 ProtoCentral
//
//    Measures difference in AIN1 and AINCOM
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
volatile char *SPI_RX_Buff_Ptr;
volatile signed long sads1262Count = 0;
volatile signed long uads1262Count=0;
double resolution = (double)((double)VREF/pow(2,31)); //resolution= Vref/(2^n-1) , Vref=2.5, n=no of bits
float averageVal; // to calibrate
float sample; // holds the sample

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
  // new sample when DRDY low
  if((digitalRead(ADS1262_DRDY_PIN)) == LOW) {
    sample = (receive32BitSample() - averageVal)*1000000.0/TOTAL_GAIN; // convert to uA and divide by gain
    Serial.print("Current [uA]: ");
    Serial.println(sample);
  }
}

// call when DRDY low
float receive32BitSample() {  
  SPI_RX_Buff_Ptr = PC_ADS1262.ads1262_Read_Data(); // read 6 bytes conversion register

  for(int i = 0; i < 5; i++) {
    SPI_RX_Buff[i] = *(SPI_RX_Buff_Ptr + i);              
  }
  
  ads1262_rx_Data[0] = (unsigned char)SPI_RX_Buff[1];  // first byte is status
  ads1262_rx_Data[1] = (unsigned char)SPI_RX_Buff[2];
  ads1262_rx_Data[2] = (unsigned char)SPI_RX_Buff[3];
  ads1262_rx_Data[3] = (unsigned char)SPI_RX_Buff[4];

  uads1262Count = (signed long) (((unsigned long)ads1262_rx_Data[0]<<24)|((unsigned long)ads1262_rx_Data[1]<<16)|(ads1262_rx_Data[2]<<8)|ads1262_rx_Data[3]);//get the raw 32-bit adc count out by shifting
  sads1262Count = (signed long) (uads1262Count); // get signed value
  volt_V = (resolution)*(float)sads1262Count; // voltage = resolution * adc count
    
  return volt_V;
}

// take 10 samples and find the average for calibration
float calcAverage() {
  float avg = 0;
  // discard first 100 samples
  for (int i = 0; i < 100; i++) {
    while (digitalRead(ADS1262_DRDY_PIN)); // wait for DRDY to go low
    receive32BitSample(); // get sample
  }

  int N = 100;
  // use next N samples for averaging
  for (int i = 0; i < N; i++) {
    while (digitalRead(ADS1262_DRDY_PIN)); // wait for DRDY to go low
    avg += receive32BitSample(); // get sample and add
  }
  return avg / N;
}
