#include <FastLED.h>
#define NUM_LEDS 223
#define DATA_PIN 2  
#include <SoftwareSerial.h>
#define MAX_SNAKE_LENGTH 300 // Maximum length of the snake
#define MAX_FIREWORKS 5
int snakeLength = 1; // Initial length of the snake
int snakeHeadIndex = 0; // Current head position of the snake
unsigned long lastUpdateTime = 0; // Last update time
const int updateInterval = 250; // Time between updates in milliseconds, now 0.25s

// Define the positions of the fruits
// Note: Adjusting for continuous growth and ensuring uniqueness as the snake loops
int fruitPositions[] = {5, 15, 25, 35, 50, 65, 80, 100, 120, 140, 160, 180, 200, 225, 250, 275, 295, 315, 335, 355, 375, 395, 415, 435, 455, 475, 495, 515, 535, 555};
int fruitIndex = 0; // Index to keep track of the next fruit position

#define sensor1 A3
#define sensor2 A5
#define sensor3 3
#define sensor4 4
#define sensor5 5
#define sensor6 8
#define sensor7 9
#define sensor8 12
#define sensor9 10
#define sensor10 A0
#define sensor11 13
#define sensor12 11
#define sensor13 7
#define sensor14 A4
#define sensor15 A1
#define sensor16 A2
#define STATEPin 6

struct Firework {
  int launchIndex = -1;
  int maxHeight;
  CHSV color;
  int explosionSize;
  unsigned long explosionStart = 0;
  bool exploding = false;

  Firework() : launchIndex(-1), maxHeight(0), color(CHSV(0, 0, 0)), explosionSize(0), explosionStart(0), exploding(false) {}
};

Firework fireworks[MAX_FIREWORKS];

struct Tile{
  int sensorPin;
  int startLED;
  int endLED;
  bool isLit;
};

Tile tiles[] = {
  {sensor1, 0, 13, false},
  {sensor2, (1*14)-1, (2*14)-1, false},
  {sensor3, (2*14)-1, (3*14)-1, false},
  {sensor4, (3*14)-1, (4*14)-1, false},
  {sensor5, (4*14)-1, (5*14)-1, false},
  {sensor6, (5*14)-1, (6*14)-1, false},
  {sensor7, (6*14)-1, (7*14)-1, false},
  {sensor8, (7*14)-1, (8*14)-1, false},
  {sensor9, (8*14)-1, (9*14)-1, false},
  {sensor10, (9*14)-1, (10*14)-1, false},
  {sensor11, (10*14)-1, (11*14)-1, false},
  {sensor12, (11*14)-1, (12*14)-1, false},
  {sensor13, (12*14)-1, (13*14)-1, false},
  {sensor14, (13*14)-1, (14*14)-1, false},
  {sensor15, (14*14)-1, (15*14)-1, false},
  {sensor16, (15*14)-1, (16*14)-1, false},
};

SoftwareSerial mySerial(0, 1); // TX, RX
char data_modes = 'B';
char data_bright = 'I';
char input;
int i;
char list_modes[19] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'L', 'M','N', 'O','P','Q', 'R', 'S','T', 'U','V','W'};
char list_bright[4] = {'H', 'I', 'J', 'K'};
bool ledsON[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}; // Initialize all to false
int sensorPins[16] = {sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7, sensor8, sensor9, sensor10, sensor11, sensor12, sensor13, sensor14, sensor15, sensor16};

bool delayDone;
int score;
int noOfTiles;
int lives;
unsigned long currentTime;
unsigned long endTime;

//REDS
CRGB red = CRGB(255,0,0); //1st Red
CRGB orange = CRGB(255,30,0);
CRGB l_orange = CRGB(255,80,0); //2nd Orange

// GREENS
CRGB green = CRGB(0,150,0); //1st Green
CRGB mint = CRGB(0,200,30);
CRGB yellow = CRGB(255,130,0);

// BLUES
CRGB blue = CRGB(0,0,255);
CRGB cyan = CRGB(0,150,128); //2nd Blue
CRGB teal = CRGB(0,50,220);

// PURPLES
CRGB pink = CRGB(150,0,50); //
CRGB purple = CRGB(100,0,255); //this is
CRGB violet = CRGB(200, 0, 200); // 

CRGB leds[NUM_LEDS];
CRGB currentColor;

