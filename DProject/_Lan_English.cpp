


#include "stdafx.h"
#include "_Lan_English.h"


TCHAR*	__LanGetString_Eng( TCHAR* str )
{
	size_t nRes = CsFPS::GetHashCode( str );
	switch( nRes )
	{
	case 3574150398:
		/* rato não encontrado. */
		return			L"Nao foi possivel detectar o mouse.";

	case 170863830:
		/* Teclado não encontrado. */
		return			L"Nao foi possivel detectar o teclado.";

	case 3102853748:
		/* Qualidade de cor do monitor não suportada. e Formato do Quadro = %d */
		return			L"A cor selecionada não é suportada. Formato do quadro = %d";

	case 2712967072:
		/* Sem aceleração 3D de hardware. Verifique o driver gráfico ou a aceleração Direct X ou D3D. */
		return			L"A aceleração 3D de hardware nao esta disponível. Por favor, verifique seu driver gráfico ou Direct X ou D3D Acceleration.";

	case 2170343467:
		/* A versão do Direct X é inferior a 9.0. Faça o upload da versão do Direct X. */
		return			L"Sua versao do Direct X e inferior a 9.0. Atualize o Direct X.";

	case 2230314086:
		/* Devido às baixas especificações da placa gráfica atual, o terreno ou os personagens podem aparecer anormais na tela. */
		return			L"Sua placa grafica atual pode nao exibir o terreno ou os personagens corretamente porque nao atende aos requisitos minimos. ";

	case 2443360582:
		/* Usarei o shader padrão porque as especificações da placa gráfica são baixas.\nAs configurações do shader podem ser ajustadas nas opções gráficas. */
		return			L"Devido aos baixos requisitos da placa grafica, o shader basico foi aplicado. \n Voce pode definir a configuracao Shader em Graphic Option.";

	case 192517641:
		/* a tabela de arquivos é inválida */
		return			L"Erro na tabela de arquivos";

	case 1242789363:
		/* O Digimon Masters já está em execução.\nVocê pode ser penalizado por usar programas ilegais. */
		return			L"O Digimon Masters já esta em execucao.\n Voce pode ter problemas se estiver usando uma copia ilegal.";

	case 2044459055:
		/*A versão Direct X9.0 é inferior. Gostaria de instalar o Direct X mais recente?*/
		return L"Sua versao do Direct X e baixa. Você instalaria a versao mais recente do Direct X agora?";

	case 1973959437:
		/* Execute-o a partir do lançador!! */
		return			L"Execute DMLauncher.exe.";

		// String relacionada ao GameGuard
		// Isso não deve ser colocado na tabela UI Text.
		// Razão: Isso ocorre porque é antes de carregar a tabela ao enviar esta string.
	case 3947362763:	
		/*O GameGuard está em execução. Tente novamente depois de um tempo ou reinicie.*/
		return			L"O Game Guard foi ativado. Reinicie um pouco mais tarde ou após reiniciar o computador.";
	case 1378959422:	 
		/*O jogo foi executado duas vezes ou o GameGuard já está em execução. Por favor, tente novamente depois de fechar o jogo..*/
		return			L"O jogo iniciado esta duplicado ou o Game Guard foi ativado. Reinicie após encerrar o jogo.";
	case 3489414536:	 
		/*Erro de inicialização do Gameguard. Tente executá-lo novamente após reiniciar ou fechar outros programas que possam estar em conflito..*/
		return			L"Erro de inicializacao do Game Guard. Reinicie apos reiniciar o computador ou apos encerrar algum programa que pode causar falha.";
	case 1409890449:	
		/*O arquivo GameGuard está faltando ou foi adulterado. Instale o arquivo de configuração do GameGuard.*/
		return			L"Nao há arquivos do Game Guard ou eles sao flasifados. Instale o arquivo de configuracao do Game Guard.";
	case 2653703415:	
		/*Alguns arquivos de sistema no Windows estão corrompidos. Reinstale o Internet Explorer (IE)\t.*/		
		return			L"Algum arquivo de sistema do Windows está danificado. Reinstale o Internet Explorer(IE).";

	case 1237949768:	 
		/*A execução do GameGuard falhou. Reinstale o arquivo de configuração do GameGuard.*/
		return			L"Falha ao iniciar o Game Guard. Reinstale o arquivo de configuracao do Game Guard.";
	case 2318342878:	
		/*Um programa ilegal foi encontrado. Feche os programas desnecessários e tente novamente.*/
		return			L"Programa ilegal detectado. Reinicie após encerrar o programa desnecessario.";
	case 1477149656:	
		/*Você cancelou a atualização do GameGuard. Se ainda não conseguir se conectar, tente ajustar suas configurações de Internet e firewall pessoal.*/
		return			L"A atualizacao do Game Guard foi cancelada. Se nao puder ser conectado continuamente, verifique a condicao de sua rede e ajuste as configuracoes do firewall privado.";
	case 2429585766:	
		/*Falha ao conectar ao servidor de atualização do GameGuard. Reconecte depois de um tempo ou verifique o status da rede.*/
		return			L"Falha ao conectar com o servidor de atualizacao do Game Guard. Tente novamente um pouco mais tarde ou verifique a condicao da sua rede.";
	case 1426417480:	
		/*Erro de inicialização do Gameguard ou versão antiga do arquivo GameGuard. Reinstale o arquivo de configuração do GameGuard e execute o jogo.*/
		return			L"Erro de inicializacao do Game Guard ou a versao do Game Guard e antiga. Reinstale o arquivo de configuração do Game Guard e reinicie o jogo.";
	case 1544598421:	
		/*ini está faltando ou foi adulterado. Isso pode ser resolvido instalando o arquivo de configuração do GameGuard.*/
		return			L"Não há arquivo ini ou está flasifado. Instale o arquivo de configuração do Game Guard para resolver o problema.";
	case 274170548:		
		/*erro de inicialização npgmup.des. Depois de excluir a pasta GameGuard, tente executar o jogo novamente.*/
		return			L"erro de inicialização npgmup.des. Reinicie o jogo após excluir a pasta Game Guard.";
	case 890072434:		
		/*Falha ao conectar ao servidor de atualização do GameGuard. Tente novamente mais tarde ou ajuste suas configurações de firewall pessoal, se tiver um.*/
		return			L"Falha ao conectar com o servidor de atualizacao do Game Guard. Tente novamente um pouco mais tarde ou ajuste as configurações do firewall privado.";
	case 1913749563:	
		/*A atualização do GameGuard não pôde ser concluída. Tente novamente depois de pausar seu antivírus ou ajuste as configurações se você usar um programa de gerenciamento de PC..*/
		return			L"A atualizacao do Game Guard não foi concluída. Tente novamente apos desativar temporariamente o programa de vacinas.";
	case 3404157133:	
		/*Falha ao carregar o módulo de varredura da ferramenta de vírus e hackers. Pode ser devido a memória insuficiente ou infecção por um vírus.*/
		return			L"Falha ao carregar o modulo de inspeção da ferramenta de vírus e hackers. Verifique se a memória tem capacidade suficiente e se ha um virus.";
	case 2877035747:	
		/*Foi detectado um programa que entra em conflito com o GameGuard.*/
		return			L"Foi detectado um problema de travamento do Game Guard.";
	case 395939174:		
		/*Ocorreu um erro ao executar o GameGuard. Por favor, envie arquivos *.erl na pasta Game Guard na pasta do jogo para Digimon M@inca.co.kr por e-mail.*/
		return			L"Ocorreu um erro ao ativar o Game Guard. Envie por e-mail os arquivos incluídos que foram inclinados com *.erl na pasta Game Guard da pasta Game";
	case 2935271129:	
		/*Erro de inicialização do Gameguard: %lu*/
		return			L"Erro de inicializacao do Game Guard : %lu";
	case 1944341339:	
		/*Um hack de velocidade foi detectado.*/
		return			L"O hack de velocidade foi detectado.";
	case 1214876551:	
		/*Um hack de jogo foi encontrado.*/
		return			L"O hack do jogo foi detectado.";
	case 295769150:	
		/*O jogo ou guarda do jogo foi adulterado.*/
		return			L"O Game ou Gamer Guard e falsificado.";
	case 2182461964:	
		/*O GameGuard não está em execução.*/
		return			L"O Game Guard nao esta em execucao.";

		////////////Strings relacionadas a game guard//////////////////////////////////////////////////////////////
		////////////Strings relacionadas a XIGNCODE3////////////////////////////////////////////////////////////// 
	case 2717075904:
		/*O jogo está encerrando devido ao desempenho anormal do jogo.\n\t\t CODE = %08X*/
		return L"O encerramento do jogo como atividade anormal do jogo foi detectado.\n\t\t CODE = %08X";
		////////////Strings relacionadas a XIGNCODE3//////////////////////////////////////////////////////////////
	}
// 	assert_cs( false );
//#ifndef _GIVE
	CsMessageBoxA( MB_OK, "%s\nHashCode : %lu\n\n", str, nRes );
//#endif

	return L"Translation needed";
}
