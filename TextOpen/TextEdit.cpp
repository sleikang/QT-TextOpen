#include "TextEdit.h"
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>

TextEdit::TextEdit(QWidget *parent)
	: QTextEdit(parent)
{
}

TextEdit::~TextEdit()
{
}


//ÍÏÈëÊÂ¼þ
void TextEdit::dropEvent(QDropEvent * e)
{
	if (e->mimeData()->hasUrls()) {
		emit dropFileName(e->mimeData()->urls().at(0).path().remove(0, 1));
	}
	QTextEdit::dropEvent(e);
}
