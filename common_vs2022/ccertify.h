#pragma once

#include "nScope.h"
#include <iostream>

namespace pCertify
{
	enum
	{
		SvrCertify = nScope::Certify,		// autenticação do servidor
		HeartBeat,							// verificação periódica
		Close,	
		IntegrityHash,						//Pacote de hash de recurso do cliente
		End,
	};
};

namespace cCertify
{
#define MAX_SECURITY_CODE	64

	enum CHECK_RESULT
	{
		CHECK_NOT_REQUEST	= 0,				// Se não houver número de consulta	
		CHECK_SUCCESS		= 1,				// normal

		// 이 이하는 에러
		CHECK_FAIL			= 60000,			// Código de senha incorreto
		CHECK_REQUEST_NO_ER = 60001,			// Número de índice inválido
	};

	class cSecurityCode
	{
	private:
		const	double	m_nCertifyVersion;				// Versão de autenticação
		const	__int64	m_nBuildDate;					// Data de construção
		const	double	m_nRanNo;						// número aleatório
		
		short			m_nRequestIndex;				// índice de consulta
		double			m_szSecurityCode[MAX_SECURITY_CODE];		// Código de segurança
	public:
		cSecurityCode() : m_nCertifyVersion(0.1), m_nBuildDate(131004), m_nRanNo(3.5)
		{
			m_nRequestIndex = -1;			
		}

		~cSecurityCode()
		{

		}
		
//		cSecurityCode & operator=(const cSecurityCode &) { return *this; }

		void CodeGenerator()
		{
			memset( m_szSecurityCode, 0, sizeof(m_szSecurityCode) );

			for(int i = 0; i < MAX_SECURITY_CODE; i++)
			{
				m_szSecurityCode[i]	= m_nBuildDate + m_nRanNo * m_nCertifyVersion * i;				
			}
		}

		short MakeRequestNo()
		{	// Retornar um resultado usando um código aleatório
			m_nRequestIndex = (SHORT)nBase::rand(0,MAX_SECURITY_CODE-1);
			return m_nRequestIndex;
		}

		double GetSecurity(short nIndex)
		{
			if( nIndex >= MAX_SECURITY_CODE )
			{
				return 0;
			}

			return m_szSecurityCode[nIndex];			
		}

		// Verifique o código de resposta recebido
		unsigned short CheckAnswer(double nSecurityCode)
		{
			if( m_nRequestIndex < 0 )
			{				
				return CHECK_NOT_REQUEST;
			}

			if( m_nRequestIndex >= MAX_SECURITY_CODE )
			{
				return CHECK_REQUEST_NO_ER;
			}

			unsigned short nResult = CHECK_FAIL;

			if( m_szSecurityCode[m_nRequestIndex] == nSecurityCode )
			{				
				nResult = CHECK_SUCCESS;
			}

			LOG("%f,%f",m_szSecurityCode[m_nRequestIndex],nSecurityCode);

			m_nRequestIndex = -1;
			return nResult;
		}
	};
};