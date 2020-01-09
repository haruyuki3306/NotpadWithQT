#include "NoteLiteMainPanel.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFontDialog>

NoteLiteMainPanel::NoteLiteMainPanel(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(windowTitleName);
	statusBarInitSetting();
	ui.textEdit->installEventFilter(this);//给textEdit绑定事件过滤器，当textEdit有事件发生时会调用事件过滤器的eventFilter函数
	ui.actionStatusBar->setCheckable(true);
	ui.actionStatusBar->setChecked(true);
	actionZoomIn = new QAction(tr("zoomIn"));
	actionZoomOut = new QAction(tr("zoomOut"));
	actionZoomDefault = new QAction(tr("defaultZoom"));
	zoomMenus->addAction(actionZoomIn);
	zoomMenus->addAction(actionZoomOut);
	zoomMenus->addAction(actionZoomDefault);
	ui.menuView->addMenu(zoomMenus);
	initActions();
}

NoteLiteMainPanel::~NoteLiteMainPanel()
{
	if (w == nullptr)
	{
	}
	else
	{
		w = nullptr;//这里是将W指为空指针，如果这里用delete w的方法是会出错的，而且错得隐秘，要dbug模式才能看到出错
	}
}

void NoteLiteMainPanel::initActions(void)
{
	QObject::connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	QObject::connect(ui.actionNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
	QObject::connect(ui.actionNewWindow, SIGNAL(triggered()), this, SLOT(newWindow()));
	QObject::connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
	QObject::connect(ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
	QObject::connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));//因为close()会自动调用closeEvent()，所以直接绑定close就可以了
	QObject::connect(ui.textEdit, SIGNAL(textChanged()), this, SLOT(windowTitleNameAddStar()));
	QObject::connect(ui.actionRedo, SIGNAL(triggered()), ui.textEdit, SLOT(undo()));
	QObject::connect(ui.actionCopy, SIGNAL(triggered()), ui.textEdit, SLOT(copy()));
	QObject::connect(ui.actionCut, SIGNAL(triggered()), ui.textEdit, SLOT(cut()));
	QObject::connect(ui.actionPaste, SIGNAL(triggered()), ui.textEdit, SLOT(paste()));
	QObject::connect(ui.actionSelectAll, SIGNAL(triggered()), ui.textEdit, SLOT(selectAll()));
	QObject::connect(ui.actionStatusBar, SIGNAL(triggered()), this, SLOT(showStatusOrNot()));
	QObject::connect(ui.actionFont, SIGNAL(triggered()), this, SLOT(setTextFontSlot()));
}

void NoteLiteMainPanel::statusBarInitSetting(void)
{
	//this->statusBar()->setStyleSheet(QString("QStatusBar::item{border:5px}"));//设置状态栏里的东西的边框为0，比如放一个label在状态栏里，那么这个label的边框为0
	/** 状态栏里加控件有几种方法，一个可以用布局管理器，布局管理器里可以再放很多控件。二是直接用statusBar()->addPermanentWidget将控件放进去 */
	statusLineNumLabel = new QLabel(tr("row 1,column 1"));
	statusZoomLabel = new QLabel(tr("100%"));
	statusSystemLabel = new QLabel(tr("windows"));
	statusChartsetLabel = new QLabel(tr("UTF-8"));
	/** 这里不用布局管理，直接在状态栏加控件 */
	this->statusBar()->addPermanentWidget(statusLineNumLabel);
	this->statusBar()->addPermanentWidget(statusZoomLabel);
	this->statusBar()->addPermanentWidget(statusSystemLabel);
	this->statusBar()->addPermanentWidget(statusChartsetLabel);
}

