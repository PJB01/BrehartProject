// 2. Implement the class MYGPIO in a file named myGpio.cpp.
/*
 * File name: myGpio.cpp
 * Created on: 11/5/2023
 * By: Amer Qouneh
 *
 * Very simple gpio class.
 * Similar to Molloy's GPIO class except that
 * it is smaller and much simpler implementation.
 *
 */
#include "myGPIO.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
using namespace std;
MYGPIO::MYGPIO(int n)
{
    number = n;
    ostringstream s;
    s << "gpio" << number;
    name = string(s.str());
    path = GPIO_PATH + name + "/";
    // need to give Linux time to set up the sysfs structure
    usleep(250000); // 250ms delay
}
int MYGPIO::getNumber()
{
    return number;
}
void MYGPIO::setDirection(GPIO_DIRECTION dir)
{
    if (dir == INPUT)
    {
        write(path, "/direction", "in");
    }
    else
    {
        write(path, "/direction", "out");
    }
}
GPIO_DIRECTION MYGPIO::getDirection()
{
    string input = read(path, "direction");
    if (input == "in")
        return INPUT;
    else
        return OUTPUT;
}
int MYGPIO::setValue(GPIO_VALUE value)
{
    switch (value)
    {
    case HIGH:
        return write(path, "value", "1");
        break;
    case LOW:
        return write(path, "value", "0");
        break;
    }
    return -1;
}
GPIO_VALUE MYGPIO::getValue()
{
    string input = read(path, "value");
    if (input == "0")
        return LOW;
    else
        return HIGH;
}
void MYGPIO::toggleOutput()
{
    // read value of pin and then set it to the other value.
    // string input = read(path, "value");
    // if (input == "0"){
    // write(path, "/value", "1");
    //}
    // else{
    // write(path, "/value", "0");
    //}//if
    // Or using public member functions.
    if (getValue() == LOW)
    {
        setValue(HIGH);
    }
    else
    {
        setValue(LOW);
    }
    // This works too, but may not be as good as above
    // solution due to the explicit casting of type to GPIO_VALUE.
    // setValue(GPIO_VALUE(!getValue()));
}
int MYGPIO::write(string path, string filename, string value)
{
    ofstream fs;
    fs.open((path + filename).c_str());
    if (!fs.is_open())
    {
        perror("GPIO: write failed to open file ");
        return -1;
    }
    fs << value;
    fs.close();
    return 0;
}
int MYGPIO::write(string path, string filename, int value)
{
    stringstream s;
    s << value;
    return write(path, filename, s.str());
}
string MYGPIO::read(string path, string filename)
{
    ifstream fs;
    fs.open((path + filename).c_str());
    if (!fs.is_open())
    {
        perror("GPIO: read failed to open file ");
    }
    string input;
    getline(fs, input);
    fs.close();
    return input;
}

MYGPIO::~MYGPIO()
{
}