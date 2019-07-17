#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TextOpen.h"
#include "ui_skipPage.h"
#include "ui_search.h"

class TextOpenThread;
class TextScanThread;
class QLabel;
class QScrollBar;

class TextOpen : public QMainWindow
{
	Q_OBJECT

public:
	TextOpen(QWidget *parent = Q_NULLPTR);
	~TextOpen();
	void loadOpenFile(const QString &str);  //���ļ�

private:
	Ui::TextOpenClass ui;
	Ui::Skip uiSkip;
	Ui::Search uiSearch;
	QWidget *widgetSkip;
	QWidget *widgetSearch;
	TextOpenThread *textOpen;  //���ı��߳�
	TextScanThread *textScan;  //ɨ���ı��߳�
	QLabel *label;  //ҳ��״̬
	QLabel *label_2;  //����
	QLabel *label_3;  //���λ��
	qint64 totalPage;  //��ҳ��
	qint64 nowPage;  //����ҳ��
	QMap<int, qint64> pagePos;  //ҳ��λ��
	bool loadModel;  //ɨ���߳��˳����Ƿ����
	struct {
		int pageUp;
		int pageDown;
		bool pageUpModel;
		bool pageDownModel;
	}pageMove;  //�Զ���ҳ
	QScrollBar *bar;  //�ı��������
	int codeModel;  //����ģʽ
	bool saveStatus;  //�Ƿ����ڱ���
	int saveModel; //����ģʽ
	bool openStatus;  //�Ƿ��Ѿ����ļ�

private slots:
	void openFile();  //���ļ�
	void readData(const QString &data, int model);  //�����ı�
	void scanLoading();  //ɨ����
	void scanFinish();  //ɨ�����
	void skipPage();  //��תҳ��
	void skipPushButton();  //��ת��ť
	void exitFinish();  //�˳����
	void about(); //����
	void actionTriggered(int value);  //�Ķ��������仯
	void findText();  //��������
	void findTextUp();  //������һ��
	void findTextDown();  //������һ��
	void switchFineMode(int state);  //������������ѡ״̬
	void setFont();  //��������
	void saveText();  //�����ı�
	void saveFinish(bool state);  //�������
	void lineNum();  //�����к�
	void lineModel(bool checked);  //�л�����ģʽ
	void saveAs();  //���Ϊ
	void codeModelAction(QAction *a);  //����ģʽ�ı�

protected:
	void closeEvent(QCloseEvent *event);  //�ر��¼�

};
