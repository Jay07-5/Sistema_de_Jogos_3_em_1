// web.c: servidor web minimal para Pico W.
//
// O arquivo usa a API RAW do lwIP para aceitar conexões HTTP,
// gerar uma página HTML simples e permitir que o navegador
// altere o estado do jogo remotamente.

#include "web.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#define WIFI_SSID "JAYSSA MARIA" //nome do roteador Wi-Fi a ser conectado
#define WIFI_PASSWORD "JJM07042005" //senha do roteador Wi-Fi a ser conectado
#define WIFI_AUTH CYW43_AUTH_WPA2_AES_PSK
#define WEB_PORT 80
#define WEB_BUFFER_SIZE 1024

static struct tcp_pcb *http_pcb = NULL;
static bool wifi_connected = false;
static game_context_t *web_ctx = NULL;
static char status_text[40] = "WIFI: DESLIGADO";
static char ip_text[40] = "IP: -";

// Atualiza a string de status do Wi-Fi para exibir no OLED.
static void web_set_status(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(status_text, sizeof(status_text), fmt, args);
    va_end(args);
}

// Atualiza a string do IP atual para exibir no OLED e no HTML.
static void web_set_ip(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(ip_text, sizeof(ip_text), fmt, args);
    va_end(args);
}

const char *web_status_string(void) {
    return status_text;
}

const char *web_ip_string(void) {
    return ip_text;
}

static void web_update_ip(void) {
    cyw43_arch_lwip_begin();
    struct netif *netif = netif_default;
    if (netif != NULL && !ip_addr_isany(&netif->ip_addr)) {
        const char *ip = ipaddr_ntoa(&netif->ip_addr);
        if (ip != NULL) {
            web_set_ip("IP: %s", ip);
            web_set_status("WIFI: OK");
            cyw43_arch_lwip_end();
            return;
        }
    }
    cyw43_arch_lwip_end();
    web_set_ip("IP: -");
    if (wifi_connected) {
        web_set_status("WIFI: CONECTANDO...");
    } else {
        web_set_status("WIFI: ERRO");
    }
}

// Analisa a requisição HTTP e modifica o estado do jogo conforme o parâmetro "game".
// Aqui é onde o navegador controla remotamente o jogo: ao clicar em botões na página,
// o estado do jogo muda imediatamente (ex: de MENU para SNAKE, PONG, etc.).
static void web_process_request(game_context_t *ctx, const char *request) {
    const char *path = strstr(request, "GET /");
    if (!path) {
        return;
    }

    const char *game_param = strstr(path, "game=");
    if (!game_param) {
        return;
    }

    game_param += strlen("game=");
    char value[16] = {0};
    int i = 0;
    while (game_param[i] != '\0' && game_param[i] != '&' && game_param[i] != ' ' && i < (int)sizeof(value) - 1) {
        value[i] = game_param[i];
        i++;
    }

    if (strcmp(value, "snake") == 0) {
        ctx->estado_atual = STATE_SNAKE;
    } else if (strcmp(value, "pong") == 0) {
        ctx->estado_atual = STATE_PONG;
    } else if (strcmp(value, "bird") == 0) {
        ctx->estado_atual = STATE_FLAPPY;
    } else if (strcmp(value, "menu") == 0) {
        ctx->estado_atual = STATE_MENU;
    } else if (strcmp(value, "reset") == 0) {
        ctx->estado_atual = STATE_MENU;
        ctx->menu_option = 0;
    }
}

// Gera a página HTML retornada ao navegador. Exibe o IP e permite trocar de jogo.
static const char *web_generate_page(void) {
    static char page[WEB_BUFFER_SIZE];
    const char *current = "MENU";
    if (web_ctx->estado_atual == STATE_SNAKE) current = "Cobra";
    else if (web_ctx->estado_atual == STATE_PONG) current = "Pong";
    else if (web_ctx->estado_atual == STATE_FLAPPY) current = "Bird";

    snprintf(page, sizeof(page),
        "<html><head><meta charset=\"utf-8\"><title>Sistema de Jogos</title>"
        "<style>body{font-family:Arial,Helvetica,sans-serif;text-align:center;}"
        "button{width:120px;height:40px;margin:6px;font-size:16px;}</style></head>"
        "<body><h1>Sistema de Jogos</h1>"
        "<p>%s</p><p>Jogo atual: %s</p>"
        "<p><a href=\"/?game=menu\"><button>Menu</button></a>"
        "<a href=\"/?game=snake\"><button>Cobra</button></a>"
        "<a href=\"/?game=pong\"><button>Pong</button></a>"
        "<a href=\"/?game=bird\"><button>Bird</button></a></p>"
        "<p><a href=\"/?game=reset\"><button>Reiniciar</button></a></p>"
        "</body></html>", web_ip_string(), current);

    return page;
}

