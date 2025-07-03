#include "ParameterEditor.h"

ManualEditor::ManualEditor()
    : currentMode(NONE), inputPos(0) 
{
    inputBuffer[0] = '\0';
};

ManualEditor::EditMode ManualEditor::getCurrentMode() const 
{
    return currentMode; 
};

const char* ManualEditor::getInputBuffer() const 
{
    return inputBuffer; 
};

int ManualEditor::getInputPos() const
{
    return inputPos;
};

void ManualEditor::selectMode(char key) 
{
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
};

bool ManualEditor::processDigit(char digit) 
{
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
};

void ManualEditor::commitEdit(int& hours, int& minutes, int& temp, int& span) 
{
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
};

void ManualEditor::abortEdit() 
{
    currentMode = NONE;
    inputPos = 0;
    inputBuffer[0] = '\0';
};

bool ManualEditor::validateTimeDigit(char digit, int pos) 
{
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
};

bool ManualEditor::validateTempDigit(char digit, int pos) 
{
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
};

bool ManualEditor::validateSpanDigit(char digit, int pos) 
{
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
};

bool ManualEditor::isEditingComplete() const 
{
    return (currentMode == TIME_EDIT && inputPos == 4) ||
        (currentMode == TEMP_EDIT && (inputPos == 2)) ||
        (currentMode == SPAN_EDIT && (inputPos == 2));
};



DisplayFormatter::DisplayFormatter()
    : lastBlinkTime(0), blinkState(false)
{
};

void DisplayFormatter::updateBlink() 
{
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= 1000) {
        blinkState = !blinkState;
        lastBlinkTime = currentTime;
    }
};

String DisplayFormatter::formatIdleDisplay(int hours, int minutes, int temp, int span) 
{
    String timeStr = (hours < 10 ? "0" : "") + toString(hours) + ":" +
        (minutes < 10 ? "0" : "") + toString(minutes);
    String tempStr = toString(temp) + "°C";
    String spanStr = toString(span) + "min";

    return timeStr + ", " + tempStr + ", " + spanStr;
};

String DisplayFormatter::formatEditDisplay(ManualEditor::EditMode mode, const char* inputBuffer, int inputPos,
    int hours, int minutes, int temp, int span) 
{
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
};

String DisplayFormatter::formatTimeEdit(const char* inputBuffer, int inputPos) 
{
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
};

String DisplayFormatter::formatTempEdit(const char* inputBuffer, int inputPos) 
{
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
};

String DisplayFormatter::formatSpanEdit(const char* inputBuffer, int inputPos) 
{
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
};


ParameterEditor::ParameterEditor()
    : manualEditor(), displayFormatter()
    , timeHours(12), timeMinutes(0)
    , temperature(20), timeSpan(30)
{ }

void ParameterEditor::processKey(char key) 
{
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

String ParameterEditor::getDisplayString() {
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

int ParameterEditor::getTimeHours() const
{
    return timeHours;
};

int ParameterEditor::getTimeMinutes() const
{
    return timeMinutes;
};

int ParameterEditor::getTimeInMinutes() const
{
    return timeHours * 60 + timeMinutes;
};

int ParameterEditor::getTemperature() const
{
    return temperature;
};

int ParameterEditor::getTimeSpan() const
{
    return timeSpan;
};

void ParameterEditor::setTime(int hours, int minutes) 
{
    if (hours >= 0 && hours <= MAX_HOURS && minutes >= 0 && minutes <= MAX_MINUTES) {
        timeHours = hours;
        timeMinutes = minutes;
    }
}

void ParameterEditor::setTemperature(int temp) 
{
    if (temp >= 0 && temp <= MAX_TEMPERATURE) {
        temperature = temp;
    }
}

void ParameterEditor::setTimeSpan(int span) 
{
    if (span >= 0 && span <= MAX_SPAN) {
        timeSpan = span;
    }
}

