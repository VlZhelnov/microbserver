# microbserver
## Information system. Collection, processing and visualization of readings from sensors.
### Install sequence:
1. git clone https://github.com/VlZhelnov/microbserver.git
2. create new name_database and role "pi"
3. psql name_database < dbexport.pgsql
4. setup avr-libc and avrdude
5. Use make for compile source code and load in microcontroller ATmega8
6. Use micreq.py to create a request
