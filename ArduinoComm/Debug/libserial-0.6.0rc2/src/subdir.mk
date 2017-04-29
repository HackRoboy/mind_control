################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../libserial-0.6.0rc2/src/SerialStream.cc \
../libserial-0.6.0rc2/src/SerialStreamBuf.cc 

CPP_SRCS += \
../libserial-0.6.0rc2/src/PosixSignalDispatcher.cpp \
../libserial-0.6.0rc2/src/SerialPort.cpp 

CC_DEPS += \
./libserial-0.6.0rc2/src/SerialStream.d \
./libserial-0.6.0rc2/src/SerialStreamBuf.d 

OBJS += \
./libserial-0.6.0rc2/src/PosixSignalDispatcher.o \
./libserial-0.6.0rc2/src/SerialPort.o \
./libserial-0.6.0rc2/src/SerialStream.o \
./libserial-0.6.0rc2/src/SerialStreamBuf.o 

CPP_DEPS += \
./libserial-0.6.0rc2/src/PosixSignalDispatcher.d \
./libserial-0.6.0rc2/src/SerialPort.d 


# Each subdirectory must supply rules for building sources it contributes
libserial-0.6.0rc2/src/%.o: ../libserial-0.6.0rc2/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

libserial-0.6.0rc2/src/%.o: ../libserial-0.6.0rc2/src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


