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

  QString inputText = text;

  // Enum for capturing groups indices.
  // Group 0 is the full match, so indexing starts from 1.
  enum Group {
    // NOTE: Group 1 is the full string token ("...")
    STRING_CONTENT = 2,     // Group 2: The content of the string (without quotes)
    COMMENT = 3,            // Group 3: The comment token (#.*)
    HEX_BYTE = 4,           // Group 4: The hex byte token (1 or 2 chars)
    DELIMITER = 5           // Group 5: The delimiter token (\s,+)
  };

  // Regex definition using five capturing groups (excluding Group 0 - full match):
  // Group 1 & 2: String literal ("...") - Must be first for priority.
  // Group 3: Comment (#.*)
  // Group 4: Hex Byte (1 or 2 chars, e.g., 50, f, AA).
  // Group 5: Delimiters (whitespace, comma).
  QRegularExpression tokenRegex(
        "(\"([^\"\\\\]*(?:\\\\.[^\"\\\\]*)*)\")"   // Group 1 (full string) & Group 2 (content)
        "|(#.*)"                                  // Group 3 (COMMENT)
        "|([0-9a-fA-F]{1,2})"                     // Group 4 (HEX_BYTE)
        "|([\\s,]+)"                              // Group 5 (DELIMITER)
        );

  int offset = 0;
  while (offset < inputText.length()) {
    QRegularExpressionMatch match = tokenRegex.match(inputText, offset);

    if (match.hasMatch() && match.capturedStart() == offset) {
      offset = match.capturedEnd();

      // Handle String literal (We check if Group 1, the full token, is not empty)
      if (!match.captured(1).isEmpty()) {
        // Use the enum for the content group index
        QString content = match.captured(Group::STRING_CONTENT);

        // Simple C-style escape sequences unescaping.
        QString unescapedContent = content;
        unescapedContent.replace("\\n", "\n");
        unescapedContent.replace("\\t", "\t");
        unescapedContent.replace("\\r", "\r");
        unescapedContent.replace("\\a", "\a");
        unescapedContent.replace("\\b", "\b");
        unescapedContent.replace("\\f", "\f");
        unescapedContent.replace("\\0", "\0");
        unescapedContent.replace("\\\"", "\"");
        unescapedContent.replace("\\\'", "\'");
        unescapedContent.replace("\\\\", "\\");

        // Append unescaped string as UTF-8 bytes
        result.append(unescapedContent.toUtf8());

        // Handle Comment (Group 3)
      } else if (!match.captured(Group::COMMENT).isEmpty()) {
        // Ignore the comment, offset is already advanced.

        // Handle Hexadecimal byte (Group 4)
      } else if (!match.captured(Group::HEX_BYTE).isEmpty()) {
        QString hexByte = match.captured(Group::HEX_BYTE);
        bool ok;
        // Convert 1 or 2-char hex string to an 8-bit unsigned integer (byte)
        char byteValue = hexByte.toUShort(&ok, 16);

        if (ok) {
          result.append(byteValue);
        } else {
          qWarning() << "Parsing error for hex byte:" << hexByte;
        }

        // Handle Delimiters (Group 5)
      } else if (!match.captured(Group::DELIMITER).isEmpty()) {
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
