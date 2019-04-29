// TsStream.h: TS�X�g���[�����b�p�[�N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "MediaData.h"


/////////////////////////////////////////////////////////////////////////////
// TS�p�P�b�g���ۉ��N���X
/////////////////////////////////////////////////////////////////////////////

#define TS_PACKETSIZE	(188U)	// TS�p�P�b�g�T�C�Y

class CTsPacket : public CMediaData
{
public:
	CTsPacket();
	CTsPacket(const BYTE *pHexData);
	CTsPacket(const CTsPacket &Operand);
	
	CTsPacket & operator = (const CTsPacket &Operand);

	void ParseHeader(void);
	const bool CheckPacket(BYTE *pContinuityCounter = NULL) const;

	BYTE * GetPayloadData(void) const;
	const BYTE GetPayloadSize(void) const;

	const WORD GetPID(void) const;
	const bool HaveAdaptationField(void) const;
	const bool HavePayload(void) const;
	const bool IsScrambled(void) const;

	struct TAG_TSPACKETHEADER{
		BYTE bySyncByte;					// Sync Byte
		bool bTransportErrorIndicator;		// Transport Error Indicator
		bool bPayloadUnitStartIndicator;	// Payload Unit Start Indicator
		bool TransportPriority;				// Transport Priority
		WORD wPID;							// PID
		BYTE byTransportScramblingCtrl;		// Transport Scrambling Control
		BYTE byAdaptationFieldCtrl;			// Adaptation Field Control
		BYTE byContinuityCounter;			// Continuity Counter
		} m_Header;

	struct TAG_ADAPTFIELDHEADER{
		BYTE byAdaptationFieldLength;		// Adaptation Field Length
		bool bDiscontinuityIndicator;		// Discontinuity Indicator
		bool bRamdomAccessIndicator;		// Random Access Indicator
		bool bEsPriorityIndicator;			// Elementary Stream Priority Indicator
		bool bPcrFlag;						// PCR Flag
		bool bOpcrFlag;						// OPCR Flag
		bool bSplicingPointFlag;			// Splicing Point Flag
		bool bTransportPrivateDataFlag;		// Transport Private Data Flag
		bool bAdaptationFieldExtFlag;		// Adaptation Field Extension Flag
		const BYTE *pOptionData;			// �I�v�V�����t�B�[���h�f�[�^
		BYTE byOptionSize;					// �I�v�V�����t�B�[���h��
		} m_AdaptationField;
};


/////////////////////////////////////////////////////////////////////////////
// PSI�Z�N�V�������ۉ��N���X
/////////////////////////////////////////////////////////////////////////////

class CPsiSection : public CMediaData
{
public:
	CPsiSection();
	CPsiSection(const DWORD dwBuffSize);
	CPsiSection(const CPsiSection &Operand);

	CPsiSection & operator = (const CPsiSection &Operand);
	const bool operator == (const CPsiSection &Operand) const;

	const bool ParseHeader(const bool bIsExtended = true);
	void Reset(void);

	BYTE * GetPayloadData(void) const;
	const WORD GetPayloadSize(void) const;

	const BYTE GetTableID(void) const;
	const bool IsExtendedSection(void) const;
	const bool IsPrivate(void) const;
	const WORD GetSectionLength(void) const;
	const WORD GetTableIdExtension(void) const;
	const BYTE GetVersionNo(void) const;
	const bool IsCurrentNext(void) const;
	const BYTE GetSectionNumber(void) const;
	const BYTE GetLastSectionNumber(void) const;

protected:
	struct TAG_PSIHEADER{
		BYTE byTableID;						// �e�[�u��ID
		bool bSectionSyntaxIndicator;		// �Z�N�V�����V���^�b�N�X�C���W�P�[�^
		bool bPrivateIndicator;				// �v���C�x�[�g�C���W�P�[�^
		WORD wSectionLength;				// �Z�N�V������

		WORD wTableIdExtension;				// �e�[�u��ID�g��
		BYTE byVersionNo;					// �o�[�W�����ԍ�
		bool bCurrentNextIndicator;			// �J�����g�l�N�X�g�C���W�P�[�^
		BYTE bySectionNumber;				// �Z�N�V�����ԍ�
		BYTE byLastSectionNumber;			// ���X�g�Z�N�V�����ԍ�
		} m_Header;
};


/////////////////////////////////////////////////////////////////////////////
// TS PID�}�b�v�ΏۃN���X
/////////////////////////////////////////////////////////////////////////////

