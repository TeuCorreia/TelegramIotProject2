# Central de Segurança IoT — versão simples

Projeto mínimo para a atividade **Sistema Embarcado de Detecção de Intrusos** (ESP32 + PIR + Telegram no Wokwi).

## O que tem
- ESP32
- 1 sensor PIR (pino 32)
- LED verde = sistema **DESARMADO** (pino 18)
- LED vermelho = sistema **ARMADO / ALERTA** (pino 19)
- Buzzer com padrão BEEP-BEEP-BEEP (pino 21)
- Push button para armar/desarmar — **Opção A: alternância** (pino 22)
- Wi-Fi `Wokwi-GUEST`
- Notificações no Telegram com **cooldown de 10s** (evita flood)
- Monitor Serial com status a cada 2s

## Como usar no Wokwi
1. Crie um novo projeto ESP32 no Wokwi.
2. Substitua o `sketch.ino`, `diagram.json` e `libraries.txt` pelos arquivos desta pasta.
3. No Library Manager, adicione `UniversalTelegramBot` e `ArduinoJson` (ou use o `libraries.txt`).
4. Inicie a simulação. O ESP32 conecta no `Wokwi-GUEST` automaticamente.
5. Pressione o botão para **ARMAR** (LED vermelho acende).
6. Clique no PIR para simular movimento.
7. Veja o alerta no Monitor Serial e a mensagem no Telegram.
8. Pressione o botão de novo para **DESARMAR** (LED verde acende).

## Segurança do token
O `BOT_TOKEN` já está no `sketch.ino` só para o projeto funcionar. **Não publique** o código com o token em repositórios, prints ou grupos. Se vazar, gere um novo token no @BotFather.

## Respostas da atividade
Veja o arquivo `respostas.md`.
