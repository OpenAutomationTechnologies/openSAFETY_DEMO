System Integration {#page_system_integration}
============

[TOC]

This pages provides information on how to integrate the openSAFETY_DEMO into
a control system and putting the whole system into operation
explained by taking the example of a B&R control system, using openSAFETY.

# Automation Studio and SafeDESIGNER {#sect_gs_as_sd_intro}

In order to integrate the openSAFETY_DEMO in the B&R system and putting the
whole system into operation the tools Automation Studio and SafeDESIGNER are
needed. Automation Studio is an integrated development environment used,
among others, for planning implementing and configuring B&R control systems.
SafeDESIGNER is used to to develop safe logic for the SafePLC and configuring
the openSAFETY safe nodes.

Make sure the tools are installed:
  - Automation Studio
  - SafeDESIGNER

> **Note:** The help pages of Automation Studio and SafeDESIGNER provide
> comprehensive information about the B&R hardware, its configuration
> possibilities and much more. Refer to this help for information not covered
> in this guide.

## Setting up the hardware {#sect_gs_as_hw_setup}

1. Ensure that the development hardware is set up as described in
   \ref page_setup_hardware.
2. Connect the openSAFETY_DEMO device to the B&R system as shown in the picture
   below using POWERLINK / Ethernet cables.
3. Plug the SafeKEY into the SafePLC.
4. Set the station number of the PLC to a value between 1 and 253,
   set the node number of the SafePLC to 1 and the node number of the
   openSAFETY_DEMO device to 2 (switch SW11 up, all others down).

![Hardware structure](br_hardware.png)
@image latex br_hardware.png "Hardware structure" width=0.75\textwidth

## Importing the openSAFETY_DEMO device to Automation Studio {#sect_gs_as_osdd_import}

For importing the openSAFETY_DEMO device, perform the following steps:

### Automation Studio 4.3.4 and SafeDESIGNER 4.3 onwards {#sect_gs_as_osdd_import_new_as_sd}

1. Open Automation Studio and select `Tools -> Import Fieldbus Device...`
2. Change to the folder **devicedescription** and select the
   **openSAFETY_DEMO.xosdd** file, then select the **openSAFETY_DEMO.xdd** file.

## Opening the sample project in Automation Studio {#sect_gs_opening_sample_project}

1. Make sure the steps in \ref sect_gs_as_osdd_import have been carried out,
   to ensure Automation Studio can find all hardware used in the sample project.
2. Open Automation Studio
3. Open the sample project shipped with the demo by opening Automation Studio,
   `File -> Open Project` and select the `.zip` file located in the subfolder
   **automation_studio** of the demo package.

   The software will ask to specify a storage location. Unless an other location
   is wanted, the default location is just fine. Accept with a click on the
   `Open` button. If the specified directory does not exist, it will be created
   > **Note:** If the sample project contains hardware with newer description
   > files than those installed with Automation Studio, the software will ask
   > whether the new version should be downloaded or the existing one should
   > be used.
   > Select `Use Existing`. If the version comparison shows that the device
   > description file of the demo is **missing**, carry out the steps described
   > in \ref sect_gs_as_osdd_import.

4. To open the System Designer view click the menu `Open -> System Designer`.

![Automation Studio, System Designer](as_system_designer.png)
@image latex as_system_designer.png "Automation Studio, System Designer" width=0.75\textwidth

## Replace hardware, if necessary {#sect_gs_as_replace_hw}

1. Check, if the used hardware in the project matches with the physical hardware
   setup.
2. If using a different PLC or SafeLOGIC, right-click on the related
   hardware and select `Replace Hardware Module...`.
3. Search in the Hardware Catalog on the right for the correct hardware and drag
   it onto the hardware to replace.

## Connecting to the PLC {#sect_gs_as_connect}

1. Connect the Ethernet interface of the PLC to the same network as the
   computer or connect it directly to the computer and power up the PLC system.

2. In Automation Studio click `Online -> Settings... ` from the menu and then
   the `Browse` button. If needed, click `Refresh` button to refresh the list
   of found devices.

   ![Automation Studio, Online Settings](as_online_settings.png)
   @image latex as_online_settings.png "Automation Studio, Online Settings" width=0.75\textwidth

   If the PLC shows up with a exclamation mark,
   right click on the PLC in the list and select `Set IP Parameters`.
   In the opened window, set the IP parameters matching your computer's IP
   settings, but with another IP address or select
   `get IP address from DHCP server`.
   Click `OK` and refresh the device list.
   The exclamation mark should be gone.
   > **Note:** The settings are only temporary.
   > To permanently set the IP settings, change to the System Designer and
   > select IF2 of the PLC. Change to the tab `Configuration` and set the
   > IP parameters. To apply the settings, the project has to be downloaded
   > to the PLC.
   > ![Automation Studio, IF2 IP settings](as_if2_ipsettings.png)
   > @image latex as_if2_ipsettings.png "Automation Studio, IF2 IP settings" width=0.75\textwidth

3. Right-click on the PLC and select `Connect`.

4. The status bar at the bottom of the Automation Studio should now show the
   connected PLC and its status.

## Downloading the configuration and application to the PLC {#sect_gs_as_sw_setup}

There are 2 possibilities to transfer the configuration and application to the
B&R PLC.

> **Note:** If the compilation of the project aborts with an error stating
> issues with a not installed Automation Runtime version, select the highest
> available version from the `Automation Runtime` drop-down
> accessible via the menu `Project -> Change Runtime Versions...`

- For downloading the configuration and application to the B&R PLC via network,
  perform the following steps:

  When the connection with the PLC is established, compile the Automation
  Studio Project and click `Transfer to Target` to download it to the PLC.

  ![Automation Studio, Build toolbar, Transfer](as_build_toolbar.png)
  @image latex as_build_toolbar.png "Automation Studio, Build toolbar, Transfer" width=0.2\textwidth

- For transferring the configuration and application via a Compact Flash card,
  perform the following steps:

  If there is a Compact Flash card reader available, ensure the PLC is powered
  off and remove the Compact Flash card. Insert the card into the card reader
  and click from the menu `Tools -> Create Compact Flash` or
  `Tools -> Offline Install`.
  Select the proper drive and click `Create Compact Flash` or `Download to
  application storage`.
  After the creation has finished, put the card back into the PLC.
  Power on the system again.

## Downloading the SafePLC application {#sect_gs_as_safedesigner}

To download the application to the safe PLC (B&R SafeLOGIC),
perform the following steps:

1. In the System Designer view of Automation Studio, open the
   SafeDESIGNER by right-clicking on the safe PLC and selecting `SafeDESIGNER`.
   > **Note:** If an error message states, that the SafeDESIGNER used in the
   > project is not installed, right-click on the safe PLC and select
   > `Configuration`. Find the entry `SafeDESIGNER version` and select an
   > installed version from the drop down list.

   ![SafeDESIGNER](as_sd.png)
   @image latex as_sd.png "SafeDESIGNER" width=0.75\textwidth

2. When prompted, enter the project password : 123456

3. Click the button `Compile` to build the project.

4. Find the module SL1.M1 in the Safety View located in the left bottom corner
  of the SafeDESIGNER. Right-click on the module and  select `Remote Control`.
  If downloading the application for the first time, a prompt will be shown
  asking to set a new password. Set the password to 123456 and confirm it.
  If a password is pre-set, but unknown, the SafeKEY can be formatted.
  Follow the instructions in the Automation Studio Help in
  `Hardware -> X20 System -> X20 modules -> X20 CPUs -> X20SL80XX -> Control and
  connection elements -> Safety processor -> Selection switch and confirmation
  button`

5. In the remote control window, select `Download` to download the application.

   ![SafeDESIGNER Remote Control](as_sd_remote.png)
   @image latex as_sd_remote.png "SafeDESIGNER Remote Control" width=0.75\textwidth

6. After the download has finished, wait for the SafePLC to restart.
  Observe the buttons for confirming a new SafeKEY (`SK-XCHG`), modules
  (`1 , 2 , 3, 4, n`) or firmware (`FW-ACKN`).
  The related button will be coloured, if a confirmation is needed.
  Click the button and wait for the SafePLC to restart.