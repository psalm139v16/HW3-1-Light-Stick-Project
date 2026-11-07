// --- Light Stick + Rainbow Mode ---

const int buttonPin = 2;
const int RledPin = 9;   
const int GledPin = 10;  
const int BledPin = 11;  


int buttonState = 0;
bool buttonPressed = false;
unsigned long pressingTime = 0;
const int longPressInterval = 2000; // 2 seconds for Long Press


int currentMode = 0; // 0: Constant, 1: Blink, 2: Breathing, 3: Rainbow 
int lightNum = 0;    // Color selection index (0: White, 1: Red, 2: Green, 3: Blue, 4: Yellow)

// Target Color Variables (Common Anode: 0 = Max Brightness, 255 = OFF)
int RLightColor = 0;
int GLightColor = 0;
int BLightColor = 0;

// Current Output Color Values
int RCurrentColor = 0;
int GCurrentColor = 0;
int BCurrentColor = 0;

// Non-Blocking Timing Variables for Blink Mode
bool blinkOn = true;
unsigned long blinkTimer = 0;
const long blinkInterval = 300; // Blink interval (ms)

// Variables for Breathing Mode
const int fadeAmount = 5;
int fadeDirection = 1;

// Variables for Mode 3: Rainbow Mode (Bonus)
int rainbowHue = 0;
unsigned long rainbowTimer = 0;
const long rainbowInterval = 20; // Speed of rainbow rotation (ms)

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP); // Keeps pin stable HIGH when unpressed
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);


  RLightColor = 0; GLightColor = 0; BLightColor = 0;
  RCurrentColor = 0; GCurrentColor = 0; BCurrentColor = 0;
}

void loop() {
  checkButton();
  updateLEDColor();
  setRGBLEDColor(RCurrentColor, GCurrentColor, BCurrentColor);
  delay(10); 
}


void checkButton() {
  buttonState = digitalRead(buttonPin);

  // Button Pressed DOWN (Reads LOW when pressed)
  if (buttonState == LOW && !buttonPressed) {
    pressingTime = millis();
    buttonPressed = true;
  }

  // Button RELEASED (Reads HIGH when let go)
  if (buttonState == HIGH && buttonPressed) {
    unsigned long currentTime = millis();

    if (currentTime - pressingTime < longPressInterval) {
      // Short Click (< 2s) -> Change Color
      Serial.println("Short Click: Color Changed");
      changeLEDColor();
    } else {
      // Long Press (>= 2s) -> Change Mode
      currentMode = (currentMode + 1) % 4;
      Serial.print("Long Press: Switched to Mode ");
      Serial.println(currentMode);
    }

    buttonPressed = false;
  }
}

// -------------------------------------------------------------
// 2. Cycle Colors (White, Red, Green, Blue, Yellow)
// -------------------------------------------------------------
void changeLEDColor() {
  lightNum = (lightNum + 1) % 5;

  if (lightNum == 0) {       // White (R+G+B ON)
    RLightColor = 0; GLightColor = 0; BLightColor = 0;
  } else if (lightNum == 1) { // Red ON
    RLightColor = 0; GLightColor = 255; BLightColor = 255;
  } else if (lightNum == 2) { // Green ON
    RLightColor = 255; GLightColor = 0; BLightColor = 255;
  } else if (lightNum == 3) { // Blue ON
    RLightColor = 255; GLightColor = 255; BLightColor = 0;
  } else if (lightNum == 4) { // Yellow (Red + Green ON)
    RLightColor = 0; GLightColor = 0; BLightColor = 255;
  }

  RCurrentColor = RLightColor;
  GCurrentColor = GLightColor;
  BCurrentColor = BLightColor;
}

// -------------------------------------------------------------
// 3. Render LED Output Based on Selected Mode
// -------------------------------------------------------------
void updateLEDColor() {
  // --- Mode 0: Constant Light Mode ---
  if (currentMode == 0) {
    RCurrentColor = RLightColor;
    GCurrentColor = GLightColor;
    BCurrentColor = BLightColor;
  }
  // --- Mode 1: Blink Mode ---
  else if (currentMode == 1) {
    unsigned long currentTime = millis();
    if (currentTime - blinkTimer >= blinkInterval) {
      blinkOn = !blinkOn;
      blinkTimer = currentTime;
    }

    if (blinkOn) {
      RCurrentColor = RLightColor;
      GCurrentColor = GLightColor;
      BCurrentColor = BLightColor;
    } else {
      RCurrentColor = 255; GCurrentColor = 255; BCurrentColor = 255; // OFF
    }
  }
  // --- Mode 2: Breathing Light Mode ---
  else if (currentMode == 2) {
    bool negativeDir = false;

    if (RLightColor == 0) {
      RCurrentColor += fadeDirection * fadeAmount;
      if (RCurrentColor > 255 || RCurrentColor < 0) negativeDir = true;
    }
    if (GLightColor == 0) {
      GCurrentColor += fadeDirection * fadeAmount;
      if (GCurrentColor > 255 || GCurrentColor < 0) negativeDir = true;
    }
    if (BLightColor == 0) {
      BCurrentColor += fadeDirection * fadeAmount;
      if (BCurrentColor > 255 || BCurrentColor < 0) negativeDir = true;
    }

    RCurrentColor = constrain(RCurrentColor, 0, 255);
    GCurrentColor = constrain(GCurrentColor, 0, 255);
    BCurrentColor = constrain(BCurrentColor, 0, 255);

    if (negativeDir) fadeDirection = -fadeDirection;
  }
  // --- Mode 3: Rainbow Spectrum Mode (BONUS 10%) ---
  else if (currentMode == 3) {
    unsigned long currentTime = millis();
    if (currentTime - rainbowTimer >= rainbowInterval) {
      rainbowHue = (rainbowHue + 1) % 768; // Increment hue angle
      rainbowTimer = currentTime;
    }
    
    // Calculate RGB values from hue (0 to 767)
    int r = 0, g = 0, b = 0;
    if (rainbowHue < 256) {
      r = 255 - rainbowHue; g = rainbowHue; b = 0;
    } else if (rainbowHue < 512) {
      r = 0; g = 255 - (rainbowHue - 256); b = rainbowHue - 256;
    } else {
      r = rainbowHue - 512; g = 0; b = 255 - (rainbowHue - 512);
    }

    // Invert output for Common Anode RGB LED
    RCurrentColor = 255 - r;
    GCurrentColor = 255 - g;
    BCurrentColor = 255 - b;
  }
}

// -------------------------------------------------------------
// 4. Output Signal to Pins
// -------------------------------------------------------------
void setRGBLEDColor(int r, int g, int b) {
  analogWrite(RledPin, r);
  analogWrite(GledPin, g);
  analogWrite(BledPin, b);
}












