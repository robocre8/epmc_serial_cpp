
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