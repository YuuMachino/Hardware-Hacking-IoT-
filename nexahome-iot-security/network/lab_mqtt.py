#!/usr/bin/env python3
"""
Laboratório de rede NexaSense (100% local, 127.0.0.1) - Etapas 5 e 6.

Como o ESP32 só existe dentro do simulador Wokwi, este script REPRODUZ no
laboratório o comportamento de rede descrito no firmware (mesmo broker TCP/1883,
mesmos tópicos, mesmo formato de payload e mesmas credenciais FICTÍCIAS) para:
  1) capturar o tráfego em pcap (scapy) e provar que é texto claro;
  2) mostrar que um observador SEM credenciais lê a telemetria (sem confidencialidade);
  3) mostrar que um comando forjado em ..../cmd é aceito pelo dispositivo (sem autenticação de origem).
Nada sai de 127.0.0.1.  Uso: python3 lab_mqtt.py
"""
import asyncio, threading, time, json, re, sys
import paho.mqtt.client as mqtt
from scapy.all import sniff, wrpcap, TCP, Raw
from amqtt.broker import Broker

HOST, PORT = "127.0.0.1", 1883
DEVICE_ID, FW = "nexasense-0001", "0.3.1-lab"
MQTT_USER, MQTT_PASS = "nexasense", "Nexa@2024-lab"      # FICTÍCIAS (iguais ao firmware)
API_TOKEN, MAC = "nx_lab_token_FAKE_123456", "24:0a:c4:00:01:10"
T_DATA, T_CMD = f"nexahome/lab/{DEVICE_ID}/telemetry", f"nexahome/lab/{DEVICE_ID}/cmd"
log = []
def say(m):
    line = f"[{time.strftime('%H:%M:%S')}] {m}"; print(line); log.append(line)

# ---------------- broker local ----------------
def run_broker():
    cfg = {"listeners": {"default": {"type": "tcp", "bind": f"{HOST}:{PORT}"}},
           "plugins": {"amqtt.plugins.authentication.AnonymousAuthPlugin": {"allow_anonymous": True}}}
    loop = asyncio.new_event_loop(); asyncio.set_event_loop(loop)
    b = Broker(cfg, loop=loop)
    loop.run_until_complete(b.start()); loop.run_forever()

# ---------------- "dispositivo" ----------------
led = {"state": "off"}
def device():
    c = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=f"{DEVICE_ID}-lab")
    c.username_pw_set(MQTT_USER, MQTT_PASS)
    def on_msg(cl, ud, m):
        msg = m.payload.decode()
        say(f"DISPOSITIVO recebeu cmd '{msg}' em {m.topic} (sem verificar origem)")
        if msg == "led_on": led["state"] = "on"
        if msg == "led_off": led["state"] = "off"
    c.on_message = on_msg
    c.connect(HOST, PORT); c.subscribe(T_CMD); c.loop_start()
    for t, h in [(27.5, 61.0), (27.6, 60.8), (27.5, 61.1)]:
        payload = ('{"id":"%s","fw":"%s","mac":"%s","temp":%.1f,"hum":%.1f,"token":"%s"}'
                   % (DEVICE_ID, FW, MAC, t, h, API_TOKEN))
        c.publish(T_DATA, payload); time.sleep(1)
    return c

def observer():
    """Cliente SEM usuário/senha: apenas assina o curinga do namespace."""
    seen = []
    o = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="observador-sem-credenciais")
    o.on_message = lambda cl, ud, m: (seen.append(m.payload.decode()),
                                      say(f"OBSERVADOR (sem credenciais) leu {m.topic}: {m.payload.decode()}"))
    o.connect(HOST, PORT); o.subscribe("nexahome/lab/#"); o.loop_start()
    return o, seen

if __name__ == "__main__":
    threading.Thread(target=run_broker, daemon=True).start(); time.sleep(2)
    pk = []
    sn = threading.Thread(target=lambda: pk.extend(sniff(iface="lo", lfilter=lambda p: p.haslayer(TCP) and PORT in (p[TCP].sport, p[TCP].dport), timeout=14)), daemon=True)
    sn.start(); time.sleep(1.5)
    say("== Etapa 5: captura de tráfego em 127.0.0.1:1883 iniciada")
    o, seen = observer(); time.sleep(0.5)
    dev = device(); time.sleep(0.5)
    say("== Etapa 6: observador sem credenciais publica comando forjado 'led_on'")
    o.publish(T_CMD, "led_on"); time.sleep(1.5)
    say(f"Estado do LED do dispositivo após comando forjado: {led['state']}")
    sn.join(); wrpcap("evidence/mqtt-plaintext.pcap", pk)
    raw = b"".join(bytes(p[Raw].load) for p in pk if p.haslayer(Raw))
    say(f"Pacotes capturados: {len(pk)}; bytes de aplicação: {len(raw)}")
    for label, needle in [("usuário MQTT", MQTT_USER), ("senha MQTT", MQTT_PASS),
                          ("token de API no payload", API_TOKEN), ("MAC", MAC), ("ID do dispositivo", DEVICE_ID)]:
        say(f"Texto claro no pcap -> {label}: {'ENCONTRADO' if needle.encode() in raw else 'não encontrado'}")
    open("evidence/lab-run.log", "w").write("\n".join(log) + "\n")
    sys.exit(0)
