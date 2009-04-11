#ifndef TANO_EPG_H_
#define TANO_EPG_H_

#include <QHttp>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QTextCodec>
#include <QTextEdit>

class Epg : public QHttp {
Q_OBJECT
public:
	Epg();
	virtual ~Epg();

	void getEpg(QString epgP);

public slots:
	void refresh();
	void reload();
	void stop();

signals:
	void epgDone(QStringList);
	void epgDoneFull(QStringList);

private slots:
	void epg();
	void epgPrint();
	void epgNow();
	void epgInit();
	void epgInitDone();

private:
	bool init;

	QString epgValue;
	QString epgFull;
	QString epgChannel;
	QString epgFlag;
	QStringList epgList;
	QStringList epgListNow;

	QTimer *timer;
	QTextCodec *codec;
	QTextEdit *edit;
};

#endif /* EPG_H_ */
