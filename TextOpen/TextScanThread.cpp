#include "TextScanThread.h"
#include <QFile>
#include <QDebug>
#include <QTextCodec>

TextScanThread::TextScanThread(QObject *parent)
	: QThread(parent)
{
	open = true;
}

TextScanThread::~TextScanThread()
{
}

void TextScanThread::run()
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << QStringLiteral("打开文件失败！");
		return;
	}
	totalPage = 1;
	pagePos.clear();
	pagePos[totalPage] = 0;
	int i;
	for (i = 1; !file.atEnd() && open; i++) {
		file.readLine();
		if (i == 2000) {
			i = 0;
			totalPage++;
			pagePos[totalPage] = file.pos();
			if (totalPage % 10 == 0)
				emit scanLoading();
		}
	}
	if (i == 1)
		totalPage--;

	file.close();
	emit scanFinish();
}

//设置文件名
void TextScanThread::setFileName(const QString & str)
{
	fileName = str;
}

//返回页面位置
const QMap<int, qint64> TextScanThread::readPagePos()
{
	return pagePos;
}

//返回总页数
const qint64 TextScanThread::readPage()
{
	return totalPage;
}

//设置线程开关
void TextScanThread::setOpenModel(bool o)
{
	open = o;
}
