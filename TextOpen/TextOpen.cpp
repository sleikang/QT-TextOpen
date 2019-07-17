#include "TextOpen.h"
#include <QFileDialog>
#include "TextOpenThread.h"
#include "TextScanThread.h"
#include <QLabel>
#include <QIntValidator>
#include <QTextCursor>
#include <QMessageBox>
#include <QEvent>
#include <QScrollBar>
#include <QFontDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QActionGroup>
#include <QDebug>

TextOpen::TextOpen(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	saveStatus = false;
	widgetSkip = new QWidget;
	uiSkip.setupUi(widgetSkip);
	widgetSearch = new QWidget;
	uiSearch.setupUi(widgetSearch);
	uiSkip.lineEdit->setValidator(new QIntValidator(1, 100000000, this));
	saveModel = 0;
	openStatus = false;

	//跳转页面按钮信号
	connect(uiSkip.pushButton, &QPushButton::clicked, this, &TextOpen::skipPushButton);

	//查找内容按钮信号上一个 下一个
	connect(uiSearch.pushButton, &QPushButton::clicked, this, &TextOpen::findTextUp);
	connect(uiSearch.pushButton_2, &QPushButton::clicked, this, &TextOpen::findTextDown);

	connect(ui.action, &QAction::triggered, this, &TextOpen::openFile);
	connect(ui.action_2, &QAction::triggered, this, &TextOpen::skipPage);
	connect(ui.action_3, &QAction::triggered, this, &TextOpen::about);
	connect(ui.action_4, &QAction::triggered, this, &TextOpen::findText);
	connect(ui.action_5, &QAction::triggered, this, &TextOpen::findTextUp);
	connect(ui.action_6, &QAction::triggered, this, &TextOpen::findTextDown);
	connect(ui.action_7, &QAction::triggered, this, &TextOpen::setFont);
	connect(ui.action_8, &QAction::triggered, this, &TextOpen::saveText);
	connect(ui.action_9, &QAction::triggered, this, &TextOpen::lineModel);
	connect(ui.action_10, &QAction::triggered, this, &TextOpen::saveAs);

	//编码设置按钮组
	QActionGroup *actionGroup = new QActionGroup(this);
	actionGroup->addAction(ui.action_11);
	actionGroup->addAction(ui.action_12);
	connect(actionGroup, &QActionGroup::triggered, this, &TextOpen::codeModelAction);

	//读取文本线程
	textOpen = new TextOpenThread(this);
	connect(textOpen, &TextOpenThread::readData, this, &TextOpen::readData);
	connect(textOpen, &TextOpenThread::saveFinish, this, &TextOpen::saveFinish);

	//扫描文本线程
	textScan = new TextScanThread(this);
	connect(textScan, &TextScanThread::scanLoading, this, &TextOpen::scanLoading);
	connect(textScan, &TextScanThread::scanFinish, this, &TextOpen::scanFinish);
	//扫描线程退出完成信号
	loadModel = false;
	connect(textScan, &TextScanThread::finished, this, &TextOpen::exitFinish);

	//状态栏信息
	label = new QLabel(this);
	label_2 = new QLabel(this);
	label_3 = new QLabel(this);
	label->setMinimumWidth(80);
	label_2->setMinimumWidth(50);
	label_3->setMinimumWidth(100);
	ui.statusBar->addPermanentWidget(new QLabel(this));
	ui.statusBar->addPermanentWidget(label_2);
	ui.statusBar->addPermanentWidget(label_3);
	ui.statusBar->addPermanentWidget(label);
	label->setText("1/1");
	label_2->setText("ANSI");
	label_3->setText(QStringLiteral("第1行 第1列"));

	//初始化总页数 现在页数
	totalPage = 1;
	nowPage = 1;

	//初始化滑动检测自动翻页
	pageMove.pageUp = 0;
	pageMove.pageDown = 0;
	pageMove.pageUpModel = false;
	pageMove.pageDownModel = false;
	bar = ui.textEdit->verticalScrollBar();
	connect(bar, &QScrollBar::actionTriggered, this, &TextOpen::actionTriggered);

	//查找内容正则勾选信号
	connect(uiSearch.checkBox_3, &QCheckBox::stateChanged, this, &TextOpen::switchFineMode);

	//拖入文件
	connect(ui.textEdit, &TextEdit::dropFileName, this, &TextOpen::loadOpenFile);

	//光标位置变化
	connect(ui.textEdit, &TextEdit::cursorPositionChanged, this, &TextOpen::lineNum);
}

