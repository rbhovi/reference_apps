---
grand_parent: Reference Applications
parent: SAM E51 Curiosity Nano Evaluation Kit
title: microSD Click Example
nav_order: 7
---

<img src = "images/microchip_logo.png">
<img src = "images/microchip_mplab_harmony_logo_small.png">

# microSD Click example on SAM E51 Curiosity Nano Evaluation Kit
<h2 align="center"> <a href="https://github.com/Microchip-MPLAB-Harmony/reference_apps/releases/latest/download/microsd.zip" > Download </a> </h2>

-----
## Description:

> This application showcases the capabilities of the microSD Click module. It demonstrates various file operations that can be performed on an SD card.

### File Operations:

-	Creating a file
-	Writing data to the file
-	Reading data from the file
-	Creating a directory

These operations are accomplished using the MPLAB Harmony File System, SDSPI driver, and SPI PLIB.

### File System Operations on the SD Card:

1. Create a file and write data to it
    - The application creates a file named **"mchp_sd.txt"**
    - The file is opened in write mode
    - A text message is written to the file

2. Create a directory and read data from a file
    - The application creates a directory named **Dir1** in the root directory
    - A new file named **"mchp_sd2.txt"** is created inside **"Dir1"**
    - The content of **"mchp_sd.txt"** is read and copied into **"mchp_sd2.txt"**


### File system layer uses:

- SDSPI Driver to communicate to SD Card over SPI interface.

## Modules/Technology Used:
- Peripheral Modules
	- Core
	- GPIO
	- SERCOM (SPI)
    - TC0
    - Timer System Service
    - SDSPI Driver
    - File System Service

## Hardware Used:

