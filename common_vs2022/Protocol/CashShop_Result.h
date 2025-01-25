#pragma once

namespace nsCashShopResult
{
	const int	BILLING_RESULT_CROPCASHITEM_SUCCESS				= 100;			// Dinheiro, sucesso bem -sucedido

	const int	BILLING_RESULT_CASH_NOT_ENABLE_SLOT				= 20150;		// Não é um slot disponível.
	const int	BILLING_RESULT_CASH_NOT_SERVER_DATA				= 20150;		// As informações do item de slot solicitadas ao servidor e as informações reais do item são diferentes.Receber informações de renovação

	const int	BILLING_RESULT_CASH_NOT_ENOUGH					= 30310;		// Falta de equilíbrio.
	const int	BILLING_RESULT_CASHITEM_NOT_PURCHASE			= 30311;		// Falha na compra do item

	const int	BILLING_RESULT_CASHITEM_NOT_INVENTORY_LIMITE	= 30322;		// Tempo limitado ao procurar itens no armazém de itens de caixa

	const int Success				= 0;
	const int CashError				= 31010;		//Erros relacionados ao dinheiro (falta, etc.)
	const int ItemError				= 31011;		//Não há item
	const int PartBuyError			= 31017;		//Alguns dos processos de compra foram bem -sucedidos, mas o restante falhou.
	const int BuyError				= 31012;		//O erro ocorre durante o processo de compra

	const int GiftPeerNotBuddy		= 31013;		//O alvo do presente não é um amigo
	const int GiftpeerError			= 31014;		//Não consigo encontrar um alvo de presente
	const int GiftError				= 31015;		//O erro ocorre durante o processo de presente
	const int GiftCashError			= 31016;		//Erros relacionados ao dinheiro (falta, etc.)
}