void setup() {
  Serial.begin(9600);
  pinMode(sensor1, INPUT); 
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT); 
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT); 
  pinMode(sensor6, INPUT);
  pinMode(sensor7, INPUT); 
  pinMode(sensor8, INPUT);
  pinMode(sensor9, INPUT); 
  pinMode(sensor10, INPUT);
  pinMode(sensor11, INPUT); 
  pinMode(sensor12, INPUT);
  pinMode(sensor13, INPUT);
  pinMode(sensor14, INPUT); 
  pinMode(sensor15, INPUT);
  pinMode(sensor16, INPUT);
  pinMode(STATEPin, INPUT);
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.clear();  // clear all pixel data
  FastLED.show();

  mySerial.begin(9600);
  currentColor = blue;
  data_modes = 'B';
  score = 0;
  noOfTiles = 0;
  Serial.print(score); // Send the message over Bluetooth
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  lives = 5;
}

void loop() {
  
  if (digitalRead(STATEPin) == HIGH) {
    if (mySerial.available()){ 
      input = mySerial.read();
      // Serial.print("Input: ");
      // Serial.println(input);
      for (i=0; i<19; i++){ //dont need to touch this
        if (list_modes[i] == input){
          data_modes = input;
          // Serial.print("modes: ");
          // Serial.println(data_modes);
          break;
        }
      }
    
      for (i=0; i<4; i++){ // dont need to touch this
        if (list_bright[i] == input){
          data_bright = input;
          // Serial.print("brightness: ");
          // Serial.println(data_bright);
          break;
          }
      }
    }

    if(data_modes == 'A' || data_modes == 'C') // ON
    {
      resetGame();
      checkcheck();
    }
    
    if(data_modes == 'B') // OFF
    {
      // Serial.println("OFF");
      resetGame();
      turnOff();
    }
    if(data_bright == 'H') // 25%
    {
      FastLED.setBrightness(64);
      // Serial.println("25% Brightness Mode Code Runs");
    }
    if(data_bright == 'I') // 50%
    {
      FastLED.setBrightness(128);
      // Serial.println("50% Brightness Mode Code Runs");
    }
    if(data_bright == 'J') // 75%
    {
      FastLED.setBrightness(192);
      // Serial.println("75% Brightness Mode Code Runs");
    }
    if(data_bright == 'K') // 100% 
    {
      FastLED.setBrightness(255);
      // Serial.println("100% Brightness Mode Code Runs");
    }
    if(data_modes == 'L') // red
    {
      // Serial.println("Red color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = red; 
      checkcheck();  
    }
    if(data_modes == 'M') // green
    {
      // Serial.println("Green color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = green; 
      checkcheck();  
    }
    if(data_modes == 'N') // blue
    {
      // Serial.println("Blue color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = blue; 
      checkcheck();  
    }
    
    if(data_modes == 'O') // pink
    {
      // Serial.println("Pink color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = pink; 
      checkcheck();  
    }
    if(data_modes == 'P') // orange
    {
      // Serial.println("Orange color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = orange; 
      checkcheck();  
    }
    if(data_modes == 'Q') // mint
    {
      // Serial.println("Mint color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = mint; 
      checkcheck();  
    }
    if(data_modes == 'R') // cyan
    {
      // Serial.println("Cyan color LED Code runs");
      // Serial.println("Standard Mode Code Runs"); 
      resetGame();
      currentColor = cyan; 
      checkcheck();  
    }
    if(data_modes == 'S') // purple
    {
      // Serial.println("Purple color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = purple; 
      checkcheck();  
    }
    if(data_modes == 'T') // light orange
    {
      // Serial.println("Light Orange color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = l_orange; 
      checkcheck();  
    }
    if(data_modes == 'U') // yellow
    {
      // Serial.println("Yellow color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = yellow; 
      checkcheck();  
    }
    if(data_modes == 'V') // teal
    {
      // Serial.println("Teal color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();
      currentColor = teal; 
      checkcheck();  
    }
    if(data_modes == 'W') // violet
    {
      // Serial.println("Violet color LED Code runs");
      // Serial.println("Standard Mode Code Runs");  
      resetGame();   
      currentColor = violet; 
      checkcheck();  
    }
    if(data_modes == 'D') // Animation 1
    {
    //  Serial.println("A1 Mode Code Runs");
    resetGame();   
    snakeAnimation();
    }
    if(data_modes == 'E') // Animation 2
    {
    // Serial.println("A2 Mode Code Runs");
      resetGame();
      pacmanAnimation();
    }
    if(data_modes == 'F') // Animation 3
    {
    //  Serial.println("A3 Mode Code Runs");
      resetGame();
     fireworksAnimation();
    }
    if(data_modes == 'G') // Game
    {
    //  Serial.println("Game Mode Code Runs");
     whackAMole();
    }

  } 

  FastLED.show();
}

