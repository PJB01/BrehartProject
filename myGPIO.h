/*
 * File name: myGpio.h
 * Created on: 11/5/2023
 * By: Amer Qouneh
 *
 * Very simple gpio class.
 * Similar to Molloy's GPIO class except that
 * it is smaller and much simpler implementation.
 */
#ifndef MYGPIO_H_
#define MYGPIO_H_

#include <string>
#include <fstream>
using std::ofstream;
using std::string;
#define GPIO_PATH "/sys/class/gpio/"
enum GPIO_DIRECTION
{
    INPUT,
    OUTPUT
};
enum GPIO_VALUE
{
    LOW,
    HIGH
};
class MYGPIO
{
private:
    int number;
    string name, path;
    int write(string path, string filename, string value);
    int write(string path, string filename, int value);
    string read(string path, string filename);
    ofstream stream;

public:
    MYGPIO(int number); // constructor
    int getNumber();
    // General Input and Output Settings
    // Molloy's class sets direction to either
    // input or output only. It does not seem
    // that it is possible to set pull-up or pull-down
    // resistors using C++ code.
    void setDirection(GPIO_DIRECTION dir);
    GPIO_DIRECTION getDirection();
    int setValue(GPIO_VALUE val);
    GPIO_VALUE getValue();
    void toggleOutput();
    ~MYGPIO(); // destructor
};
#endif /* MYGPIO_H_ */