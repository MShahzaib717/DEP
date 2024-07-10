#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "curl/curl.h"
#include "json/json.h"

using namespace std;

// Location class
class Location {
public:
    string name;
    double latitude;
    double longitude;

    Location(string n, double lat, double lon) : name(n), latitude(lat), longitude(lon) {}

    void display() {
        cout << "Location: " << name << ", Latitude: " << latitude << ", Longitude: " << longitude << endl;
    }
};

// WeatherVariable class
class WeatherVariable {
public:
    string name;
    double value;

    WeatherVariable(string n, double v) : name(n), value(v) {}

    void display() {
        cout << "Weather Variable: " << name << ", Value: " << value << endl;
    }
};

// WeatherForecastingSystem class
class WeatherForecastingSystem {
public:
    string apiKey;
    string apiUrl;

    WeatherForecastingSystem(string apikey, string apiurl) : apiKey(apikey), apiUrl(apiurl) {}

    vector<WeatherVariable> fetchWeatherForecast(Location location) {
        CURL* curl;
        CURLcode res;
        string readBuffer;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (curl) {
            string locationKey = getLocationKey(location);
            string url = apiUrl + "/currentconditions/v1/" + locationKey + "?apikey=" + apiKey;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                cout << "cURL error: " << curl_easy_strerror(res) << endl;
            }
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();

        vector<WeatherVariable> weatherVariables;
        Json::CharReaderBuilder builder;
        Json::Value root;
        istringstream s(readBuffer);
        string errs;

        if (Json::parseFromStream(builder, s, &root, &errs)) {
            double temperature = root[0]["Temperature"]["Metric"]["Value"].asDouble();
            double windSpeed = root[0]["Wind"]["Speed"]["Metric"]["Value"].asDouble();

            WeatherVariable temperatureVar("Temperature", temperature);
            WeatherVariable windSpeedVar("Wind Speed", windSpeed);

            weatherVariables.push_back(temperatureVar);
            weatherVariables.push_back(windSpeedVar);
        } else {
            cout << "Error parsing JSON: " << errs << endl;
        }

        return weatherVariables;
    }

    static size_t writeMemoryCallback(void* ptr, size_t size, size_t nmemb, void* data) {
        string& readBuffer = *(static_cast<string*>(data));
        readBuffer.append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

private:
    string getLocationKey(Location location) {
        CURL* curl;
        CURLcode res;
        string readBuffer;
        string locationKey;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (curl) {
            string url = apiUrl + "/locations/v1/cities/geoposition/search?apikey=" + apiKey + "&q=" + to_string(location.latitude) + "," + to_string(location.longitude);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                cout << "cURL error: " << curl_easy_strerror(res) << endl;
            }
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();

        Json::CharReaderBuilder builder;
        Json::Value root;
        istringstream s(readBuffer);
        string errs;

        if (Json::parseFromStream(builder, s, &root, &errs)) {
            locationKey = root["Key"].asString();
        } else {
            cout << "Error parsing JSON: " << errs << endl;
        }

        return locationKey;
    }
};

// HistoricalWeatherSystem class
class HistoricalWeatherSystem {
public:
    string apiKey;
    string apiUrl;

    HistoricalWeatherSystem(string apikey, string apiurl) : apiKey(apikey), apiUrl(apiurl) {}

    vector<WeatherVariable> fetchHistoricalWeather(Location location) {
        // Implement similar to fetchWeatherForecast
        return vector<WeatherVariable>();
    }

    static size_t writeMemoryCallback(void* ptr, size_t size, size_t nmemb, void* data) {
        string& readBuffer = *(static_cast<string*>(data));
        readBuffer.append((char*)ptr, size * nmemb);
        return size * nmemb;
    }
};

// AirQualityForecastingSystem class
class AirQualityForecastingSystem {
public:
    string apiKey;
    string apiUrl;

    AirQualityForecastingSystem(string apikey, string apiurl) : apiKey(apikey), apiUrl(apiurl) {}

    vector<WeatherVariable> fetchAirQualityForecast(Location location) {
        // Implement similar to fetchWeatherForecast
        return vector<WeatherVariable>();
    }

