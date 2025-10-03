#include "DisplayWriter.h"

DisplayWriter::DisplayWriter(Display126x64* display)
    : display(display)
{
    clearAllLines();
}

DisplayWriter::~DisplayWriter() {
}

void DisplayWriter::setAllProvider(ContentProvider line1Provider, ContentProvider line2Provider,
    ContentProvider line3Provider, ContentProvider line4Provider)
{
    m_lineProviders[0] = line1Provider;
    m_lineProviders[1] = line2Provider;
    m_lineProviders[2] = line3Provider;
    m_lineProviders[3] = line4Provider;
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

void DisplayWriter::update() {
    for (int i = 0; i <= 3; ++i) {
        updateLine(i);
    }
    display->write(m_content);
}

void DisplayWriter::clearAllLines() {
    for (int i = 0; i <= 3; ++i) {
        m_content[i] = "";
    }

    display->write(m_content);
}

void DisplayWriter::clearLine(int lineNumber) {
    m_content[lineNumber] = "";
}

bool DisplayWriter::isValidLineNumber(int lineNumber) const {
    return lineNumber >= 0 && lineNumber <= 3;
}