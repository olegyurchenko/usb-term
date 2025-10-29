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
#include <QtDebug>

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
  // --- 2. Формат для шістнадцяткових байтів (00-FF) ---
  hexByteFormat.setForeground(QColor("#0548ff")); // Blue
  hexByteFormat.setFontWeight(QFont::Bold);
  // --- 3. Формат для рядків ---
  stringFormat.setForeground(QColor("#0548ff")); // Blue
  stringFormat.setFontWeight(QFont::Bold);

  // 1. Коментарі (#...)
  commentRules.append({QRegularExpression("#.*$"), commentFormat});

  // 2. Шістнадцяткові байти (1 або 2 символи)
  // Використовуємо \\b для точного виділення токенів
  singleLineRules.append({QRegularExpression("\\b[0-9a-fA-F]{1,2}\\b"), hexByteFormat});

  // --- Регулярні вирази для БАГАТОРЯДКОВИХ об'єктів ---

  // Для початку і кінця багаторядкового рядка достатньо простої лапки
  stringDelimiter.setPattern("\"");
}

void TextHighlighter::highlightBlock(const QString &text)
{
  setCurrentBlockState(NormalState);
  int startIndex = 0;
  int length = text.length();

  // -----------------------------------------------------------
  // 1. Обробка продовження багаторядкового рядка (InString)
  // -----------------------------------------------------------
  if (previousBlockState() == InString) {
    // Рядок почався у попередньому блоці. Підсвічуємо з самого початку.
    startIndex = 0;
    // Шукаємо закриваючу лапку
    QRegularExpressionMatch match = stringDelimiter.match(text, startIndex);

    if (match.hasMatch()) {
      // Закриваюча лапка знайдена: підсвічуємо до кінця лапки
      int endIndex = match.capturedEnd(0);
      setFormat(startIndex, endIndex - startIndex, stringFormat);

      // Наступний пошук продовжуємо після знайденої лапки
      startIndex = endIndex;
      setCurrentBlockState(NormalState);
    } else {
      // Закриваюча лапка НЕ знайдена: весь рядок є частиною рядка
      setFormat(startIndex, text.length() - startIndex, stringFormat);
      setCurrentBlockState(InString);
      return; // Завершуємо, оскільки весь блок вже підсвічено
    }
  }

  // -----------------------------------------------------------
  // 2. Обробка однорядкових правил (Hex...)
  // -----------------------------------------------------------
  // Застосовуємо ці правила, починаючи з поточного startIndex (після багаторядкового рядка)
  for (const HighlightingRule &rule : qAsConst(singleLineRules)) {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text, startIndex);
    while (matchIterator.hasNext()) {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(0), match.capturedLength(0), rule.format);
      //qDebug() << text << "|" << match.capturedStart(0) << "|" <<  match.capturedLength(0);
    }
  }

  // -----------------------------------------------------------
  // 3. Обробка початку нового багаторядкового рядка
  // -----------------------------------------------------------
  // Шукаємо відкриваючу лапку (це також може бути і однорядковий рядок)
  QRegularExpressionMatch startMatch = stringDelimiter.match(text, startIndex);

  while (startMatch.hasMatch()) {
    int start = startMatch.capturedStart(0);
    // Шукаємо закриваючу лапку, починаючи одразу після відкритої
    QRegularExpressionMatch endMatch = stringDelimiter.match(text, start + 1);

    if (endMatch.hasMatch()) {
      // Знайдено закриваючу лапку на цьому ж рядку: Однорядковий рядок
      int end = endMatch.capturedEnd(0);
      setFormat(start, end - start, stringFormat);

      // Продовжуємо пошук після закриваючої лапки
      startIndex = end;
    } else {
      // Закриваюча лапка НЕ знайдена: Початок багаторядкового рядка
      setFormat(start, text.length() - start, stringFormat);
      setCurrentBlockState(InString);
      break; // Завершуємо, оскільки решта рядка є частиною рядка
    }

    // Шукаємо наступний початок рядка після попереднього знайденого
    startMatch = stringDelimiter.match(text, startIndex);
  }

  //Handle comments
  for (const HighlightingRule &rule : qAsConst(commentRules)) {
    QRegularExpressionMatch match = rule.pattern.match(text, startIndex);
    if(match.hasMatch()) {
      setFormat(match.capturedStart(0), length - match.capturedStart(0), rule.format);
    }
  }

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
