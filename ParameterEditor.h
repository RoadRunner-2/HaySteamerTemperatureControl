#ifndef PARAMETER_EDITOR_H
#define PARAMETER_EDITOR_H

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/millis.h" // Include a custom millis function for sandbox environment
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
		/// <summary> 
		/// Returns the current editing mode.
		/// </summary>
		/// <returns>The current editing mode as an EditMode enum value.</returns>
        EditMode getCurrentMode() const;
        /// <summary>
        /// Returns a pointer to the current input buffer.
        /// </summary>
        /// <returns>A pointer to a null-terminated character array containing the input buffer.</returns>
        const char* getInputBuffer() const;
		/// <summary>
		/// Returns the current position in the input buffer.
		/// </summary>
		/// <returns>The current position in the input buffer as an integer.</returns>
        int getInputPos() const;

        // interface to ParameterEditor
		/// <summary>
		/// Processes a key input for editing parameters.
		/// </summary>
		/// <param name="key">The key character to process.</param>
        void selectMode(char key);
        /// <summary>
        /// Processes a single digit character.
        /// </summary>
        /// <param name="digit">The character representing the digit to process.</param>
        /// <returns>true if the input is complete for the current parameter; otherwise, false.</returns>
        bool processDigit(char digit);
        /// <summary>
        /// Commits an edit by updating the hours and minutes, temperatore or time span out of the inputBuffer,
        /// depending on the editing mode.
        /// </summary>
        /// <param name="hours">Reference to the variable holding the hours value to be updated.</param>
        /// <param name="minutes">Reference to the variable holding the minutes value to be updated.</param>
        /// <param name="temp">Reference to the variable holding the temperature value to be updated.</param>
        /// <param name="span">Reference to the variable holding the time span value to be updated.</param>
        void commitEdit(int& hours, int& minutes, int& temp, int& span);
        /// <summary>
		/// Aborts the current edit operation and resets the input buffer.
        /// </summary>
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

        /// <summary>
        /// Updates the state of the blinkState using millis().
        /// </summary>
        void updateBlink();
        /// <summary>
        /// Formats a string for the idle (non-editing) display using the provided time and temperature values.
        /// </summary>
        /// <param name="hours">The number of hours to display.</param>
        /// <param name="minutes">The number of minutes to display.</param>
        /// <param name="temp">The temperature value to display.</param>
        /// <param name="span">The time span value to display.</param>
        /// <returns>A formatted string representing the idle display in the format "HH:MM, TT°C, SSmin"</returns>
        String formatIdleDisplay(int hours, int minutes, int temp, int span);
        /// <summary>
        /// Formats and returns a string representation of the current edit state for display purposes.
        /// </summary>
        /// <param name="mode">The current edit mode, indicating which field is being edited.</param>
        /// <param name="inputBuffer">A pointer to the character buffer containing the user's current input.</param>
        /// <param name="inputPos">The current cursor position within the input buffer.</param>
        /// <param name="hours">The current value of the hours field.</param>
        /// <param name="minutes">The current value of the minutes field.</param>
        /// <param name="temp">The current value of the temperature field.</param>
        /// <param name="span">The current value of the span field.</param>
        /// <returns>A formatted string representing the current edit state for display in the format "HH:MM, TT°C, SSmin".</returns>
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

        /// <summary>
        /// Processes a single keyboard key input.
        /// </summary>
        /// <param name="key">The character representing the key to process.</param>
        void processKey(char key);

        /// <summary>
        /// Retrieves the display string.
        /// </summary>
        /// <returns>A String containing the display text.</returns>
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