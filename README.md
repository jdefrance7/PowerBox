# PowerBox
Code for an interface with power sensors for the Innovative Engineer's Wind Turbine.

## Authors

Primary: Joe DeFrance

### Installing

1. Download the [Arduino IDE](https://www.arduino.cc/en/main/software).

2. Replace the created `Arduino` folder with the one from this repository.

   * Alternatively copy the `Arduino\libraries` folder contents to your local `Arduino\libraries` and move desired project sketch folders to the `Arduino` folder.

3. Use the Arduino IDE Boards Manager to add the Adafruit Feather M0 board to the Arduino IDE.

   * Follow the steps in the [Adafruit Feather M0 Guide](https://learn.adafruit.com/adafruit-feather-m0-basic-proto/using-with-arduino-ide) section 'Install SAMD Support'.

### Compiling

1. Open the project sketch `PowerBox.ino` in the Arduino IDE.

2. Select the Adafruit Feather M0 board from the `Tools->Board` menu in the Arduino IDE toolbar.

3. Either click the check mark button in the upper left hand corner of the Arduino IDE or select `Sketch->Verify/Compile` from the Arduino IDE toolbar.

### Uploading

1. Connect the Adafruit Feather M0 to a computer through a data transfer capable Micro USB cable.

2. Select the proper port from the `Tools->Port` menu in the Arduino IDE toolbar.

3. Either click the right arrow button in the upper left hand corner of the Arduino IDE or select `Sketch->Upload` from the Arduino IDE toolbar.

4. Wait for the upload to complete and read diagnostic info in the Arduino IDE console.
