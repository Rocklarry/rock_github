################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../AlarmRecord.o \
../Gprs.o \
../KuoZhan.o \
../audio_play.o \
../card.o \
../cdzChargeRecord.o \
../cdzDataType.o \
../cdzProtocol.o \
../convertion.o \
../display.o \
../main.o \
../meter.o \
../mycom.o \
../mytcpip.o \
../sql.o \
../timer.o 

C_SRCS += \
../AlarmRecord.c \
../Card.c \
../Gprs.c \
../KuoZhanBoard.c \
../Meter.c \
../audio_play.c \
../cdzChargeRecord.c \
../cdzDataType.c \
../cdzProtocol.c \
../convertion.c \
../display.c \
../main.c \
../mycom.c \
../mytcpip.c \
../sql.c \
../timer.c 

OBJS += \
./AlarmRecord.o \
./Card.o \
./Gprs.o \
./KuoZhanBoard.o \
./Meter.o \
./audio_play.o \
./cdzChargeRecord.o \
./cdzDataType.o \
./cdzProtocol.o \
./convertion.o \
./display.o \
./main.o \
./mycom.o \
./mytcpip.o \
./sql.o \
./timer.o 

C_DEPS += \
./AlarmRecord.d \
./Card.d \
./Gprs.d \
./KuoZhanBoard.d \
./Meter.d \
./audio_play.d \
./cdzChargeRecord.d \
./cdzDataType.d \
./cdzProtocol.d \
./convertion.d \
./display.d \
./main.d \
./mycom.d \
./mytcpip.d \
./sql.d \
./timer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-none-linux-gnueabi-gcc -lpthread  -lsqlite3 -L/home/sqlite/sqlite-arm/_install/lib -I/home/sqlite/sqlite-arm/_install/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