void displayLEDS(){

  for(int i = 15; i>0; i--){ //hardcoded for now
    int j = random(0, i+1);
    Tile temp = tiles[i];
    tiles[i] = tiles[j];
    tiles[j] = temp;
  }

  int tilesNum = random(1,6);
  noOfTiles += tilesNum;
  for(int i = 0; i< tilesNum; i++){
    turnOn(tiles[i].startLED, tiles[i].endLED, currentColor);
    tiles[i].isLit = true;
  }
  FastLED.show();

}

void checkInputs() {
  for (int i = 0; i < 16; i++) {
    if (tiles[i].isLit) {
      if (digitalRead(tiles[i].sensorPin) == LOW) {
        turnOffTile(tiles[i].startLED, tiles[i].endLED);
        FastLED.show();
        tiles[i].isLit = false;
        score++;
        Serial.println(score); // Send the message over Bluetooth
        //if(noOfTiles - score > 5){
          //unsigned long timeFirst = millis();
         // unsigned long timeEnd = millis() + 4000;
        //  while(timeEnd > millis()){
          //  blink();
          //}
          //data_modes = 'C';
        }
      }
    }
}



void whackAMole(){
  if(!delayDone){
      delay(1000);
      delayDone = true;
    }
    displayLEDS();
    currentTime = millis();
    endTime = millis() + 1500;
    while(millis() < endTime){
      checkInputs();
    }
    checkLives();
    //turnOff();
    delay(200);
}

void checkLives(){
  for (int i = 0; i < 16; i++) {
    if (tiles[i].isLit) {
      lives--;
      tiles[i].isLit = false;
    }
  }
  if(lives <=0){
    unsigned long timeFirst = millis();
    unsigned long timeEnd = millis() + 4000;
    while(timeEnd > millis()){
      blink();
    }
    data_modes = 'C';
  }

}

void pacmanAnimation(){
  FastLED.setBrightness(10);
  static int pacmanIndex = 0; // Current position of Pac-Man
  
  // Set all LEDs to blue as the background
  fill_solid(leds, NUM_LEDS, CRGB::Blue);

  // Check if there's enough space for the pattern to be placed
  if (pacmanIndex + 6 < NUM_LEDS) {
    // Pac-Man, always ahead
    leds[pacmanIndex + 6] = CRGB::Yellow;
    // Ghosts with a blue LED space between Pac-Man and the first ghost
    leds[pacmanIndex + 4] = CRGB(255, 140, 0); // Dark Orange Ghost
    leds[pacmanIndex + 3] = CRGB::Red; // Red Ghost
    leds[pacmanIndex + 2] = CRGB(255, 105, 180); // Dark Pink Ghost
    leds[pacmanIndex + 1] = CRGB(0, 255, 255); // Light Blue Ghost
    // Note: The space between Pac-Man and the first ghost is achieved by not placing a ghost at pacmanIndex + 5
  }

  // Update the LEDs
  FastLED.show();
  
  // Move to the next index at a faster rate of 0.5 seconds
  delay(500);

  // Update Pac-Man's position, looping back if needed
  pacmanIndex++;
  if (pacmanIndex + 6 >= NUM_LEDS) {
    // Reset the position to start the pattern over from the beginning
    pacmanIndex = 0;
  }
}

void launchFirework(int i) {
  if (fireworks[i].launchIndex == -1) { // Start a new firework
    fireworks[i].launchIndex = 0;
    fireworks[i].maxHeight = random(20, 200); // Random max height between 20 and 200
    fireworks[i].color = CHSV(random(0, 256), 255, 255);
    fireworks[i].explosionSize = random(5, 11); // Random explosion size between 5 and 10
    fireworks[i].exploding = false;
  }
}

