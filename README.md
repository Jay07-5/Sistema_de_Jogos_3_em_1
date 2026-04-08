# Sistema de Jogos 3 em 1

## Apresentação do Projeto

O Sistema de Jogos Arcade 2040 é um projeto inovador desenvolvido para a placa Raspberry Pi Pico W, que combina a nostalgia dos jogos retrô clássicos com tecnologias modernas de conectividade e interfaces digitais. Inspirado nos consoles portáteis dos anos 80 e 90, o sistema oferece uma experiência de jogo imersiva em um dispositivo compacto e acessível.

### Funcionalidades Principais:
- **Três Jogos Clássicos**: Implementação fiel de Snake, Pong e Flappy Bird, com mecânicas originais e controles intuitivos.
- **Interface OLED**: Display monocromático de alta resolução para gráficos nítidos e menus responsivos.
- **Controle Dual**: Suporte a controles físicos locais via joystick analógico e botões, além de controle remoto via navegador web.
- **Conectividade Wi-Fi**: Servidor HTTP embarcado permite acesso remoto em tempo real, transformando qualquer dispositivo com navegador em controle do jogo.
- **Efeitos Multimídia**: Áudio PWM para sons e efeitos, LEDs RGB para feedback visual dinâmico.

### Tecnologias Empregadas:
- **Hardware**: Raspberry Pi Pico W com RP2040, display OLED SSD1306, periféricos analógicos e digitais.
- **Software**: Firmware C otimizado usando Pico SDK, pilha de rede lwIP para Wi-Fi, drivers customizados para OLED e áudio.
- **Conectividade**: Protocolo HTTP para interface web, com página HTML responsiva para controle remoto.

### Público-Alvo:
- Entusiastas de jogos retrô buscando experiências modernas.
- Estudantes e profissionais de engenharia embarcada interessados em projetos práticos.
- Desenvolvedores explorando IoT e aplicações conectadas.
- Educadores utilizando o projeto como exemplo de integração hardware-software.

O projeto serve como ponte entre o passado e o futuro dos jogos, demonstrando como conceitos clássicos podem ser revitalizados com tecnologia contemporânea.

## Objetivo

O objetivo principal do Sistema de Jogos Arcade 2040 é criar um dispositivo de entretenimento portátil e acessível que combine jogos clássicos com tecnologias modernas, servindo como plataforma educacional e demonstrativa para integração de sistemas embarcados.

### Objetivos Específicos:
- **Entretenimento Acessível**: Oferecer jogos clássicos em hardware de baixo custo, democratizando o acesso a experiências de jogo retrô.
- **Demonstração Técnica**: Ilustrar a integração harmoniosa de hardware embarcado (GPIO, ADC, I2C, PWM) com software eficiente e conectividade de rede.
- **Inovação em Controles**: Explorar interfaces híbridas, combinando controles físicos tradicionais com controle remoto via web, abrindo possibilidades para aplicações IoT.
- **Educação Prática**: Fornecer um exemplo completo de desenvolvimento embarcado, desde configuração de periféricos até implementação de protocolos de rede.

### Benefícios Almejados:
- **Para Usuários Finais**: Experiência de jogo nostálgica com conveniência moderna, possibilidade de controle remoto para acessibilidade.
- **Para Desenvolvedores**: Base de código bem documentada para aprendizado de Pico SDK, lwIP e desenvolvimento de jogos embarcados.
- **Para a Comunidade**: Inspiração para projetos similares, promovendo inovação em sistemas de baixo custo e conectados.
- **Para Educação**: Material didático para cursos de engenharia, demonstrando conceitos de tempo real, multitarefa e comunicação de rede.

### Metas Técnicas:
- **Performance**: Manter taxa de quadros consistente (>30 FPS) durante jogos, com latência de rede <100ms.
- **Confiabilidade**: Sistema robusto com tratamento de erros para falhas de Wi-Fi e periféricos.
- **Escalabilidade**: Arquitetura modular que permite adição fácil de novos jogos e funcionalidades.
- **Eficiência**: Consumo de energia otimizado para uso prolongado com baterias.

O projeto visa não apenas criar um produto funcional, mas também estabelecer um padrão para desenvolvimento de dispositivos IoT de entretenimento, contribuindo para o avanço da tecnologia embarcada acessível.

## Arquitetura de Hardware

