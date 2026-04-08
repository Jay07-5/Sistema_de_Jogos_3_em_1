#pragma once

#include "game.h"

// Inicializa a conexão Wi-Fi e o servidor web.
// Recebe o contexto do jogo para permitir controle remoto.
void web_init(game_context_t *ctx);

// Deve ser chamada regularmente no loop principal para processar
// requisições HTTP e atualizar o estado de rede.
void web_poll(game_context_t *ctx);

// Retorna a string de status atual do Wi-Fi para exibir no OLED.
const char *web_status_string(void);

// Retorna o endereço IP atual obtido por DHCP para exibir no OLED.
const char *web_ip_string(void);
