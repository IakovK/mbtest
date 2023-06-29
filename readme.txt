To build successfully:
sudo apt-get install libasio-dev
sudo apt-get install pkg-config
sudo apt-get install libmodbus-dev

Supported commands:
1. считать цифровой вход (номер входа), (discrete input - 02) command: read discrete n
2. считать цифровой выход (номер выхода), (coil - 01) command: read coil n
3. считать входной регистр (номер регистра), (input - 04) command: read input n
4. считать выходной регистр (номер регистра), (holding - 03) command: read holding n
where n is number of register. In this demo only 1...10 are supported

5. считать все входа, all 1 command: read discrete all
6. считать все выхода, all 2 command: read coil all
7. считать все входные регистры, all 3 command: read input all
8. считать все выходные регистры, all 4 command: read holding all
9. start - starts reading all data in the loop. Reports number of bytes read.
10. stop - stops the reading


