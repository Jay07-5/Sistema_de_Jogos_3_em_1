#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

// Configurações de opção lwIP usadas pelo projeto Pico W.
// Este arquivo adapta o stack lwIP para rodar com a biblioteca
// cyw43 e com o servidor web básico implementado em web.c.

#define NO_SYS                          1
#define SYS_LIGHTWEIGHT_PROT           1
#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        (16 * 1024)
#define ETH_PAD_SIZE                    0
#define PBUF_POOL_SIZE                  4

// Usamos a API RAW do lwIP, não sockets nem netconn, para evitar
// conflitos de compatibilidade com o driver do Pico W.
#define LWIP_RAW                        1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_DHCP                       1
#define LWIP_ICMP                       1
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define LWIP_IPV4                       1
#define LWIP_IPV6                       0
#define LWIP_ARP                        1
#define LWIP_SINGLE_NETIF               0

// Tamanho de buffers TCP e janela de envio.
#define TCP_MSS                         (1500 - 20 - 20)
#define TCP_SND_BUF                     (4 * TCP_MSS)
#define TCP_WND                         (4 * TCP_MSS)

// Opções gerais de compatibilidade e tamanho do stack.
#define LWIP_TIMEVAL_PRIVATE            0
#define LWIP_STATS                      0
#define LWIP_PROVIDE_ERRNO              1
#define LWIP_MPU_COMPATIBLE             0
#define IP_REASSEMBLY                   1
#define IP_FRAG                         1
#define ETHARP_SUPPORT_STATIC_ENTRIES   1

#endif /* __LWIPOPTS_H__ */
