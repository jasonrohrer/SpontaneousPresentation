# SpontaneousPresentation
A tool for giving on-the-spot non-linear presentations with visuals

Easiest to build on Linux, but also possible on Mac and Windows (MinGW) with scome fiddling.

Requires SDL 1.2, libpng, and libjpg development libraries for building.


#### Pull like this:
```
git clone https://github.com/jasonrohrer/SpontaneousPresentation.git
git clone https://github.com/jasonrohrer/minorGems.git
```

#### Build like this:
```
./configure
make
```

Drop whatever slides (PNG or JPG) into the "slides" folder.  Add "_pixels" to the name of a file to use sharp-pixel scaling (useful for pixel art screen shots).


#### Run like this:
```
./RunPresentation
```

#### Connect to the web-based control UI like this:
```
http://192.168.1.4:8080
```
(or whatever the IP address where RunPresentation is running).



Touch (or click) an image in the web-based UI to change which image is showing on the host machine.

The general idea is to connect to the web-based control UI using a touch screen of some kind, though you can also control it with a mouse pointer.


You can control the target web grid size with webHeight and webWidth in the settings folder.