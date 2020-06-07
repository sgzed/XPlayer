#include "XPlayer2.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QThread>

#include "XVideoWidget.h"
#include "XDemux.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"

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
		//url = "v720p.mp4";
		url = "1080.mp4";
		cout << "demux.Open = " << demux.Open(url);
		cout << "CopyVPara = " << demux.CopyVCodePara() << endl;
		cout << "CopyAPara = " << demux.CopyACodePara() << endl;

		cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVCodePara()) << endl;
		cout << "adecode.Open() = " << adecode.Open(demux.CopyACodePara()) << endl;
		cout << "resample.Open = " << resample.Open(demux.CopyACodePara()) << endl;
		cout << "audioPlay.Open = " << XAudioPlay::GetAduioPlay()->Open(demux.sampleRate, demux.channels) << endl;
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
				adecode.SendPkt(pkt);
				auto frame = adecode.RecvPkt();
				int len = resample.ReSample(frame, data);
				cout << "Resample:" << len << " ";
				while (len > 0)
				{
					if (XAudioPlay::GetAduioPlay()->GetFree() >= len)
					{
						XAudioPlay::GetAduioPlay()->PlayAudio((const char*)data, len);
						break;
					}
					msleep(1);
				}
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
	XResample resample;
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
