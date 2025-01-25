


//*************************************************************************
//
// Pool de memória
//
// -Este deve ser mantido-
// 1. A memória não deve ser removida pelos usuários.(Se a memória for removida, o destruidor será rígido)
// 2. O destruidor deve ser chamado (não deve ser declarado como Molloc).
//Se você protege 1 ou 2, o fenômeno de vazamento de memória não ocorre.(Remova toda a memória internamente)
// 3. Não coloque um ponteiro no modelo.(Você deve colocar o próprio objeto para ser alocado)
//
//-É bom mantê-lo-
// 1. O pop é bom para empurrar novamente (porque é alocado sempre que você não tem memória livre)
// 2. É uma boa idéia definir a bandeira de acordo com o número de número aumentado.
// 3. Depois de confirmar o arquivo de log, o número inicial é definido (alocação de memória frequente ou alocação muito generosa)
//
//*************************************************************************


#ifndef CS_MEM_POOL
#define CS_MEM_POOL

// 메모리 추가 바이트량 계산 플래그
enum eMemPool_Reallot
{
	MPR_DOUBLE		=		0x00000001,			// m_TCount *= 2
	MPR_ADD			=		0x00000002,			// Manter fenômenos
	MPR_PLUSONE		=		0x00000003,			// m_TCount += 1
};

//*************************************************************************
//
// 선언
//
//*************************************************************************
template < typename T >
class CsMemPool
{
public:
	CsMemPool();
	CsMemPool( int nTCount, DWORD plag = MPR_DOUBLE );	
	~CsMemPool(){ _ReleaseAll(); }

protected:

	int					m_nTCount;			// Número de buffers
	DWORD				m_dwPlag;			// bandeira
	std::stack< T* >	m_FreeStack;		// Pilha de memória livre
	std::stack< T* >	m_DelStack;			// Pilha para remoção de memória


public:

	bool				Init( int nTCount, DWORD plag = MPR_DOUBLE );
	void				Push( T* pNode );


	T*					Pop();

private:

	void				_ReleaseAll();
	void				_Addallot();
};

//*************************************************************************
//
// Justiça
//
//*************************************************************************

//========================================================================
// Construtor
//========================================================================
template < typename T >
CsMemPool<T>::CsMemPool( int nTCount, DWORD plag /*=MPR_DOUBLE*/ )
{
	m_nTCount	=	nTCount;
	m_dwPlag	=	plag;

#ifdef CSMEM_LOG
	_LogPath();
	_LogWriteDate();
#endif CSMEM_LOG

	_Addallot();
}

//========================================================================
// Construtor
//========================================================================
template < typename T >
CsMemPool<T>::CsMemPool()
{
	m_nTCount	=	10;
	m_dwPlag	=	MPR_DOUBLE;
}

//========================================================================
// remover todos os nós
//========================================================================
template < typename T >
void CsMemPool<T>::_ReleaseAll()
{
	// Remover pilha para array
	T* pNode;
	while( !m_DelStack.empty() )
	{
		pNode = m_DelStack.top();
		m_DelStack.pop();
		delete[] pNode;
	}
}

//========================================================================
// Inicialização ao chamar o construtor sem argumentos
//========================================================================
template < typename T >
bool CsMemPool<T>::Init( int nTCount, DWORD plag = MPR_DOUBLE )
{
	// Retorne se você já tiver o nó
	if( m_nTCount )
		return false;

	m_nTCount	=	nTCount;
	m_dwPlag	=	plag;

#ifdef CSMEM_LOG
	_LogPath();
	_LogWriteDate();
#endif CSMEM_LOG

	_Addallot();
	return true;
}


//========================================================================
// Todos os nós livres consumidos.. alocação adicional
//========================================================================
template < typename T >
void CsMemPool<T>::_Addallot()
{
	// 노드들을 스택에 집어 넣는다
	T* pNode = csnew T[ m_nTCount ];
	for( int i=0; i<m_nTCount; ++i )
		m_FreeStack.push( &pNode[i] );
	// 가장 첫 포인터를 제거 스택에 집어 넣는다	
	m_DelStack.push( pNode );

	switch( m_dwPlag&0x000000ff )
	{
	case MPR_DOUBLE:		m_nTCount *= 2;		return;
	case MPR_ADD:								return;
	case MPR_PLUSONE:		++m_nTCount;		return;
	default:				assert_csm( false, _T( "Valor desconhecido inserido como sinalizador." ) );
	}
}

//========================================================================
// remover o nó livre
//========================================================================
template < typename T >
T* CsMemPool<T>::Pop()
{
	assert_csm( m_nTCount, _T( "não inicializado ( m_TCount == 0 )" ) );
	if( m_FreeStack.empty() )
		_Addallot();

	T* pNode = m_FreeStack.top();
	m_FreeStack.pop();
	return pNode;
}


//========================================================================
// retornar ao nó livre
//========================================================================
template < typename T >
void CsMemPool<T>::Push( T* pNode )
{
	assert_cs( pNode != NULL );
	m_FreeStack.push( pNode );
}



#endif /*CS_MEM_POOL*/