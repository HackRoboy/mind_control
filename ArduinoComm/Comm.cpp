/*
 * Comm.cpp
 *
 *  Created on: 29-04-2017
 *      Author: mayank
 */
#include <SerialStream.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string>

 using namespace std;
 using namespace LibSerial ;

int main( int    argc, char** argv  ){
     //
     // Open the serial port.
     //
     SerialStream serial_port ;
     char c;
     serial_port.Open( "/dev/ttyACM0" ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
                   << "Error: Could not open serial port."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Set the baud rate of the serial port.
     //
     serial_port.SetBaudRate( SerialStreamBuf::BAUD_57600 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the baud rate." <<
std::endl ;
         exit(1) ;
     }
     //
     // Set the number of data bits.
     //
     serial_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the character size." <<
std::endl ;
         exit(1) ;
     }
     //
     // Disable parity.
     //
     serial_port.SetParity( SerialStreamBuf::PARITY_NONE ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not disable the parity." <<
std::endl ;
         exit(1) ;
     }
     //
     // Set the number of stop bits.
     //
     serial_port.SetNumOfStopBits( 1 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the number of stop bits."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Turn off hardware flow control.
     //
     serial_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not use hardware flow control."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Do not skip whitespace characters while reading from the
     // serial port.
     //
     // serial_port.unsetf( std::ios_base::skipws ) ;
     //
     // Wait for some data to be available at the serial port.
     //
     //
     // Keep reading data from serial port and print it to the screen.
     //
  // Wait for some data to be available at the serial port.
     //
     while( serial_port.rdbuf()->in_avail() == 0 )
     {
         usleep(100) ;
     }

     	 ofstream writer;
     	 writer.open("Log.txt");
     char out_buf[] = "check";
     serial_port.write(out_buf, 5);
     //while( serial_port.rdbuf()->in_avail() > 0 )
     while(1)
     {
         char next_byte;
         serial_port.get(next_byte);
         std::cerr << next_byte;
         	 	 	writer<<next_byte;

     }
     std::cerr << std::endl ;
     	 	 writer.close();
     return EXIT_SUCCESS ;
}
