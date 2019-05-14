# Application Note: RIIoT_SGPC3_Sensor

Make an application on RIIoT for monitoring air quality using a VOC sensor. The Sensor Board got a VOC sensor SGPC3 from Sensirion that is connected to the RIIoT module. It is an application based on the ICI (Intelligent C-programmable I/O) framework.

## Requirements for application ##
1. Make periodic transmissions every 1 hours under normal operation.
2. The sensor shall be monitored every 1 minute. If the VOC reading is out of the normal range, make an immediate “alarm” transmission.
3. The normal range is up to 20 ppm.

## Code flowchart ##
![Sequence diagram](https://github.com/ryanjh/RIIoT_SGPC3_Sensor/blob/master/SGPC3.png)
