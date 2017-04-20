#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>
#include <QImage>
#include <QRgb>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QClipboard>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:

		void on_imagePath_textEdited(const QString &arg1);

		void on_imagePathBrowseButton_clicked();

		void on_convertButton_clicked();

		void on_outputFormatComboBox_currentIndexChanged(const QString &arg1);

		void on_mirrorHorizontallyCheckBox_toggled(bool checked);

		void on_mirrorVerticallyCheckBox_toggled(bool checked);

	private:
		Ui::MainWindow *ui;
		QImage image;
		QImage imagePreview;
		QString currentFormat;
		int totalByteCount;
		void checkPaths();
		void loadImage();
		void writeOutputByte(int *bytesPerRow, int bytesPerRowInit, QString *output, QString byte);
		void updateImageViews();
};

#endif // MAINWINDOW_H
