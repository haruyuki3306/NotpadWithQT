#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NoteLiteMainPanel.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBlock>
#include <QTextCursor>
#include <QWidget>
#include <qDebug>
#include <QAction>

class NoteLiteMainPanel : public QMainWindow
{
	Q_OBJECT

public:
	NoteLiteMainPanel(QWidget *parent = Q_NULLPTR);
	~NoteLiteMainPanel();
	/** 状态栏用到的变量 */
	QWidget *statusPanel;
	QHBoxLayout *statusLayout;
	QLabel *statusLineNumLabel;
	QLabel *statusZoomLabel;
	QLabel *statusSystemLabel;
	QLabel *statusChartsetLabel;

	NoteLiteMainPanel *w;
	QString windowTitleName = tr("Notepad-unknowtitle");

protected:
	bool eventFilter(QObject *target,QEvent *event);
	void closeEvent(QCloseEvent *event);
private:
	Ui::NoteLiteMainPanelClass ui;
	QString openFileName;
	QMenu *zoomMenus = new QMenu(tr("zoom"),this);
	QAction *actionZoomIn;
	QAction *actionZoomOut;
	QAction *actionZoomDefault;

private:
	void initActions(void);
	void statusBarInitSetting(void);/** 状态栏设置 */
	void getTextEditPos(void);

	void setLanguage();

	public slots:
	void openFile(void);
	void newFile(void);
	void newWindow(void);
	void saveFile(void);
	void saveFileAs(void);
	void windowTitleNameAddStar();
	void showStatusOrNot(void);
	void setTextFontSlot(void);
};