//打开文件
void TextOpen::openFile()
{
	if (!saveStatus) {
		QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开文本文件"));
		if (fileName.isEmpty()) {
			ui.statusBar->showMessage(QStringLiteral("未选择文件！"), 1000);
		}
		else {
			ui.textEdit->clear();
			textOpen->setFileName(fileName);
			textScan->setFileName(fileName);

			if (textOpen->isRunning())
				textOpen->exit();
			if (textScan->isRunning()) {
				textScan->setOpenModel(false);
				textScan->exit();
				loadModel = true;
			}
			else {
				textOpen->setNowSeek(0);
				textScan->setOpenModel(true);
				loadModel = false;
				textOpen->setOpenModel(1);
				textOpen->start();
				textScan->start();
				nowPage = 1;
				totalPage = 0;
				ui.statusBar->showMessage(QStringLiteral("总页面数量扫描中..."));
				openStatus = true;
			}
		}
	}
	else {
		ui.statusBar->showMessage(QStringLiteral("正在保存文件，无法打开文件！"));
	}
}

TextOpen::~TextOpen()
{
}

//打开文件
void TextOpen::loadOpenFile(const QString & str)
{
	if (!str.isEmpty()) {
		ui.textEdit->clear();
		textOpen->setFileName(str);
		textScan->setFileName(str);

		if (textOpen->isRunning())
			textOpen->exit();
		if (textScan->isRunning()) {
			textScan->setOpenModel(false);
			textScan->exit();
			loadModel = true;
		}
		else {
			textOpen->setNowSeek(0);
			textScan->setOpenModel(true);
			loadModel = false;
			textOpen->setOpenModel(1);
			textOpen->start();
			textScan->start();
			nowPage = 1;
			totalPage = 0;
			ui.statusBar->showMessage(QStringLiteral("总页面数量扫描中..."));
			openStatus = true;
		}
	}
}

//加载文本
void TextOpen::readData(const QString & data, int model)
{
	ui.textEdit->clear();
	ui.textEdit->append(data);
	codeModel = model;

	//翻页模式
	if (pageMove.pageDownModel) {
		pageMove.pageDownModel = false;
		QTextCursor textCursor = ui.textEdit->textCursor();
		textCursor.setPosition(0);
		ui.textEdit->setTextCursor(textCursor);
	}
	else if (pageMove.pageUpModel) {
		pageMove.pageUpModel = false;
	}
	else {
		QTextCursor textCursor = ui.textEdit->textCursor();
		textCursor.setPosition(0);
		ui.textEdit->setTextCursor(textCursor);
	}

	label->setText(QStringLiteral("%1/%2").arg(nowPage).arg(totalPage));
	if (codeModel == 1) {
		label_2->setText("ANSI");
		ui.action_11->setChecked(true);
	}
	else if (codeModel == 2) {
		label_2->setText("UTF-8");
		ui.action_12->setChecked(true);
	}
}

//扫描中
void TextOpen::scanLoading()
{
	totalPage = textScan->readPage();
	pagePos = textScan->readPagePos();
	label->setText(QString("%1/%2").arg(nowPage).arg(totalPage));
}


//扫描完成
void TextOpen::scanFinish()
{
	totalPage = textScan->readPage();
	pagePos = textScan->readPagePos();
	label->setText(QString("%1/%2").arg(nowPage).arg(totalPage));
	ui.statusBar->showMessage(QStringLiteral("总页面数量扫描完成！"), 1000);
}

