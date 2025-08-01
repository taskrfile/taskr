#pragma once

#include <cstdlib>
#include <string>

#if defined(_WIN32)
#include <windows.h>

std::string get_env(const char* var) {
    char* value = nullptr;
    size_t len = 0;
    if (_dupenv_s(&value, &len, var) == 0 && value != nullptr) {
        std::string result(value);
        free(value);
        return result;
    }
    return {};
}

#else

inline std::string get_env(const char* var) {
    const char* value = std::getenv(var);
    return value ? std::string(value) : "";
}
#endif

inline void set_env(const char* key, const char* value) {
#if defined(_WIN32)
    SetEnvironmentVariableA(key, value);
#else
    setenv(key, value, 1);
#endif
}
