/*----------------------------------------------------------------------------*/
/**
* @pkg hex_dump
*/
/**
* Generates a hexadecimal dump of QByteArray data.
*
* (C) T&T, Kiev, Ukraine 2025.<br>
* started 27.10.2025 10:42:04<br>
* @pkgdoc hex_dump
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "hex_dump.h"
/*----------------------------------------------------------------------------*/
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QTextStream>

/**
 * @brief Generates a hexadecimal dump of QByteArray data.
 * * The output format includes 16 columns of hex values on the left
 * and their ASCII representation on the right. Non-printable characters
 * (less than 0x20 or greater than or equal to 0x7F) are displayed as a dot ('.').
 * * @param data The input byte array.
 * @return QString containing the formatted hex dump.
 */
QString hexDump(const QByteArray& data) {
  if (data.isEmpty()) {
    return QString();
  }

  QString output;
  QTextStream stream(&output);
  const int bytesPerRow = 16;
  int dataSize = data.size();

  for (int i = 0; i < dataSize; i += bytesPerRow) {
    // 1. Output the offset (address)
    // Format: 8-digit hex number, padded with zero.
    stream << QString("%1: ").arg(i, 4, 16, QChar('0')).toUpper();

    // Temporary strings for the two sections of the row
    QString hexSection;
    QString asciiSection;

    // 2. Iterate through the 16 bytes (or fewer for the last row)
    for (int j = 0; j < bytesPerRow; ++j) {
      int currentPos = i + j;

      if (currentPos < dataSize) {
        char byte = data.at(currentPos);
        unsigned char ubyte = (unsigned char)byte;

        // Hex Section: Two-digit hex number, space separated
        hexSection.append(QString("%1 ").arg(ubyte, 2, 16, QChar('0')).toUpper());

        // ASCII Section: Convert to ASCII char or dot '.'
        if (ubyte >= 0x20 && ubyte < 0x7F) {
          // Printable ASCII character
          asciiSection.append(QChar(byte));
        } else {
          // Non-printable character (control or outside 7-bit ASCII)
          asciiSection.append('.');
        }
      } else {
        // Pad short rows with spaces in the hex section
        hexSection.append("   ");
      }

      // Add an extra space after the 8th byte for readability (optional, but standard)
      if (j == 7) {
        hexSection.append(" ");
      }
    }

    // 3. Combine sections and write to the output stream
    // Hex section is left-aligned, followed by a separator and ASCII section.
    stream << hexSection.leftJustified((bytesPerRow * 3) + 1, ' ') << " " << asciiSection << "\n";
  }

  return output;
}

// -----------------------------------------------------------------------------
// Example Usage Function (renamed to hexDumpTest)
// -----------------------------------------------------------------------------

void hexDumpTest() {
  // Example data including hex bytes, strings, control characters (0A, 0D), and non-ASCII (E0)
  QByteArray demoData;
  // Row 1: 16 bytes of data
  demoData.append(QByteArray::fromHex("50617273696E67206461746120616E64")); // 'Parsing data and'
  // Row 2: Contains control chars (0A, 0D, 00, 7F), and non-ASCII (E0)
  demoData.append(QByteArray::fromHex("0A0D007F207E4279746573212121E0")); // CR, LF, NULL, DEL, Space, ~, Bytes!!!, E0
  // Row 3: Short row
  demoData.append(QByteArray::fromHex("010203"));

  QString dumpOutput = hexDump(demoData);

  qDebug() << "---------------------------------";
  qDebug() << "Generated Hex Dump Test Output:";
  // Output the resulting hex dump string directly, using noquote() to maintain formatting
  qDebug().noquote() << dumpOutput;
  qDebug() << "---------------------------------";
}