void updateFireworks() {
  fill_solid(leds, NUM_LEDS, CRGB::Black); // Reset the strip

  for (int i = 0; i < MAX_FIREWORKS; i++) {
    if (fireworks[i].launchIndex >= 0 && !fireworks[i].exploding) {
      // Simulate launch
      int launchSpeed = 1; // Adjust for faster or slower launch speed
      fireworks[i].launchIndex += launchSpeed;
      if (fireworks[i].launchIndex >= fireworks[i].maxHeight) {
        fireworks[i].exploding = true;
        fireworks[i].explosionStart = millis();
      } else {
        leds[fireworks[i].launchIndex] = CRGB::White;
      }
    } else if (fireworks[i].exploding) {
      // Simulate explosion with non-linear brightness and color gradients
      unsigned long explosionDuration = millis() - fireworks[i].explosionStart;
      if (explosionDuration < 3000) { // 2-3 seconds explosion
        for (int j = -fireworks[i].explosionSize; j <= fireworks[i].explosionSize; j++) {
          int idx = fireworks[i].maxHeight + j;
          if (idx >= 0 && idx < NUM_LEDS) {
            // Calculate non-linear brightness and apply slight randomization
            float distanceFactor = (float)abs(j) / fireworks[i].explosionSize;
            byte brightness = 255 * (1 - pow(distanceFactor, 2.5)); // Non-linear decrease
            brightness = max(25, brightness); // Ensure minimum brightness for furthest LEDs
            brightness += random(-30, 31); // Add randomness
            brightness = constrain(brightness, 25, 255); // Constrain to valid range
            
            byte hueShift = random(-10, 11); // Slight random hue shift for gradient effect
            
            leds[idx] = CHSV(fireworks[i].color.hue + hueShift, 255, brightness);
          }
        }
      } else {
        // Reset firework after explosion
        fireworks[i].launchIndex = -1;
      }
    }
  }

  FastLED.show();
}

void fireworksAnimation(){
  for (int i = 0; i < MAX_FIREWORKS; i++) {
    if (random(0, 100) < 2) { // Low chance to start a new firework
      launchFirework(i);
    }
  }

  updateFireworks();
  delay(50); // Adjust timing as needed
}

void snakeAnimation(){
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime > updateInterval) {
    lastUpdateTime = currentTime;

    // Reset the display
    //fill_solid(leds, NUM_LEDS, CRGB::Yellow);

    //Dimmer yellow
    fill_solid(leds, NUM_LEDS, CRGB(32,32,0));

    // Determine the next fruit position considering the loop
    int nextFruitPos = fruitPositions[fruitIndex % (sizeof(fruitPositions)/sizeof(fruitPositions[0]))] % NUM_LEDS;

    // Check for fruit consumption
    if (snakeHeadIndex == nextFruitPos) {
      if (snakeLength < MAX_SNAKE_LENGTH) {
        snakeLength++;
      }
      fruitIndex++; // Move to the next fruit
    }

    // Place the snake
    for (int i = 0; i < snakeLength && i < NUM_LEDS; i++) {
      int index = (snakeHeadIndex - i + NUM_LEDS) % NUM_LEDS;
      leds[index] = CRGB::Purple;
    }

    // Place the fruit
    leds[nextFruitPos] = CRGB::Red;

    // Show the LEDs
    FastLED.show();

    // Move the snake
    snakeHeadIndex = (snakeHeadIndex + 1) % NUM_LEDS;
  }
}

void turnOn(int start, int end, CRGB color){
  
    for(int i = start; i< end; i++){
        leds[i] = color;
      }
}

void turnOff(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

void fadeOutSingle(int start, int end){
  for (int i = start; i<end; i++){
    if(leds[i] != 0x000000){
      leds[i].fadeToBlackBy(30);
    }
    
  }
  delay(2);
}

void checkcheck(){
  
  if (digitalRead(sensorPins[0]) == LOW) {
          if(ledsON[0] == false)
          turnOn(0, 13, currentColor);
          ledsON[i] = true;
        } 
        else if (digitalRead(sensorPins[0]) == HIGH) {
            fadeOutSingle(0, 13);
            ledsON[i] = false;
        }
  for (int i = 1; i < 16; i++) {
        if (digitalRead(sensorPins[i]) == LOW) {
          if(ledsON[i] == false){
            turnOn((i * 14) -1, ((i + 1) * 14) -1, currentColor);
            ledsON[i] = true;
          }
          
        } 
        else if (digitalRead(sensorPins[i]) == HIGH) {
            fadeOutSingle((i * 14) -1, ((i + 1) * 14) -1);
            ledsON[i] = false;
        }
      
      }
}

void turnOffTile(int start, int end){
  for(int i = start; i<end; i++){
    leds[i] = CRGB::Black;
  }
}

void blink(){
  fill_solid(leds, NUM_LEDS, red);
  delay(1000);
  turnOff();
  delay(1000);
}

void resetGame(){
  score = 0;
  Serial.println(score);  
  noOfTiles = 0;
  delayDone = false;
  lives = 5;
}

