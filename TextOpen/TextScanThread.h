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
	void setFileName(const QString &str);  //�����ļ���
	const QMap<int, qint64> readPagePos();  //����ҳ��λ��
	const qint64 readPage();  //������ҳ��
	void setOpenModel(bool o); //�����߳̿���

private:
	qint64 totalPage;  //��ҳ��
	QMap<int, qint64> pagePos;  //ҳ��λ��
	QString fileName;  //�ļ���
	bool open;  //�߳̿���

signals:
	void scanLoading();  //ɨ����
	void scanFinish();  //ɨ�����
};
