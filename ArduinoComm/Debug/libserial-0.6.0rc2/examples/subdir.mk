################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libserial-0.6.0rc2/examples/read_port.cpp \
../libserial-0.6.0rc2/examples/write_port.cpp 

OBJS += \
./libserial-0.6.0rc2/examples/read_port.o \
./libserial-0.6.0rc2/examples/write_port.o 

CPP_DEPS += \
./libserial-0.6.0rc2/examples/read_port.d \
./libserial-0.6.0rc2/examples/write_port.d 


# Each subdirectory must supply rules for building sources it contributes
libserial-0.6.0rc2/examples/%.o: ../libserial-0.6.0rc2/examples/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


