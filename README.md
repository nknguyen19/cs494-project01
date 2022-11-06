# cs494-project01

## Set up
make
sudo make install
sudo ldconfig

## Compile and run file
```shell
g++ helloworld.cpp -o prog `wx-config --cppflags --libs` | ./prog
```