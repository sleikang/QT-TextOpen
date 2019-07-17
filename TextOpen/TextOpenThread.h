#pragma once

#include <QThread>

class QFile;

class TextOpenThread : public QThread
{
	Q_OBJECT

public:
	TextOpenThread(QObject *parent = nullptr);
	~TextOpenThread();
	void run();
	void setFileName(const QString &str);  //设置文件名
	void setNowSeek(qint64 num);  //设置文件位置
	void setOpenModel(int model);  //设置打开模式
	void setSaveData(const QString &data);  //设置要保存的数据
	void setSaveCodeModel(int model);  //设置要保存的编码模式
	void setSaveAsFileName(const QString &file);  //设置另存文件名
	void setCodeModel(int model);  //设置当前编码模式

private:
	qint64 nowSeek;  //现在文本位置
	qint64 nowSeekEnd;  //现在读取文本结束位置
	QString fileName;  //文件名
	QString saveAsFile;  //另存文件名
	QString data;  //保存内容
	int codeModel;  //编码模式
	int saveCodeModel;  //要保存的编码模式
	int openModel;  //打开模式 1表示读取 2表示保存写入
	QString saveData;  //保存数据

signals:
	void readData(const QString &data, int model);  //发送读取数据
	void saveFinish(bool state);  //保存完成
};
