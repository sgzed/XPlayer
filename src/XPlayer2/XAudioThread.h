#pragma once

#include <QThread>
#include <list>
#include <mutex>

struct AVCodecParameters;
struct AVPacket;
class XDecode;
class XResample;

class XAudioThread :public QThread
{
public:
	XAudioThread();

	virtual ~XAudioThread();

	virtual int Open(std::shared_ptr<AVCodecParameters> para,int sampleRate,int channels);

	void run();

	void Push(std::shared_ptr<AVPacket>);

	int maxList = 100;

protected:
	std::list<std::shared_ptr<AVPacket>> packs;
	std::mutex mutex;

	XDecode* decode = nullptr;
	XResample* resample = nullptr;
};

