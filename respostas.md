# Respostas — Atividade Prática: Sistema Embarcado de Detecção de Intrusos

## Parte III — Sistema inteligente

**O que deverá acontecer se o sistema estiver DESARMADO e houver movimento?**

Nada de alarme: o LED verde continua indicando "desarmado", nenhum buzzer/notificação é acionado. O movimento pode aparecer no Monitor Serial, mas não dispara alerta nem envia Telegram.

**O que deverá acontecer se o sensor detectar movimento várias vezes seguidas?**

Cada movimento conta como uma ocorrência e aciona o alerta local (LED vermelho + buzzer). Porém, as notificações no Telegram respeitam um intervalo mínimo de 10 segundos (cooldown), para evitar o envio excessivo de mensagens.

**O sistema deverá enviar uma mensagem para cada movimento? Justifique.**

Não. Enviar uma mensagem para cada movimento pode sobrecarregar o Telegram e o usuário. Por isso o sistema envia no máximo uma notificação a cada 10 segundos; os demais movimentos continuam sendo contados no contador de ocorrências, apenas sem reenviar a mensagem.

## Parte IV — Controle do sistema

**Opção escolhida:** ☒ A – Alternância   ☐ B – Temporização

Cada pressionamento do botão alterna entre DESARMADO → ARMADO → DESARMADO, com um beep de confirmação e envio de mensagem de estado no Telegram.

## Parte V — Indicadores

- LED verde: sistema DESARMADO
- LED vermelho: sistema ARMADO / ALERTA
- Buzzer: toca em padrão BEEP-BEEP-BEEP quando ocorre invasão

## Parte X — Controle de notificações

Implementado um **cooldown de 10 segundos**: após o envio de uma notificação, o sistema só envia outra depois de 10s. Ocorrências continuam sendo registradas/contadas mesmo com a notificação suprimida.

## Reflexão final

**1. O que torna esse projeto um sistema IoT?**

Ele conecta um dispositivo físico (ESP32 com sensores e atuadores) à internet (Wi-Fi) e a um serviço online (Telegram), permitindo monitoramento e ação remota — ou seja, objeto físico + conectividade + troca de dados.

**2. Qual é a função do ESP32 nesse sistema?**

É o controlador central: lê o PIR, toma a decisão (armado/desarmado + detecção), aciona LED/buzzer, conecta ao Wi-Fi e envia a notificação ao Telegram.

**3. Qual é a diferença entre detectar um movimento e identificar uma possível intrusão?**

Detectar movimento é apenas perceber que algo passou no sensor. Identificar uma possível intrusão é interpretar esse movimento **no contexto certo**: sistema armado, horário, zona monitorada, etc. O mesmo movimento pode ser irrelevante (sistema desarmado) ou um alerta (sistema armado).

**4. Por que o sistema precisa de uma lógica de decisão?**

Para evitar falsos alarmes e excesso de notificações. Sem lógica, qualquer movimento (até de um animal ou uma cortina) dispararia sirene e Telegram. A lógica (armado/desarmado + cooldown + contagem de ocorrências) torna o sistema útil e confiável.

**5. Quais problemas poderiam ocorrer em um sistema real utilizando apenas um sensor PIR?**

Falsos positivos (animais, calor, luz solar, ar-condicionado), falsos negativos (invasor parado ou coberto), alcance/ângulo limitados, sem identificação da pessoa (não distingue intruso de morador autorizado) e sem redundância (se o PIR falhar, o sistema fica cego). Em um projeto real seriam necessários mais sensores (porta/janela, câmera, RFID) e confirmação em múltiplas camadas.

## Fluxo implementado (resumo)

INÍCIO → Inicializa ESP32 → Conecta ao Wi-Fi (Wokwi-GUEST) → Botão arma/desarma → Se armado, lê o PIR → Movimento? → Aciona LED vermelho + buzzer → Envia Telegram (se respeitado o cooldown de 10s) → Registra ocorrência → Repete.
