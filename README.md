## Easy PID Motor Controller (EPMC) Cpp Library (amd64-build) i.e PC
C++ serial interface for the Easy PID Motor Controller (EPMC).

> you can use it in your microcomputer robotics project (e.g Raspberry Pi, PC, etc.) running ubuntu

#

## Install
- download and install the epmc-serial-dev pkg. you can also check the latest release [here](https://github.com/robocre8/epmc_serial_cpp/releases/)

**PC (AMD64)**
```shell
wget https://github.com/robocre8/epmc_serial_cpp/releases/download/v1.3.0/epmc-serial-dev_1.3.0_amd64.deb
```
```shell
sudo apt install ./epmc-serial-dev_1.3.0_amd64.deb
```

#

## Uninstall
- uninstall the .deb file any time with this
  ```shell
    sudo apt remove epmc-serial-dev
  ```

#

## How to Use the Library
- Ensure you have the **`EPMC MODULE`** interfaced with your preferred motors

- Ensure you've already setup the encoder and PID parameters with the [epmc_setup_application](https://github.com/robocre8/epmc_setup_application).

- check the serial port the driver is connected to:
  ```shell
  ls /dev/ttyA*
  ```
  OR
  ```shell
  ls /dev/ttyU*
  ```
  > you should see /dev/ttyACM0 or /dev/ttyUSB0 and so on

- use the serial port in your code

- A simple way to get started is simply to try out the example code below

#

## Basic Library functions and usage (Two Motor Support Control)

- connect to epmc_driver shield module
  > epmc_serial::EPMCSerialClient controller
  >
  >  _//ensure you set/call **supportedNumOfMotors()** before **connect()** as below:_
  >
  > controller.supportedNumOfMotors(epmc_serial::SupportedNumOfMotors::TWO);
  >
  > controller.connect("port_name or port_path")

- clear speed, position, e.t.c data buffer on the EPMC module
  > controller.clearDataBuffer() // returns bool -> success

- send target angular velocity command
  > controller.writeSpeed(motor0_TargetVel, motor1_TargetVel)

- send PWM command
  > controller.writePWM(motor0_PWM, motor1_PWM)

- set motor command timeout
  > controller.setCmdTimeout(timeout_ms)

- get motor command timeout
  > controller.getCmdTimeout() // returns std::tuple -> bool, float

- read motors angular position and angular velocity
  > controller.readMotorData() // returns std::tuple -> bool, std::vector<float> (pos0, pos1, vel0, vel1)

- read motors angular position
  > controller.readPos() // returns std::tuple -> bool, std::vector<float> (pos0, pos1)

- read motors angular velocity
  > controller.readVel() // returns std::tuple -> bool, std::vector<float> (vel0, vel1)

- read motor maximum commandable angular velocity
  > controller.getMaxVel(motor_no) // returns std::tuple -> bool, float
  > maxVel0 or maxVel1 based on the specified motor number

- while these function above help communicate with the already configure EPMC module, more examples of advanced funtions usage for parameter tuning can be found in the [epmc_setup_application](https://github.com/robocre8/epmc_setup_application) source code

#

## example code - two_motor_control.cpp

```
/two_motor_control
├── include/
└── src
    ├── two_motor_control.cpp
 CMakeLists.txt
```

```cpp
#include <sstream>
#include <iostream>
#include <unistd.h>

#include <chrono>

#include <iomanip>

#include "epmc_serial.hpp"

epmc_serial::EPMCSerialClient controller;

void delay_ms(unsigned long milliseconds)
{
  usleep(milliseconds * 1000);
}

int main(int argc, char **argv)
{
  // variable for communication
  bool success; float val0;
  std::vector<float> val;

  float pos0=0.0, pos1=0.0;
  float vel0=0.0, vel1=0.0;

  // [4 rev/sec, 2 rev/sec, 1 rev/sec, 0.5 rev/sec]
  float targetVel[] = {1.571, 3.142, 6.284, 12.568}; // in rad/sec
  float vel = targetVel[1]; // in rad/sec
  float v = 0.0;

  auto cmdTime = std::chrono::system_clock::now();
  std::chrono::duration<double> cmdDuration;
  float cmdTimeInterval = 5.0;

  auto readTime = std::chrono::system_clock::now();
  std::chrono::duration<double> readDuration;
  float readTimeInterval = 0.02; // 50Hz

  // 50Hz comm setup
  std::string serial_port = "/dev/ttyACM0";
  int serial_baudrate = 115200;
  int serial_timeout_ms = 18; // value < 20ms (50 Hz comm)
  controller.supportedNumOfMotors(epmc_serial::SupportedNumOfMotors::TWO);
  controller.connect(serial_port, serial_baudrate, serial_timeout_ms);
  
  success = controller.clearDataBuffer();
  controller.writeSpeed(v, v);

  int motor_cmd_timeout_ms = 10000;
  controller.setCmdTimeout(motor_cmd_timeout_ms); // set motor command timeout
  std::tie(success, val0) = controller.getCmdTimeout();
  if (success) { // only update if read was successfull
    motor_cmd_timeout_ms = val0;
    std::cout << "motor command timeout: " << motor_cmd_timeout_ms << " ms" << std::endl;
  } else {
    std::cerr << "ERROR: could not read motor command timeout" << std::endl;
  }

  bool sendHigh = true;

  cmdTime = std::chrono::system_clock::now();
  readTime = std::chrono::system_clock::now();

  while (true)
  {

    cmdDuration = (std::chrono::system_clock::now() - cmdTime);
    if (cmdDuration.count() > cmdTimeInterval)
    {
      if (sendHigh)
      {
        v = vel;
        controller.writeSpeed(v, v);
        vel *= -1;
        sendHigh = false;
      }
      else
      {
        v = 0.0;
        controller.writeSpeed(v, v);
        sendHigh = true;
      }

      cmdTime = std::chrono::system_clock::now();
    }

    readDuration = (std::chrono::system_clock::now() - readTime);
    if (readDuration.count() > readTimeInterval)
    {

      // controller.writeSpeed(v, v);
      std::tie(success, val) = controller.readMotorData();
      if (success) { // only update if read was successfull
        pos0 = val.at(0); pos1 = val.at(1);
        vel0 = val.at(2); vel1 = val.at(3);

        std::cout << "----------------------------------" << std::endl;
        std::cout << "motor0_readings: [" << pos0 << "," << vel0 << "]" << std::endl;
        std::cout << "motor1_readings: [" << pos1 << "," << vel1 << "]" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << std::endl;
      }
      else {
        std::cout << "----------------------------------" << std::endl;
        std::cout << "error reading motor data" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << std::endl;
      }
      

      readTime = std::chrono::system_clock::now();
    }
  }
}
```

#

## example CMakeLists.txt
```txt
  cmake_minimum_required(VERSION 3.16)
  project(two_motor_control LANGUAGES CXX)

  find_package(epmc_serial REQUIRED)

  add_executable(twomotor_control src/two_motor_control.cpp)
  target_link_libraries(two_motor_control epmc_serial::epmc_serial)

```


## Basic Library functions and usage (Four Motor Support Control)

- connect to epmc_driver shield module
  > epmc_serial::EPMCSerialClient controller
  >
  >  _//ensure you set/call **supportedNumOfMotors()** before **connect()** as below:_
  >
  > controller.supportedNumOfMotors(epmc_serial::SupportedNumOfMotors::FOUR)
  >
  > controller.connect("port_name or port_path")

- clear speed, position, e.t.c data buffer on the EPMC module
  > controller.clearDataBuffer() // returns bool -> success

- send target angular velocity command
  > controller.writeSpeed(motor0_TargetVel, motor1_TargetVel, motor2_TargetVel, motor3_TargetVel)

- send PWM command
  > controller.writePWM(motor0_PWM, motor1_PWM, motor2_PWM, motor3_PWM)

- set motor command timeout
  > controller.setCmdTimeout(timeout_ms)

- get motor command timeout
  > controller.getCmdTimeout() // returns std::tuple -> bool, float

- read motors angular position and angular velocity
  > controller.readMotorData() // returns std::tuple -> bool, std::vector<float> (pos0, pos1, pos2, pos3, vel0, vel1, vel2, vel3)

- read motors angular position
  > controller.readPos() // returns std::tuple -> bool, std::vector<float> (pos0, pos1, pos2, pos3)

- read motors angular velocity
  > controller.readVel() // returns std::tuple -> bool, std::vector<float> (vel0, vel1, vel2, vel3)

- read motor maximum commandable angular velocity
  > controller.getMaxVel(motor_no) // returns std::tuple -> bool, float
  > maxVel0 or maxVel1 based on the specified motor number

- while these function above help communicate with the already configure EPMC module, more examples of advanced funtions usage for parameter tuning can be found in the [epmc_setup_application](https://github.com/robocre8/epmc_setup_application) source code

#

## example code - four_motor_control.cpp

```
/four_motor_control
├── include/
└── src
    ├── four_motor_control.cpp
 CMakeLists.txt
```

```cpp
#include <sstream>
#include <iostream>
#include <unistd.h>

#include <chrono>

#include <iomanip>

#include "epmc_serial.hpp"

epmc_serial::EPMCSerialClient controller;

void delay_ms(unsigned long milliseconds)
{
  usleep(milliseconds * 1000);
}

int main(int argc, char **argv)
{
  // variable for communication
  bool success; float val0;
  std::vector<float> val;

  float pos0=0.0, pos1=0.0, pos2=0.0, pos3=0.0;
  float vel0=0.0, vel1=0.0, vel2=0.0, vel3=0.0;

  // [4 rev/sec, 2 rev/sec, 1 rev/sec, 0.5 rev/sec]
  float targetVel[] = {1.571, 3.142, 6.284, 12.568}; // in rad/sec
  float vel = targetVel[1]; // in rad/sec
  float v = 0.0;

  auto cmdTime = std::chrono::system_clock::now();
  std::chrono::duration<double> cmdDuration;
  float cmdTimeInterval = 5.0;

  auto readTime = std::chrono::system_clock::now();
  std::chrono::duration<double> readDuration;
  float readTimeInterval = 0.02; // 50Hz

  // 50Hz comm setup
  std::string serial_port = "/dev/ttyUSB0";
  int serial_baudrate = 115200;
  int serial_timeout_ms = 18; // value < 20ms (50 Hz comm)
  controller.supportedNumOfMotors(epmc_serial::SupportedNumOfMotors::FOUR);
  controller.connect(serial_port, serial_baudrate, serial_timeout_ms);
  
  success = controller.clearDataBuffer();
  controller.writeSpeed(v, v, v, v);

  int motor_cmd_timeout_ms = 10000;
  controller.setCmdTimeout(motor_cmd_timeout_ms); // set motor command timeout
  std::tie(success, val0) = controller.getCmdTimeout();
  if (success) { // only update if read was successfull
    motor_cmd_timeout_ms = val0;
    std::cout << "motor command timeout: " << motor_cmd_timeout_ms << " ms" << std::endl;
  } else {
    std::cerr << "ERROR: could not read motor command timeout" << std::endl;
  }

  bool sendHigh = true;

  cmdTime = std::chrono::system_clock::now();
  readTime = std::chrono::system_clock::now();

  while (true)
  {

    cmdDuration = (std::chrono::system_clock::now() - cmdTime);
    if (cmdDuration.count() > cmdTimeInterval)
    {
      if (sendHigh)
      {
        v = vel;
        controller.writeSpeed(v, v, v, v);
        vel *= -1;
        sendHigh = false;
      }
      else
      {
        v = 0.0;
        controller.writeSpeed(v, v, v, v);
        sendHigh = true;
      }

      cmdTime = std::chrono::system_clock::now();
    }

    readDuration = (std::chrono::system_clock::now() - readTime);
    if (readDuration.count() > readTimeInterval)
    {

      // controller.writeSpeed(v, v, v, v);
      std::tie(success, val) = controller.readMotorData();
      if (success) { // only update if read was successfull
        pos0 = val.at(0); pos1 = val.at(1); pos2 = val.at(2); pos3 = val.at(3);
        vel0 = val.at(4); vel1 = val.at(5); vel2 = val.at(6); vel3 = val.at(7);

        std::cout << "----------------------------------" << std::endl;
        std::cout << "motor0_readings: [" << pos0 << "," << vel0 << "]" << std::endl;
        std::cout << "motor1_readings: [" << pos1 << "," << vel1 << "]" << std::endl;
        std::cout << "motor2_readings: [" << pos2 << "," << vel2 << "]" << std::endl;
        std::cout << "motor3_readings: [" << pos3 << "," << vel3 << "]" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << std::endl;
      }
      else {
        std::cout << "----------------------------------" << std::endl;
        std::cout << "error reading motor data" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << std::endl;
      }
      

      readTime = std::chrono::system_clock::now();
    }
  }
}
```

#

## example CMakeLists.txt
```txt
  cmake_minimum_required(VERSION 3.16)
  project(four_motor_control LANGUAGES CXX)

  find_package(epmc_serial REQUIRED)

  add_executable(four_motor_control src/four_motor_control.cpp)
  target_link_libraries(four_motor_control epmc_serial::epmc_serial)

```

## how to build your sample package
- build your sample code.
  > cd into the root directory
  >
  > mkdir build (i.e create a folder named build)
  >
  > enter the following command in the terminal in the root folder:
    ````
    cmake -B ./build/
    ````
    ````
    cmake --build ./build/
    ````
    ````
    ./build/two_motor_control
    ````
    OR
    ````
    ./build/four_motor_control
    ````