### Componentes Principais:
- **Raspberry Pi Pico W**: Microcontrolador principal com processador RP2040 dual-core ARM Cortex-M0+ a 133MHz, 264KB RAM e módulo Wi-Fi CYW43 para conectividade 802.11b/g/n.
- **Display OLED SSD1306**: Tela monocromática de 128x64 pixels, interface I2C, utilizada para renderização gráfica dos jogos e menus.
- **Joystick Analógico**: Módulo com potenciômetros para eixos X/Y e botão digital, conectado via ADC para controle preciso.
- **Botões**: Dois botões digitais (A e B) para ações secundárias nos jogos.
- **Buzzer Piezoelétrico**: Gerador de som PWM para efeitos sonoros e música de fundo.
- **LED RGB**: Três LEDs individuais para indicações visuais e efeitos de iluminação.

### Conexões:
- OLED: I2C (GP14/SDA, GP15/SCL) com endereço 0x3C.
- Joystick: ADC (GP26/X, GP27/Y), GPIO (GP22/BUTTON).
- Botões: GPIO com pull-up interno (GP20/A, GP21/B).
- Buzzer: PWM (GP10) com frequência variável.
- LED RGB: GPIO (GP11/Red, GP12/Green, GP13/Blue).
- Alimentação: 5V via USB, com reguladores internos para 3.3V.

### Especificações Técnicas:
- Consumo de energia: ~100mA em operação normal.
- Interfaces: USB, I2C, ADC, GPIO, PWM.
- Armazenamento: 2MB flash para firmware e dados.

## Funcionamento (Hardware)

O hardware opera com alimentação de 5V via USB, processando entradas do joystick e botões em tempo real. O display OLED atualiza a 60Hz aproximadamente, enquanto o módulo Wi-Fi mantém conectividade para controle remoto. O sistema consome baixa energia, adequado para uso portátil.

## Arquitetura de Firmware

### Módulos Principais:
- **main.c**: Ponto de entrada (`main()`), inicialização de periféricos e loop principal de execução.
- **game_core.c**: Definições de estados de jogo, funções utilitárias compartilhadas (joystick, botões, entrada de estado).
- **menu.c**: Lógica de navegação do menu principal, renderização e seleção de jogos.
- **snake.c**: Implementação completa do jogo Snake, incluindo lógica de movimento, colisões e pontuação.
- **pong.c**: Jogo Pong com física de bola, raquetes e IA simples para oponente.
- **flappy.c**: Jogo Flappy Bird com geração procedural de obstáculos e física de gravidade.
- **oled.c**: Driver para controle do display OLED, funções de desenho (pixels, linhas, texto).
- **audio.c**: Gerenciamento de áudio PWM, reprodução de tons e efeitos sonoros.
- **effects.c**: Animações e efeitos visuais, controle de LEDs RGB.
- **web.c**: Servidor HTTP lwIP para controle remoto via navegador, processamento de requisições.

### Bibliotecas Utilizadas:
- **Pico SDK 2.0.0**: Framework oficial para RP2040, inclui drivers para GPIO, ADC, PWM, I2C, USB.
- **lwIP**: Pilha de protocolos TCP/IP para conectividade Wi-Fi, configurada para API RAW.
- **SSD1306 OLED Driver**: Biblioteca customizada para controle do display via I2C.
- **CYW43 Wi-Fi Driver**: Driver integrado do Pico SDK para módulo Wi-Fi CYW43.

### Estrutura de Dados:
- `game_context_t`: Estrutura principal contendo estado atual, pontuações, configurações.
- Estados de jogo: `STATE_MENU`, `STATE_SNAKE`, `STATE_PONG`, `STATE_FLAPPY`.
- Buffers para renderização OLED e processamento de rede.

## Funcionamento (Firmware)

O firmware inicializa todos os periféricos e entra em um loop principal que:
1. Processa entradas do usuário (joystick, botões)
2. Atualiza o estado do jogo atual
3. Renderiza gráficos no OLED
4. Reproduz áudio e efeitos
5. Processa requisições HTTP para controle remoto

O sistema suporta múltiplos estados: MENU, SNAKE, PONG, FLAPPY, com transições suaves.

## Fluxograma

```
Inicialização
    ↓
Configuração Hardware (OLED, ADC, GPIO, Wi-Fi)
    ↓
Loop Principal
    ↓
Processamento Entradas + Web Poll
    ↓
Atualização Estado Jogo
    ↓
Renderização OLED + Áudio
    ↓
Verificação Estado (Menu/Jogo)
    ↓
Loop
```

## Uso de Inteligência Artificial

Este projeto não utiliza inteligência artificial avançada ou aprendizado de máquina, mas incorpora elementos de IA simples em um dos jogos para demonstrar conceitos básicos de tomada de decisão automatizada em sistemas embarcados.

