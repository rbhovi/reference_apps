---
grand_parent: 32-bit MCU Reference Applications
parent: SAM D21 Curiosity Nano Evaluation Kit
title: Amazon Alexa Connect (ACK) kit port OTA and Applications
nav_order: 5
---
<img src = "images/microchip_logo.png">
<img src = "images/microchip_mplab_harmony_logo_small.png">

# Amazon Alexa Connect Kit (ACK) SDK port, Smart Applications and Over the Air (OTA) firmware upgrade on SAM D21 Curiosity Nano

-----

## Description:

> This folder contains the SAM D21 microcontroller port for Amazon's Alexa Connect Kit (ACK) Software Development Kit (SDK) and smart applications
demonstrating the port. The folder also contains Over the Air (OTA) firmware upgrade feature for the smart Applications.

> SAM D21 is an low-power, high-performance Microchip's ARM® Cortex®-M0+ based flash microcontroller, it is ideal for a wide range of home automation, consumer, metering, and industrial applications. Microchip offers a low cost SAM D21 Curiosity Nano evaluation kit to evaluate SAMD21G17D microcontroller.

> Alexa Connect Kit (ACK) is Amazon's managed service that makes it easy to integrate Alexa into your product With ACK, you don't need to write an Alexa skill, manage a cloud service, or develop complex network and security firmware to connect your product to Alexa. Instead, you integrate the ACK module into your product. ACK provides the managed services, software, and tools you can use to develop and manage your Alexa-connected products on an ongoing basis. Users can benefit from Alexa device control and other features, such as Amazon's Wi-fi Simple Setup (WSS).

> The communication interface between SAM D21 and ACK module is through UART.

