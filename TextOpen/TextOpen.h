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
	void loadOpenFile(const QString &str);  //打开文件

private:
	Ui::TextOpenClass ui;
	Ui::Skip uiSkip;
	Ui::Search uiSearch;
	QWidget *widgetSkip;
	QWidget *widgetSearch;
	TextOpenThread *textOpen;  //打开文本线程
	TextScanThread *textScan;  //扫描文本线程
	QLabel *label;  //页面状态
	QLabel *label_2;  //编码
	QLabel *label_3;  //光标位置
	qint64 totalPage;  //总页数
	qint64 nowPage;  //现在页数
	QMap<int, qint64> pagePos;  //页面位置
	bool loadModel;  //扫描线程退出后是否加载
	struct {
		int pageUp;
		int pageDown;
		bool pageUpModel;
		bool pageDownModel;
	}pageMove;  //自动翻页
	QScrollBar *bar;  //文本框进度条
	int codeModel;  //编码模式
	bool saveStatus;  //是否正在保存
	int saveModel; //保存模式
	bool openStatus;  //是否已经打开文件

private slots:
	void openFile();  //打开文件
	void readData(const QString &data, int model);  //加载文本
	void scanLoading();  //扫描中
	void scanFinish();  //扫描完成
	void skipPage();  //跳转页面
	void skipPushButton();  //跳转按钮
	void exitFinish();  //退出完成
	void about(); //关于
	void actionTriggered(int value);  //阅读进度条变化
	void findText();  //查找内容
	void findTextUp();  //查找上一个
	void findTextDown();  //查找下一个
	void switchFineMode(int state);  //查找内容正则勾选状态
	void setFont();  //设置字体
	void saveText();  //保存文本
	void saveFinish(bool state);  //保存完成
	void lineNum();  //更新行号
	void lineModel(bool checked);  //切换换行模式
	void saveAs();  //另存为
	void codeModelAction(QAction *a);  //编码模式改变

protected:
	void closeEvent(QCloseEvent *event);  //关闭事件

};
