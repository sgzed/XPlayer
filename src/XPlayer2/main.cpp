#include "XPlayer2.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QThread>

#include "XVideoWidget.h"
#include "XDemux.h"
#include "XDecode.h"
#include "XAudioThread.h"

using std::cout;
using std::endl;

class TestThread : public QThread
{
public:
	void Init()
	{
		//œ„∏€Œ¿ ”
		char *url = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
		/*cout << "demux.Open = " << demux.Open(url);
		demux.Read();
		demux.Clear();
		demux.Close();*/
		url = "v720p.mp4";
		//url = "1080.mp4";
		cout << "demux.Open = " << demux.Open(url);
		cout << "CopyVPara = " << demux.CopyVCodePara() << endl;
		cout << "CopyAPara = " << demux.CopyACodePara() << endl;

		cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVCodePara()) << endl;
		cout << "adecode.Open() = " << adecode.Open(demux.CopyACodePara()) << endl;
		
		at = new XAudioThread();
		at->Open(demux.CopyACodePara(), demux.sampleRate, demux.channels);
		at->start();
	}

	void run()
	{
		unsigned char *data = new unsigned char[1024 * 1024 * 10];

		//demux.Seek(0.9);
		while (true)
		{
			std::shared_ptr<AVPacket> pkt = demux.Read();
			//cout << "use_count = " << pkt.use_count() << endl;

			if (demux.IsAudioPkt(pkt))
			{
				at->Push(pkt);
			}
			else
			{
				vdecode.SendPkt(pkt);
				auto frame = vdecode.RecvPkt();
				video->Repaint(frame);
				msleep(40);
			}
			if (!pkt)
				break;
		}
	}

	///≤‚ ‘XDemux
	XDemux demux;
	///Ω‚¬Î≤‚ ‘
	XDecode vdecode;
	XDecode adecode;
	XVideoWidget *video;
	XAudioThread* at;
};

int main(int argc, char *argv[])
{
	TestThread tt;
	tt.Init();

    QApplication a(argc, argv);
    XPlayer2 w;
    w.show();

	w.ui.XPlayerWidget->Init(tt.demux.width, tt.demux.height);
	tt.video = w.ui.XPlayerWidget;
	tt.start();

    return a.exec();
}
