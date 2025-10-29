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
#include "text_highlighter.h"
/*----------------------------------------------------------------------------*/
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QPlainTextEdit>
#include <QBrush>
#include <QColor>

TextHighlighter::TextHighlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  setupRules();
}

void TextHighlighter::setupRules()
{
  // --- 1. Формат для коментарів (#...) ---
  commentFormat.setForeground(QColor("#558a55"));
  commentFormat.setFontItalic(true);
  // Регулярний вираз: від # до кінця рядка
  highlightingRules.append({QRegularExpression("#.*"), commentFormat});

  // --- 2. Формат для рядків ("...") ---
  stringFormat.setForeground(QColor("#0548ff")); // Blue
  stringFormat.setFontWeight(QFont::Bold);
  // Регулярний вираз: Рядки у лапках з підтримкою екранування (той самий, що у парсері)
  highlightingRules.append({QRegularExpression("\"([^\"\\\\]*(?:\\\\.[^\"\\\\]*)*)\""), stringFormat});

  // --- 3. Формат для шістнадцяткових байтів (00-FF) ---
  hexByteFormat.setForeground(QColor("#0548ff")); // Blue
  hexByteFormat.setFontWeight(QFont::Bold);
  // Регулярний вираз: два шістнадцяткові символи, оточені роздільниками або початком/кінцем рядка
  // Використовуємо lookbehind/lookahead (?<=\s|^) та (?=\s|,|$) для точності.
  // NOTE: QRegularExpression не повністю підтримує lookbehind змінної довжини, тому спрощуємо:
  QRegularExpression hexRegex("\\b[0-9a-fA-F]{1,2}\\b");
  highlightingRules.append({hexRegex, hexByteFormat});
}

void TextHighlighter::highlightBlock(const QString &text)
{
  // Застосування всіх правил
  for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    while (matchIterator.hasNext()) {
      QRegularExpressionMatch match = matchIterator.next();

      // Для рядків, якщо є групи, то підсвічуємо ВЕСЬ токен (Група 0)
      int startIndex = match.capturedStart(0);
      int length = match.capturedLength(0);

      // Використовуємо setFormat для застосування формату
      setFormat(startIndex, length, rule.format);
    }
  }

  // ВАЖЛИВО: Оскільки цей синтаксис не є багаторядковим (немає блоків /*...*/)
  // ми не використовуємо setCurrentBlockState/previousBlockState.
  // Всі підсвічування застосовуються в межах одного рядка (блоку).
}

/*
// ----------------------------------------------------------------
// Як підключити в основному коді (наприклад, у конструкторі вікна):
// ----------------------------------------------------------------
void MainWindow::setupTextEdit()
{
    QPlainTextEdit *editor = new QPlainTextEdit(this);
    // Створення об'єкта підсвітки, передача йому документа редактора
    new TextHighlighter(editor->document());

    // ... інший код налаштування редактора
}
*/
