
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
  // buffer variable for communication
  bool success; float buffer0;
  std::vector<float> buffer;

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
  controller.connect(serial_port, serial_baudrate, serial_timeout_ms);
  
  success = controller.clearDataBuffer();
  controller.writeSpeed(v, v, v, v);

  int motor_cmd_timeout_ms = 10000;
  controller.setCmdTimeout(motor_cmd_timeout_ms); // set motor command timeout
  std::tie(success, buffer0) = controller.getCmdTimeout();
  if (success) { // only update if read was successfull
    motor_cmd_timeout_ms = buffer0;
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
      std::tie(success, buffer) = controller.readMotorData();
      if (success) { // only update if read was successfull
        pos0 = buffer.at(0); pos1 = buffer.at(1); pos2 = buffer.at(2); pos3 = buffer.at(3);
        vel0 = buffer.at(4); vel1 = buffer.at(5); vel2 = buffer.at(6); vel3 = buffer.at(7);

        std::cout << "----------------------------------" << std::endl;
        std::cout << "motorX_readings: [position, velocity]" << std::endl;
        std::cout << "motor0_readings: [" << pos0 << ", " << vel0 << "]" << std::endl;
        std::cout << "motor1_readings: [" << pos1 << ", " << vel1 << "]" << std::endl;
        std::cout << "motor2_readings: [" << pos2 << ", " << vel2 << "]" << std::endl;
        std::cout << "motor3_readings: [" << pos3 << ", " << vel3 << "]" << std::endl;
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