//跳转页面
void TextOpen::skipPage()
{
	if (widgetSkip->isHidden())
		widgetSkip->show();
}

//跳转按钮
void TextOpen::skipPushButton()
{
	if (!saveStatus) {
		if (totalPage == 0) {
			ui.statusBar->showMessage(QStringLiteral("正在扫描页面数量，无法跳转！"), 1000);
		}
		else {
			if (uiSkip.lineEdit->text().toInt() <= 0) {
				ui.statusBar->showMessage(QStringLiteral("页面必须大于等于1，无法跳转！"), 1000);
			}
			else if (uiSkip.lineEdit->text().toInt() <= totalPage) {
				textOpen->setNowSeek(pagePos.value(uiSkip.lineEdit->text().toInt()));
				textOpen->setOpenModel(1);
				textOpen->start();
				nowPage = uiSkip.lineEdit->text().toInt();
				label->setText(QStringLiteral("%1/%2 跳转中...").arg(nowPage).arg(totalPage));
			}
			else {
				ui.statusBar->showMessage(QStringLiteral("页面大于已扫描页面，无法跳转！"), 1000);
			}
		}
	}
	else {
		ui.statusBar->showMessage(QStringLiteral("正在保存文件，无法跳转！"));
	}
}

//退出完成
void TextOpen::exitFinish()
{
	if (loadModel) {    
		textScan->setOpenModel(true);
		textOpen->setNowSeek(0);
		textOpen->setOpenModel(1);
		textOpen->start();
		textScan->start();
		nowPage = 1;
		totalPage = 0;
		loadModel = false;
		ui.statusBar->showMessage(QStringLiteral("总页面数量扫描中..."));
		openStatus = true;
	}
}

//关于
void TextOpen::about()
{
	QMessageBox::about(this, QStringLiteral("关于文本阅读器"), QStringLiteral("版本：1.2\n开发：神秘\nQQ：735609378"));
}

//阅读进度条变化
void TextOpen::actionTriggered(int value)
{
	if (!saveStatus) {
		if (bar->value() == bar->minimum()) {
			if (++pageMove.pageUp == 3) {
				pageMove.pageUp = 0;
				pageMove.pageDown = 0;
				if (nowPage > 1) {
					textOpen->setNowSeek(pagePos.value(--nowPage));
					textOpen->setOpenModel(1);
					textOpen->start();
					label->setText(QStringLiteral("%1/%2 翻页中...").arg(nowPage).arg(totalPage));
					pageMove.pageUpModel = true;
				}
			}
		}
		else if (bar->value() == bar->maximum()) {
			if (++pageMove.pageDown == 3) {
				pageMove.pageUp = 0;
				pageMove.pageDown = 0;
				if (nowPage < totalPage) {
					textOpen->setNowSeek(pagePos.value(++nowPage));
					textOpen->setOpenModel(1);
					textOpen->start();
					label->setText(QStringLiteral("%1/%2 翻页中...").arg(nowPage).arg(totalPage));
					pageMove.pageDownModel = true;
				}
			}
		}
	}
}

//查找内容
void TextOpen::findText()
{
	if (widgetSearch->isHidden())
		widgetSearch->show();
}

//查找上一个
void TextOpen::findTextUp()
{
	if (uiSearch.checkBox_3->isChecked()) {
		bool findModel;
		findModel = ui.textEdit->find(QRegExp(uiSearch.lineEdit->text()), QTextDocument::FindBackward);

		if (findModel) {
			ui.statusBar->showMessage(QStringLiteral("查找到内容！"), 1000);
		}
		else {
			ui.statusBar->showMessage(QStringLiteral("未查找到内容！"), 1000);
		}
		this->activateWindow();
	}
	else {
		bool findModel;
		if (uiSearch.checkBox->isChecked() && uiSearch.checkBox_2->isChecked()) {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text(), QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords | QTextDocument::FindBackward);
		}
		else if (uiSearch.checkBox->isChecked()) {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text(), QTextDocument::FindCaseSensitively | QTextDocument::FindBackward);
		}
		else if (uiSearch.checkBox_2->isChecked()) {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text(), QTextDocument::FindWholeWords | QTextDocument::FindBackward);
		}
		else {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text(), QTextDocument::FindBackward);
		}

		if (findModel) {
			ui.statusBar->showMessage(QStringLiteral("查找到内容！"), 1000);
		}
		else {
			ui.statusBar->showMessage(QStringLiteral("未查找到内容！"), 1000);
		}
		this->activateWindow();
	}
}

