# RIIoT_SGPC3_Sensor

Make an application on RIIoT for monitoring air quality using a VOC sensor. The Sensor Board got a VOC sensor SGPC3 from Sensirion that is connected to the RIIoT module. It is an application based on the ICI (Intelligent C-programmable I/O) framework.

Scenario:
- Make periodic transmissions every 1 hours under normal operation.
- The sensor shall be monitored every 1 minute. The normal range is up to 20 ppm. If the VOC reading is out of the normal range, make an immediate “alarm” transmission.
