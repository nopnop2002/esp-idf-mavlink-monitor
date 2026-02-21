# esp-idf-mavlink-monitor
Example for receiving MAVLINK messages via UDP.   
You can use [this](https://github.com/BeyondRobotix/mavesp8266) as a transmitter for your aircraft.   

```
+----------+         +------------+        +----------+
| aircraft |--UART-->| mavesp8266 |--UDP-->|  ESP32   |
+----------+         +------------+        +----------+
```

# Software requirements   
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   

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
