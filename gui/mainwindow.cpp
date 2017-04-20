#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QStringList formats;
	formats << "Mono" << "Grayscale 8" << "RGB444" << "RGB565" << "RGB666" << "RGB888";
	this->ui->outputFormatComboBox->insertItems(0, formats);
	this->ui->outputFormatComboBox->setCurrentText("RGB444");
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::checkPaths()
{
	QFileInfo imageFile(this->ui->imagePath->text());

	if(imageFile.exists())
	{
		this->ui->convertButton->setEnabled(true);
		this->loadImage();
	}
	else
	{
		this->ui->convertButton->setEnabled(false);
	}
}

void MainWindow::on_imagePath_textEdited(const QString &arg1)
{
	this->ui->outputTextEditor->setPlainText("");
	this->checkPaths();
}

void MainWindow::on_imagePathBrowseButton_clicked()
{
	QString fileFilter;
	fileFilter						= tr("bmp, jpg, gif, png (*.bmp; *.jpg; *.gif; *.png)");
	QFileDialog *dialog				= new QFileDialog();
	QString startpath;
	if(ui->imagePath->text() == "")
	{
		startpath					= QDir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0)).absolutePath();
	}
	else
	{
		startpath					= ui->imagePath->text();
	}
	QString imageFile				= dialog->getOpenFileName(this, tr("choose bmp file"), startpath, fileFilter);
	this->ui->outputTextEditor->setPlainText("");

	QFileInfo file(imageFile);

	if(file.exists())
	{
		ui->imagePath->setText(imageFile);
		this->checkPaths();
	}
}

void MainWindow::on_convertButton_clicked()
{
	this->ui->convertButton->setEnabled(false);
	this->totalByteCount			= 0;
	this->ui->outputTextEditor->setPlainText("");

	QString command;
	command							+= "ImageConverterCmd.exe";
	command							+= " -i \"" + this->ui->imagePath->text() + "\"";
	command							+= " -f ";
	if(this->currentFormat == "MONO")
	{
		command						+= " mono";
	}
	else if(this->currentFormat == "GRAYSCALE 8")
	{
		command						+= " gray";
	}
	else if(this->currentFormat == "RGB444")
	{
		command						+= " rgb444";
	}
	else if(this->currentFormat == "RGB565")
	{
		command						+= " rgb565";
	}
	else if(this->currentFormat == "RGB666")
	{
		command						+= " rgb666";
	}
	else if(this->currentFormat == "RGB888")
	{
		command						+= " rgb888";
	}

	if(this->ui->mirrorHorizontallyCheckBox->isChecked())
	{
		command						+= " -mh";
	}

	if(this->ui->mirrorVerticallyCheckBox->isChecked())
	{
		command						+= " -mv";
	}

	command							+= " -b " + QString::number(this->ui->bytesPerRowSpinBox->value());

	qDebug() << command;
	QProcess *proc					= new QProcess(this);
	proc->start(command);
	proc->waitForFinished();
	delete proc;

	QFileInfo outputFileInfo		= this->ui->imagePath->text();
	QString outputFilePath			= outputFileInfo.absolutePath() + "/" + outputFileInfo.baseName() + ".c";
	QFile outputFile(outputFilePath);
	QString output;
	if(outputFile.exists())
	{
		if(outputFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream content(&outputFile);
			while(!content.atEnd())
			{
				output				+= content.readLine() + "\n";
			}
			this->ui->outputTextEditor->setPlainText(output);
		}
	}

	QClipboard *clipboad		= QApplication::clipboard();
	clipboad->setText(output);

	QString status				= output.mid(output.indexOf("// ") + 3);
	this->ui->statusLabel->setText(status);
	this->ui->convertButton->setEnabled(true);

	QMessageBox::information(this, "conversion finished", "The conversion has finished and the converted code has been copied to your clipboard");
}

void MainWindow::loadImage()
{
	this->image.load(this->ui->imagePath->text());

	this->updateImageViews();

	QString details;
	details							= "Width:  " + QString::number(this->image.width()) + " pixels\n";
	details							+= "Height: " + QString::number(this->image.height()) + " pixels\n";
	this->ui->imageDetailsLabel->setText(details);
}

void MainWindow::updateImageViews()
{
	if(this->image.isNull())
	{
		return;
	}

	if(this->image.width() > this->image.height())
	{
		this->ui->graphicsView->setFixedWidth(200);
		this->ui->graphicsView->setFixedHeight(200 * this->image.height() / this->image.width());
		this->ui->previewImage->setFixedWidth(200);
		this->ui->previewImage->setFixedHeight(200 * this->image.height() / this->image.width());
	}
	else
	{
		this->ui->graphicsView->setFixedHeight(128);
		this->ui->graphicsView->setFixedWidth(128 * this->image.width() / this->image.height());
		this->ui->previewImage->setFixedHeight(128);
		this->ui->previewImage->setFixedWidth(128 * this->image.width() / this->image.height());
	}

	this->currentFormat				= this->ui->outputFormatComboBox->currentText().toUpper();

	if(this->currentFormat == "MONO")
	{
		this->imagePreview			= this->image.convertToFormat(QImage::Format_Mono);
	}
	else if(this->currentFormat == "GRAYSCALE 8")
	{
		this->imagePreview			= this->image.convertToFormat(QImage::Format_Grayscale8);
	}
	else if(this->currentFormat == "RGB444")
	{
		this->imagePreview			= this->image.convertToFormat(QImage::Format_RGB444);
	}
	else if(this->currentFormat == "RGB565")
	{
		this->imagePreview			= this->image.convertToFormat(QImage::Format_RGB666);
	}
	else if(this->currentFormat == "RGB666")
	{
		this->imagePreview			= this->image.convertToFormat(QImage::Format_RGB666);
	}
	else if(this->currentFormat == "RGB888")
	{
		this->imagePreview			= this->image.convertToFormat(QImage::Format_RGB888);
	}

	QGraphicsScene *scene			= new QGraphicsScene(this->ui->graphicsView);
	scene->setSceneRect(this->image.rect());
	scene->addPixmap(QPixmap::fromImage(this->image));

	this->ui->graphicsView->setScene(scene);
	this->ui->graphicsView->fitInView(this->image.rect());

	this->imagePreview				= this->imagePreview.mirrored(this->ui->mirrorVerticallyCheckBox->isChecked(), this->ui->mirrorHorizontallyCheckBox->isChecked());

	QGraphicsScene *scenePreview	= new QGraphicsScene(this->ui->previewImage);
	scenePreview->setSceneRect(this->imagePreview.rect());
	scenePreview->addPixmap(QPixmap::fromImage(this->imagePreview));

	this->ui->previewImage->setScene(scenePreview);
	this->ui->previewImage->fitInView(this->imagePreview.rect());
}

void MainWindow::on_outputFormatComboBox_currentIndexChanged(const QString &arg1)
{
	this->updateImageViews();
}


void MainWindow::on_mirrorHorizontallyCheckBox_toggled(bool checked)
{
	this->updateImageViews();
}

void MainWindow::on_mirrorVerticallyCheckBox_toggled(bool checked)
{
	this->updateImageViews();
}
