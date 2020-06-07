#pragma once

class QAudioOutput;
class QIODevice;

class XAudioPlay
{
public:
	XAudioPlay();
	~XAudioPlay();

	static XAudioPlay* GetAduioPlay();

	virtual bool Open(int sampleRate, int channels) = 0;

	virtual int GetFree() = 0;

	virtual int PlayAudio(const char *data,int len) = 0;

protected:
	QAudioOutput* output = nullptr;
	QIODevice* io = nullptr;
};

