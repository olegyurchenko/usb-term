/*----------------------------------------------------------------------------*/
/**
* @pkg text_highlighter
*/
/**
* Syntex highlighter for output form.
*
 * * The gramar rules are:
 * 1. Comments start with '#' and run to the end of the line.
 * 2. Strings are enclosed in double quotes (") and support C-style escape sequences (\n, \t, \", \\).
 * 3. Delimiters are blank characters (spaces, tabs) and commas.
 * 4. Individual words are two-character hexadecimal bytes (e.g., '5A', 'ff') without '0x' or 'h'.
* (C) T&T, Kiev, Ukraine 2025.<br>
* started 29.10.2025 10:38:36<br>
* @pkgdoc text_highlighter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef TEXT_HIGHLIGHTER_H_1761727116
#define TEXT_HIGHLIGHTER_H_1761727116
/*----------------------------------------------------------------------------*/
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QObject>
#include <QVector>

class TextHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  TextHighlighter(QTextDocument *parent = nullptr);

protected:
  void highlightBlock(const QString &text) override;

private:
  enum {
    NormalState = -1, // Стандартний стан
    InString = 1      // Стан "Всередині багаторядкового рядка"
  };

  struct HighlightingRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
  };
  QVector<HighlightingRule> singleLineRules;

  // Регулярні вирази та формати для багаторядкових об'єктів
  QRegularExpression stringDelimiter; // Просто лапка (")
  QTextCharFormat stringFormat;
  QTextCharFormat commentFormat;
  QTextCharFormat hexByteFormat;
  void setupRules();
};
/*----------------------------------------------------------------------------*/
#endif /*TEXT_HIGHLIGHTER_H_1761727116*/

