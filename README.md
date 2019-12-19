# ENES100_WaterGroup_2019
This repository was created for the Arduino code in the class ENES100: Introduction to Engineering Design. Within this class, 
I worked on the water subteam. Our mission was to create an Over-Sand Vehicle (OSV) capable of autonomously navigating obstacles,
collecting a water sample, and testing this sample for salt and pollution content.

## WiFi_Communcation
This code tests the WiFi communication module necessary for our OSV's navigation system to function. If the module is operating correctly,
the code will transmit our OSV's coordinates to the class' vision system monitor.

## Full Code
This code is the final version uploaded to the Arduino that controlled my group's OSV. It includes functions for forwards and backwards 
movement of the motors, turning, obstacle recognition and obstacle avoidance--depending on which of the ultrasonic range finders recognize
an obstacle in the arena, and usage of the mission apparatus. The mission apparatus consisted of a linear actuator with an ultrasonic 
range finder attached, coded to calculate the depth of the water, as well as one end of the peristaltic pump, the other of which feed into 
a storage tank, located on the top of our OSV. Within the storage tank lay our pollution and salinity sensors, coded to detect the 
presence or absence of salt and color in the collected water.
