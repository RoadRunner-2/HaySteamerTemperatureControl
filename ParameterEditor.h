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

namespace ParameterEditor {
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

    private:
        // Editing state
        char inputBuffer[6];  // Max needed: "HH:MM" + null terminator
        int inputPos;

        EditMode currentMode;

    public:
        ManualEditor()
            : currentMode(NONE), inputPos(0) {
            inputBuffer[0] = '\0';
        };

        // interface to Formatter
        EditMode getCurrentMode() const { return currentMode; }
        const char* getInputBuffer() const { return inputBuffer; }
        int getInputPos() const { return inputPos; }

        // interface to ParameterEditor
        void selectMode(char key) {
            // automatically abort any current edit
            inputPos = 0;
            inputBuffer[0] = '\0';

            // select mode for new edit
            switch (key) {
            case 'A':
                currentMode = TIME_EDIT;
                break;
            case 'B':
                currentMode = TEMP_EDIT;
                break;
            case 'C':
                currentMode = SPAN_EDIT;
                break;
            }
        }
        bool processDigit(char digit) {
            if (currentMode == NONE) return false;

            bool valid = false;

            switch (currentMode) {
            case TIME_EDIT:
                valid = validateTimeDigit(digit, inputPos);
                if (valid && inputPos < 4) {
                    inputBuffer[inputPos++] = digit;
                    inputBuffer[inputPos] = '\0';
                }
                break;

            case TEMP_EDIT:
                valid = validateTempDigit(digit, inputPos);
                if (valid && inputPos < 2) {
                    inputBuffer[inputPos++] = digit;
                    inputBuffer[inputPos] = '\0';
                }
                break;

            case SPAN_EDIT:
                valid = validateSpanDigit(digit, inputPos);
                if (valid && inputPos < 2) {
                    inputBuffer[inputPos++] = digit;
                    inputBuffer[inputPos] = '\0';
                }
                break;
            }

            return isEditingComplete();
        }
        void commitEdit(int& hours, int& minutes, int& temp, int& span) {
            if (currentMode == NONE) return;

            switch (currentMode) {
            case TIME_EDIT:
                if (inputPos == 4) { // HHMM format
                    int h = (inputBuffer[0] - '0') * 10 + (inputBuffer[1] - '0');
                    int m = (inputBuffer[2] - '0') * 10 + (inputBuffer[3] - '0');
                    if (h <= MAX_HOURS && m <= MAX_MINUTES) {
                        hours = h;
                        minutes = m;
                    }
                }
                break;

            case TEMP_EDIT:
                if (inputPos == 2) {
                    int t = 0;
                    for (int i = 0; i < inputPos; i++) {
                        t = t * 10 + (inputBuffer[i] - '0');
                    }
                    if (t <= MAX_TEMPERATURE) {
                        temp = t;
                    }
                }
                break;

            case SPAN_EDIT:
                if (inputPos == 2) {
                    int s = 0;
                    for (int i = 0; i < inputPos; i++) {
                        s = s * 10 + (inputBuffer[i] - '0');
                    }
                    if (s <= MAX_SPAN) {
                        span = s;
                    }
                }
                break;
            }

            currentMode = NONE;
            inputPos = 0;
            inputBuffer[0] = '\0';

            return;
        }
        void abortEdit() {
            currentMode = NONE;
            inputPos = 0;
            inputBuffer[0] = '\0';
        }

    private:
        bool validateTimeDigit(char digit, int pos) {
            int d = digit - '0';
            switch (pos) {
            case 0: return d <= 2;  // First hour digit: 0-2
            case 1:
                if (inputBuffer[0] == '2') return d <= 3;  // 20-23
                return true;  // 00-19
            case 2: return d <= 5;  // First minute digit: 0-5
            case 3: return true;    // Second minute digit: 0-9
            default: return false;
            }
        }

        bool validateTempDigit(char digit, int pos) {
            int d = digit - '0';
            if (pos == 0) {
                return true;  // First digit can be 0-9
            }
            else if (pos == 1) {
                int firstDigit = inputBuffer[0] - '0';
                if (firstDigit == 9) return d <= 9;  // 90-99
                return true;  // 00-89
            }
            return false;
        }

        bool validateSpanDigit(char digit, int pos) {
            int d = digit - '0';
            if (pos == 0) {
                return true;  // First digit can be 0-9
            }
            else if (pos == 1) {
                int firstDigit = inputBuffer[0] - '0';
                if (firstDigit == 6) return d == 0;  // Only 60 allowed
                return true;  // 00-59
            }
            return false;
        }

