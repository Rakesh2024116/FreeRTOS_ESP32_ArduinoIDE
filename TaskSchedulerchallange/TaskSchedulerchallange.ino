// Needed for atoi()
#include <stdlib.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buf_len = 20;

#define LED_BUILTIN   4

// Pins
static const int led_pin = LED_BUILTIN;

// Globals
static int led_delay = 500;   // ms

//*****************************************************************************
// Tasks

// Task: Blink LED at rate set by global variable
void toggleLED(void *parameter) {
  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
  }
}

// Task: Read from serial terminal
// Feel free to use Serial.readString() or Serial.parseInt(). I'm going to show
// it with atoi() in case you're doing this in a non-Arduino environment. You'd
// also need to replace Serial with your own UART code for non-Arduino.
void readSerial(void *parameters) {
  char buf[buf_len];       // Buffer to hold incoming characters
  uint8_t idx = 0;         // Index to track position in buffer

  memset(buf, 0, buf_len); // Clear the buffer initially

  while (1) {
      if (Serial.available() > 0) {
          char c = Serial.read();  // Read a single character

          if (c == '\n') {         // Check if it's the end of input
              buf[idx] = '\0';     // Null-terminate the string
              led_delay = atoi(buf);  // Convert to integer

              // Print the updated delay value
              Serial.print("Updated LED delay to: ");
              Serial.println(led_delay);

              idx = 0;             // Reset index for next input
              memset(buf, 0, buf_len);  // Clear the buffer
          } else {
              // Store the character if there's space
              if (idx < buf_len - 1) {
                  buf[idx] = c;
                  idx++;
              }
          }
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);  // Small delay to avoid busy-waiting
  }
}

//*****************************************************************************
// Main

void setup() {

  // Configure pin
  pinMode(led_pin, OUTPUT);

  // Configure serial and wait a second
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("Multi-task LED Demo");
  Serial.println("Enter a number in milliseconds to change the LED delay.");

  // Start blink task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
            toggleLED,      // Function to be called
            "Toggle LED",   // Name of task
            1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,           // Parameter to pass
            1,              // Task priority
            NULL,           // Task handle
            app_cpu);       // Run on one core for demo purposes (ESP32 only)
            
  // Start serial read task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
            readSerial,     // Function to be called
            "Read Serial",  // Name of task
            1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,           // Parameter to pass
            1,              // Task priority (must be same to prevent lockup)
            NULL,           // Task handle
            app_cpu);       // Run on one core for demo purposes (ESP32 only)

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
  // Execution should never get here
}