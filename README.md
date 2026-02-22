# esp-idf-mavlink-monitor
Example for receiving MAVLINK messages via UDP.   
You can use [mavesp8266](https://github.com/BeyondRobotix/mavesp8266) as a WiFi Access Point and MavLink Bridge.   

The Flight Controller sends and receives MavLink messages via UART.    
The mavesp8266 forwards MavLink messages from UART to WiFi.   
The mavesp8266 forwards MavLink messages from WiFi to UART.   
The ESP32 sends and receives MavLink messages over UDP.   
The ESP32 acts as a ground station.   
```
+-----------------------------------------------+
| AirCraft/Drone                                |
| +-----------------+          +------------+   |      +----------+
| |Flight Controller|<--UART-->| mavesp8266 |<--|UDP-->|  ESP32   |
| +-----------------+          +------------+   |      +----------+
+-----------------------------------------------+
```

# Software requirements   
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   
I used [this](https://github.com/mavlink/c_library_v2) library.   

# Installation   

```
git clone https://github.com/nopnop2002/esp-idf-mavlink-monitor
cd esp-idf-mavlink-monitor
idf.py menuconfig
idf.py flash
```

# Configuration   
![Image](https://github.com/user-attachments/assets/12928748-6400-4785-bdb0-19916221591c)
![Image](https://github.com/user-attachments/assets/7175a40b-0c3a-4363-940c-93fc7c9d2595)
