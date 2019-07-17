#pragma once

#include <QThread>
#include <QMap>

class TextScanThread : public QThread
{
	Q_OBJECT

public:
	TextScanThread(QObject *parent);
	~TextScanThread();
	void run();
	void setFileName(const QString &str);  //设置文件名
	const QMap<int, qint64> readPagePos();  //返回页面位置
	const qint64 readPage();  //返回总页数
	void setOpenModel(bool o); //设置线程开关

private:
	qint64 totalPage;  //总页数
	QMap<int, qint64> pagePos;  //页面位置
	QString fileName;  //文件名
	bool open;  //线程开关

signals:
	void scanLoading();  //扫描中
	void scanFinish();  //扫描完成
};
