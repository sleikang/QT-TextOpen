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
	void setFileName(const QString &str);  //�����ļ���
	void setNowSeek(qint64 num);  //�����ļ�λ��
	void setOpenModel(int model);  //���ô�ģʽ
	void setSaveData(const QString &data);  //����Ҫ���������
	void setSaveCodeModel(int model);  //����Ҫ����ı���ģʽ
	void setSaveAsFileName(const QString &file);  //��������ļ���
	void setCodeModel(int model);  //���õ�ǰ����ģʽ

private:
	qint64 nowSeek;  //�����ı�λ��
	qint64 nowSeekEnd;  //���ڶ�ȡ�ı�����λ��
	QString fileName;  //�ļ���
	QString saveAsFile;  //����ļ���
	QString data;  //��������
	int codeModel;  //����ģʽ
	int saveCodeModel;  //Ҫ����ı���ģʽ
	int openModel;  //��ģʽ 1��ʾ��ȡ 2��ʾ����д��
	QString saveData;  //��������

signals:
	void readData(const QString &data, int model);  //���Ͷ�ȡ����
	void saveFinish(bool state);  //�������
};