class CTsPidMapTarget
{
public:
	virtual const bool StorePacket(const CTsPacket *pPacket) = 0;

	virtual void OnPidMapped(const WORD wPID, const PVOID pParam);
	virtual void OnPidUnmapped(const WORD wPID);
};


/////////////////////////////////////////////////////////////////////////////
// TS PID�}�b�v�Ǘ��N���X
/////////////////////////////////////////////////////////////////////////////

class CTsPidMapManager
{
public:
	typedef void (CALLBACK * PIDMAPHANDLERFUNC)(const WORD wPID, CTsPidMapTarget *pMapTarget, CTsPidMapManager *pMapManager, const PVOID pParam);

	CTsPidMapManager();
	virtual ~CTsPidMapManager();

	const bool StorePacket(const CTsPacket *pPacket);

	const bool MapTarget(const WORD wPID, CTsPidMapTarget *pMapTarget, const PIDMAPHANDLERFUNC pMapCallback = NULL, const PVOID pMapParam = NULL);
	const bool UnmapTarget(const WORD wPID);
	void UnmapAllTarget(void);

	CTsPidMapTarget * GetMapTarget(const WORD wPID) const;

	const WORD GetMapCount(void) const;
	
protected:
	struct TAG_MAPTARGETITEM
	{
		CTsPidMapTarget *pMapTarget;
		PIDMAPHANDLERFUNC pMapCallback;
		PVOID pMapParam;
	} m_PidMap[0x2000];
	
	WORD m_wMapCount;
};


/////////////////////////////////////////////////////////////////////////////
// PSI�Z�N�V�������o�N���X
/////////////////////////////////////////////////////////////////////////////

class CPsiSectionParser
{
public:
	typedef void (CALLBACK * SECTIONRECVFUNC)(const CPsiSection *pSection, const PVOID pParam);	// �Z�N�V�����󂯎��R�[���o�b�N�^

	CPsiSectionParser();
	CPsiSectionParser(const CPsiSectionParser &Operand);

	CPsiSectionParser & operator = (const CPsiSectionParser &Operand);

	void SetRecvCallback(const bool bTargetExt, const SECTIONRECVFUNC pCallback, const PVOID pParam = NULL);
	void StorePacket(const CTsPacket *pPacket);
	void Reset(void);

	const DWORD GetCrcErrorCount(void) const;

private:
	const BYTE StoreHeader(const BYTE *pPayload, const BYTE byRemain);
	const BYTE StorePayload(const BYTE *pPayload, const BYTE byRemain);

	const BYTE StartUnit(const BYTE *pPayload, const BYTE byRemain);
	const BYTE StoreUnit(const BYTE *pPayload, const BYTE byRemain);

	static const DWORD CalcCrc(const BYTE *pData, const WORD wDataSize, DWORD dwCurCrc = 0xFFFFFFFFUL);

	SECTIONRECVFUNC m_pfnSectionRecvFunc;
	PVOID m_pCallbackParam;

	bool m_bTargetExt;

	CPsiSection m_PsiSection;

	bool m_bIsStoring;
	DWORD m_dwStoreCrc;
	WORD m_wStoreSize;
	DWORD m_dwCrcErrorCount;
};


/////////////////////////////////////////////////////////////////////////////
// PCR���ۉ��N���X
/////////////////////////////////////////////////////////////////////////////

class CTsClockRef
{
public:
	CTsClockRef();
	CTsClockRef(const CTsClockRef &Operand);

	CTsClockRef & operator = (const CTsClockRef &Operand);

	const bool StorePacket(const CTsPacket *pPacket, const WORD wPcrPID);
	void Reset(void);

	const LONGLONG GetGlobalPcr(void) const;
	const LONGLONG GetCurrentPcr(void) const;
	const LONGLONG PtsToGlobalPcr(const LONGLONG llPts) const;

protected:
	void InitPcrPll(const LONGLONG llCurPcr);
	void ProcPcrPll(const LONGLONG llCurPcr);
	void SyncPcrPll(const LONGLONG llCurPcr);

	static inline const LONGLONG GetPcrFromHex(const BYTE *pPcrData);

	LONGLONG m_llHrcUnitFreq;
	LONGLONG m_llHrcLastTime;
	
	LONGLONG m_llCurPcrCount;
	double m_lfPllFeedBack;

	LONGLONG m_llGlobalPcrCount;
	LONGLONG m_llBasePcrCount;
};