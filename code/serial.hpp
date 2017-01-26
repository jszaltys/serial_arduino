#include <Windows.h>
#include <wchar.h>
#include <tchar.h>
#include <chrono>
#include "reader.hpp"

using namespace chrono;

void delay(const unsigned long &millis)
{
	auto t0 = high_resolution_clock::now();
	while (duration_cast<chrono::milliseconds>(high_resolution_clock::now() - t0).count() <= millis){}
}
enum INSTRUCTIONS
{
	READ_AND_SEND_EEPROM_REQUEST = 1,
	WRITE_EEPROM_REQUEST
};
enum COMMUNICATION_TYPE
{
	SPI = 1,
	I2C
};
struct Frames
{
	static const char write_eeprom;
	static const char read_and_send_eeprom;
	static const char spi_type;
	static const char i2c_type;
};

const char Frames::read_and_send_eeprom =	0x02;
const char Frames::write_eeprom =			0x03;
const char Frames::spi_type =				0x00;
const char Frames::i2c_type =				0x01;

struct Serial
{
	void set_communication_type()
	{
		while (flag == false)
		{
			cin >> get_value;
			switch (get_value)
			{
			case SPI:
				write(port, Frames::spi_type);
				delay(10);
				read(port, complete_flag, 1);
				complete_flag[0] = 0x00;
				flag = true;
				break;

			case I2C:
				write(port, Frames::i2c_type);
				delay(10);
				read(port, complete_flag, 1);
				complete_flag[0] = 0x00;
				flag = true;
				break;
			default:
				cout << "Bad selected option try again!" << endl;
				break;
			}		
		}
		flag = false;
	}
	void get_serial_port()
	{
		while (flag == false)
		{
			cin >> get_value;
			switch (get_value)
			{
			case 1:
				for (unsigned int i = 0; i < 255; ++i)
				{
					wsprintf(com, _T("\\\\.\\COM%d"), i);
					HANDLE port = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
					if (INVALID_HANDLE_VALUE != port)
					{
						com_id.push_back(i);
						CloseHandle(port);
					}
				}
				cout << "Avaiable serial ports: " << endl;
				for (unsigned int i = 0; i < com_id.size(); ++i)
					cout << to_string(i + 1) + ".COM" + to_string(com_id[i]) << "  ";
				cout << endl;
				flag = true;
				break;
			case 2:
				flag = true;
				break;
			default:
				cout << "Bad number try again!" << endl;
				break;
			}
		}
		flag = false;
	}
	void select_serial_port()
	{
		while (flag == false)
		{
			cin >> get_value;
			if (get_value > com_id.size() || get_value <= 0)
				cout << "Bad port try again!" << endl;
			else
			{
				wsprintf(com, _T("\\\\.\\COM%d"), com_id[get_value - 1]);
				if (INVALID_HANDLE_VALUE != port)
					cout << "COM" + to_string(com_id[get_value - 1]) << " Opened!" << endl;
				flag = true;
			}
		}
		flag = false;
	}
	void select_operation()
	{
		char data[0x04][0x10];
		while (flag == false)
		{
			cin >> get_value;
			switch (get_value)
			{
			case READ_AND_SEND_EEPROM_REQUEST:
				port = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
				write(port, Frames::read_and_send_eeprom);	
				for (unsigned int i = 0; i < 0x04; ++i)
				{
					read(port, data[i], 0x10);
					delay(10);
				}
				reader.write_data<0x04, 0x10>(data, "spi_i2c_eeprom_data.txt");
				CloseHandle(port);
				cout << "Reading Eeprom Complete!" << endl;
				flag = true;
				break;
			case WRITE_EEPROM_REQUEST:
				reader.read_data("spi_i2c_eeprom_data_to_send.txt");
				port = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
				for (unsigned int i = 0; i < 4; ++i)
				{
					reader.data[i].insert(reader.data[i].begin(), Frames::write_eeprom);
					write(port, reader.data[i].data(), 0x11);

					delay(100);
					read(port, complete_flag, 0x01);
					delay(100);
				}
				cout << "Writing Eeprom Complete!" << endl;
				CloseHandle(port);
				flag = true;
				break;
			case 3:
				stop = 1;
				cout << "Port Closed!" << endl;
				flag = true;
				break;
			default:
				cout << "Bad selected option try again!" << endl;
				break;
			}
		}
	}
	Reader reader;
	BOOL read(HANDLE &port, char* ret_val, DWORD length)
	{
		DCB       dcb;
		BOOL      retVal = 0x00;
		DWORD     dwBytesTransferred = 0x00;
		DWORD     dwCommModemStatus = 0x00;
		DWORD	  to_read = 0x00;
		
		if (!GetCommState(port, &dcb))
			return  0x0;

		dcb.BaudRate = CBR_115200;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;

		if (!SetCommState(port, &dcb))
			return  0x00;

		SetCommMask(port, EV_RXCHAR | EV_ERR);

		while (1)
		{
			ClearCommError(port, &errors, &status);
			if (status.cbInQue != 0) break;
			Sleep(90*length);
		}
		retVal = ReadFile(port, ret_val, length, &dwBytesTransferred, 0);
	}
	BOOL write(HANDLE &port,const char data)
	{
		DCB dcb;
		DWORD byteswritten;

		if (!GetCommState(port, &dcb))
			return false;

		dcb.BaudRate = CBR_115200;
		dcb.ByteSize = 8;                  
		dcb.Parity = NOPARITY;            
		dcb.StopBits = ONESTOPBIT;       
		if (!SetCommState(port, &dcb))
			return  false;

		BOOL retVal = WriteFile(port, &data, 1, &byteswritten, NULL);
		return  retVal;
	}
	BOOL write(HANDLE &port, const char *data,const int &length)
	{
		DCB dcb;
		DWORD byteswritten;
		if (!GetCommState(port, &dcb))
			return false;

		dcb.BaudRate = CBR_115200;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		if (!SetCommState(port, &dcb))
			return  false;

		BOOL retVal = WriteFile(port, data, length, &byteswritten, NULL);
		return  retVal;
	}

	char complete_flag[1];
	std::vector<int> com_id;
	bool flag = false;
	int get_value = 0;
	HANDLE port = NULL;
	TCHAR com[11];
	char write_data[16];
	COMSTAT status;
	DWORD errors;
	int stop = 0;
};


