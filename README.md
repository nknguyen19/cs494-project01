# cs494-project01

## Install wxWidgets
Download source from https://l.facebook.com/l.php?u=https%3A%2F%2Fgithub.com%2FwxWidgets%2FwxWidgets%2Freleases%2Fdownload%2Fv3.2.1%2FwxWidgets-3.2.1.tar.bz2%3Ffbclid%3DIwAR3IVzmJFMTPyL2a12m-eB7djZSm1Lwuuk6FUC1fVZAEglCWBbxBKvn2Y4U&h=AT2F2xFCGq6gEIZ5x-ceyB57-SkyYzXlhbJ_OuK0jhn3L5jtI8X6oKSVC2A0BFvcYBRgSWZeFu6M31pdXfbS_9_96Q9uiKyX_1sqfsgm1NGzbKd3fl046IpaASsttQ

go to project root directory and run `cmake <path to wxWidgets-[version]>`

`make`

`sudo make install`

`sudo ldconfig`

## Compile and run file
```shell
g++ helloworld.cpp -o prog `wx-config --cppflags --libs` | ./prog
```

Youtube Link to Demo video: https://www.youtube.com/watch?v=98MRb0gom54