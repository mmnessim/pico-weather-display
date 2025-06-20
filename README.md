# Pico Weather Display
This is a simple project to display the current weather with a Pico 2 W and an E-Ink display. For this project I used the Waveshare [EPD2in9B](https://www.waveshare.com/wiki/Pico-ePaper-2.9-B) display and Waveshare's provided C libraries and drivers. It's also a work in progress! Expect bugs, naive implementations, and all sorts of other problems as I work my way through.


## TODOS
This is very much a work in progres, so there are many things to do.

* Create a web portal to reset wifi credentials, zip code, or do other management
* Parse headers in http response more effectively
* Refine backend server
* ~~Send zip code to backend server~~
* Incorporate mbedTLS to make https requests and publicly host backend server
* Revisit LCD1602 implementation and make sure it's up to date
* ~~Take and upload pictures for README~~
* Maybe combine frontend and backend into one repo

## Backend
The Pico makes requests to a backend server written in Go, which in turn fetches and returns weather data from the [OpenWeather API](https://openweathermap.org/api). It is an extremely minimal server at this point with just one endpoint.

## Getting Started
First off, you'll need a Pico 2 W (I have not tested with a Pico W, but you can give it a try!) and either an EPD2in9B display or an LCD1602 display. If you haven't already, follow Raspberry Pi's [instructions](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html) for installing the Pico C/C++ SDK or official VSCode extension. *If you're using the VSCode extension, you're probably best off following the official instructions to import and build the project, so feel free to disregard the rest of these instructions.*

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

### EPD 
Once the Pico boots up, the screen will flash a few times and should display this message: 
![connect](https://github.com/user-attachments/assets/ac3359fb-4b54-4195-b964-67901a6ed1c2)

Connect to the WeatherDisplay_AP network to view the captive portal where you will enter your wifi credentials and zip code.
![portal](https://github.com/user-attachments/assets/4ba3343a-6f11-40ac-a8da-88ef9508d0d9)

When you hit submit, the Pico saves this information to flash and restarts. It should now connect to your wifi network and begin querying the backend every 10 minutes. Output looks like this
![weather](https://github.com/user-attachments/assets/b4bfc086-04a3-4e9d-b7b8-508d8aec7a7e)

### Troubleshooting 
I often find that I have to enter my information in the captive portal twice before the Pico correctly connects to wifi. I have not tested this extensively, but if your project doesn't connect, try just entering it again. 
After power cycling the Pico, I also sometimes find that it boots in captive portal mode again.
If you need to change the wifi information, currently there are only two ways
* The connection timeout is set to two minutes, after which, it will clear flash and boot in captive portal mode, allowing you to enter new credentials
* Reflash the device

One TODO item is to create a web console to manage the zipcode and wifi information, but it may take a while!


## Getting started with the backend
*Disclaimer: The backend can really be whatever you want it to be, so long as the response it sends to the Pico is formatted correctly. If you prefer to use Python or whatever other language, just be sure to format responses correctly. The Pico expects:* `"cur={current temp} high={high} low={low} weather={current condition} precip={precip}"`. *There's also a chance that the headers on a custom backend might break how I parse the response.*


Make sure you have Go installed on your backend device of choice. Clone the [backend repo](https://github.com/mmnessim/pico-weather-display-backend). Visit the repo for more detailed instructions (in progress).
```sh
git clone https://github.com/mmnessim/pico-weather-display-backend.git
cd pico-weather-display-backend
```
You will need an API key from OpenWeatherMap, which you can make by creating a free account and following their instructions.

Create a .env file in the root directory of the Go server and enter your api key.
```
API_KEY=[YOUR API KEY]
```

Finally, run the server.
```sh
go run .
```

