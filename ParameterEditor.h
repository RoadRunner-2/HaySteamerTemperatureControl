#ifndef PARAMETER_EDITOR_H
#define PARAMETER_EDITOR_H

#ifdef SANDBOX_ENVIRONMENT
#include <string> // Include the standard string library
using String = std::string; // Define "String" as an alias for std::string
// Define toString(int) as std::to_string
using Fptr = std::string(*)(int);
constexpr Fptr toString = &std::to_string;

#include <chrono>
// Mock implementation of millis() for sandbox environment
inline unsigned long millis() {
    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    return static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count());
}
#endif

#ifdef ARDUINO
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

// millis() is provided by the Arduino framework, no need to define it
#endif

    // Constants for parameter limits
    const int MAX_HOURS = 23;
    const int MAX_MINUTES = 59;
    const int MAX_TEMPERATURE = 99;
    const int MAX_SPAN = 60;

    class ManualEditor {
    public:
        enum EditMode {
            NONE,
            TIME_EDIT,
            TEMP_EDIT,
            SPAN_EDIT
        };

        ManualEditor();

        // interface to DisplayFormatter
        EditMode getCurrentMode() const;
        const char* getInputBuffer() const;
        int getInputPos() const;

        // interface to ParameterEditor
        void selectMode(char key);
        bool processDigit(char digit);
        void commitEdit(int& hours, int& minutes, int& temp, int& span);
        void abortEdit();

    private:
        bool validateTimeDigit(char digit, int pos);
        bool validateTempDigit(char digit, int pos);
        bool validateSpanDigit(char digit, int pos);

        bool isEditingComplete() const;

        // Editing state
        char inputBuffer[6];  // Max needed: "HH:MM" + null terminator
        int inputPos;

        EditMode currentMode;
	}; // class ManualEditor

    class DisplayFormatter {
    private:
        unsigned long lastBlinkTime;
        bool blinkState;

    public:
        DisplayFormatter();

		// Interface to ParameterEditor
        void updateBlink();
        String formatIdleDisplay(int hours, int minutes, int temp, int span);
        String formatEditDisplay(ManualEditor::EditMode mode, const char* inputBuffer, int inputPos,
            int hours, int minutes, int temp, int span);

    private:
        String formatTimeEdit(const char* inputBuffer, int inputPos);
        String formatTempEdit(const char* inputBuffer, int inputPos);
        String formatSpanEdit(const char* inputBuffer, int inputPos);
    }; // class DisplayFormatter

    class ParameterEditor {
    private:
        // Parameter storage
        int timeHours;      // 0-23
        int timeMinutes;    // 0-59
        int temperature;    // 0-99
        int timeSpan;       // 0-60

        // add editors to modify parameters, multiple editors may be used
        ManualEditor manualEditor;

        // add options for displaying parameters
        DisplayFormatter displayFormatter;

    public:
        ParameterEditor();

        // Main input processing function
        void processKey(char key);

		// main ouput function
        String getDisplayString();

        // Getters for current parameter values
        int getTimeHours() const;
        int getTimeMinutes() const;
        int getTimeInMinutes() const;
        int getTemperature() const;
        int getTimeSpan() const;

        // Setters for initial parameter values
        void setTime(int hours, int minutes);
        void setTemperature(int temp);
        void setTimeSpan(int span);

    }; // class ParameterEditor

#endif