    static size_t writeMemoryCallback(void* ptr, size_t size, size_t nmemb, void* data) {
        string& readBuffer = *(static_cast<string*>(data));
        readBuffer.append((char*)ptr, size * nmemb);
        return size * nmemb;
    }
};

// Function to export all data to CSV file
void exportAllToCSV(const vector<vector<WeatherVariable>>& weatherData, const string& filename, const Location& location) {
    ofstream file(filename);
    if (file.is_open()) {
        // Write location information
        file << "Location: " << location.name << ", Latitude: " << location.latitude << ", Longitude: " << location.longitude << endl;

        // Write weather variables
        file << "Weather Variable,Value" << endl;
        for (const auto& weatherVariables : weatherData) {
            for (const auto& weatherVariable : weatherVariables) {
                file << weatherVariable.name << "," << weatherVariable.value << endl;
            }
        }
        file.close();
    }
    else {
        cout << "Unable to open file" << endl;
    }
}


// Function to export all data to JSON file
void exportAllToJSON(const vector<vector<WeatherVariable>>& weatherData, const string& filename, const Location& location) {
    Json::Value root;
    root["Location"]["Name"] = location.name;
    root["Location"]["Latitude"] = location.latitude;
    root["Location"]["Longitude"] = location.longitude;

    for (const auto& weatherVariables : weatherData) {
        for (const auto& weatherVariable : weatherVariables) {
            Json::Value var;
            var["name"] = weatherVariable.name;
            var["value"] = weatherVariable.value;
            root["WeatherData"].append(var);
        }
    }

    ofstream file(filename);
    if (file.is_open()) {
        file << root;
        file.close();
    }
    else {
        cout << "Unable to open file" << endl;
    }
}

// Function to export all data to TXT file
void exportAllToTXT(const vector<vector<WeatherVariable>>& weatherData, const string& filename, const Location& location) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "Location: " << location.name << ", Latitude: " << location.latitude << ", Longitude: " << location.longitude << endl;

        for (const auto& weatherVariables : weatherData) {
            for (const auto& weatherVariable : weatherVariables) {
                file << "Weather Variable: " << weatherVariable.name << ", Value: " << weatherVariable.value << endl;
            }
        }
        file.close();
    }
    else {
        cout << "Unable to open file" << endl;
    }
}

int main() {
    string apiKey = "YOUR_API_KEY";
    string apiUrl = "http://dataservice.accuweather.com";

    WeatherForecastingSystem weatherForecastingSystem(apiKey, apiUrl);
    HistoricalWeatherSystem historicalWeatherSystem(apiKey, apiUrl);
    AirQualityForecastingSystem airQualityForecastingSystem(apiKey, apiUrl);

    cout << "Enter location: ";
    string locationName;
    double lat1, lon1;
    cin >> locationName;
    cout << "Enter Latitude: ";
    cin >> lat1;
    cout << "Enter Longitude: ";
    cin >> lon1;
    Location location(locationName, lat1, lon1);

    vector<WeatherVariable> weatherForecast = weatherForecastingSystem.fetchWeatherForecast(location);
    vector<WeatherVariable> historicalWeather = historicalWeatherSystem.fetchHistoricalWeather(location);
    vector<WeatherVariable> airQualityForecast = airQualityForecastingSystem.fetchAirQualityForecast(location);

    cout << "Weather Forecast:" << endl;
    for (auto& weatherVariable : weatherForecast) {
        weatherVariable.display();
    }

    cout << "Historical Weather:" << endl;
    for (auto& weatherVariable : historicalWeather) {
        weatherVariable.display();
    }

    cout << "Air Quality Forecast:" << endl;
    for (auto& weatherVariable : airQualityForecast) {
        weatherVariable.display();
    }

    vector<vector<WeatherVariable>> allWeatherData = {weatherForecast, historicalWeather, airQualityForecast};

    exportAllToCSV(allWeatherData, "all_weather_data.csv");
    exportAllToJSON(allWeatherData, "all_weather_data.json", location);
    exportAllToTXT(allWeatherData, "all_weather_data.txt", location);

    return 0;
}
