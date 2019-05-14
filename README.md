# Application Note: RIIoT_SGPC3_Sensor

Make an application on RIIoT for monitoring air quality using a VOC sensor. The Sensor Board got a VOC sensor SGPC3 from Sensirion that is connected to the RIIoT module. It is an application based on the ICI (Intelligent C-programmable I/O) framework.

## Background ##
RIIoT™ (Radiocrafts Industrial Internet of Things) is an RF system designed to meet the sensor and actuator connectivity requirements for the Industrial IoT. The RF protocol is the IEEE802.15.4 g/e standard that supports both 868 MHz and 915 MHz for global applications. RIIoT™ includes an Intelligent C-programmable I/O (i:zi), which makes it possible to directly interface to any sensor or actuator and Mist computing. RIIoT™ does not require any license or subscription fee.
https://radiocrafts.com/technologies/riiot-rf-technology-overview/
![RIIoT](https://radiocrafts.com/wp-content/uploads/2019/04/RIIoT-network.jpg)

The SGPC3 is a digital ultra-low power gas sensor designed for mobile and battery-driven indoor air quality applications. Sensirion’s CMOSens®technology offers a complete sensor system on a single chip featuring a digital I2C interface, a
temperature-controlled micro-hotplate and a preprocessed indoor air quality signal. The sensing element features an unmatched robustness against contamination by siloxanes present in real-world applications enabling a unique long-term stability and
low drift.
https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/0_Datasheets/Gas/Sensirion_Gas_Sensors_SGPC3_Datasheet.pdf
![Table 10 Measurement commands (https://github.com/ryanjh/RIIoT_SGPC3_Sensor/blob/master/Sensirion_Gas_Sensors_SGPC3_Datasheet.png)

## Requirements for application ##
1. Make periodic transmissions every 1 hours under normal operation.
2. The sensor shall be monitored every 1 minute. If the VOC reading is out of the normal range, make an immediate “alarm” transmission.
3. The normal range is up to 20 ppm.

## Code flowchart ##
![Sequence diagram](https://github.com/ryanjh/RIIoT_SGPC3_Sensor/blob/master/SGPC3.png)
