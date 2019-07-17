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
			qDebug() << QStringLiteral("打开文件失败！");
			return;
		}
		int i;
		file.seek(nowSeek);

		//自动检测编码
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
			qDebug() << QStringLiteral("打开文件失败！");
			emit saveFinish(false);
			return;
		}

		QFile fileOut(fileName + ".bak");
		if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("创建文件失败！");
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
			qDebug() << QStringLiteral("打开文件失败！");
			emit saveFinish(false);
			return;
		}

		QFile fileOut(saveAsFile);
		if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << QStringLiteral("创建文件失败！");
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
			qDebug() << QStringLiteral("创建文件失败！");
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

//设置文件名
void TextOpenThread::setFileName(const QString & str)
{
	fileName = str;
}

//设置文件位置
void TextOpenThread::setNowSeek(qint64 num)
{
	nowSeek = num;
}

//设置打开模式
void TextOpenThread::setOpenModel(int model)
{
	openModel = model;
}

//设置要保存的数据
void TextOpenThread::setSaveData(const QString & data)
{
	saveData = data;
}

//设置要保存的编码模式
void TextOpenThread::setSaveCodeModel(int model)
{
	saveCodeModel = model;
}

//设置另存文件名
void TextOpenThread::setSaveAsFileName(const QString & file)
{
	saveAsFile = file;
}

//设置当前编码模式
void TextOpenThread::setCodeModel(int model)
{
	codeModel = model;
}
