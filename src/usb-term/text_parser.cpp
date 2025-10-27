/*----------------------------------------------------------------------------*/
/**
* @pkg text_parser
*/
/**
* Parsing text to prepare data to send via connection.
*
* (C) T&T, Kiev, Ukraine 2025.<br>
* started 27.10.2025 10:01:04<br>
* @pkgdoc text_parser
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "text_parser.h"
/*----------------------------------------------------------------------------*/
#include <QByteArray>
#include <QString>
#include <QRegularExpression>
#include <QDebug>

/**
 * @brief Parses a text block according to specific rules.
 * * The parsing rules are:
 * 1. Comments start with '#' and run to the end of the line.
 * 2. Strings are enclosed in double quotes (") and support C-style escape sequences (\n, \t, \", \\).
 * 3. Delimiters are blank characters (spaces, tabs) and commas.
 * 4. Individual words are two-character hexadecimal bytes (e.g., '5A', 'ff') without '0x' or 'h'.
 * * This version uses !match.captured(N).isEmpty() for group checks, which is fully compatible.
 * @param text The input text block (QString).
 * @return QByteArray containing the parsed data (bytes from strings and hex values).
 */
QByteArray parseText(const QString& text) {
  QByteArray result;

  // 1. Remove comments (# to end of line)
  QString cleanText = text;
  QRegularExpression commentRegex("#.*");
  cleanText.remove(commentRegex);

  // 2. Define a regex to find tokens:
  // Group 1: Full String token
  // Group 2: String content (unescaped part)
  // Group 3: Hexadecimal byte token
  // Group 4: Delimiters (whitespace or comma)
  QRegularExpression tokenRegex("(\"([^\"\\\\]*(?:\\\\.[^\"\\\\]*)*)\")|([0-9a-fA-F]+)|([\\s,]+)");

  int offset = 0;
  while (offset < cleanText.length()) {
    QRegularExpressionMatch match = tokenRegex.match(cleanText, offset);

    if (match.hasMatch() && match.capturedStart() == offset) {
      offset = match.capturedEnd();

      // Handle String literal: Check if Group 1 (full string token) is not empty.
      if (!match.captured(1).isEmpty()) {
        // Get the string content (Group 2)
        QString content = match.captured(2);

        // Simple C-style escape sequences unescaping.
        QString unescapedContent = content;
        unescapedContent.replace("\\n", "\n");
        unescapedContent.replace("\\t", "\t");
        unescapedContent.replace("\\r", "\r");
        unescapedContent.replace("\\\"", "\"");
        unescapedContent.replace("\\\\", "\\");

        // Append string as UTF-8 bytes
        result.append(unescapedContent.toUtf8());

        // Handle Hexadecimal byte: Check if Group 3 (hex byte token) is not empty.
      } else if (!match.captured(3).isEmpty()) {
        QString hexByte = match.captured(3);
        bool ok;
        // Convert 2-char hex string to an 8-bit unsigned integer (byte)
        char byteValue = hexByte.toUShort(&ok, 16);

        if (ok) {
          result.append(byteValue);
        } else {
          qWarning() << "Parsing error for hex byte:" << hexByte;
        }

        // Handle Delimiters: Check if Group 4 (delimiter token) is not empty.
      } else if (!match.captured(4).isEmpty()) {
        // Ignore delimiters, just advance the offset
      }
    } else {
      // Unexpected character or unrecognised token at current position
      QString errorSnippet = cleanText.mid(offset, 10);
      qWarning() << "Syntax error at position" << offset << ". Unexpected text:" << errorSnippet << "...";
      break;
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
// Example Usage Function
// -----------------------------------------------------------------------------

void parseTest() {
  QString inputText = "50 61 72 73 69 6E 67, \"string\\nwith\\tnewline\" # end of line comment\n"
                      "AA FF \"another \\\"string\\\"\" 01 02\n"
                      "0a , 0d, 00, \"\\\"\"";

  QByteArray parsedData = parseText(inputText);

  qDebug() << "Input Text:\n" << inputText;
  qDebug() << "---------------------------------";
  qDebug() << "Parsed Data (as Hex):" << parsedData.toHex();
  qDebug() << "Parsed Data (as String):" << parsedData;
}
