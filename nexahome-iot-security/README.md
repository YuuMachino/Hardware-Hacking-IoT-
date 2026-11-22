# nexahome-iot-security — Avaliação de segurança do NexaSense (IoT)

Projeto da disciplina de Hardware Hacking (IoT): *Do Protótipo ao Produto Seguro*.

**Aluna:** Bianca Arisa Ferreira Machino · **RA:** 98633 · **Entrega:** 22/11/2026

## Objetivo
Avaliar a superfície de ataque do protótipo NexaSense (ESP32 + DHT22 + Wi-Fi/MQTT), classificar riscos e propor hardening para uma versão comercial.

## Dispositivo e escopo
ESP32 DevKit simulado no **Wokwi**, firmware educacional v0.3.1-lab com fragilidades intencionais (todos os segredos são **fictícios**). Testes somente em simulador e em 127.0.0.1. Sem testes contra terceiros; nenhuma eFuse foi gravada.

## Arquitetura
`architecture/architecture-diagram.png` e `architecture/attack-surface-map.png` (S1 UART, S2 JTAG/download, S3 firmware/flash, S4 Wi-Fi, S5 MQTT, S6 OTA).

## Ferramentas
Wokwi, PlatformIO (Arduino-ESP32), esptool, `strings`, sha256sum, `gen_esp32part.py`, Python (paho-mqtt, amqtt, scapy).

## Organização
| Pasta | Conteúdo |
|---|---|
| `architecture/` | diagramas |
| `wokwi/` | `sketch.ino`, `diagram.json`, `libraries.txt`, passo a passo |
| `firmware/` | código (`src/`), `original/` (evidência preservada), `hashes.txt`, `analysis/`, `analysis-notes.md`, `analyze_firmware.sh` |
| `hardware/` | `interface-map.md`, `evidence/` (prints do Wokwi) |
| `network/` | `protocol-analysis.md`, `lab_mqtt.py`, `evidence/` (pcap, log) |
| `findings/` | `findings.md` (F01–F09) |
| `remediation/` | `hardening-plan.md` |

## Resumos
- **Firmware:** imagem ESP32 válida, só checksum/SHA-256 (sem assinatura); segredos, URL de OTA HTTP e mensagens de debug legíveis em strings.
- **Hardware:** UART com console e logs que vazam segredos; JTAG/download e eFuses tratados de forma conceitual.
- **Protocolos:** MQTT/1883 em texto claro; comando forjado aceito; broker sem ACL.
- **Principais achados:** F01 segredos fixos, F02 UART, F03 sem TLS, F04 comandos sem autenticação, F06 OTA sem assinatura (Alto).
- **Recomendações:** ver `remediation/hardening-plan.md`.

## Reprodução (ambiente autorizado)
1. Firmware: `cd firmware && ./analyze_firmware.sh` (verifica hash e gera `analysis/`). Para recompilar: `pio run`.
2. Hardware: siga `wokwi/README.md`; prints já incluídos em `hardware/evidence/` (`uart-boot-log.png`, `uart-config-cmd.png`, `wokwi-diagram.png`).
3. Rede: `cd network && pip install amqtt paho-mqtt scapy && sudo python3 lab_mqtt.py`.

Vídeo de demonstração: não entregue nesta versão (roteiro disponível separadamente).

> Nenhuma credencial real foi publicada; valores como `Nexa@2024-lab` e `nx_lab_token_FAKE_123456` são fictícios.
