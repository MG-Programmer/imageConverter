#include <QCoreApplication>
#include "QCommandLineParser"
#include <QTextStream>
#include "QStringList"
#include "iostream"
#include "QFileInfo"
#include "QImage"
#include "QRgb"

#define BYTES_PER_ROW				10

using namespace std;

void writeOutputByte(int *bytesPerRow, int bytesPerRowInit, int *totalByteCount, QTextStream *output, QString byte, bool isLastPixel);

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName("image to c-code converter");
	QCoreApplication::setApplicationVersion("1.0");

	QImage image;
	enum
	{
		MONO,
		GRAYSCALE_8,
		RGB444,
		RGB565,
		RGB666,
		RGB888
	}imageFormat;
	QFileInfo imageFile;
	bool mirrorHorizontaly;
	bool mirrorVertically;
	int bytesPerRow;
	QString outputFilePath;
	QFile outputFile;

	/////////////////////////////////
	// command line arguments init //
	/////////////////////////////////
	QCommandLineParser parser;
	parser.setApplicationDescription("image to c-code converter");
	QCommandLineOption helpOption	= parser.addHelpOption();
	QCommandLineOption versionOption= parser.addVersionOption();
	QCommandLineOption imageFileOption(
				QStringList() << "i" << "image-file",
				"image filename",
				"image");
	parser.addOption(imageFileOption);

	QCommandLineOption outputFileOption(
				QStringList() << "o" << "output-file",
				"output filename. If no output file is defined, the file is named like the image file",
				"outputFile");
	parser.addOption(outputFileOption);

	QCommandLineOption  formatOption(
				QStringList() << "f" << "image-format",
				"output image format\n" \
				"mono:	black/white	(1bit/pixel)\n" \
				"gray8:	grayscale	(8bit/pixel)\n" \
				"rgb444:	rgb444		(12bit/pixel)\n" \
				"rgb565:	rgb565		(16bit/pixel)\n" \
				"rgb666:	rgb666		(18bit/pixel)\n" \
				"rgb888:	rgb888		(24bit/pixel)\n",
				"format");
	parser.addOption(formatOption);

	QCommandLineOption mirrorHorizontallyOption(
				QStringList() << "mh" << "mirror-horizontally",
				"mirror the image horizontally");
	parser.addOption(mirrorHorizontallyOption);

	QCommandLineOption mirrorVerticallyOption(
				QStringList() << "mv" << "mirror-vertically",
				"mirror the image vertically");
	parser.addOption(mirrorVerticallyOption);

	QCommandLineOption bytesPerRowOption(
				QStringList() << "b" << "bytes-per-row",
				"number of bytes per row in c-code before a new line command\n" \
				"default value: " + QString::number(BYTES_PER_ROW),
				"byteNum");
	bytesPerRowOption.setDefaultValue(QString::number(BYTES_PER_ROW));
	parser.addOption(bytesPerRowOption);

	parser.process(a);


	/////////////////////////
	// check the arguments //
	/////////////////////////

	// help option
	if(parser.isSet(helpOption))
	{
		parser.showHelp();
	}

	// image file
	if(parser.isSet(imageFileOption))
	{
		imageFile.setFile(parser.value(imageFileOption));
		QString suffix				= imageFile.suffix().toLower();
		QStringList fileFormats	;
		fileFormats					<< "bmp"
									<< "gif"
									<< "png"
									<< "jpg"
									<< "jpeg";

		if(imageFile.exists() && fileFormats.contains(suffix))
		{
			if(!image.load(imageFile.absoluteFilePath()))
			{
				cout << "error loading the image file" << endl;
				return a.exit();
			}
		}
		else
		{
			cout << "the defined image file does not exist or the file format is not supported" << endl;
			return a.exit();
		}
	}
	else
	{
		cout << "no image file defined" << endl;
		parser.showHelp();
	}

	// image format
	if(parser.isSet(formatOption))
	{
		QString format				=  parser.value(formatOption).toLower();
		if(format == "mono")
		{
			imageFormat				= MONO;
		}
		else if(format == "gray8")
		{
			imageFormat				= GRAYSCALE_8;
		}
		else if(format == "rgb444")
		{
			imageFormat				= RGB444;
		}
		else if(format == "rgb565")
		{
			imageFormat				= RGB565;
		}
		else if(format == "rgb666")
		{
			imageFormat				= RGB666;
		}
		else if(format == "rgb888")
		{
			imageFormat				= RGB888;
		}
		else
		{
			cout << "unknown image format '" << format.toStdString() << "'" << endl;
			parser.showHelp();
		}
	}
	else
	{
		cout << "no image format defined" << endl;
		parser.showHelp();
	}

	// mirror options
	mirrorHorizontaly				= parser.isSet(mirrorHorizontallyOption);
	mirrorVertically				= parser.isSet(mirrorVerticallyOption);
	image							= image.mirrored(mirrorVertically, mirrorHorizontaly);

	// bytes per row
	if(parser.isSet(bytesPerRowOption))
	{
		bytesPerRow					= parser.value(bytesPerRowOption).toInt();
		if(bytesPerRow <= 0)
		{
			bytesPerRow				= BYTES_PER_ROW;
		}
	}
	else
	{
		bytesPerRow					= BYTES_PER_ROW;
	}

	// output file
	if(parser.isSet(outputFileOption))
	{
		outputFilePath				= parser.value(outputFileOption);
	}
	else
	{
		outputFilePath				= imageFile.absolutePath() + "/" + imageFile.baseName() + ".c";
	}
	outputFile.setFileName(outputFilePath);
	if(!outputFile.open(QIODevice::ReadWrite))
	{
		cout << "cannot create or open output file" << endl;
		return a.exit();
	}
	outputFile.resize(0);
	QTextStream outputStream(&outputFile);


	//////////////////////
	// image processing //
	//////////////////////
	QString name					= imageFile.baseName().trimmed().toLower();
	outputStream					<< "const int " << name << "_width = " << QString::number(image.width()) << ";\n";
	outputStream					<< "const int " << name << "_height = " << QString::number(image.height()) << ";\n";
	outputStream					<< "const char " << name << "[] = {\n\t";

	QRgb rgb;
	uint8_t red, green, blue;
	int pixelIndex;

	uint8_t byte1, byte2, byte3;
	int bytesPerRowInit				= bytesPerRow;
	int totalByteCount				= 0;
	bool isLastPixel;
	for(int i = 0; i < image.height(); i++)
	{
		for(int j = 0; j < image.width(); j++)
		{
			isLastPixel				= (i == (image.height() - 1)) && (j == (image.width() - 1));

			pixelIndex				= i * image.width() + j;
			rgb						= image.pixel(j, i);
			red						= qRed(rgb);
			green					= qGreen(rgb);
			blue					= qBlue(rgb);
			switch(imageFormat)
			{
				case MONO:
					if((pixelIndex % 8 == 0) || isLastPixel)
					{
						writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte1, 16), isLastPixel);
						byte1			= 0;
						if(red > 0)
						{
							byte1			|= 1;
						}
					}
					else
					{
						byte1				<<= 1;
						if(red > 0)
						{
							byte1			|= 1;
						}
					}
					break;

				case GRAYSCALE_8:
					byte1					= red;
					writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte1, 16), isLastPixel);
					break;

				case RGB444:
					if(pixelIndex % 2 == 0)
					{
						byte1			= (red << 4) | (green & 0xf);
						byte2			= (blue << 4);
						if(isLastPixel)
						{
							writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte1, 16), false);
							writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte2, 16), isLastPixel);
						}
					}
					else
					{
						byte2			= byte2 | (red & 0xf);
						byte3			= (green << 4) | (blue & 0xf);
						writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte1, 16), false);
						writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte2, 16), false);
						writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte3, 16), isLastPixel);
					}
					break;

				case RGB565:
					byte1				= (red << 3) | (green & 0xf8);
					byte2				= (green << 5) | (blue & 0x1f);
					writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte1, 16), false);
					writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte2, 16), isLastPixel);
					break;

				case RGB666:
				case RGB888:
					byte1				= red;
					byte2				= green;
					byte3				= blue;
					writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte1, 16), false);
					writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte2, 16), false);
					writeOutputByte(&bytesPerRow, bytesPerRowInit, &totalByteCount, &outputStream, QString::number(byte3, 16), isLastPixel);
					break;
			}
		}
	}
	outputStream						<< "\n};";
	outputStream						<< "// array size " << totalByteCount << " bytes";
	cout << "image converted to " << outputFile.fileName().toStdString() << endl;
	outputFile.close();
}

void writeOutputByte(int *bytesPerRow, int bytesPerRowInit, int *totalByteCount, QTextStream *outputStream, QString byte, bool isLastPixel)
{
	*outputStream					<< "0x" << byte;
	if(!isLastPixel)
	{
		*outputStream				<< ", ";
	}
	(*bytesPerRow)--;
	if(*bytesPerRow <= 0)
	{
		*outputStream				<< "\n\t";
		*bytesPerRow				= bytesPerRowInit;
	}
	(*totalByteCount)++;
}
