/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#pragma once

#include "libEasyPlayerAPI.h"
#include "EasyRTSPClient\EasyRTSPClientAPI.h"
#include "FFDecoder\FFDecoderAPI.h"
#include "D3DRender\D3DRenderAPI.h"
#include "SoundPlayer.h"
#include "ssqueue.h"
#pragma comment(lib, "EasyRTSPClient/libEasyRTSPClient.lib")
#pragma comment(lib, "FFDecoder/FFDecoder.lib")
#pragma comment(lib, "D3DRender/D3DRender.lib")

#include "IntelHardCodec/IntelHardCodec_Interface.h"
#pragma comment(lib, "IntelHardCodec/IntelHardCodec.lib")

#include "libEasyAACEncoder/EasyAACEncoderAPI.h"
#pragma comment(lib, "libEasyAACEncoder/libEasyAACEncoder.lib")

// ����MP4box��SaveJPG���֧�� [9/20/2016 dingshuai]
// JPGSave
#include "SaveJPGDll/SaveJpg_Interface.h"
#pragma comment(lib, "SaveJPGDll/SaveJpgDll.lib")
//gpac write mp4 file
#include "EasyMP4Writer/EasyMP4Writer.h"

extern "C"
{
#include "mp4creator\libmp4creator.h"
//#pragma comment(lib, "libMp4Creator.lib")
}

#define		MAX_CHANNEL_NUM		64		//���Խ�����ʾ�����ͨ����
#define		MAX_DECODER_NUM		5		//һ�������߳���������������
#define		MAX_YUV_FRAME_NUM	3		//���������YUV֡��
#define		MAX_CACHE_FRAME		30		//���֡����,������ֵ��ֻ����I֡
#define		MAX_AVQUEUE_SIZE	(1024*1024)	//���д�С
//#define		MAX_AVQUEUE_SIZE	(1920*1080*2)	//���д�С

typedef struct __CODEC_T
{
	//Video Codec
	unsigned int	vidCodec;
	int				width;
	int				height;
	int				fps;
	float			bitrate;

	//Audio Codec
	unsigned int	audCodec;
	int				samplerate;
	int				channels;
}CODEC_T;
typedef struct __DECODER_OBJ
{
	bool bHardDecode;
	CODEC_T			codec;
	FFD_HANDLE		ffDecoder;
	LPIntelHardDecoder pIntelDecoder;
	int				yuv_size;
}DECODER_OBJ;


typedef struct __THREAD_OBJ
{
	int			flag;
	HANDLE		hThread;
}THREAD_OBJ;

typedef struct _YUV_FRAME_INFO			//YUV��Ϣ
{
	MEDIA_FRAME_INFO	frameinfo;
	char	*pYuvBuf;
	int		Yuvsize;
}YUV_FRAME_INFO;

typedef struct __PLAY_THREAD_OBJ
{
	THREAD_OBJ		decodeThread;		//�����߳�
	THREAD_OBJ		displayThread;		//��ʾ�߳�

	Easy_RTSP_Handle		nvsHandle;
	HWND			hWnd;				//��ʾ��Ƶ�Ĵ��ھ��
	int				channelId;			//ͨ����
	int				showStatisticalInfo;//��ʾͳ����Ϣ

	int				frameCache;		//֡����(���ڵ���������),���ϲ�Ӧ������
	int				initQueue;		//��ʼ�����б�ʶ
	SS_QUEUE_OBJ_T	*pAVQueue;		//����rtsp��֡����
	int				frameQueue;		//�����е�֡��
	int				findKeyframe;	//�Ƿ���Ҫ���ҹؼ�֡��ʶ
	int				decodeYuvIdx;

	DWORD			dwLosspacketTime;	//����ʱ��
	DWORD			dwDisconnectTime;	//����ʱ��

	DECODER_OBJ		decoderObj[MAX_DECODER_NUM];
	D3D_HANDLE		d3dHandle;		//��ʾ���
	D3D_SUPPORT_FORMAT	renderFormat;	//��ʾ��ʽ
	int				ShownToScale;		//��������ʾ
	int				decodeKeyFrameOnly;	//��������ʾ�ؼ�֡

	unsigned int	rtpTimestamp;
	LARGE_INTEGER	cpuFreq;		//cpuƵ��
	_LARGE_INTEGER	lastRenderTime;	//�����ʾʱ��

	int				yuvFrameNo;		//��ǰ��ʾ��yuv֡��
	YUV_FRAME_INFO	yuvFrame[MAX_YUV_FRAME_NUM];
	CRITICAL_SECTION	crit;
	bool			resetD3d;		//�Ƿ���Ҫ�ؽ�d3dRender
	RECT			rcSrcRender;
	D3D9_LINE		d3d9Line;
	
	char			url[MAX_PATH];//���ŵ�URL·��
	char			manuRecordingPath[MAX_PATH];//¼����·��
	char			strScreenCapturePath[MAX_PATH];//ץͼ���·��
	//char			manuRecordingFile[MAX_PATH];
	int				manuRecording;	//=1 ����¼��
	int				manuScreenshot;//=1 ����ץͼ
	//MP4CreatorдMP4
	MP4C_Handler	mp4cHandle;
	//�洢JPG��
	LPSaveJpg	   m_pSaveImage;
	//MP4Box Writer
	EasyMP4Writer* m_pMP4Writer;
	int				vidFrameNum;
	MediaSourceCallBack pCallback;
	void			*pUserPtr;

}PLAY_THREAD_OBJ;


