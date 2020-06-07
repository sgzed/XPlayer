#include "XAudioPlay.h"
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/qaudiooutput.h>

class QXAudioPlay : public XAudioPlay
{
public:
	bool Open(int sampleRate, int channels)
	{
		QAudioFormat fmt = QAudioFormat();
		fmt.setSampleRate(sampleRate);
		fmt.setSampleSize(16);
		fmt.setChannelCount(channels);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);

		output = new QAudioOutput(fmt);
		io = output->start();
		return true;
	}

	int PlayAudio(const char * data, int len)
	{
		if (!output)	return 0;
		if (output->bytesFree() < len)  //缓冲不足写，需要等待
		{
			return 0;
		}
		int ret = io->write(data, len);
		return ret;
	}

	int GetFree()
	{
		if (!output)	return 0;
		return output->bytesFree();
	}
};

XAudioPlay::XAudioPlay()
{
}

XAudioPlay::~XAudioPlay()
{
}

XAudioPlay * XAudioPlay::GetAduioPlay()
{
	static QXAudioPlay qPlayer;
	return &qPlayer;
}
