#include "XAudioThread.h"
#include "XDecode.h"
#include "XResample.h"
#include "XAudioPlay.h"

#include <iostream>
using std::cout;
using std::endl;

XAudioThread::XAudioThread()
{
	if (!decode)
		decode = new XDecode();
	if (!resample)
		resample = new XResample();
}

XAudioThread::~XAudioThread()
{
	wait();
}

int XAudioThread::Open(std::shared_ptr<AVCodecParameters> para,int sampleRate, int channels)
{
	if (!decode || !resample)	return false;
	
	if (decode)
		decode->Open(para);
	
	if (resample)
		resample->Open(para);

	XAudioPlay::GetAduioPlay()->Open(sampleRate, channels);
}

void XAudioThread::run()
{

	unsigned char *data = new unsigned char[1024 * 1024 * 10];
	std::shared_ptr<AVPacket> pkt;

	while (1)
	{
		//std::lock_guard<std::mutex> lck(mutex);
		mutex.lock();
		{
			if (packs.size() < 1)
			{
				mutex.unlock();
				msleep(1);
				continue;
			}
			pkt = packs.front();
			packs.pop_front();
		}
		mutex.unlock();

		decode->SendPkt(pkt);
		auto frame = decode->RecvPkt();
		int len = resample->ReSample(frame, data);
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
	
	delete []data;
}

void XAudioThread::Push(std::shared_ptr<AVPacket> pkt)
{
	if (!pkt)	return;

	while (1)
	{
		mutex.lock();
		if (packs.size() < maxList)
		{
			packs.push_back(pkt);
			mutex.unlock();
			break;
		}
		mutex.unlock();
		msleep(1);
	}
}