void NoteLiteMainPanel::getTextEditPos(void)
{
	/** 这个函数用来获取文本区，这里是textEdit的光标所在的位置，然后显示在状态栏上 */
	QTextCursor cursor = ui.textEdit->textCursor();
	QTextLayout *tLayout = cursor.block().layout();
	int colPos = cursor.position() - cursor.block().position();//获得textEdit中光标所在的列数。cursor.position()是光标在所有内容中排序的位置，cursor.block().position()是内容格式化后的位置，两个值相减得出光标在一行中的哪一列
	int lineNum = tLayout->lineForTextPosition(colPos).lineNumber() + cursor.block().firstLineNumber();//获得textEdit中光标的行数。tLayout->lineForTextPosition(colPos).lineNumber()是光标所在的行，cursor.block().firstLineNumber()是格式化文本的行，两者相加才是光标在文本中的行
																									   //qDebug()<<"position:"<<cursor.position()- cursor.block().position();
	statusLineNumLabel->setText(tr("line:") + QString::number(lineNum + 1) + tr(",column:") + QString::number(colPos + 1) + tr(" "));//因为程序内部计数是从0开始的，显示出来要以人的习惯为准，所以要lineNum+1，从1开始
	//statusLineNumLabel->setText("p:"+pos.y());
}

void NoteLiteMainPanel::newFile(void)
{
	if (ui.textEdit->document()->isEmpty())
	{
	}
	else
	{
		this->setWindowTitle(windowTitleName);
		ui.textEdit->clear();
		openFileName = nullptr;
	}
}

void NoteLiteMainPanel::newWindow(void)
{
	w = new NoteLiteMainPanel;//这个方法是new一个新对像，而new出来的对像不用时是要手动delete的，所以用这种方法产生新窗口是不好的，会造成内存问题。这里是通过在析构函数~NoteLiteMainPanel中delete
	w->show();
}

void NoteLiteMainPanel::saveFile(void)
{
	if (openFileName.isEmpty())
	{
		saveFileAs();
	}
	else
	{
		if (ui.textEdit->document()->isModified())
		{
			QFile *file = new QFile;
			file->setFileName(openFileName);
			bool openOK = file->open(QIODevice::WriteOnly);
			if (openOK)
			{
				QTextStream out(file);
				out << ui.textEdit->toPlainText();//将textEdit中的内容转换成纯文本
				file->close();
				QObject::connect(ui.textEdit, SIGNAL(textChanged()), this, SLOT(windowTitleNameAddStar()));
			}
			delete file;
		}
	}
}

void NoteLiteMainPanel::saveFileAs(void)
{
	QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save File ..."), QDir::currentPath(), tr("text file(*.txt);;all file(*)"));
	/** 这里应该要有一行用来判断要保存的文件名是否存在或者重复之类的 */
	QFile *file = new QFile;
	file->setFileName(saveFileName);
	bool openOK = file->open(QIODevice::WriteOnly);
	if (openOK)
	{
		QTextStream out(file);
		out << ui.textEdit->toPlainText();
		file->close();
		openFileName = saveFileName;
		this->setWindowTitle(openFileName);
		delete file;
		QObject::connect(ui.textEdit, SIGNAL(textChanged()), this, SLOT(windowTitleNameAddStar()));
	}
	else {
		QMessageBox::information(this, tr("Error Message"), tr("File save error") + file->errorString());
		delete file;
	}
}

void NoteLiteMainPanel::windowTitleNameAddStar()
{
	/** 这个槽是用来在窗口标题栏加一个星号（*）的。在记事本的文本编辑的时候在标题栏加一个*号来表示文本未保存 */
	/** 在文本内容更新的时候触发这个槽，在标题栏加星号，但要保证只触发一次，就要在触发完之后disconnect掉，在文本保存动作时再connect回来 */
	this->setWindowTitle("*" + this->windowTitle());
	QObject::disconnect(ui.textEdit, SIGNAL(textChanged()), this, SLOT(windowTitleNameAddStar()));
}

void NoteLiteMainPanel::showStatusOrNot(void)
{
	if (ui.actionStatusBar->isChecked())
	{
		this->statusBar()->show();
	}
	else
	{
		this->statusBar()->hide();
	}
}

void NoteLiteMainPanel::setTextFontSlot(void)
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, QFont("Times",12), this);
	if (ok)
	{
		ui.textEdit->setFont(font);
	}
	else
	{
		//进入这里说明设置字体不成功，应该在这里作出错处理
		return;
	}
}

