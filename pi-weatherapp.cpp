#include <curl/curl.h>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include <fstream>

using json = nlohmann::json;
using namespace std;

// init day calculation
std::string dayOfWeek(const std::string& dateStr) {
    using namespace std::chrono;

    int y, m, d;
    sscanf_s(dateStr.c_str(), "%d-%d-%d", &y, &m, &d);

    year_month_day ymd{
        year{ y },
        month{ static_cast<unsigned>(m) },
        day{static_cast<unsigned>(d)} 
    };
    weekday wd{ sys_days{ymd} };

    static const char* names[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    return names[wd.c_encoding()];
}


// Callback function libcurl uses to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

int main() {
    CURL* curl = curl_easy_init();
    CURLcode res;
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL,
            "http://api.weatherapi.com/v1/forecast.json?key=9dfb87d24cb74c79a1222730261601&q=Dallas&days=4&aqi=no&alerts=no");

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

        // Tell curl where and how to write response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl error: "
                << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    json j = json::parse(response);

    // build json
    json out;

    // location
    out["locaton"] = {
        {"name", j["location"]["name"]},
        {"region", j["location"]["region"]}
    };

    // current weather
    out["current"] = {
        {"temp", j["current"]["temp_f"]},
        {"high", j["forecast"]["forecastday"][0]["day"]["maxtemp_f"]},
        {"low", j["forecast"]["forecastday"][0]["day"]["mintemp_f"] },
        {"rain", j["forecast"]["forecastday"][0]["day"]["daily_chance_of_rain"]},

        {"condition", {
            {"text", j["current"]["condition"]["text"]},
            {"code", j["current"]["condition"]["code"]}
        }},
        
        {"wind", j["current"]["wind_mph"]},
        {"gusts", j["current"]["gust_mph"]},
        {"humidity", j["current"]["humidity"]},
        {"feelslike", j["current"]["feelslike_f"] },
        {"uv", j["current"]["uv"]}
    };

    // rain?
    double curPrecip = j["current"]["precip_in"];
    double totPrecip = j["forecast"]["forecastday"][0]["day"]["totalprecip_in"];
    int rainChance = j["forecast"]["forecastday"][0]["day"]["daily_chance_of_rain"];

    bool showRadar = false;
    if (curPrecip > 0.0 || totPrecip > 0.05 || rainChance >= 30)
        showRadar = true;

    out["current"]["show_radar"] = showRadar;
    
    // 3 day forecast
    for (const auto& day : j["forecast"]["forecastday"]) {
        json d;

        // calc day of week
        std::string date = day["date"];
        d["date"] = date;
        d["day"] = dayOfWeek(date);

        // get data
        d["high"] = day["day"]["maxtemp_f"];
        d["low"] = day["day"]["mintemp_f"];
        d["rain"] = day["day"]["daily_chance_of_rain"];
        d["condition"] = day["day"]["condition"];

        out["forecast"].push_back(d);
    }

    // timestamp
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &t);

    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);

    out["last_updated"] = buf;

    // get out!
    std::ofstream file("C:/Users/adria/source/repos/pi-weatherapp/weather-ui/data/weather.json");
    if (!file) {
        std::cerr << "Failed to open path for writing\n";
    }
    else {
        file << out.dump(4);
    }

    return 0;
}
