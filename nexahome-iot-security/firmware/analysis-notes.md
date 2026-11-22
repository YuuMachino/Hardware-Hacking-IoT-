# Análise de firmware — NexaSense v0.3.1-lab

## Preservação da evidência (Etapa 2)
| Campo | Valor |
|---|---|
| Arquivo analisado | `original/nexasense-v0.3.1-lab.app.bin` (+ `.factory.bin`, `.elf`, `bootloader.bin`, `partitions.bin`) |
| Tamanho (app) | 999 088 bytes |
| Origem | Compilado do código `wokwi/sketch.ino` (= `src/main.cpp`) com PlatformIO + Arduino-ESP32, placa `esp32dev` — firmware educacional do laboratório |
| Data da análise | 2026-11-21 (UTC 04:30) |
| SHA-256 (app) | `bc23babd824a003b550c9b990287ed6bb7848a1485dfb627d682b510549c0e38` |
| SHA-256 (factory) | `f2cb3c6a5fd44348347f265184a44585356e08dfcf20d032ddac416d5e212590` |

Todos os hashes estão em `hashes.txt`. Os originais ficam em `original/` (somente leitura, `chmod 444`); a análise é feita sobre cópia em `working/`. Verificação: `cd original && sha256sum -c ../hashes.txt`.

## Estrutura (Etapa 3) — `analysis/image-info.txt`, `analysis/partitions.txt`
- Formato: imagem de app ESP32 (magic 0xE9), 6 segmentos (DROM, DRAM, IRAM, IROM, IRAM, RTC_DATA), flash 4 MB, DIO 40 MHz.
- Rodapé: checksum (válido) + **hash SHA-256 de validação (válido)**. Não há bloco de assinatura (Secure Boot v2) — o hash apenas detecta corrupção, não autenticidade.
- Partições: `nvs` 20K, `otadata` 8K, `app0`/`app1` 1280K (OTA A/B), `spiffs` 1408K, `coredump` 64K — todas sem indicação de criptografia no esquema padrão.

## Informações relevantes encontradas (`analysis/strings-interesting.txt`)
| Linha | Conteúdo | Relevância |
|---|---|---|
| 138 | `http://updates.nexahome.example/nexasense/fw.bin` | OTA por HTTP (sem TLS) |
| 139, 164 | `[OTA] ... (sem validar)` / `(sem verificacao de assinatura)` | ausência de validação de origem |
| 145 | `Wokwi-GUEST` | SSID gravado no firmware |
| 147–148 | `Nexa@2024-lab` / `nexasense` | senha e usuário MQTT fixos (fictícios) |
| 149 | `broker.hivemq.com` | endpoint do broker |
| 151 | `nx_lab_token_FAKE_123456` | token de API fixo (fictício) |
| 143–154 | mensagens `[DBG] ...` | imprimem segredos no boot (UART) |
| 159, 168 | tópicos `.../cmd` e `.../telemetry` | estrutura do namespace MQTT |
| 161 | `cmds: help \| info \| config \| mac \| ota \| reboot` | console de depuração ativo |

Limites: não houve descompilação; consulta a eFuses não é possível no simulador (Secure Boot/Flash Encryption tratados de forma conceitual).
