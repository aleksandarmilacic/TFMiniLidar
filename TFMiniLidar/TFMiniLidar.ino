#include <LiquidCrystal_I2C.h>
// Set the LCD I2C address, usually it's 0x27 or 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the address & dimensions for your display


void setup() {
    Serial.begin(9600); // For debugging with the Serial Monitor
    Serial1.begin(115200); // Initialize Serial1 for TFmini-S

    // Command to set frame rate to 100Hz
        byte frameRateCommand[] = { 0x5A, 0x06, 0x03, 0x64, 0x00 }; // Last byte is placeholder for checksum
    byte checksum = 0;

    // Calculate checksum for the command
    for (int i = 1; i < sizeof(frameRateCommand) - 1; i++) { // Start at 1 to skip header
        checksum += frameRateCommand[i];
    }
    frameRateCommand[sizeof(frameRateCommand) - 1] = checksum; // Set checksum byte

    // Send the frame rate command
    Serial1.write(frameRateCommand, sizeof(frameRateCommand));
    lcd.init();                      // Initialize the LCD
    lcd.backlight();                 // Turn on the backlight
    lcd.clear();                     // Clear the display
    lcd.setCursor(0, 0);             // Set the cursor to the top-left position
    lcd.print("Initializing...");    // Display a message on the LCD
    // Wait for a short delay to allow the TFmini-S to process the command
    delay(100);
}

void loop() {
    if (Serial1.available()) {
        uint8_t ch1 = Serial1.read();
        // Looking for the frame header
        if (ch1 == 0x59) {
            if (Serial1.available() && Serial1.read() == 0x59) {
                // Now read the rest of the data frame
                uint8_t dataFrame[7];
                for (int i = 0; i < 7; i++) {
                    while (!Serial1.available()); // Wait for next byte
                    dataFrame[i] = Serial1.read();
                }

                int distance = dataFrame[0] + dataFrame[1] * 256; // Assuming distance is in bytes 2 and 3
                int strength = dataFrame[2] + dataFrame[3] * 256; // Assuming strength is in bytes 4 and 5

                // Process the data as needed
                Serial.print("Distance: ");
                Serial.print(distance);
                Serial.println(" cm");

                Serial.print("Strength: ");
                Serial.println(strength);

                lcd.clear();                 // Clear the previous display
                lcd.setCursor(0, 0);         // Set cursor to the first line
                lcd.print("Dist: ");
                lcd.print(distance);
                lcd.print(" cm");

                lcd.setCursor(0, 1);         // Move cursor to the second line
                lcd.print("Str: ");
                lcd.print(strength);
                // now we wait for 1 second
                delay(1000);
                // ...Add checksum verification as needed...
            }
        }
    }
}
