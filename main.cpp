#include "serial.hpp"

int main()
{
	Serial serial;
	cout << "1. Search for avaiable COM  " << "2.Exit" << endl;
	serial.get_serial_port();

	cout << "Select port" << endl;
	serial.select_serial_port();

	//cout << "Set communication type: "<< "1.SPI "<<"2.I2C" << endl;
	//serial.set_communication_type();
	while (serial.stop == 0)
	{
		cout << "1.Read Eeprom " << "2.Write Eeprom " << "3.Close COM" << endl;
		serial.select_operation();
		serial.flag = false;
	}	
	return 0;
}  