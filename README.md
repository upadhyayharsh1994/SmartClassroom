# Smart Classroom Attendance and Answering Device using IoT specific protocols

This project was intended for students which will have Heltec LoRa devices that marks their attendance as soon as they are in the class and also 
marks the out time while leaving the class.

The BLE is scanned by Server device every 5 seconds to check for students which is uniquely identified.

The attendance details are sent to AWS IoT thing which is handled by Lambda Function(AWS) and then stores it in DynamoDB for the reference

The device is also configured with buttons to answer questions for the professor asked during the class which is communicated via LoRa protocol.
The server manages the student answers at a particular time 

Files descriotion
index.js: Lambda function for intime
main-client.cpp: Student device code
main.cpp: Main Server code
index2.js: out time code (Lambda function)

