#pragma once
#include <string>

namespace SAPay
{

#define SAVE_REQ_RESPS_AND_SET_RET_NETWORK_CODE(__req__, __resps__, __ret__, __code__)\
m_strLastReq = __req__;\
m_strLastResps = __resps__;\
m_iLastNetWorkCode = __code__;\
iRet = __ret__

#define SAVE_REQ_RESPS_AND_SET_RET(__req__, __resps__, __ret__)\
m_strLastReq = __req__;\
m_strLastResps = __resps__;\
iRet = __ret__

#define SAVE_REQ_RESPS(__req__, __resps__)\
m_strLastReq = __req__;\
m_strLastResps = __resps__;

class CPayBase
{
public:
	CPayBase() { clearErrorInfo(); }
	virtual ~CPayBase() {}

	//clear all error info
	virtual void clearErrorInfo() 
	{
		m_iLastNetWorkCode = 0;
		m_strLastReq.clear();
		m_strLastResps.clear();
		m_strLastErrInfo.clear();
	}

	//get last error info
	virtual int getLastNetWorkCode() { return m_iLastNetWorkCode; }
	virtual const std::string& getLastReq() { return m_strLastReq; }
	virtual const std::string& getLastResps() { return m_strLastResps; }
	virtual const std::string& getLastErrInfo() { return m_strLastErrInfo; }

protected:
	int m_iLastNetWorkCode;
	std::string m_strLastReq;
	std::string m_strLastResps;
	std::string m_strLastErrInfo;
};

}