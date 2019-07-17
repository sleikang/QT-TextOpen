#pragma once

#include <QTextEdit>

class TextEdit : public QTextEdit
{
	Q_OBJECT

public:
	TextEdit(QWidget *parent = nullptr);
	~TextEdit();

protected:
	void dropEvent(QDropEvent *e) override;  //�����¼�

signals:
	void dropFileName(const QString &file);
};