static err_t web_close_connection(struct tcp_pcb *pcb, err_t err) {
    (void)err;
    if (pcb != NULL) {
        tcp_arg(pcb, NULL);
        tcp_sent(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_close(pcb);
    }
    return ERR_OK;
}

static err_t web_sent_callback(void *arg, struct tcp_pcb *pcb, u16_t len) {
    (void)arg;
    (void)pcb;
    (void)len;
    return ERR_OK;
}

// Callback chamada quando chegam dados TCP de um cliente.
// Converte o pedido em texto, processa a ação de troca de jogo e responde com HTML.
static err_t web_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (err != ERR_OK || p == NULL) {
        if (p) pbuf_free(p);
        return web_close_connection(pcb, err);
    }

    char request[WEB_BUFFER_SIZE];
    int request_len = pbuf_copy_partial(p, request, sizeof(request) - 1, 0);
    request[request_len] = '\0';

    if (arg != NULL) {
        web_process_request((game_context_t *)arg, request);
    }

    const char *body = web_generate_page();
    char header[256];
    int body_len = strlen(body);
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        body_len);

    if (tcp_write(pcb, header, header_len, TCP_WRITE_FLAG_COPY) != ERR_OK ||
        tcp_write(pcb, body, body_len, TCP_WRITE_FLAG_COPY) != ERR_OK) {
        pbuf_free(p);
        return web_close_connection(pcb, ERR_MEM);
    }

    tcp_output(pcb);
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);
    web_close_connection(pcb, ERR_OK);
    return ERR_OK;
}

static err_t web_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    (void)err;
    tcp_arg(newpcb, arg);
    tcp_recv(newpcb, web_recv_callback);
    tcp_sent(newpcb, web_sent_callback);
    return ERR_OK;
}

static bool web_create_server(void) {
    cyw43_arch_lwip_begin();
    http_pcb = tcp_new();
    if (http_pcb == NULL) {
        cyw43_arch_lwip_end();
        web_set_status("WIFI: ERRO SERVIDOR");
        return false;
    }

    if (tcp_bind(http_pcb, IP_ADDR_ANY, WEB_PORT) != ERR_OK) {
        tcp_close(http_pcb);
        http_pcb = NULL;
        cyw43_arch_lwip_end();
        web_set_status("WIFI: ERRO BIND");
        return false;
    }

    http_pcb = tcp_listen(http_pcb);
    if (http_pcb == NULL) {
        cyw43_arch_lwip_end();
        web_set_status("WIFI: ERRO LISTEN");
        return false;
    }

    tcp_arg(http_pcb, web_ctx);
    tcp_accept(http_pcb, web_accept_callback);
    cyw43_arch_lwip_end();

    web_set_status("WIFI: CONECTADO");
    return true;
}

// Inicializa o Wi-Fi em modo STA e cria o servidor HTTP.
// O contexto do jogo é armazenado para que comandos do navegador
// possam alterar o estado do jogo remotamente.
void web_init(game_context_t *ctx) {
    web_ctx = ctx;
    wifi_connected = false;

    cyw43_arch_enable_sta_mode();
    web_set_status("WIFI: CONECTANDO...");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, WIFI_AUTH, 15000) != 0) {
        // Tenta manter o status como conectando ao invés de erro imediato
        web_set_status("WIFI: CONECTANDO...");
        return;
    }

    wifi_connected = true;
    web_update_ip();
    if (!web_create_server()) {
        wifi_connected = false;
        web_set_status("WIFI: CONECTANDO...");
    }
}

// Deve ser chamada no loop principal para processar eventos Wi-Fi e
// atualizar o endereço IP exibido no OLED.
void web_poll(game_context_t *ctx) {
    if (!wifi_connected) {
        return;
    }

    (void)ctx;
    cyw43_arch_poll();
    web_update_ip();
}
