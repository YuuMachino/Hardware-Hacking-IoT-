# Análise de comunicações (Etapa 5) e demonstração controlada (Etapa 6)

Ambiente: 100% local (127.0.0.1). Script: `lab_mqtt.py`. Saídas em `evidence/`:
`mqtt-plaintext.pcap` (captura real em loopback), `lab-run.log`, `mqtt-connect-extract.txt`.

> Transparência metodológica: o ESP32 existe apenas no simulador Wokwi. O script **reproduz** o comportamento de rede definido no firmware (mesmos tópicos, payload, porta e credenciais fictícias) contra um broker local e captura o tráfego com scapy. No Wokwi, o mesmo fluxo é visto no Serial Monitor (`[PUB] ...`) e no cliente MQTT.

| Item | Resultado |
|---|---|
| Protocolo | MQTT 3.1.1 sobre TCP/1883 |
| Origem → destino | dispositivo (`nexasense-0001`) → broker; broker → plataforma |
| Informação transmitida | id, versão de fw, MAC, temperatura, umidade, **token de API** |
| Autenticação | usuário/senha enviados **no pacote CONNECT em texto claro**; broker de lab aceita anônimo |
| Criptografia | nenhuma (sem TLS) |
| Identificadores expostos | `nexasense-0001`, MAC `24:0a:c4:00:01:10`, versão do firmware |
| Mensagens de controle | tópico `.../cmd` com `led_on`, `led_off`, `reboot`, `ota` |
| Informação sensível | usuário/senha MQTT e token (fictícios) no pcap |

Identificar o protocolo ≠ concluir que é seguro: MQTT é adequado quando usado com TLS (8883), credenciais por dispositivo e ACL por tópico. Aqui nenhum dos três existe.

## Demonstração controlada
1. Captura iniciada; dispositivo conecta e publica 3 leituras.
2. `observador-sem-credenciais` assina `nexahome/lab/#` e lê toda a telemetria, inclusive o token.
3. O mesmo observador publica `led_on` em `.../cmd`; o dispositivo executa (`lab-run.log`: "DISPOSITIVO recebeu cmd 'led_on' ... (sem verificar origem)"; estado do LED: `on`).
4. No pcap, `CONNECT` contém `nexasense` / `Nexa@2024-lab` e os PUBLISH contêm o token (ver `mqtt-connect-extract.txt`).
Impacto limitado ao laboratório; sem exploração fora de 127.0.0.1.

Reprodução: `pip install amqtt paho-mqtt scapy && sudo python3 lab_mqtt.py` (sudo para captura em `lo`).
