# Simulação no Wokwi (https://wokwi.com)

**Opção A — projeto novo (mais simples)**
1. Wokwi → *New Project* → **ESP32** → *Arduino*.
2. Substitua `sketch.ino` pelo arquivo desta pasta; cole `diagram.json` na aba *diagram.json*; cole `libraries.txt` na aba *Library Manager* (DHT sensor library, PubSubClient).
3. ▶ *Start Simulation*. No Serial Monitor você verá o banner `[DBG]` com segredos, `[WiFi] ok`, `[MQTT] conectado` e `[PUB] ...`.
4. Digite `help`, `config` e `mac` no Serial Monitor (console de depuração) e tire os prints.

**Opção B — usar o firmware já compilado:** F1 → *Upload Firmware and Start Simulation…* → `../firmware/original/nexasense-v0.3.1-lab.factory.bin` (imagem única com bootloader + partições + app). Assim o mesmo binário que foi hasheado e analisado é o que roda.

Observações: o sketch usa o broker público `broker.hivemq.com:1883` apenas para *publicar* telemetria com tópico único `nexahome/lab/nexasense-0001/...`; não faça testes ofensivos contra ele. Para a demonstração controlada use `../network/lab_mqtt.py` (tudo local). Os valores sensíveis são fictícios. Velxio e Cirkit Designer também aceitam ESP32 + DHT22, mas o Wi-Fi/MQTT é melhor suportado no Wokwi.