### IA no Jogo Pong:
- **Implementação**: A IA do oponente é baseada em lógica determinística. A raquete controlada pela máquina move-se verticalmente para acompanhar a posição Y da bola, com uma pequena margem de erro para simular imperfeição humana.
- **Algoritmo**: 
  - Se a bola está acima da raquete, move para cima.
  - Se a bola está abaixo, move para baixo.
  - Inclui atraso controlado para evitar movimento perfeito e tornar o jogo desafiador.
- **Limitações**: A IA é previsível e não adapta-se ao estilo de jogo do jogador. Não considera velocidade da bola, ângulos de rebote ou padrões de movimento complexos.
- **Potenciais Melhorias**:
  - **Algoritmos de Busca**: Implementar A* ou minimax para antecipar trajetórias da bola.
  - **Aprendizado por Reforço**: Usar Q-learning simples para aprender padrões de jogo do jogador.
  - **Redes Neurais**: Treinar uma rede neural básica (usando bibliotecas como TensorFlow Lite for Microcontrollers) para reconhecer e reagir a estilos de jogo.
  - **Adaptação Dinâmica**: Ajustar dificuldade baseada no desempenho do jogador, tornando a IA mais desafiadora ao longo do tempo.

### Aplicações Futuras de IA:
- **Geração Procedural**: IA para criar níveis dinâmicos em jogos como Snake ou Flappy Bird.
- **Reconhecimento de Padrões**: Detectar gestos do joystick para comandos especiais.
- **Otimização**: Algoritmos genéticos para balancear dificuldade dos jogos.
- **Interação com Usuário**: Análise de comportamento para personalizar experiência de jogo.

A implementação atual serve como ponto de partida educacional para explorar IA em sistemas com recursos limitados, demonstrando que até lógica simples pode criar experiências de jogo envolventes.

## Conclusão

O Sistema de Jogos 3 em 1 representa uma fusão bem-sucedida entre nostalgia dos jogos retrô e tecnologias modernas de conectividade, demonstrando o potencial dos microcontroladores de baixo custo como o Raspberry Pi Pico W para aplicações inovadoras.

### Principais Conquistas:
- **Integração Hardware/Software**: Demonstrou a capacidade de integrar múltiplos periféricos (OLED, joystick, Wi-Fi) em um sistema coeso, com gerenciamento eficiente de recursos limitados.
- **Conectividade em Tempo Real**: Implementação de servidor web embarcado permite controle remoto via navegador, abrindo portas para aplicações IoT e interfaces homem-máquina remotas.
- **Experiência de Usuário**: Combinação de controles físicos locais com controle remoto web, oferecendo flexibilidade de uso.
- **Eficiência Energética**: Sistema otimizado para baixo consumo, adequado para dispositivos portáteis alimentados por bateria.

### Desafios Enfrentados e Soluções:
- **Configuração Wi-Fi**: Adaptação da pilha lwIP para compatibilidade com o driver CYW43, resolvida através de configuração customizada em `lwipopts.h`.
- **Gerenciamento de Estado**: Implementação de máquina de estados para transições suaves entre jogos e menu.
- **Otimização de Performance**: Balanceamento entre atualização de display, processamento de entrada e polling de rede para manter responsividade.

### Impacto Educacional e Tecnológico:
- **Aprendizado**: Serve como exemplo prático para estudantes de engenharia embarcada, cobrindo tópicos como drivers de hardware, protocolos de rede e desenvolvimento de jogos.
- **Inovação**: Demonstra como dispositivos IoT podem ser usados para entretenimento, potencialmente inspirando aplicações em educação, reabilitação e interação social.
- **Acessibilidade**: Jogos clássicos em hardware acessível promovem inclusão digital e preservação de jogos históricos.

### Possibilidades de Expansão:
- **Novos Jogos**: Adição de títulos como Tetris, Pac-Man ou jogos originais com elementos de IA avançada.
- **Multiplayer**: Suporte a jogos multiplayer via Wi-Fi, com comunicação peer-to-peer.
- **Sensores Adicionais**: Integração de acelerômetro, giroscópio ou câmera para controles inovadores.
- **Atualizações OTA**: Capacidade de atualizar firmware remotamente via Wi-Fi.
- **Integração com Nuvem**: Conexão a serviços web para rankings globais, compartilhamento de pontuações ou downloads de jogos.
- **Melhorias na IA**: Implementação de algoritmos de aprendizado para oponentes mais inteligentes e adaptação dinâmica de dificuldade.

Este projeto valida a viabilidade de criar dispositivos de entretenimento modernos e conectados usando tecnologias acessíveis, estabelecendo uma base sólida para futuras inovações em sistemas embarcados e jogos interativos. A combinação de simplicidade técnica com funcionalidade avançada destaca o potencial da plataforma Raspberry Pi Pico para projetos criativos e educacionais.