//��Ƶ�����߳�
typedef struct __AUDIO_PLAY_THREAD_OBJ
{
	int				channelId;		//��ǰ����ͨ����

	unsigned int	samplerate;	//������
	unsigned int	audiochannels;	//����
	unsigned int	bitpersample;

	//CWaveOut		*pWaveOut;
	CSoundPlayer	*pSoundPlayer;
}AUDIO_PLAY_THREAD_OBJ;

//ץͼ�߳̽ṹ��
typedef struct tagPhotoShotThreadInfo
{
	unsigned char* pYuvBuf;
	int width;
	int height;
	char strPath[512];
}PhotoShotThreadInfo;

class CChannelManager
{
public:
	CChannelManager(void);
	virtual ~CChannelManager(void);

	int		Initial();

	//OpenStream ����һ�����õ�ͨ��ID
	int		OpenStream(const char *url, HWND hWnd, RENDER_FORMAT renderFormat, int _rtpovertcp, const char *username, const char *password, MediaSourceCallBack callback=NULL, void *userPtr=NULL, bool bHardDecode=true);
	void	CloseStream(int channelId);
	int		ShowStatisticalInfo(int channelId, int _show);
	int		SetFrameCache(int channelId, int _cache);
	int		SetShownToScale(int channelId, int ShownToScale);
	int		SetDecodeType(int channelId, int _decodeKeyframeOnly);
	int		SetRenderRect(int channelId, LPRECT lpSrcRect);
	int		DrawLine(int channelId, LPRECT lpRect);
	int		SetDragStartPoint(int channelId, POINT pt);
	int		SetDragEndPoint(int channelId, POINT pt);
	int		ResetDragPoint(int channelId);

	//ͬһʱ��ֻ֧��һ·��������
	int		PlaySound(int channelId);
	int		StopSound();

	int		StartManuRecording(int channelId);
	int		StopManuRecording(int channelId);

	int		SetManuRecordPath(int channelId, const char* recordPath);
	int		SetManuPicShotPath(int channelId, const char* shotPath);

	int		StartManuPicShot(int channelId);//pThread->manuScreenshot
	int		StopManuPicShot(int channelId);

	static LPTHREAD_START_ROUTINE __stdcall _lpDecodeThread( LPVOID _pParam );
	static LPTHREAD_START_ROUTINE __stdcall _lpDisplayThread( LPVOID _pParam );
	static LPTHREAD_START_ROUTINE __stdcall _lpPhotoShotThread( LPVOID _pParam );

	int		ProcessData(int _chid, int mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo);

protected:
	bool				GetD3DSupportFormat();			//��ȡD3D֧�ֵĸ�ʽ
	void	CreatePlayThread(PLAY_THREAD_OBJ	*_pPlayThread);
	void	ClosePlayThread(PLAY_THREAD_OBJ		*_pPlayThread);

	int		SetAudioParams(unsigned int _channel, unsigned int _samplerate, unsigned int _bitpersample);
	void	ClearAllSoundData();
	void	Release();	
	
private:
	PLAY_THREAD_OBJ			*pRealtimePlayThread;		//ʵʱ�����߳�
	AUDIO_PLAY_THREAD_OBJ	*pAudioPlayThread;			//��Ƶ�����߳�
	CRITICAL_SECTION		crit;
	D3D_ADAPTER_T		d3dAdapter;
};
extern CChannelManager	*pChannelManager;