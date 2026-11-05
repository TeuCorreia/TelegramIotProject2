// Central de Segurança IoT - versão simples (atividade de detecção de intrusos)
// ESP32 + PIR + LED verde/vermelho + buzzer + botão + Wi-Fi (Wokwi-GUEST) + Telegram
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define BOT_TOKEN "9272900286:AAEe71o6tlQ1Q3wFsiFbu_8pq0pv0QhTYSQ"
#define CHAT_ID   "6990845731"

#define PIN_PIR       32
#define PIN_LED_VERDE 18
#define PIN_LED_VERM  19
#define PIN_BUZZER    21
#define PIN_BOTAO     22   // INPUT_PULLUP: pressionado = LOW

const char* LOCAL = "Sala de Equipamentos";
const unsigned long COOLDOWN_MS = 10000;  // 1 notificação a cada 10s, no máximo
const unsigned long ALARME_MS   = 5000;   // duração do alarme após o movimento
const unsigned long STATUS_MS   = 2000;   // status no Monitor Serial
const unsigned long DEBOUNCE_MS = 50;

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

bool armado = false;
bool alarme = false;
bool telegramOk = false;
bool pirAnterior = false;

unsigned long ultimoEnvio = 0;   // 0 = ainda não enviou nada
unsigned long inicioAlarme = 0;
unsigned long ultimoStatus = 0;
unsigned long ultimoDebounce = 0;
int botaoAnterior = HIGH;
int botaoEstavel = HIGH;
int ocorrencias = 0;

String horarioUptime() {
  unsigned long s = millis() / 1000;
  char buf[16];
  snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", s / 3600, (s / 60) % 60, s % 60);
  return String(buf);
}

void atualizaLeds() {
  digitalWrite(PIN_LED_VERDE, armado ? LOW : HIGH);  // verde = DESARMADO
  digitalWrite(PIN_LED_VERM,  armado ? HIGH : LOW);  // vermelho = ARMADO / ALERTA
}

void enviarTelegram(const String& texto) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Telegram: FALHA (Wi-Fi desconectado)");
    return;
  }
  if (bot.sendMessage(CHAT_ID, texto, "")) {
    Serial.println("Telegram: MENSAGEM ENVIADA");
  } else {
    Serial.println("Telegram: FALHA NO ENVIO (confira BOT_TOKEN, CHAT_ID e /start no bot)");
  }
}

void imprimeStatus(bool pir) {
  Serial.println("================================");
  Serial.println(" CENTRAL DE SEGURANCA IoT");
  Serial.println("================================");
  Serial.printf("Wi-Fi: %s\n", WiFi.status() == WL_CONNECTED ? "CONECTADO" : "DESCONECTADO");
  Serial.printf("Telegram: %s\n", telegramOk ? "CONECTADO" : "INDISPONIVEL");
  Serial.printf("Sistema: %s\n", armado ? "ARMADO" : "DESARMADO");
  Serial.printf("PIR: %s\n", pir ? "MOVIMENTO DETECTADO" : "SEM MOVIMENTO");
  Serial.printf("LED: %s\n", armado ? "VERMELHO" : "VERDE");
  Serial.printf("Buzzer: %s\n", alarme ? "LIGADO" : "DESLIGADO");
  Serial.printf("Ocorrencias: %d\n\n", ocorrencias);
}

void setArmado(bool v) {
  if (v == armado) return;
  armado = v;
  alarme = false;
  noTone(PIN_BUZZER);
  atualizaLeds();
  tone(PIN_BUZZER, armado ? 2400 : 1000, 150);  // beep curto de confirmação

  if (armado) {
    enviarTelegram("SISTEMA ARMADO\nA central de seguranca foi ativada.\nMonitoramento iniciado.");
  } else {
    enviarTelegram("SISTEMA DESARMADO\nA central de seguranca foi desativada.");
  }
  imprimeStatus(digitalRead(PIN_PIR));
}

void disparaAlarme() {
  ocorrencias++;
  alarme = true;
  inicioAlarme = millis();
  digitalWrite(PIN_LED_VERM, HIGH);

  Serial.println("--------------------------------");
  Serial.println("MOVIMENTO DETECTADO!");
  Serial.println("--------------------------------");
  Serial.printf("Ocorrencia n. %d (%s)\n", ocorrencias, horarioUptime().c_str());
  Serial.println("LED vermelho: LIGADO");
  Serial.println("Buzzer: LIGADO\n");

  // Controle de notificações: só envia se passou o cooldown de 10s
  if (ultimoEnvio == 0 || millis() - ultimoEnvio >= COOLDOWN_MS) {
    String msg = "ALERTA DE INTRUSAO\n\n";
    msg += String("Sistema: CENTRAL DE SEGURANCA\n");
    msg += String("Local: ") + LOCAL + "\n";
    msg += "Sensor: PIR\n";
    msg += "Estado: ARMADO\n";
    msg += "Acao: ALARME ATIVADO\n";
    msg += "Horario: " + horarioUptime() + "\n";
    msg += "Ocorrencias: " + String(ocorrencias);
    enviarTelegram(msg);
    ultimoEnvio = millis();
  } else {
    Serial.printf("Notificacao suprimida (faltam %lus)\n",
                  (COOLDOWN_MS - (millis() - ultimoEnvio)) / 1000);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_VERM, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BOTAO, INPUT_PULLUP);
  atualizaLeds();

  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");

  client.setInsecure();
  telegramOk = bot.getMe();
  if (!telegramOk) {
    Serial.println("Telegram indisponivel (confira BOT_TOKEN, CHAT_ID e se enviou /start ao bot)");
  }

  imprimeStatus(false);
}

void loop() {
  // ---- Botão: alterna DESARMADO -> ARMADO -> DESARMADO (Opção A)
  int leitura = digitalRead(PIN_BOTAO);
  if (leitura != botaoAnterior) ultimoDebounce = millis();
  if (millis() - ultimoDebounce > DEBOUNCE_MS && leitura != botaoEstavel) {
    botaoEstavel = leitura;
    if (botaoEstavel == LOW) setArmado(!armado);   // pressionou
  }
  botaoAnterior = leitura;

  // ---- Leitura do PIR (borda de subida)
  bool pir = digitalRead(PIN_PIR);
  if (armado && pir && !pirAnterior) disparaAlarme();

  // ---- Duração/padrão do alarme (BEEP-BEEP-BEEP)
  if (alarme) {
    if ((millis() / 200) % 2 == 0) tone(PIN_BUZZER, 1800);
    else noTone(PIN_BUZZER);

    if (millis() - inicioAlarme >= ALARME_MS) {
      alarme = false;
      noTone(PIN_BUZZER);
      Serial.println("Alarme encerrado. Retomando monitoramento.\n");
    }
  } else {
    noTone(PIN_BUZZER);
  }

  // ---- Status periódico no Monitor Serial
  if (millis() - ultimoStatus >= STATUS_MS) {
    ultimoStatus = millis();
    imprimeStatus(pir);
  }

  pirAnterior = pir;
  delay(10);
}
