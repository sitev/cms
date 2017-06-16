# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/builderModule.cpp \
../src/cms.cpp \
../src/connectionPool.cpp \
../src/modal.cpp \
../src/siteManager.cpp \
../src/taskModule.cpp \
../src/userModule.cpp \
../src/webModule.cpp \
../src/webPage.cpp \
../src/webSite.cpp \
../src/webstudio.cpp \
../src/websurf.cpp \
../src/webTemplate.cpp \
../src/widget.cpp 

OBJS += \
./src/builderModule.o \
./src/cms.o \
./src/connectionPool.o \
./src/modal.o \
./src/siteManager.o \
./src/taskModule.o \
./src/userModule.o \
./src/webModule.o \
./src/webPage.o \
./src/webSite.o \
./src/webstudio.o \
./src/websurf.o \
./src/webTemplate.o \
./src/widget.o

CPP_DEPS += \
./src/builderModule.d \
./src/cms.d \
./src/connectionPool.d \
./src/modal.d \
./src/siteManager.d \
./src/taskModule.d \
./src/userModule.d \
./src/webModule.d \
./src/webPage.d \
./src/webSite.d \
./src/webstudio.d \
./src/websurf.d \
./src/webTemplate.d \
./src/widget.d

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I"../../core/src" -I"../../qqq/src" -I"../../mysql/src" -I"../../lib/mysql-connector-c/include" -I"../../webserver/src" -I"/usr/include/x86_64-linux-gnu" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


