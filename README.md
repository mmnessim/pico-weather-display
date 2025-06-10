# Pico Weather Display
This is a simple project to display the current weather with a Pico 2 W and an E-Ink display. For this project I used the Waveshare [EPD2in9B](https://www.waveshare.com/wiki/Pico-ePaper-2.9-B) display and Waveshare's provided C libraries and drivers.


## TODOS
This is very much a work in progres, so there are many things to do

* Display more weather information
* Refine backend server and publish to github
* Use some kind of GPS module to dynamically get lat and long

## Backend
The Pico makes requests to a backend server written in Go, which in turn fetches and returns weather data from the [OpenWeather API](https://openweathermap.org/api). It is an extremely minimal server at this point with just one endpoint.

## Getting Started
First off, you'll need a Pico 2 W (I have not tested with a Pico W, but you can give it a try!) and either an EPD2in9B display or an LCD1602 display. I wrote the LCD1602 driver myself, so no guarantees it will work with a different model

Next, clone this repo on your computer, cd into the directory, and create a build directory.
```sh
git clone https://github.com/mmnessim/pico-weather-display.git
cd pico-weather-display
mkdir build
```

Step into the build directory and generate the build files with cmake.
```sh
cmake ..
```

Finally build the project.
```sh
cmake --build .
```

There should now be many files in your build directory. Plug in your Pico with the bootsel button pressed and drag the `weather-display.uf2` file over to your Pico. The Pico should restart and immediately begin running the program. If using the LCD1602 display, make sure you use pins 4 and 5 respectively for SDA and SCL.

## Getting started with the backend
Disclaimer: The backend can really be whatever you want it to be, so long as the response it sends to the Pico is formatted correctly. If you prefer to use Python or whatever other language, just be sure to format responses correctly
TODO put response format

Make sure you have Go installed on your backend device of choice. Clone the [backend repo](SOME URL).
```sh
git clone [SOMEURL]
cd [SOMEDIR]
```
You will need an API key from OpenWeather, which you can make by creating a free account and following their instructions.

Create a .env file in the root directory of the Go server and enter your api key.
```
API_KEY=[YOUR API KEY]
```
OpenWeather uses latitude and longitude for determining locaiton, so look up your desired latitude and longitude and enter them in your .env as well.
```
LAT=[YOUR LAT]
LONG=[YOUR LONG]
```

Finally, run the server.
```sh
go run .
```
