# :earth_asia: RTK - Work In Progress 

##### :round_pushpin: Hello There!   

This readme shares the design of RTK GNSS and future integration of the correction measurements to be transmitted over DDS or as a ROS2 service. 



### Documentation

- To configure the uBlox receiver through u-center -  [Ublox M8P Configuration](documents/configuration.md)
- Using RTKLIB - [str2str_app](documents/str2str.md)
- RTCM3 Message Type -  [RTCM3](documents/rtcm3.md) 



------



### Theory of operation 

Two methods of GNSS correction: Differential GNSS and Real Time Kinematic (RTK). Both methods correct the position through error calculation from a fixed based station. The set of error correction values are transmitted over a network/data link to the Rover to achieve a more accurate solution.

For the differences between DGNSS and RTK, refer to the quotes below 

> "The major difference between DGNSS and RTK methods is the approach used to calculate the error correction terms. DGNSS uses a transmitted code based difference to determine the error terms, which is a mostly logical layer approach; RTK instead leverages the cyclic nature of the carrier wave to determine the errors, which is a more physical layer approach." - Paul Piong

> "The configuration of Differential GNSS (DGNSS) and RTK systems are similar in that both methods require a base station receiver setup at a known location, a rover receiver that gets corrections from the base station and a communication link between the two receivers. The difference is that RTK (a carrier phase method) is significantly more accurate than DGNSS (a code-based method).
>
> The advantage of DGNSS is that it is useful over a longer baseline (distance between base station and rover receivers) and a DGNSS system is less expensive. The technology required to achieve the higher accuracy of RTK performance makes the cost of a RTK-capable receiver higher than one that is DGNSS-capable only." - [Novatel](https://www.novatel.com/an-introduction-to-gnss/chapter-5-resolving-errors/gnss-data-post-processing/)



------



### Design Notes

- [x] Use of open source RTKLIB library to accept packages from base station and rover unit    :x: To port over to PX4 GPSdriver 
  - [x] 6 Aug - Using RTKexplorer's RTKLIB which is updated for use with cheaper RTK units
  - [x] The version of RTKexplorer's RTKLIB used is b29d (7 Aug 2018)

❌ To port over to PX4 - GPSdriver

- [x] Use of serial library (LibSerial) to read packages from base station and rover unit's serial port

- [ ] RTCM over TCP/IP     

  - Reduce dependency on built-in radio transmission

  - Laptop runs CorrectBase and sends correction over TCP/IP

  - Laptop is connected to M8P through a serial port

    

- [ ] NTRIP (Networked Transport of RTCM over Internet Protocol)

- [ ] Consider the use of ubx files in [PX4 - GPS Driver](https://github.com/PX4/GpsDrivers)



### Other notes

- [RTKNAVI](http://rtkexplorer.com/getting-started-with-rtknavi-2/)
- [Configuring GPS receiver](http://rtkexplorer.com/configuring-the-gps-receiver/)
- [RTKexplorer - Getting Started](http://rtkexplorer.com/how-to/posts-getting-started/)
- [PX4 - GPS Driver](https://github.com/PX4/GpsDrivers)
- [UBX-M8P-2 ROS-MAVROS](https://github.com/AlexisTM/rtk_ros)
- [PX4 - RTK doc](https://dev.px4.io/en/advanced/rtk_gps.html)

