# Mapa de interfaces físicas (Etapas 1 e 4)

## Reconhecimento (Etapa 1)
| Item | NexaSense (laboratório) |
|---|---|
| Microcontrolador | ESP32 DevKit (Xtensa LX6 dual-core), no Wokwi: `board-esp32-devkit-c-v4` |
| Memória | Flash SPI 4 MB (nvs, otadata, app0/app1, spiffs, coredump); SRAM 520 KB |
| Sensores | DHT22 (temperatura/umidade) em GPIO15 |
| Atuador | LED verde + 220 Ω em GPIO2 |
| Comunicação | Wi-Fi 2,4 GHz (STA) → MQTT/TCP 1883 |
| Interfaces físicas | UART0 (GPIO1/3), JTAG (GPIO12–15), modo download (GPIO0), SPI da flash |
| Alimentação | 3V3 / USB 5 V (DevKit) |
| Armazenamento | NVS + SPIFFS + coredump na flash |

Diagrama: `../architecture/architecture-diagram.png`. Esquema Wokwi: `../wokwi/diagram.json`.

## Interfaces e risco (Etapa 4) — "o que permite e qual o risco em produção?"
| Interface | O que permite | Evidência no laboratório | Proteção existente | Risco em produto comercial |
|---|---|---|---|---|
| UART0 / console | Ver logs de boot e digitar comandos (`config`, `mac`, `reboot`) | Serial Monitor do Wokwi mostra `[DBG] mqtt ... pass=...` e `api_token=...`; comando `config` repete os segredos (`evidence/`) | Nenhuma (sem senha, sem desativar log) | Atacante com acesso físico obtém credenciais e identificadores → ver F02 |
| Modo download (GPIO0 + UART) | Ler/gravar flash via ROM bootloader | Conceitual (não simulado) | Nenhuma (eFuse não configurado) | Extração/substituição do firmware → F08 |
| JTAG (GPIO12/13/14/15) | Depuração de baixo nível, leitura de memória | Conceitual; nota: DHT22 usa GPIO15 = MTDO | Nenhuma conhecida no protótipo | Acesso à RAM/flash se não desabilitado por eFuse → F09 |
| SPI flash | Leitura direta do chip (clip) | Conceitual; imagem em claro (strings legíveis) | Sem Flash Encryption | Clonagem e extração de segredos → F08 |
| I²C | Não utilizada | — | — | N/A |
| Pontos de teste | Não há PCB real neste ambiente | — | — | Inspecionar na placa física |

Uma interface acessível não é automaticamente vulnerabilidade: o risco só é classificado em `../findings/findings.md` considerando contexto (acesso físico), ausência de proteção e impacto.

## Evidências coletadas no Wokwi (screenshots em `evidence/`)
1. `uart-boot-log.png` — Serial Monitor mostrando o banner de boot com segredos.
2. `uart-config-cmd.png` — saída do comando `config`.
3. `wokwi-diagram.png` — circuito (ESP32 + DHT22 + LED).

Observado em 2026-11-22: o boot imprime `wifi ssid=Wokwi-GUEST pass=''`, `mqtt ... user=nexasense pass=Nexa@2024-lab` e `api_token=...`; o comando `config` repete os segredos sem qualquer autenticação.
