#ifndef DISPLAY_WRITER_H
#define DISPLAY_WRITER_H

#include <functional>

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/millis.h"
#include "Sandbox/Drawer.h"
#endif

#ifdef ARDUINO
#include <string>
#include <functional>
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

// millis() is provided by the Arduino framework, no need to define it
#include "Draw.h"
#endif

using Display126x64 = Drawer<4>;

class DisplayWriter {
public:
    using ContentProvider = std::function<String()>;
    DisplayWriter(Display126x64* display);
    ~DisplayWriter();

    /// <summary>
	/// set (and overwrite) content provider for all lines
    /// </summary>
    void setAllProvider(ContentProvider line1Provider, ContentProvider line2Provider,
        ContentProvider line3Provider, ContentProvider line4Provider);

	/// <summary>
    /// change content provider for a specific line (0-3)
	/// </summary>
    void setLineProvider(int lineNumber, ContentProvider provider);

	/// <summary>
	/// calls ContentProvider to update all lines and writes to hardware
    /// call cyclically
	/// </summary>
    void updateAllLines();

private:
    String m_content[4];
    ContentProvider m_lineProviders[4];

    bool isValidLineNumber(int lineNumber) const;
    void clearAllLines();
    void clearLine(int lineNumber);
    void updateLine(int lineNumber);

	Display126x64* display;
};

#endif
