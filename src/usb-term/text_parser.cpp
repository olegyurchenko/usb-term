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
 * @brief Parses a text block containing strings, hex bytes, delimiters, and comments.
 * * Comments (#) are handled during tokenization to prevent them from interfering
 * with string literals (e.g., "string # comment").
 * * @param text The input text block (QString).
 * @return QByteArray containing the parsed data (bytes from strings and hex values).
 */
QByteArray parseText(const QString& text) {
  QByteArray result;

  // The initial step of removing comments is removed, as they are now handled by the regex.
  QString inputText = text;

  // Regex definition using five capturing groups (excluding Group 0 - full match):
  // Group 1 & 2: String literal ("...") - Must be first for priority.
  // Group 3: Comment (#.*) - New group to handle comments inline.
  // Group 4: Hex Byte (1 or 2 chars, e.g., 50, f, AA).
  // Group 5: Delimiters (whitespace, comma).
  QRegularExpression tokenRegex(
        "(\"([^\"\\\\]*(?:\\\\.[^\"\\\\]*)*)\")"   // Group 1 & 2: String token (full & content)
        "|(#.*)"                                  // Group 3: Comment token
        "|([0-9a-fA-F]{1,2})"                     // Group 4: Hex Byte token
        "|([\\s,]+)"                              // Group 5: Delimiter token
        );

  int offset = 0;
  while (offset < inputText.length()) {
    QRegularExpressionMatch match = tokenRegex.match(inputText, offset);

    if (match.hasMatch() && match.capturedStart() == offset) {
      offset = match.capturedEnd();

      // Handle String literal (Group 1 - full match, Group 2 - content)
      // Check if Group 1 (the full string token) is not empty.
      if (!match.captured(1).isEmpty()) {
        QString content = match.captured(2);

        // Simple C-style escape sequences unescaping.
        QString unescapedContent = content;
        unescapedContent.replace("\\n", "\n");
        unescapedContent.replace("\\t", "\t");
        unescapedContent.replace("\\r", "\r");
        unescapedContent.replace("\\\"", "\"");
        unescapedContent.replace("\\\\", "\\");

        // Append unescaped string as UTF-8 bytes
        result.append(unescapedContent.toUtf8());

        // Handle Comment (Group 3)
        // Check if Group 3 (the comment token) is not empty.
      } else if (!match.captured(3).isEmpty()) {
        // Ignore the comment; the offset has already been advanced past it.

        // Handle Hexadecimal byte (Group 4)
        // Check if Group 4 (the hex byte token) is not empty.
      } else if (!match.captured(4).isEmpty()) {
        QString hexByte = match.captured(4);
        bool ok;
        // Convert 1 or 2-char hex string to an 8-bit unsigned integer (byte)
        char byteValue = hexByte.toUShort(&ok, 16);

        if (ok) {
          result.append(byteValue);
        } else {
          qWarning() << "Parsing error for hex byte:" << hexByte;
        }

        // Handle Delimiters (Group 5)
        // Check if Group 5 (the delimiter token) is not empty.
      } else if (!match.captured(5).isEmpty()) {
        // Ignore delimiters, just advance the offset
      }
    } else {
      // Unexpected character or unrecognised token at current position
      QString errorSnippet = inputText.mid(offset, 10);
      qWarning() << "Syntax error at position" << offset << ". Unexpected text:" << errorSnippet << "...";
      break;
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
// Example Usage Function (parseTest for demonstration)
// -----------------------------------------------------------------------------
/*
void parseTest() {
    QString inputText = "50 61, \"string # with # hash\\n\" # end of line comment\n"
                        "AA F \"another \\\"string\\\"\" 01 02, 0a";

    QByteArray parsedData = parseText(inputText);

    qDebug() << "Input Text:\n" << inputText;
    qDebug() << "---------------------------------";
    qDebug() << "Parsed Data (as Hex):" << parsedData.toHex();
    qDebug() << "Parsed Data (as String):" << parsedData;
}
*/