- This folder contains a set of applications (under folder "**applications**) which uses Amazon Alexa connect kit (ACK) SDK with host microcontroller SAM D21.  
  - The Application "**HelloWorld**" demonstrates turning ON/OFF of the development device (SAM D21 Curiosity Nano evaluation kit)
  - The Application "**SmartLight**" demonstrates controlling of smart light (LED0) on development device (SAM D21 Curiosity Nano evaluation kit)
  - The Application "**Microwave**" demonstrates controlling of a Microwave oven
  - The Application "**DashReplenishment**" demonstrates dashboard replenishment  
- The folder contains a folder "**bootloader**". The bootloader helps demonstrate OTA firmware upgrade feature.

This following bullet points provides links to the detailed topics:

- [Modules/Technology Used](#Modules-Technology-Used)
- [Hardware Used](#Hardware-Used)
- [Software/Tools Used](#software-Tools-Used)
- [Hardware Setup](#Hardware-setup)
- [Setting up the build platform](#setting-up-the-build-platform)
- [Project file structure](#project-structure)
- [Steps to program the hex file](#steps-to-program-the-hex-file)
- [Programming/Debugging Application Projects](#programming-smart-light-application-project)
- [Register your device](#register-your-device)
- [Running the demo](#running-the-demo)
- [OTA Architecture](#ota-architecture)
- [Setting up environment for OTA update](#setting-up-environment-for-OTA-update)
- [Building and upgrading the OTA application](#building-the-ota-application)
- [Comments](#comments)

## <a id="Modules-Technology-Used"> </a>
## Modules/Technology Used:
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="Hardware-Used"> </a>
## Hardware Used:
- [SAM D21 Curiosity Nano Evaluation Kit](https://www.microchip.com/Developmenttools/ProductDetails/DM320119)   
- [Espressif Alexa Connect Kit - ACK Module](https://developer.amazon.com/en-US/docs/alexa/ack/get-ack-development-kit.html)
- Connection wires  

## <a id="software-Tools-Used"> </a>
## Software/Tools Used:
<span style="color:blue"> This project has been verified to work with the following versions of software tools:</span>  

Refer [Demo Application specific Project Manifest]() present in harmony-manifest-success.yml under the project folder *firmware/src/config/default*  
- Refer the [Release Notes](../../../release_notes.md#development-tools) to know the **MPLAB X IDE** and **MCC** Plugin version.  

<span style="color:blue"> Because Microchip regularly update tools, occasionally issue(s) could be discovered while using the newer versions of the tools. If the project doesn’t seem to work and version incompatibility is suspected, It is recommended to double-check and use the same versions that the project was tested with. </span> To download original version of MPLAB Harmony v3 packages, refer to document [How to Use the MPLAB Harmony v3 Project Manifest Feature](https://ww1.microchip.com/downloads/en/DeviceDoc/How-to-Use-the-MPLAB-Harmony-v3-Project-Manifest-Feature-DS90003305.pdf)

## <a id="Hardware-setup"> </a>
## Hardware Setup:
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="setting-up-the-build-platform"> </a>
## Setting up the build platform
- Download and extract [ACK Device SDK 4.2](https://developer.amazon.com/alexa/console/ack/resources) from Amazon developer website.
- Copy and paste extracted  SAM D21 ACK port folder "samd21_amazon_ack" into ACK device SDK's user platform folder  
  `<Your ACK SDK Downloaded folder>/ACK_Device_SDK_4.2.202201251359/user/platform`

**Note**
- Make sure you create **amazon.com** account for downloading the SDK. You would use your Amazon credentials to register your development device (amazon.com account provides an option to register your development device.)
- "ACK Module with Espressif chipset" is used in these demo's. make sure you download proper ACK SDK from the Amazon website
- [Create Product](https://developer.amazon.com/alexa/console/ack/products) and add device capablities to the product once product is created.
- Download the product configuration file from created product and provision the Ack module using [CLI command](https://developer.amazon.com/en-US/docs/alexa/ack/provision-module-tutorials.html)

## <a id="project-structure"> </a>
## Project file structure
- ACK SDK file/directory structure, refer "Readme.txt" in  
`<Your ACK SDK Downloaded folder>\ACK_Device_SDK_4.2.202201251359\`
- SAM D21 ACK port file/directory is as follows
  - user/platform/samd21_amazon_ack/
    - **applications**                : Contains all the ACK host sample applications built on MPLABX Tools and xc32 compiler
    - **bootloader**                  : Contains bootloader for over-the-air(OTA) firmware upgrade feature
    - **hmcu_port**                   : Contains shared host specific platform and OTA files
    - **ota**                         : Contains OTA source, config and header files
    - **ack_samd21_cutom_loader.c**   : Contains bootloader specific code
    - **ack_samd21_platform.c**       : Contains SAMD21 platform specific code   
  

## <a id="steps-to-program-the-hex-file"> </a>
## Programming hex file:
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="programming-smart-light-application-project"> </a>
## Programming/Debugging Application Projects:
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="register-your-device"> </a>
## Register your device
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="running-the-demo"> </a>
## Running the Demo:
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="ota-architecture"> </a>
### OTA architecture
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="setting-up-environment-for-OTA-update"> </a>
### Setting up environment for OTA update
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## <a id="building-the-ota-application"> </a>
### Building and upgrading the OTA application
<span style="color:blue"> *Refer the documentation in individual application folder*</span>

## Comments:
- Reference Training Module: [Getting Started with Harmony v3 Peripheral Libraries on SAM D21 MCUs](https://microchipdeveloper.com/harmony3:samd21-getting-started-training-module)
- This application demo builds and works out of box by following the instructions above in "Running the Demo" section. If you need to enhance/customize this application demo, you need to use the MPLAB Harmony v3 Software framework. Refer links below to setup and build your applications using MPLAB Harmony.
	- [How to Setup MPLAB Harmony v3 Software Development Framework](https://ww1.microchip.com/downloads/en/DeviceDoc/How_to_Setup_MPLAB_%20Harmony_v3_Software_Development_Framework_DS90003232C.pdf)
	- [How to Build an Application by Adding a New PLIB, Driver, or Middleware to an Existing MPLAB Harmony v3 Project](http://ww1.microchip.com/downloads/en/DeviceDoc/How_to_Build_Application_Adding_PLIB_%20Driver_or_Middleware%20_to_MPLAB_Harmony_v3Project_DS90003253A.pdf)  
	- <span style="color:blue"> **MPLAB Harmony v3 is also configurable through MPLAB Code Configurator (MCC). Refer to the below links for specific instructions to use MPLAB Harmony v3 with MCC.**</span>
		- [Create a new MPLAB Harmony v3 project using MCC](https://microchipdeveloper.com/harmony3:getting-started-training-module-using-mcc)
		- [Update and Configure an Existing MHC-based MPLAB Harmony v3 Project to MCC-based Project](https://microchipdeveloper.com/harmony3:update-and-configure-existing-mhc-proj-to-mcc-proj)
		- [Getting Started with MPLAB Harmony v3 Using MPLAB Code Configurator](https://www.youtube.com/watch?v=KdhltTWaDp0)
		- [MPLAB Code Configurator Content Manager for MPLAB Harmony v3 Projects](https://www.youtube.com/watch?v=PRewTzrI3iE)	
- This solution is currently available only in US, Canada, and Mexico. Other regions contact Amazon.

### Revision:
- v1.6.0 Regenerated and tested the application
- v1.5.0 Updated to support Amazon ACK SDK 4.2 and ACK module with Espressif chipset
- v1.4.0 Added MCC support, Regenerated and tested application.
- v1.3.0 Updated to support Amazon ACK SDK 4.1
- v1.2.0 Updated to support Amazon ACK SDK 3.2
- v1.1.0 released demo application
