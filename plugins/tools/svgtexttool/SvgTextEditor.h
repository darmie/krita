/* This file is part of the KDE project
 *
 * Copyright 2017 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef TEXTNGSHAPECONFIGWIDGET_H
#define TEXTNGSHAPECONFIGWIDGET_H

#include <QWidget>
#include <QTextEdit>

#include <kxmlguiwindow.h>
#include <KoColor.h>
#include <KoSvgText.h>//for the enums

#include <BasicXMLSyntaxHighlighter.h>

#include "ui_WdgSvgTextEditor.h"
#include "ui_WdgSvgTextSettings.h"

class KoSvgTextShape;
class KoDialog;

class SvgTextEditor : public KXmlGuiWindow
{
    Q_OBJECT
public:
    SvgTextEditor(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~SvgTextEditor();

    //tiny enum to keep track of the tab on which editor something happens while keeping the code readable.
    enum Editor {
        Richtext, // 0
        SVGsource // 1
    };

    // enum to store which tabs are visible in the configuration
    enum EditorMode {
        RichText,
        SvgSource,
        Both
    };

    void setShape(KoSvgTextShape *shape);

private Q_SLOTS:
    /**
     * switch the text editor tab.
     */
    void switchTextEditorTab(bool convertData = true);

    void slotCloseEditor();

    /**
     * in rich text, check the current format, and toggle the given buttons.
     */
    void checkFormat();

    void slotFixUpEmptyTextBlock();

    void save();

    void undo();
    void redo();

    void cut();
    void copy();
    void paste();

    void selectAll();
    void deselect();

    void find();
    void findNext();
    void findPrev();
    void replace();

    void zoomOut();
    void zoomIn();
#ifndef Q_OS_WIN
    void showInsertSpecialCharacterDialog();
    void insertCharacter(const QChar &c);
#endif
    void setTextBold(QFont::Weight weight = QFont::Bold);
    void setTextWeightLight();
    void setTextWeightNormal();
    void setTextWeightDemi();
    void setTextWeightBlack();

    void setTextItalic(QFont::Style style = QFont::StyleOblique);
    void setTextDecoration(KoSvgText::TextDecoration decor);
    void setTextUnderline();
    void setTextOverline();
    void setTextStrikethrough();
    void setTextSubscript();
    void setTextSuperScript();
    void increaseTextSize();
    void decreaseTextSize();

    void setLineHeight(double lineHeightPercentage);
    void setLetterSpacing(double letterSpacing);
    void alignLeft();
    void alignRight();
    void alignCenter();
    void alignJustified();

    void setFont(const QString &fontName);
    void setFontSize(qreal size);
    void setBaseline(KoSvgText::BaselineShiftMode baseline);

    void setSettings();
    void slotToolbarToggled(bool);

    void setFontColor(const KoColor &c);
    void setBackgroundColor(const KoColor &c);

    void setModified(bool modified);
    void dialogButtonClicked(QAbstractButton *button);

Q_SIGNALS:
    void textUpdated(KoSvgTextShape *shape, const QString &svg, const QString &defs, bool richTextPreferred);
    void textEditorClosed();

protected:
    void wheelEvent(QWheelEvent *event) override;
    /**
     * Selects all if there is no selection
     * @returns a copy of the previous cursor.
     */
    QTextCursor setTextSelection();

private:
    void applySettings();

    QAction *createAction(const QString &name,
                          const char *member);
    void createActions();
    void enableRichTextActions(bool enable);
    void enableSvgTextActions(bool enable);

    Ui_WdgSvgTextEditor m_textEditorWidget;
    QTextEdit *m_currentEditor {0};
    QWidget *m_page {0};
    QList<QAction*> m_richTextActions;
    QList<QAction*> m_svgTextActions;

    KoSvgTextShape *m_shape {0};
#ifndef Q_OS_WIN
    KoDialog *m_charSelectDialog {0};
#endif
    BasicXMLSyntaxHighlighter *m_syntaxHighlighter;

    QString m_searchKey;
};

#endif //TEXTNGSHAPECONFIGWIDGET_H
