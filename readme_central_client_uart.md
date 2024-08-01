Central Device with Client UART Sample Code
===========================================

NOTE: If you use this sample application for your own purposes, follow the
licensing agreement specified in `Software Use Agreement - use and` 
`accept (ONIPLAW 08142020).pdf` in the root directory of the installed CMSIS-Pack.

Overview
--------
This sample project generates a battery service and a UART interface with a 
baud rate of 115200, then sets the device in a Direct Connectable Mode for a
known Bluetooth(R) peer address. The peer address  is set through a define 
macro (`DIRECT_PEER_BD_ADDRESS`) in `ble_std.h`. Meanwhile, the address used 
for the device itself is read from `DEVICE_INFO_BLUETOOTH_ADDR` in 
non-volatile memory three (`NVR3`). If this address is not defined 
(all 1s or 0s), use a pre-defined, private Bluetooth address 
(`PRIVATE_BDADDR`) located in `ble_std.h`. Match the type of peer Bluetooth 
address with the address type used in the peer device in the function `Connection_SendStartCmd()`.

This sample project passes through several states before all services are 
enabled:

1. `APPM_INIT` (initialization)
   Application initializing and is configured into an idle state. 
2. `APPM_CREATE_DB` (configure central and client status)
   Application has configured the Bluetooth stack, including GAP, according to 
   the required role and features. It is now adding handlers for all expected 
   services that it will access.
3. `APPM_READY` (ready)
   Application has added the desired standard and custom service or profile 
   handlers into the Bluetooth GATT implementation.
4. `APPM_CONNECTING` (connecting)
   The device is waiting for a direct connection establishment.
5. `APPM_CONNECTED` (connected)
   Connection is now established with another compatible device.

Once the connection is established, the central device starts the UART 
exchange, along with battery service and custom service discovery. The UART 
data is buffered for TX and RX using two DMA buffers. The UART RX DMA buffer 
is periodically polled for data that has been received, and if data is 
available this is sent out in Bluetooth Low Energy packets. Conversely, if 
data has been received from the Bluetooth custom service, it is added to the 
UART TX DMA buffer for output.

If the battery service is discovered, the variable `basc_support_env.enable`
is set and the application sends a read request for the battery level value 
every five seconds, using a kernel timer that sets a flag for this purpose. 

If the custom service and related characteristic are discovered, the 
application sends a write request for one of the custom attributes every two 
seconds. For the custom service, if the custom service UUID and the two 
characteristics UUID are discovered, the variable `cs_env.state` is set to 
`CS_ALL_ATTS_DISCOVERED`.

NOTE: The DMA is used without interrupts, in order to demonstrate support for
      an arbitrary-length data transfer without the need to handle each
      individually received word from the specified interface.
      
This application uses the following optional features of Bluetooth low energy
technology when the connected device also supports them:

- LE 2M PHY (Bluetooth 5): Use of this feature allows the device to 
      transfer data using a symbol rate/bit rate of 2 Mb/s, reducing the 
      amount of time that the radio is active (reducing power), and allows for
      the possibility of higher throughput transfers.

- LE Data Packet Length Extension (Bluetooth 4.2): Use of this feature 
      allows this application to request an increase in the maximum transfer 
      unit (MTU) allowed. This feature supports a maximum increase in the 
      underlying data packet size from 27 bytes to 251 bytes, and this 
      application supports a maximum packet size of 250 bytes per packet.
  
Structure
---------
This sample project is structured as follows:

The source code exists in a `code` folder, all application-related include
header files are in the `include` folder, and the `main()` function `app.c` is 
located in the parent directory.

Code
----
    app_init.c    - All initialization functions are called here, but the 
                    implementation is in the respective C files
    app_process.c - Message handlers for application
    ble_basc.c    - Support functions and message handlers pertaining to the 
                    Battery Service Client
    ble_custom.c  - Support functions and message handlers pertaining to the
                    Custom Service Client
    ble_std.c     - Support functions and message handlers pertaining to
                    Bluetooth low energy technology
    uart.c        - Support code for handling DMA-based UART data transfers

Include
-------
    app.h        - Overall application header file
    ble_basc.h   - Header file for the Battery Service Client
    ble_custom.h - Header file for the Custom Service Client
    ble_std.h    - Header file for supporting the Bluetooth low energy 
                   standard
    uart.h       - Header file for the DMA-based UART data transfer support 
                   code
    
Hardware Requirements
---------------------
This application can be executed on any RSL10 Evaluation and Development 
Board. The application needs to be connected to a terminal application or 
similar that can read and write serial UART data at 115200 baud. No external 
connections are required.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Getting Started Guide* for your IDE for more information.

Select the project configuration according to the chip ID version of RSL10. 
The appropriate libraries and include files are loaded according to the build
configuration that you choose. Use **Debug** for CID 101. Ensure that the 
device CID matches the application build.
  
Verification
------------
To verify the Bluetooth Low Energy functionality, use RSL10 or another 
third-party peripheral application coupled with a Bluetooth sniffer 
application to ensure that a connection is established. If this application 
has established a link with the RSL10 peripheral server application, DIO0 and 
DIO1 will toggle every 5 and 2 seconds respectively. In addition to 
establishing a connection, this application can be used to read/write 
characteristics and receive notifications.

Alternatively, while the Bluetooth application manager is in the state 
`APPM_ADVERTISING`, the LED on the RSL10 Evaluation and Development Board 
(DIO6) is blinking. It turns on steadily once the link is established, and it 
goes to the `APPM_CONNECTED` state.

The UART functionality can be verified by connecting a terminal
application to RSL10, to send and receive serial data at the specified baud
rate. The correct COM ports to use can be identified using the computer's 
Device Manager to identify the port. Look for **JLink CDC UART Port (COMxx)**.

Once connected, character strokes written to the terminal of 
`central_client_uart` are echoed back to the terminal of 
`peripheral_server_uart`. Similarly, character strokes written to the terminal
of `peripheral_server_uart` are echoed back to the terminal of 
`central_client_uart`. Note that some terminal programs do not handle carriage
return characters cleanly, and will restart progress on a line following a
carriage return without inserting a newline.

Notes
-----
**#1:**
The theoretical maximum throughput for a  Bluetooth Low Energy
connection is 175 kB per second if sending data in only one direction,
and lower if sending bi-directional traffic. 

This application attempts to maximize the effective throughput for both 
single direction and bi-directional transfers, and implements buffering of
the incoming UART data to ensure that data is not lost if 
the effective throughput is exceeded for a short period of time.  

For larger files at high data rates (beyond 115,200 baud), or a pair of files
being transferred bi-directionally at moderate data rates (beyond 25,000
baud), this can overflow the internal buffers and cause packet loss.
A user could extend the UART application to implement UART traffic flow 
control to ensure that buffer overflows do not occur.

**#2**:
Sometimes the firmware in RSL10 cannot be successfully re-flashed, due to the
application going into Sleep Mode or resetting continuously (either by design 
or due to programming error). To circumvent this scenario, a software recovery
mode using DIO12 can be implemented with the following steps:

1.  Connect DIO12 to ground.
2.  Press the RESET button (this restarts the application, which
    pauses at the start of its initialization routine).
3.  Re-flash RSL10. After successful re-flashing, disconnect DIO12 from
    ground, and press the RESET button so that the application can work
    properly.

***
Copyright (c) 2019 Semiconductor Components Industries, LLC
(d/b/a ON Semiconductor).