void NoteLiteMainPanel::openFile(void)
{
	openFileName = QFileDialog::getOpenFileName(this, tr("Open File ..."));
	QFileInfo fileInfo = QFileInfo(openFileName);
	QString fileName = fileInfo.fileName();
	if (openFileName.isEmpty())
	{
		QMessageBox::information(this, tr("Alert"), tr("Please select a file"));
		return;
	}
	QFile *file = new QFile;
	file->setFileName(openFileName);
	bool isOpen = file->open(QIODevice::ReadOnly);
	if (isOpen)
	{
		this->setWindowTitle(fileName);
		QTextStream in(file);
		ui.textEdit->setText(in.readAll());
		file->close();
		delete file;
		//QObject::connect(ui.textEdit, SIGNAL(textChanged()), this, SLOT(windowTitleNameAddStar()));
	}
	else
	{

	}

}

bool NoteLiteMainPanel::eventFilter(QObject *target, QEvent *event)
{
	/** 在textEdit中绑定这个事件过滤，所有textEdit发生的事件都会调用这个函数，在这个函数里做需要的处理就可以了，比如对键盘某个键处理 */
	getTextEditPos();
	return QWidget::eventFilter(target, event);//调用父类的过滤方法，将事件转到父类，这样才不影响原来的功能
}

void NoteLiteMainPanel::closeEvent(QCloseEvent * event)
{
	/** close()函数个自动调用这个函数 */

	/**
	 * 这里的条件判断比较复杂，分析实际使用情景，有两种比较特殊的情况。
	 * 第一种情况：打开这个记事本程序，就在空白文本区输入文字，然后将输入的文字全删除，结果是文本区还是空白的
	 * 第二种情况：打开这个记事本程序，然后执行打开文件操作，随意打开一个有内容的txt文件，然后将内容全删除，结果是文本区是空白的
	 * 两种情况的结果文本区都是空白的，那么在这个空白的情况下用户点退出按钮，程序当然要执行退出操作。退出前如何处理呢？
	 * 第一种情况，文本区一开始的时候是空白的，结果也是空白的，这种情况通常可以认为是不需要保存的，程序直接退出即可
	 * 第二种情况，文本区一开始是有内容的，结果是空白的，那么说明内容经过了更改，通常认为是需要保存的，程序要先执行保存才能退出
	 * 用这个条件ui.textEdit->document()->isModified()&&!(ui.textEdit->document()->isEmpty())去判断，就是文本区有更新而且是空的，符合第一种情况，但是第二种情况也是符合的，没法区分
	 * f
	 */
	if (ui.textEdit->document()->isModified() && !(ui.textEdit->document()->isEmpty()))//判断依据是文本区内容发生变化，而且内容不为空。如果是空的就没必要保存了
	{
		QMessageBox msgBox;
		if (openFileName == "")
		{
			/** 没有保存过，那么openFileName就会是空的，那么给个名字，这里是无标题(unknowtitle) */
			openFileName = "unknowtitle";
		}
		msgBox.setWindowTitle("notepad-alert");
		msgBox.setInformativeText("Save edit to" + openFileName + "?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setButtonText(QMessageBox::Save, "save");
		msgBox.setButtonText(QMessageBox::Discard, "discard");
		msgBox.setButtonText(QMessageBox::Cancel, "cancel");
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		switch (ret)
		{
		case QMessageBox::Save:
			saveFileAs();
			//这里直接用保存逻辑上会有BUG，因为上面openFileName设置成了unknowtitle。用另存为会弹窗选择路径，对于已经有路径的文件这是不好的用户体验。所以这里需要优化
			//还有一个问题是，如果另存为的时候弹窗让用户选择保存路径，但用户没有选择，直接点取消，程序继续往下执行窗口关闭，那么就会引发bug了。
			break;
		case  QMessageBox::Discard:
			/** 选择不保存则直接关闭,接受事件，关闭窗口 */
			event->accept();
			break;
		case QMessageBox::Cancel:
			/** 选择取消则不作任何处理，直接将事件忽略 */
			event->ignore();
			break;
		default:
			break;
		}
	}
	else
	{
		event->accept();
	}
}

