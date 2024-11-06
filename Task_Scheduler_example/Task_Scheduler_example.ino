#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

const char msg[] = "Hello, this is a human, belongs to species HOMO sapiens from earth";

// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

#define blue 4
#define red 23

/*Tasks*/

void startTask1(void *parameter) {
  int msg_len = strlen(msg);

  while (1) {
    Serial.println();
    for (int i = 0; i < msg_len; i++) {
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1sec
  }
}

void startTask2(void *parameter) {
  while (1) {
    Serial.print("*");
    vTaskDelay(100 / portTICK_PERIOD_MS);  // 100ms
  }
}

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
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);

  // Small delay for serial data
  vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1sec

  // Print self priority
  Serial.println("----FreeRTOS Demo----");
  Serial.print("Setup and loop task is running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority");
  Serial.println(uxTaskPriorityGet(NULL));

  // Task to run forever
  xTaskCreatePinnedToCore(startTask1,
                          "Task 1",
                          1024,
                          NULL,
                          1,
                          &task_1,
                          app_cpu);

  // Task to run once with Higher priority
  xTaskCreatePinnedToCore(startTask2,
                          "Task 2",
                          1024,
                          NULL,
                          2,
                          &task_2,
                          app_cpu);

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
    Serial.println("LED Task creation failed!");
  }else
  {
    Serial.println("LED Task creation Successfull");
  }
}

void loop() {
  // Suspend Higher priority task for some intervals
  for (int i = 0; i < 3; i++) {
    vTaskSuspend(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // 2sec
    vTaskResume(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // 2sec
  }

  // Delete the lower priority task
  if (task_1 != NULL) {
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}
