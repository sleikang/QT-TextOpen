#include "TextOpenThread.h"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QTextCodec>

TextOpenThread::TextOpenThread(QObject *parent)
	: QThread(parent)
{
	nowSeek = 0;
	nowSeekEnd = 0;
	codeModel = 0;
	openModel = 0;
}

TextOpenThread::~TextOpenThread()
{
}

void TextOpenThread::run()
{
	if (openModel == 1) {
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("���ļ�ʧ�ܣ�");
			return;
		}
		int i;
		file.seek(nowSeek);

		//�Զ�������
		QTextCodec::ConverterState state;
		QTextCodec *codec = QTextCodec::codecForName("UTF-8");
		QByteArray byte;
		for (int i = 1; !file.atEnd() && i < 2000; i++) {
			byte += file.readLine();
		}
		codec->toUnicode(byte.constData(), byte.size(), &state);
		if (state.invalidChars > 0) {
			codeModel = 1;
		}
		else {
			codeModel = 2;
		}

		file.seek(nowSeek);
		for (i = 1; !file.atEnd(); i++) {
			if (codeModel == 1)
				data.append(QTextCodec::codecForName("GBK")->toUnicode(file.readLine()));
			else if (codeModel == 2)
				data.append(file.readLine());

			if (i == 2000) {
				i = 0;
				data.replace(QRegExp("\n$"), "");
				emit readData(data, codeModel);
				data.clear();
				break;
			}
		}
		nowSeekEnd = file.pos();

		data.replace(QRegExp("\n$"), "");

		if (i > 1) {
			emit readData(data, codeModel);
			data.clear();
		}

		file.close();
	}
	else if (openModel == 2) {
		QFile fileIn(fileName);
		if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("���ļ�ʧ�ܣ�");
			emit saveFinish(false);
			return;
		}

		QFile fileOut(fileName + ".bak");
		if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("�����ļ�ʧ�ܣ�");
			emit saveFinish(false);
			return;
		}

		if (nowSeek != 0) {
			while (fileIn.pos() < nowSeek) {
				if (saveCodeModel == codeModel)
					fileOut.write(fileIn.readLine());
				else {
					if (codeModel == 1) {
						fileOut.write(QTextCodec::codecForName("GBK")->toUnicode(fileIn.readLine()).toUtf8());
					}
					else if (codeModel == 2) {
						fileOut.write(QString(fileIn.readLine()).toLocal8Bit());
					}
				}
			}
		}
		if (saveCodeModel == codeModel) {
			if (codeModel == 1)
				fileOut.write(saveData.toLocal8Bit());
			else if (codeModel == 2)
				fileOut.write(saveData.toUtf8());
		}
		else {
			if (saveCodeModel == 1) {
				fileOut.write(saveData.toLocal8Bit());
			}
			else if (saveCodeModel == 2) {
				fileOut.write(saveData.toUtf8());
			}
		}

		fileIn.seek(nowSeekEnd);
		while (!fileIn.atEnd()) {
			if (saveCodeModel == codeModel)
				fileOut.write(fileIn.readLine());
			else {
				if (codeModel == 1) {
					fileOut.write(QTextCodec::codecForName("GBK")->toUnicode(fileIn.readLine()).toUtf8());
				}
				else if (codeModel == 2) {
					fileOut.write(QString(fileIn.readLine()).toLocal8Bit());
				}
			}
		}

		fileIn.close();
		fileOut.close();
		fileIn.remove(fileName);
		fileOut.rename(fileName);

		emit saveFinish(true);
	}
	else if (openModel == 3) {
		QFile fileIn(fileName);
		if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("���ļ�ʧ�ܣ�");
			emit saveFinish(false);
			return;
		}

		QFile fileOut(saveAsFile);
		if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("�����ļ�ʧ�ܣ�");
			emit saveFinish(false);
			return;
		}

		if (nowSeek != 0) {
			while (fileIn.pos() < nowSeek) {
				if (saveCodeModel == codeModel)
					fileOut.write(fileIn.readLine());
				else {
					if (codeModel == 1) {
						fileOut.write(QTextCodec::codecForName("GBK")->toUnicode(fileIn.readLine()).toUtf8());
					}
					else if (codeModel == 2) {
						fileOut.write(QString(fileIn.readLine()).toLocal8Bit());
					}
				}
			}
		}
		if (saveCodeModel == codeModel) {
			if (codeModel == 1)
				fileOut.write(saveData.toLocal8Bit());
			else if (codeModel == 2)
				fileOut.write(saveData.toUtf8());
		}
		else {
			if (saveCodeModel == 1) {
				fileOut.write(saveData.toLocal8Bit());
			}
			else if (saveCodeModel == 2) {
				fileOut.write(saveData.toUtf8());
			}
		}

		fileIn.seek(nowSeekEnd);
		while (!fileIn.atEnd()) {
			if (saveCodeModel == codeModel)
				fileOut.write(fileIn.readLine());
			else {
				if (codeModel == 1) {
					fileOut.write(QTextCodec::codecForName("GBK")->toUnicode(fileIn.readLine()).toUtf8());
				}
				else if (codeModel == 2) {
					fileOut.write(QString(fileIn.readLine()).toLocal8Bit());
				}
			}
		}

		fileIn.close();
		fileOut.close();

		emit saveFinish(true);
	}
	else if (openModel == 4) {
		QFile fileOut(saveAsFile);
		if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("�����ļ�ʧ�ܣ�");
			emit saveFinish(false);
			return;
		}

		if (saveCodeModel == 1) {
			fileOut.write(saveData.toLocal8Bit());
		}
		else if (saveCodeModel == 2) {
			fileOut.write(saveData.toUtf8());
		}

		fileOut.close();

		emit saveFinish(true);
	}
}

//�����ļ���
void TextOpenThread::setFileName(const QString & str)
{
	fileName = str;
}

//�����ļ�λ��
void TextOpenThread::setNowSeek(qint64 num)
{
	nowSeek = num;
}

//���ô�ģʽ
void TextOpenThread::setOpenModel(int model)
{
	openModel = model;
}

//����Ҫ���������
void TextOpenThread::setSaveData(const QString & data)
{
	saveData = data;
}

//����Ҫ����ı���ģʽ
void TextOpenThread::setSaveCodeModel(int model)
{
	saveCodeModel = model;
}

//��������ļ���
void TextOpenThread::setSaveAsFileName(const QString & file)
{
	saveAsFile = file;
}

//���õ�ǰ����ģʽ
void TextOpenThread::setCodeModel(int model)
{
	codeModel = model;
}