//查找下一个
void TextOpen::findTextDown()
{
	if (uiSearch.checkBox_3->isChecked()) {
		bool findModel;
		findModel = ui.textEdit->find(QRegExp(uiSearch.lineEdit->text()));

		if (findModel) {
			ui.statusBar->showMessage(QStringLiteral("查找到内容！"), 1000);
		}
		else {
			ui.statusBar->showMessage(QStringLiteral("未查找到内容！"), 1000);
		}
		this->activateWindow();
	}
	else {
		bool findModel;
		if (uiSearch.checkBox->isChecked() && uiSearch.checkBox_2->isChecked()) {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text(), QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords);
		}
		else if (uiSearch.checkBox->isChecked()) {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text(), QTextDocument::FindCaseSensitively);
		}
		else if (uiSearch.checkBox_2->isChecked()) {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text(), QTextDocument::FindWholeWords);
		}
		else {
			findModel = ui.textEdit->find(uiSearch.lineEdit->text());
		}

		if (findModel) {
			ui.statusBar->showMessage(QStringLiteral("查找到内容！"), 1000);
		}
		else {
			ui.statusBar->showMessage(QStringLiteral("未查找到内容！"), 1000);
		}
		this->activateWindow();
	}
}

//查找内容正则勾选状态
void TextOpen::switchFineMode(int state)
{
	if (state == Qt::Checked) {
		uiSearch.checkBox->setChecked(false);
		uiSearch.checkBox->setEnabled(false);
		uiSearch.checkBox_2->setChecked(false);
		uiSearch.checkBox_2->setEnabled(false);
	}
	else if (state == Qt::Unchecked) {
		uiSearch.checkBox->setEnabled(true);
		uiSearch.checkBox_2->setEnabled(true);
	}
}

//设置字体
void TextOpen::setFont()
{
	bool open;
	QFont font = QFontDialog::getFont(&open, this);
	if (open) {
		ui.textEdit->setFont(font);
		ui.statusBar->showMessage(QStringLiteral("设置字体成功！"), 1000);
	}
	else {
		ui.statusBar->showMessage(QStringLiteral("未选择字体！"), 1000);
	}
}

//保存文本
void TextOpen::saveText()
{
	if (openStatus) {
		if (textScan->isFinished()) {
			if (ui.action_11->isChecked())
				textOpen->setSaveCodeModel(1);
			else
				textOpen->setSaveCodeModel(2);
			saveModel = 1;
			textOpen->setOpenModel(2);
			textOpen->setSaveData(ui.textEdit->toPlainText());
			textOpen->start();
			saveStatus = true;
			ui.statusBar->showMessage(QStringLiteral("保存中..."));
		}
		else {
			ui.statusBar->showMessage(QStringLiteral("扫描页面未完成，当前无法保存！"), 1000);
		}
	}
	else {
		saveAs();
	}
}

