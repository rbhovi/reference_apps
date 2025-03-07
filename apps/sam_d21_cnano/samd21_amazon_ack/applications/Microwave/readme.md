---
grand_parent: 32-bit MCU Reference Applications
parent: SAM D21 Curiosity Nano Evaluation Kit
title: ACK Microwave Application
nav_order: 7
---
<img src = "images/microchip_logo.png">
<img src = "images/microchip_mplab_harmony_logo_small.png">

# Amazon Alexa Connect Kit (ACK) Microwave Application on SAM D21 Curiosity Nano
<h2 align="center"> <a href="https://github.com/Microchip-MPLAB-Harmony/reference_apps/releases/latest/download/samd21_amazon_ack.zip" > Download </a> </h2>

-----

## Description:


> The application demonstrates an integration of Amazon Alexa connect kit (ACK) SDK with host microcontroller SAM D21 and controlling microwave on SAM D21 Curiosity Nano.  

This following bullet points provides links to the detailed topics:  
- [Modules/Technology Used](#Modules-Technology-Used)  
- [Hardware Used](#Hardware-Used)  
- [Software/Tools Used](#software-Tools-Used)  
- [Hardware Setup](#Hardware-setup)  
- [Setting up the build platform](#setting-up-the-build-platform)  
- [Project file structure](#project-structure)  
- [Steps to program the hex file](#steps-to-program-the-hex-file)  
- [Programming/Debugging Microwave Application Project](#programming-microwave-application-project)  
- [Register your device](#register-your-device)  
- [Running the demo](#running-the-demo)  
- [Comments](#comments)

## <a id="Modules-Technology-Used"> </a>
## Modules/Technology Used:
- Peripheral Modules      
	- SERCOM
	- Systick
	- Port
	- NVMCTRL


## <a id="Hardware-Used"> </a>
## Hardware Used:

- [SAM D21 Curiosity Nano Evaluation Kit](https://www.microchip.com/Developmenttools/ProductDetails/DM320119)   
- [Espressif Alexa Connect Kit - ACK Module](https://developer.amazon.com/en-US/docs/alexa/ack/get-ack-development-kit.html)
- Connection wires

## <a id="software-Tools-Used"> </a>
## Software/Tools Used:
<span style="color:blue"> This project has been verified to work with the following versions of software tools:</span>  

Refer [Project Manifest](./firmware/src/config/default/harmony-manifest-success.yml) present in harmony-manifest-success.yml under the project folder *firmware/src/config/default*  
- Refer the [Release Notes](../../../../../release_notes.md#development-tools) to know the **MPLAB X IDE** and **MCC** Plugin version. Alternatively, [Click Here](https://github.com/Microchip-MPLAB-Harmony/reference_apps/blob/master/release_notes.md#development-tools)  

<span style="color:blue"> Because Microchip regularly update tools, occasionally issue(s) could be discovered while using the newer versions of the tools. If the project doesn’t seem to work and version incompatibility is suspected, It is recommended to double-check and use the same versions that the project was tested with. </span> To download original version of MPLAB Harmony v3 packages, refer to document [How to Use the MPLAB Harmony v3 Project Manifest Feature](https://ww1.microchip.com/downloads/en/DeviceDoc/How-to-Use-the-MPLAB-Harmony-v3-Project-Manifest-Feature-DS90003305.pdf)

## <a id="Hardware-setup"> </a>
## Hardware Setup:
### Hardware connections with ACK	Module
- Connect Pin "PB02(ACK HOST INTERRUPT Pin)" of *SAM D21 Curiosity Nano Evaluation Kit* to "INT"(INST) pin of *ACK Module*
- Connect Pin "PB03(ACK MODULE RESET Pin)" of *SAM D21 Curiosity Nano Evaluation Kit* to "RESET"(RES) pin of *ACK Module*  
- Connect Pin "PA20(Host MCU TX)" of *SAM D21 Curiosity Nano Evaluation Kit* to "ACK RX" pin of *ACK Module*  
- Connect Pin "PA21(Host MCU RX)" of *SAM D21 Curiosity Nano Evaluation Kit* to "ACK TX" pin of *ACK Module*
- Connect Pin "VTG(Voltage Target)" of *SAM D21 Curiosity Nano Evaluation Kit* to "IOREF" pin of *ACK Module*
- Connect common ground between *SAM D21 Curiosity Nano Evaluation Kit* and *ACK Module*
- Power the *SAM D21 Curiosity Nano Evaluation Kit* Evaluation Kit from a Host PC through a Type-A male to Micro-B USB cable connected to Micro-B port (J105)
- Power the *ACK Module* from a Host PC through a Type-A male to Micro-B USB cable connected to Micro-B port (DEBUG)

	<img src = "images/ack_hardware.png" align="middle">

**Note**: Please contact Amazon for details on the other hardware/equipment required for this demo application.

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
- For more information check [Register your device](#register-your-device)

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

## Programming hex file:
The pre-built hex file can be programmed by following the below steps.  
## <a id="steps-to-program-the-hex-file"> </a>
### Steps to program the hex file
- Open MPLAB X IDE
- Close all existing projects in IDE, if any project is opened.
- Go to File -> Import -> Hex/ELF File
- In the "Import Image File" window, Step 1 - Create Prebuilt Project, Click the "Browse" button to select the prebuilt hex file.
- Select Device has "ATSAMD21G17D"
- Ensure the proper tool is selected under "Hardware Tool"
- Click on Next button
- In the "Import Image File" window, Step 2 - Select Project Name and Folder, select appropriate project name and folder
- Click on Finish button
- In MPLAB X IDE, click on "Make and Program Device" Button. The device gets programmed in sometime
- Follow the steps in "Running the Demo" section below

## <a id="programming-microwave-application-project"> </a>
## Programming/Debugging Microwave Application Project:

 <span style="color:blue"> *Before proceeding with this step complete "Setting up the build platform" step.* </span>
- Open the project (`samd21_amazon_ack/applications/Microwave/firmware/sam_d21_cnano.X`) in MPLAB X IDE
- Ensure "SAM D21 Curiosity Nano" is selected as hardware tool to program/debug the application
- Build the code and program the device by clicking on the "make and program" button in MPLAB X IDE tool bar

## <a id="register-your-device"> </a>
## Register your device
- Create an account at developer.amazon.com website and Install the Amazon Alexa application with the same credentials on your Smartphone.
    - Select the Appropriate Marketplace and [Create a Product](https://developer.amazon.com/alexa/console/ack/products/create).
- Visit [Alexa Connect Kit - Resources](https://developer.amazon.com/alexa/console/ack/resources) and download the latest ACK Device SDK, firmware, and Module Utility for **ACK Module Espressif Chipset**.
- Download the product provisioning file of the created product.
- Check and update the existing firmware using the below command:
    - `java -jar <path>/ackmoduleutility.jar updatemodule -p <port> --updatefile <file path to the downloaded firmware>`
- The above command updates a module over the wire with a new firmware package from Amazon.
    - Use the following command to provision a module with ACK Managed Services.
    - `java -jar <path>/ackmoduleutility.jar provision -p <port> --provisionconfigfile <path>/ProvisioningInfo_[devicetypeid].conf`
- Create a Barcode using a unique UPC, which will be used to register the device.
    - The syntax to create the Barcode is:
        - `barcode --port <port> [--includedsndevice] --out <output directory> --upc <upc>`
        - **Note:** The UPC can be any unused 12-digit number. Try giving random 12-digit number untill Barcode generation is success.
- Make sure your mobile device is connected to a Wi-Fi network.
- Go to Devices -> Add New Device -> Development Device -> **ACK**
- Scan the Barcode which got generated, i.e., **DVC_XXXXXX.png**.
- Connect to the Wi-Fi network and observe the status LED on the Espressif Chipset.
    - If you successfully registered your device. you should see the status LED on your ACK development board change from **WHITE** to **GREEN**.
    - For more information refer [Amazon ACK Module Utility Reference](https://developer.amazon.com/en-US/docs/alexa/ack/module-utility-cli-reference.html).
    - For more information refer [Register ACK Module](https://developer.amazon.com/en-US/docs/alexa/ack/dev-kit-device-registration.html) with Amazon by following the procedure

## <a id="running-the-demo"> </a>
## Running the Demo:
**Note**: This demo does not have functional running/test steps as the other required hardware/equipment needs to be procured.
Please contact Amazon for help on running this demo application.

## Comments:
- **Microwave application cannot support OTA feature because of limitation of code size. You can migrate to higher memory part in SAMD 21 MCU family.**
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
