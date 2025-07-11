#include "DisplayWriter.h"

DisplayWriter::DisplayWriter(Display126x64* display, ContentProvider line1Provider, ContentProvider line2Provider,
    ContentProvider line3Provider, ContentProvider line4Provider)
    : display(display)
    , m_lineProviders{ line1Provider, line2Provider, line3Provider, line4Provider }
{
    clearAllLines();
}

DisplayWriter::~DisplayWriter() {
}

void DisplayWriter::setLineProvider(int lineNumber, ContentProvider provider) {
    if (!isValidLineNumber(lineNumber)) {
        return;
    }
    if (!provider) {
        return;
    }

    m_lineProviders[lineNumber] = provider;
}

void DisplayWriter::updateLine(int lineNumber) {
    if (!isValidLineNumber(lineNumber)) {
        return;
    }
    if (!m_lineProviders[lineNumber]) {
		clearLine(lineNumber);
        return;
	}
    m_content[lineNumber] = m_lineProviders[lineNumber]();
}

void DisplayWriter::updateAllLines() {
    for (int i = 0; i <= 3; ++i) {
        updateLine(i);
    }
    display->write(m_content);
}

void DisplayWriter::clearAllLines() {
    for (int i = 0; i <= 3; ++i) {
        m_content[i].clear();
    }

    display->write(m_content);
}

void DisplayWriter::clearLine(int lineNumber) {
    m_content[lineNumber].clear();
}

bool DisplayWriter::isValidLineNumber(int lineNumber) const {
    return lineNumber >= 0 && lineNumber <= 3;
}