// FREERTOS LED BLINK

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#define blue 4
#define red 23

void toggleBlueLed(void *parameter) {
  while (1) {
    digitalWrite(blue, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(blue, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void toggleRedLed(void *parameter) {
  while (1) {
    digitalWrite(red, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(red, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(red, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(red, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);

  BaseType_t blueTask = xTaskCreatePinnedToCore(  // xTaskCreate() in vanilla FreeRTOS
    toggleBlueLed,                                // Function to be called
    "Toggle Blue Led",                            // Name of Task
    1024,                                         // Stack Size(bytes in ESP32, words in FreeRTOS)
    NULL,                                         // Parameter to pass to function
    1,                                            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,                                         // Task handle
    app_cpu);                                     // Run on one for demo purpose (ESP32 only)


  BaseType_t redTask = xTaskCreatePinnedToCore(  // xTaskCreate() in vanilla FreeRTOS
    toggleRedLed,                                // Function to be called
    "Toggle Red Led",                            // Name of Task
    1024,                                        // Stack Size(bytes in ESP32, words in FreeRTOS)
    NULL,                                        // Parameter to pass to function
    1,                                           // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,                                        // Task handle
    app_cpu);                                    // Run on one for demo purpose (ESP32 only)

  // Check if task creation was successful
  if (blueTask != pdPASS || redTask != pdPASS)  // In FreeRTOS, pdPASS is a macro that indicates the success of certain API function calls.
  {
    Serial.println("Task creation failed!");
  }else
  {
    Serial.println("Task creation Successfull");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
