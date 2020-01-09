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
	ui.textEdit->installEventFilter(this);//��textEdit���¼�����������textEdit���¼�����ʱ������¼���������eventFilter����
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
		w = nullptr;//�����ǽ�WָΪ��ָ�룬���������delete w�ķ����ǻ����ģ����Ҵ�����أ�Ҫdbugģʽ���ܿ�������
	}
}

void NoteLiteMainPanel::initActions(void)
{
	QObject::connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	QObject::connect(ui.actionNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
	QObject::connect(ui.actionNewWindow, SIGNAL(triggered()), this, SLOT(newWindow()));
	QObject::connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
	QObject::connect(ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
	QObject::connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));//��Ϊclose()���Զ�����closeEvent()������ֱ�Ӱ�close�Ϳ�����
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
	//this->statusBar()->setStyleSheet(QString("QStatusBar::item{border:5px}"));//����״̬����Ķ����ı߿�Ϊ0�������һ��label��״̬�����ô���label�ı߿�Ϊ0
	/** ״̬����ӿؼ��м��ַ�����һ�������ò��ֹ����������ֹ�����������ٷźܶ�ؼ�������ֱ����statusBar()->addPermanentWidget���ؼ��Ž�ȥ */
	statusLineNumLabel = new QLabel(tr("row 1,column 1"));
	statusZoomLabel = new QLabel(tr("100%"));
	statusSystemLabel = new QLabel(tr("windows"));
	statusChartsetLabel = new QLabel(tr("UTF-8"));
	/** ���ﲻ�ò��ֹ���ֱ����״̬���ӿؼ� */
	this->statusBar()->addPermanentWidget(statusLineNumLabel);
	this->statusBar()->addPermanentWidget(statusZoomLabel);
	this->statusBar()->addPermanentWidget(statusSystemLabel);
	this->statusBar()->addPermanentWidget(statusChartsetLabel);
}

void NoteLiteMainPanel::getTextEditPos(void)
{
	/** �������������ȡ�ı�����������textEdit�Ĺ�����ڵ�λ�ã�Ȼ����ʾ��״̬���� */
	QTextCursor cursor = ui.textEdit->textCursor();
	QTextLayout *tLayout = cursor.block().layout();
	int colPos = cursor.position() - cursor.block().position();//���textEdit�й�����ڵ�������cursor.position()�ǹ�������������������λ�ã�cursor.block().position()�����ݸ�ʽ�����λ�ã�����ֵ����ó������һ���е���һ��
	int lineNum = tLayout->lineForTextPosition(colPos).lineNumber() + cursor.block().firstLineNumber();//���textEdit�й���������tLayout->lineForTextPosition(colPos).lineNumber()�ǹ�����ڵ��У�cursor.block().firstLineNumber()�Ǹ�ʽ���ı����У�������Ӳ��ǹ�����ı��е���
																									   //qDebug()<<"position:"<<cursor.position()- cursor.block().position();
	statusLineNumLabel->setText(tr("line:") + QString::number(lineNum + 1) + tr(",column:") + QString::number(colPos + 1) + tr(" "));//��Ϊ�����ڲ������Ǵ�0��ʼ�ģ���ʾ����Ҫ���˵�ϰ��Ϊ׼������ҪlineNum+1����1��ʼ
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
	w = new NoteLiteMainPanel;//���������newһ���¶��񣬶�new�����Ķ�����ʱ��Ҫ�ֶ�delete�ģ����������ַ��������´����ǲ��õģ�������ڴ����⡣������ͨ������������~NoteLiteMainPanel��delete
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
				out << ui.textEdit->toPlainText();//��textEdit�е�����ת���ɴ��ı�
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
	/** ����Ӧ��Ҫ��һ�������ж�Ҫ������ļ����Ƿ���ڻ����ظ�֮��� */
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
	/** ������������ڴ��ڱ�������һ���Ǻţ�*���ġ��ڼ��±����ı��༭��ʱ���ڱ�������һ��*������ʾ�ı�δ���� */
	/** ���ı����ݸ��µ�ʱ�򴥷�����ۣ��ڱ��������Ǻţ���Ҫ��ֻ֤����һ�Σ���Ҫ�ڴ�����֮��disconnect�������ı����涯��ʱ��connect���� */
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
		//��������˵���������岻�ɹ���Ӧ����������������
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
	/** ��textEdit�а�����¼����ˣ�����textEdit�������¼��������������������������������Ҫ�Ĵ���Ϳ����ˣ�����Լ���ĳ�������� */
	getTextEditPos();
	return QWidget::eventFilter(target, event);//���ø���Ĺ��˷��������¼�ת�����࣬�����Ų�Ӱ��ԭ���Ĺ���
}

void NoteLiteMainPanel::closeEvent(QCloseEvent * event)
{
	/** close()�������Զ������������ */

	/**
	 * ����������жϱȽϸ��ӣ�����ʵ��ʹ���龰�������ֱȽ�����������
	 * ��һ���������������±����򣬾��ڿհ��ı����������֣�Ȼ�����������ȫɾ����������ı������ǿհ׵�
	 * �ڶ����������������±�����Ȼ��ִ�д��ļ������������һ�������ݵ�txt�ļ���Ȼ������ȫɾ����������ı����ǿհ׵�
	 * ��������Ľ���ı������ǿհ׵ģ���ô������հ׵�������û����˳���ť������ȻҪִ���˳��������˳�ǰ��δ����أ�
	 * ��һ��������ı���һ��ʼ��ʱ���ǿհ׵ģ����Ҳ�ǿհ׵ģ��������ͨ��������Ϊ�ǲ���Ҫ����ģ�����ֱ���˳�����
	 * �ڶ���������ı���һ��ʼ�������ݵģ�����ǿհ׵ģ���ô˵�����ݾ����˸��ģ�ͨ����Ϊ����Ҫ����ģ�����Ҫ��ִ�б�������˳�
	 * ���������ui.textEdit->document()->isModified()&&!(ui.textEdit->document()->isEmpty())ȥ�жϣ������ı����и��¶����ǿյģ����ϵ�һ����������ǵڶ������Ҳ�Ƿ��ϵģ�û������
	 * f
	 */
	if (ui.textEdit->document()->isModified() && !(ui.textEdit->document()->isEmpty()))//�ж��������ı������ݷ����仯���������ݲ�Ϊ�ա�����ǿյľ�û��Ҫ������
	{
		QMessageBox msgBox;
		if (openFileName == "")
		{
			/** û�б��������ôopenFileName�ͻ��ǿյģ���ô�������֣��������ޱ���(unknowtitle) */
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
			//����ֱ���ñ����߼��ϻ���BUG����Ϊ����openFileName���ó���unknowtitle�������Ϊ�ᵯ��ѡ��·���������Ѿ���·�����ļ����ǲ��õ��û����顣����������Ҫ�Ż�
			//����һ�������ǣ�������Ϊ��ʱ�򵯴����û�ѡ�񱣴�·�������û�û��ѡ��ֱ�ӵ�ȡ���������������ִ�д��ڹرգ���ô�ͻ�����bug�ˡ�
			break;
		case  QMessageBox::Discard:
			/** ѡ�񲻱�����ֱ�ӹر�,�����¼����رմ��� */
			event->accept();
			break;
		case QMessageBox::Cancel:
			/** ѡ��ȡ�������κδ���ֱ�ӽ��¼����� */
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

