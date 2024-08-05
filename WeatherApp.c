 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define BUFFER_SIZE 8192

// Callback function to handle the data received from curl
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    strncat((char *)userp, (char *)contents, totalSize);
    return totalSize;
}

// Function to extract a specific value from JSON response based on key
void extractValue(const char *json, const char *key, char *value) {
    char *start = strstr(json, key); // Find the key in JSON string
    if (start) {
        start = strchr(start, ':'); // Move to the value part
        if (start) {
            start++;
            if (*start == ' ') start++; // Skip any leading spaces
            char *end = strpbrk(start, ",}"); // Find where the value ends
            if (end) {
                strncpy(value, start, end - start); // Copy the value to 'value'
                value[end - start] = '\0'; // Null-terminate the value string
                // Remove surrounding quotes if the value is a string
                if (value[0] == '"') {
                    memmove(value, value+1, strlen(value)); // Move content one position left
                    value[strlen(value) - 1] = '\0'; // Remove last quote
                }
            }
        }
    }
}

// Function to fetch weather data using OpenWeatherMap API
void getWeather(const char *apiKey, const char *city) {
    CURL *curl;
    CURLcode res;
    char readBuffer[BUFFER_SIZE] = {0}; // Buffer to store API response

    curl = curl_easy_init(); // Initialize curl session
    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", city, apiKey);

        curl_easy_setopt(curl, CURLOPT_URL, url); // Set URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set callback function for data handling
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, readBuffer); // Pass readBuffer as the userp parameter of WriteCallback

        res = curl_easy_perform(curl); // Perform the HTTP request
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            char temp[16] = {0}, description[64] = {0}, humidity[16] = {0}, windSpeed[16] = {0};

            // Extract specific weather data from the JSON response
            extractValue(readBuffer, "\"temp\"", temp);
            extractValue(readBuffer, "\"description\"", description);
            extractValue(readBuffer, "\"humidity\"", humidity);
            extractValue(readBuffer, "\"speed\"", windSpeed);

            // Display weather information
            printf("Weather in %s:\n", city);
            printf("Temperature: %.2f°C\n", atof(temp) - 273.15); // Convert temperature from Kelvin to Celsius
            printf("Weather: %s\n", description);
            printf("Humidity: %s%%\n", humidity);
            printf("Wind Speed: %.2f m/s\n", atof(windSpeed));
        }

        curl_easy_cleanup(curl); // Clean up curl session
    }
}

int main() {
    char apiKey[] = "43821179f1e7518905a5f8960c5a4629";
    char city[100];

    printf("Enter city: ");
    fgets(city, sizeof(city), stdin); // Read user input for city
    city[strcspn(city, "\n")] = 0;  // Remove newline character from city input

    getWeather(apiKey, city); // Retrieve and display weather information

    return 0;
}
