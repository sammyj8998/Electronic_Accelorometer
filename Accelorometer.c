#include "ADXL345.h" //This is the library for the accelorometer, where four functions are used 
#include "mbed.h"   //This has to be included for the code to function on mbed
#include "TextLCD.h" //This allows the LCD to display information

ADXL345 accelerometer(p5, p6, p7, p8); 
TextLCD lcd(p19, p20, p21, p22, p23, p24); 

DigitalOut myled(LED1);
DigitalIn button(p10);
DigitalOut buzzer(p11);

static int xOff = 0; //This declares the offset as a global variable
static int yOff = 0;

int main() {
    
    button.mode(PullUp); //This is an mbed built in function to replace the hardware pull up resistor 
    int readings[3]; //This will intialize the x,y and z axis
    int i; //This will be used for the initial for loop
    
    //Go into standby mode to configure the device.
    accelerometer.setPowerControl(0x00);
 
    //Full resolution, +/-16g, 4mg/LSB.
    accelerometer.setDataFormatControl(0x0B);
    
    //3.2kHz data rate.
    accelerometer.setDataRate(ADXL345_3200HZ);
 
    //Measurement mode.
    accelerometer.setPowerControl(0x08);

    double xsum,ysum,zsum,xAngle,yAngle,zAngle; //A double in this case would have the greatest range 
    while (1) {
    for (i =0; i< 100;i++){  //This will itterate through 100 samples of the data 
    accelerometer.getOutput(readings);
    xsum += (int16_t)readings[0]; //This will assign each sum to the reading gathered in this case it is the x axis 
    ysum += (int16_t)readings[1];
    zsum += (int16_t)readings[2]; 
    }
    xsum/=100; //This will calculate the average for the sum, which will filter out high frequency noise in the accelerometer 
    ysum/=100;
    zsum/=100;
     
    float pi = 3.14159265359; //This declares pi as a full number if it was done with int i then the number would just be 3
    
    //This uses pythagoras' theorem (c^2 = a^2 +b^2) but instead of the total we already have c, 
    //so c can then be divided by the (a^2+b^2) to give the angle
    
    double xAngle = atan( xsum/(sqrt((ysum*ysum)+(zsum*zsum)))); 
    double yAngle = atan( ysum/(sqrt((xsum*xsum)+(zsum*zsum))));
    double zAngle = atan((sqrt((xsum*xsum)+(ysum*ysum)))/zsum);
    xsum=0; //This sets the sum back to 0, not affecting the angle and reseting it for when it next cycles through 
    ysum=0;
    zsum=0;
    
    xAngle = (xAngle*180/pi) - xOff; //Currently the data is an angle but in radian rather then degrees
    yAngle = (yAngle*180/pi) - yOff; //To convert it to degrees, the standard is the angle multiplied by 180/pi then minus the offset which would be 0 


    xAngle-=4; //This is final offset which will correct any offset balances such as it being 5% off what it should be
    yAngle-=3.5; 
    
    wait(0.5); //This is how often the x and y angle on the LCD will refresh 
    
    if (button == 0)
    { 
        myled = !myled;
        xOff += xAngle;
        yOff += yAngle;
        wait(0.1);
    }
    else myled = 0;
    
    
    if (yAngle == -90)
    {
        buzzer = 1;
    }
    else buzzer = 0;
    
    lcd.printf("X Axis: %.2f\n", xAngle); //This will print off the final angle as a float therefore showing the decimal value
    lcd.printf("Y Axis: %.2f\n", yAngle);

        
    }
}
