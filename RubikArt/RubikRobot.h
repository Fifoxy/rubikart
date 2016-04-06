#ifndef RUBIKROBOT_HEADER
#define RUBIKROBOT_HEADER	

#include <string>
#include <iostream>
#include <windows.h>
#include <commctrl.h>

/* 3 moteurs */

class RubikRobot {
	private:
		std::string port;
		HANDLE hComm;
		bool sendSByte(unsigned char);
		unsigned char readSByte();
		bool openPort();
		bool closePort();
		bool setupPort();

	public:
		RubikRobot();
		RubikRobot(std::string);
		void send(unsigned char);
		unsigned char read();
		bool initPort();
		void setPort(std::string);
		std::string getPort();

};

#endif