- [SAM E51 Curiosity Nano Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/EV76S68A)
- [Curiosity Nano Base for Click Boards](https://www.microchip.com/Developmenttools/ProductDetails/AC164162)
- [MikroElektronika microSD Click](https://www.mikroe.com/microsd-click)

## Software/Tools Used:
<span style="color:blue"> This project has been verified to work with the following versions of software tools:</span>  

Refer [Project Manifest](./firmware/src/config/sam_e51_cnano/harmony-manifest-success.yml) present in harmony-manifest-success.yml under the project folder *firmware/src/config/sam_e51_cnano*  
- Refer the [Release Notes](../../../../release_notes.md#development-tools) to know the **MPLAB X IDE** and **MCC** Plugin version. Alternatively, [Click Here](https://github.com/Microchip-MPLAB-Harmony/reference_apps/blob/master/release_notes.md#development-tools).

<span style="color:blue"> Because Microchip regularly update tools, occasionally issue(s) could be discovered while using the newer versions of the tools. If the project doesn’t seem to work and version incompatibility is suspected, It is recommended to double-check and use the same versions that the project was tested with. </span> To download original version of MPLAB Harmony v3 packages, refer to document [How to Use the MPLAB Harmony v3 Project Manifest Feature](https://ww1.microchip.com/downloads/en/DeviceDoc/How-to-Use-the-MPLAB-Harmony-v3-Project-Manifest-Feature-DS90003305.pdf)

## Setup:
- Connect the SAM E51 Curiosity Nano Evaluation Kit to the Host PC as a USB Device through a Type-A male to micro-B USB cable connected to Micro-B USB (Debug USB) port

  <img src = "images/setup.png">

## Programming hex file:
The pre-built hex file can be programmed by following the below steps.  

### Steps to program the hex file
- Open MPLAB X IDE
- Close all existing projects in IDE, if any project is opened.
- Go to File -> Import -> Hex/ELF File
- In the "Import Image File" window, Step 1 - Create Prebuilt Project, Click the "Browse" button to select the prebuilt hex file.
- Select Device has "ATSAME51J20A"
- Ensure the proper tool is selected under "Hardware Tool"
- Click on Next button
- In the "Import Image File" window, Step 2 - Select Project Name and Folder, select appropriate project name and folder
- Click on Finish button
- In MPLAB X IDE, click on "Make and Program Device" Button. The device gets programmed in sometime
- Follow the steps in "Running the Demo" section below


## Programming/Debugging Application Project:
- Open the project (microsd/firmware/sam_e51_cnano.X) in MPLAB X IDE
- Ensure "SAM E51 Curiosity Nano" is selected as hardware tool to program/debug the application
- Build the code and program the device by clicking on the "Make and Program Device" button in MPLAB X IDE tool bar
- Follow the steps in "Running the Demo" section below

## Running the Demo:
- After power up, when the demo is running, The microSD-click shows as below  
<img src = "images/demo_run.png" align="middle">
- Once the application successfully creates **"mchp_sd.txt"** and **"Dir1/mchp_sd2.txt"** LED0 on the board will light up

The success of the following file operations can be verified by connecting the SD card to a PC and examining the contents of **"mchp_sd.txt"** and **"Dir1/mchp_sd2.txt."** The contents should match in order to consider them successful:

- Creating a file
- Writing data to the file
- Reading data from the file
- Creating a directory

## Instructions to add microSD Click functionality to your application:

You could use this demonstration as an example to add microSD Click functionality to your MPLAB Harmony v3 based application. Follow the below steps.  
1. If you haven't downloaded the microSD Click demo yet [Click Here](https://github.com/Microchip-MPLAB-Harmony/reference_apps/releases/latest/download/microsd.zip) to download, otherwise go to next step
2. Unzip the downloaded .zip file  
3. From the unzipped folder microsd/firmware/src, copy the folder **click_routines** to the folder firmware/src under your MPLAB Harmony v3 application project  
4. Open MPLAB X IDE  
5. Open your application project  
6. In the project explorer, Right click on folder **Header Files**  
   and add a sub folder **click_routines** by selecting "Add Existing Items from Folders..."  
	 <img src = "images/header_add.png">  

- Click on "Add Folder..." button  
	<img src = "images/header_add2.png">  

- Select the "click_routines" folder and select "Files of Types" as Header Files  
	<img src = "images/header_add3.png">

- Click on "Add" button to add the selected folder  
	<img src = "images/header_add4.png">

- The microSD-click example header files gets added to your project  
	<img src = "images/header_add5.png">

7. In the project explorer, Right click on folder **Source Files**  
   and add a sub folder **click_routines** by selecting "Add Existing Items from Folders..."   
	 <img src = "images/source_add.png">  

 - Click on "Add Folder..." button  
 	<img src = "images/source_add2.png">  

 - Select the "click_routines" folder and select "Files of Types" as Source Files  
 	<img src = "images/source_add3.png">

 - Click on "Add" button to add the selected folder  
 	<img src = "images/source_add4.png">

 - The microSD-click example source files gets added to your project  
 	<img src = "images/source_add5.png">

8. The microSD Click example uses the **SPI**, **SDSPI Driver**,**TC0**peripherals and **PORT** pins . The configuration of these peripherals for your application depends on the 32-bit MCU and development board you are using.

	- **Configure SPI**:
 		- Add the SPI peripheral block to the MCC project graph  
		- Configure SPI Pins using MCC Pin configuration Window  
		The SPI configuration depends on
 			- 32-bit MCU
 			- 32-bit MCU development board
 			- The socket on which you have mounted the microSD-click board  
 		*Example: The microSD-click example on SAM E51 Curiosity Nano Evaluation Kit uses mikroBUS socket #1 on the Curiosity Nano Base for Click boards to mount the microSD-click board. The SPI lines from MCU coming to this socket are from the SERCOM1 peripheral on the MCU.*  
		*MCC Project Graph - SPI configuration*  
		<img src = "images/spi_config.png">  

		*MCC Pin Configurator - SPI pin configuration*  
		<img src = "images/spi_pins.png">  
        
    - **Configure SDSPI Driver**:
 		- Add the SDSPI Driver peripheral block to the MCC project graph  
		- Add Instance 0 to the SDSPI Driver block and connect SERCOM 1 and File System to Instance 0
		  
		*MCC Project Graph - SDSPI configuration*
		<img src = "images/sdspi_config.png">
        
    - **Configure File System**:
 		- Add the File System block to the MCC project graph
		  
		*MCC Project Graph - File System configuration*
		<img src = "images/fs_config.png">

	- **Configure Timer**:
		- Configure Timer peripheral block in the MCC project graph  
		The Timer configuration depends on
 			- 32-bit MCU  
		*Example: The microSD-click example on SAM E51 Curiosity Nano Evaluation Kit uses TC0 timer module on the MCU to implement the time requirement of microSD-click routines.*  
		*MCC Project Graph - TC0 configuration*  
		<img src = "images/tc0.png">  
        *MCC Project Graph - Timer configuration*  
		<img src = "images/timer.png">  
        
    - **Configure Core**:
		- Add the Core block to the MCC project graph 
		*MCC Project Graph - Core configuration*  
		<img src = "images/core.png">

	- **Configure PORT Pins**:
		- Configure PORT pins needed by microSD-click in the MCC Pin Configurator  
		The PORT pin configuration depends on
			- 32-bit MCU
			- 32-bit MCU development board
			- The socket on which you have mounted the microSD-click board   
		*Example: The PORT pin configuration for the microSD-click example on SAM E51 Curiosity Nano Evaluation Kit is below.*  
		*MCC Pin Configurator - PORT pin configuration*  
		<img src = "images/pins.png">
        
9. The **click_routines** folder contain an example C source file **microsd_example.c**. You could use **microsd_example.c** as a reference to add microSD-click functionality to your application.

## Comments:
- Reference Training Module:  
	1. [Getting Started with Harmony v3 Peripheral Libraries on SAM D5x/E5x MCUs](https://microchipdeveloper.com/harmony3:same54-getting-started-training-module)  
	2. [Low Power Application on SAM E54 (Arm® Cortex® M4) MCUs Using MPLAB® Harmony v3 Peripheral Libraries](https://microchipdeveloper.com/harmony3:low-power-application-on-sam-e54)
	3. [Getting Started with Harmony v3 Drivers on SAM E54 MCUs Using FreeRTOS](https://microchipdeveloper.com/harmony3:same54-getting-started-tm-drivers-freertos#Steps_anchor)
- This application demo builds and works out of box by following the instructions above in "Running the Demo" section. If you need to enhance/customize this application demo, you need to use the MPLAB Harmony v3 Software framework. Refer links below to setup and build your applications using MPLAB Harmony.
	- [How to Setup MPLAB Harmony v3 Software Development Framework](https://ww1.microchip.com/downloads/en/DeviceDoc/How_to_Setup_MPLAB_%20Harmony_v3_Software_Development_Framework_DS90003232C.pdf)
	- [How to Build an Application by Adding a New PLIB, Driver, or Middleware to an Existing MPLAB Harmony v3 Project](http://ww1.microchip.com/downloads/en/DeviceDoc/How_to_Build_Application_Adding_PLIB_%20Driver_or_Middleware%20_to_MPLAB_Harmony_v3Project_DS90003253A.pdf)  
	- <span style="color:blue"> **MPLAB Harmony v3 is also configurable through MPLAB Code Configurator (MCC). Refer to the below links for specific instructions to use MPLAB Harmony v3 with MCC.**</span>
		- [Create a new MPLAB Harmony v3 project using MCC](https://microchipdeveloper.com/harmony3:getting-started-training-module-using-mcc)
		- [Update and Configure an Existing MHC-based MPLAB Harmony v3 Project to MCC-based Project](https://microchipdeveloper.com/harmony3:update-and-configure-existing-mhc-proj-to-mcc-proj)
		- [Getting Started with MPLAB Harmony v3 Using MPLAB Code Configurator](https://www.youtube.com/watch?v=KdhltTWaDp0)
		- [MPLAB Code Configurator Content Manager for MPLAB Harmony v3 Projects](https://www.youtube.com/watch?v=PRewTzrI3iE)	




## Revision:
- v1.6.0 - Released demo application