        bool isEditingComplete() const {
            return (currentMode == TIME_EDIT && inputPos == 4) ||
                (currentMode == TEMP_EDIT && (inputPos == 2)) ||
                (currentMode == SPAN_EDIT && (inputPos == 2));
        }
	}; // class ManualEditor

    class DisplayFormatter {
    private:
        unsigned long lastBlinkTime;
        bool blinkState;

    public:
        DisplayFormatter() : lastBlinkTime(0), blinkState(false) {}

        void updateBlink() {
            unsigned long currentTime = millis();
            if (currentTime - lastBlinkTime >= 1000) {
                blinkState = !blinkState;
                lastBlinkTime = currentTime;
            }
        }

        String formatIdleDisplay(int hours, int minutes, int temp, int span) {
            String timeStr = (hours < 10 ? "0" : "") + toString(hours) + ":" +
                (minutes < 10 ? "0" : "") + toString(minutes);
            String tempStr = toString(temp) + "°C";
            String spanStr = toString(span) + "min";

            return timeStr + ", " + tempStr + ", " + spanStr;
        }

        String formatEditDisplay(ManualEditor::EditMode mode, const char* inputBuffer, int inputPos,
            int hours, int minutes, int temp, int span) {
            String timeStr, tempStr, spanStr;

            switch (mode) {
            case ManualEditor::TIME_EDIT:
                timeStr = formatTimeEdit(inputBuffer, inputPos);
                tempStr = toString(temp) + "°C";
                spanStr = toString(span) + "min";
                break;

            case ManualEditor::TEMP_EDIT:
                timeStr = (hours < 10 ? "0" : "") + toString(hours) + ":" +
                    (minutes < 10 ? "0" : "") + toString(minutes);
                tempStr = formatTempEdit(inputBuffer, inputPos) + "°C";
                spanStr = toString(span) + "min";
                break;

            case ManualEditor::SPAN_EDIT:
                timeStr = (hours < 10 ? "0" : "") + toString(hours) + ":" +
                    (minutes < 10 ? "0" : "") + toString(minutes);
                tempStr = toString(temp) + "°C";
                spanStr = formatSpanEdit(inputBuffer, inputPos) + "min";
                break;
            }

            return timeStr + ", " + tempStr + ", " + spanStr;
        }

    private:
        String formatTimeEdit(const char* inputBuffer, int inputPos) {
            String result = "";

            for (int i = 0; i < 4; i++) {
                if (i == 2) result += ":";

                if (i < inputPos) {
                    result += inputBuffer[i];
                }
                else if (i == inputPos) {
                    result += blinkState ? "_" : " ";
                }
                else {
                    result += "_";
                }
            }

            return result;
        }

        String formatTempEdit(const char* inputBuffer, int inputPos) {
            String result = "";
            int maxDigits = 2;

            for (int i = 0; i < maxDigits; i++) {
                if (i < inputPos) {
                    result += inputBuffer[i];
                }
                else if (i == inputPos) {
                    result += blinkState ? "_" : " ";
                }
                else {
                    result += "_";
                }
            }

            return result;
        }

        String formatSpanEdit(const char* inputBuffer, int inputPos) {
            String result = "";
            int maxDigits = 2;

            for (int i = 0; i < maxDigits; i++) {
                if (i < inputPos) {
                    result += inputBuffer[i];
                }
                else if (i == inputPos) {
                    result += blinkState ? "_" : " ";
                }
                else {
                    result += "_";
                }
            }

            return result;
        }
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
        ParameterEditor()
            : manualEditor(), displayFormatter()
            , timeHours(12), timeMinutes(0)
            , temperature(20), timeSpan(30)
        {
        }

        // Main input processing function
        void processKey(char key) {
            displayFormatter.updateBlink();

            if (key >= 'A' && key <= 'C') {
                // Mode selection keys
                manualEditor.selectMode(key);
            }
            else if (key == '*') {
                // Abort current edit
                manualEditor.abortEdit();
            }
            else if (key >= '0' && key <= '9') {
                // Digit input
                if (manualEditor.processDigit(key))
                    // true if editing is complete, read values
                    manualEditor.commitEdit(timeHours, timeMinutes, temperature, timeSpan);
            }
        };

        String getDisplayString() {
            displayFormatter.updateBlink();

            if (manualEditor.getCurrentMode() == ManualEditor::NONE) {
                return displayFormatter.formatIdleDisplay(timeHours, timeMinutes, temperature, timeSpan);
            }
            else {
                return displayFormatter.formatEditDisplay(manualEditor.getCurrentMode(),
                    manualEditor.getInputBuffer(), manualEditor.getInputPos(),
                    timeHours, timeMinutes, temperature, timeSpan);
            }
        };

        // Getters for current parameter values
        int getTimeHours() const { return timeHours; }
        int getTimeMinutes() const { return timeMinutes; }
        int getTimeInMinutes() const { return timeHours * 60 + timeMinutes; }
        int getTemperature() const { return temperature; }
        int getTimeSpan() const { return timeSpan; }

        // Setters for initial parameter values
        void setTime(int hours, int minutes) {
            if (hours >= 0 && hours <= MAX_HOURS && minutes >= 0 && minutes <= MAX_MINUTES) {
                timeHours = hours;
                timeMinutes = minutes;
            }
        }

        void setTemperature(int temp) {
            if (temp >= 0 && temp <= MAX_TEMPERATURE) {
                temperature = temp;
            }
        }

        void setTimeSpan(int span) {
            if (span >= 0 && span <= MAX_SPAN) {
                timeSpan = span;
            }
        }

    }; // class ParameterEditor

} // namespace ParameterEditor
#endif