//保存完成
void TextOpen::saveFinish(bool state)
{
	if (state) {
		if (openStatus) {
			if (saveModel == 1) {
				totalPage = 0;
				pagePos.clear();
				textScan->setOpenModel(true);
				textScan->start();
				saveStatus = false;
				label->setText(QStringLiteral("%1/%2 扫描中...").arg(nowPage).arg(totalPage));
				textOpen->setCodeModel(codeModel);
				if (codeModel == 1) {
					label_2->setText("ANSI");
				}
				else if (codeModel == 2) {
					label_2->setText("UTF-8");
				}

				ui.statusBar->showMessage(QStringLiteral("保存完成，正在重新扫描页面！"));
			}
			else if (saveModel == 2) {
				saveStatus = false;
				ui.statusBar->showMessage(QStringLiteral("另存文件完成！"), 1000);
			}
		}
		else {
			textOpen->setNowSeek(0);
			loadModel = false;
			textOpen->setOpenModel(1);
			textOpen->start();

			totalPage = 0;
			pagePos.clear();
			textScan->setOpenModel(true);
			textScan->start();
			saveStatus = false;
			label->setText(QStringLiteral("%1/%2 扫描中...").arg(nowPage).arg(totalPage));
			ui.statusBar->showMessage(QStringLiteral("另存文件完成，正在扫描页面！"));
			openStatus = true;
		}
	}
	else {
		saveStatus = false;
		ui.statusBar->showMessage(QStringLiteral("保存失败！"), 1000);
	}
}

//更新行号
void TextOpen::lineNum()
{
	QTextCursor textCursor = ui.textEdit->textCursor();
	long long row = long long(nowPage - 1) * long long(2000) + long long(textCursor.blockNumber()) + long long(1);
	label_3->setText(QStringLiteral("第%1行 第%2列").arg(row).arg(textCursor.columnNumber() + 1));
}

//切换换行模式
void TextOpen::lineModel(bool checked)
{
	if (checked) {
		ui.textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
	}
	else {
		ui.textEdit->setLineWrapMode(QTextEdit::NoWrap);
	}
}

//另存为
void TextOpen::saveAs()
{
	if (openStatus) {
		if (textScan->isFinished()) {
			QString fileNameAs = QFileDialog::getSaveFileName(this, QStringLiteral("另存为"), "C:", QStringLiteral("文本文件 (*.txt);;所有文件 (.)"));
			if (ui.action_11->isChecked())
				textOpen->setSaveCodeModel(1);
			else
				textOpen->setSaveCodeModel(2);
			saveModel = 2;
			textOpen->setOpenModel(3);
			textOpen->setSaveAsFileName(fileNameAs);
			textOpen->setSaveData(ui.textEdit->toPlainText());
			textOpen->start();
			saveStatus = true;
			ui.statusBar->showMessage(QStringLiteral("另存文件中..."));
		}
		else {
			ui.statusBar->showMessage(QStringLiteral("扫描页面未完成，当前无法保存！"), 1000);
		}
	}
	else {
		QString fileNameAs = QFileDialog::getSaveFileName(this, QStringLiteral("另存为"), "C:", QStringLiteral("文本文件 (*.txt);;所有文件 (.)"));
		if (ui.action_11->isChecked())
			textOpen->setSaveCodeModel(1);
		else
			textOpen->setSaveCodeModel(2);
		saveModel = 2;
		textOpen->setOpenModel(4);
		textOpen->setFileName(fileNameAs);
		textOpen->setSaveAsFileName(fileNameAs);
		textOpen->setSaveData(ui.textEdit->toPlainText());
		textOpen->start();
		saveStatus = true;
		ui.statusBar->showMessage(QStringLiteral("另存文件中..."));
		textScan->setFileName(fileNameAs);
	}
}

//编码模式改变
void TextOpen::codeModelAction(QAction * a)
{
	if (a->text() == "ANSI") {
		codeModel = 1;
	}
	else if (a->text() == "UTF-8") {
		codeModel = 2;
	}

	if (!openStatus) {
		if (codeModel == 1) {
			label_2->setText("ANSI");
		}
		else if (codeModel == 2) {
			label_2->setText("UTF-8");
		}
	}
}

//关闭事件
void TextOpen::closeEvent(QCloseEvent * event)
{
	if (!saveStatus) {
		widgetSkip->close();
		widgetSearch->close();
		textScan->setOpenModel(false);
	}
	else {
		ui.statusBar->showMessage(QStringLiteral("当前正在保存文件，避免数据丢失请不要关闭软件！"));
		event->ignore();  //丢弃关闭事件
	}
}
