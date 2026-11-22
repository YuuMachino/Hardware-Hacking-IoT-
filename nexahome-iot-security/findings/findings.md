# Achados — NexaSense v0.3.1-lab

Matriz: Alto×Alta/Média = **Alto**; Alto×Baixa = **Médio**; Médio×Alta = **Alto**; Médio×Média = **Médio**.
Cada achado segue: Evidência → Fragilidade → Ameaça → Impacto → Risco → Mitigação.

| ID | Achado | Evidência | Impacto | Prob. | Risco | Recomendação |
|---|---|---|---|---|---|---|
| F01 | Segredos fixos no firmware (senha MQTT, token, SSID, broker) | `firmware/analysis/strings-interesting.txt` linhas 145–151 | Alto | Alta | **Alto** | Credenciais únicas por dispositivo, provisionadas em NVS criptografada; remover do código |
| F02 | UART/console de depuração ativo imprime segredos e aceita `config`/`reboot` sem autenticação | strings 143–154, 161; Serial Monitor (Wokwi) | Alto | Média (requer acesso físico) | **Alto** | Build de produção sem logs de segredos; console desativado/protegido |
| F03 | Telemetria e credenciais em texto claro (MQTT/1883, sem TLS) | `network/evidence/mqtt-plaintext.pcap`, `mqtt-connect-extract.txt` | Alto | Alta | **Alto** | MQTT sobre TLS 1.2+ (8883), validação de certificado |
| F04 | Comandos em `/cmd` aceitos sem autenticação de origem | `lab-run.log` (LED `on` após comando forjado) | Médio | Alta | **Alto** | ACL por tópico + comandos assinados/autorizados por dispositivo |
| F05 | Broker sem ACL/anônimo permite ler todo `nexahome/lab/#` | `lab-run.log` (observador sem credenciais) | Médio | Média | **Médio** | Desativar anônimo; ACL por device-id |
| F06 | OTA via HTTP sem verificação de assinatura; imagem tem só checksum/SHA-256 | strings 138, 139, 164; `image-info.txt` (sem bloco de assinatura) | Alto | Média (MITM na rede) | **Alto** | OTA por HTTPS + assinatura de firmware (Secure Boot v2) + anti-rollback |
| F07 | Identidade/credenciais não individuais (mesmo `DEVICE_ID`/senha para o lote) | strings 141, 147–148 | Médio | Média | **Médio** | Identidade única por unidade (certificado X.509) |
| F08 | Flash/NVS/coredump sem proteção; segredos legíveis na imagem (Flash Encryption ausente) | `image-info.txt`, `partitions.txt`, strings; eFuses não verificáveis no simulador | Alto | Baixa (exige hardware) | **Médio** | Habilitar Flash Encryption; proteger NVS; limpar coredump |
| F09 | JTAG e modo download sem restrição (conceitual) | `hardware/interface-map.md` | Alto | Baixa | **Médio** | Desabilitar JTAG e restringir download via eFuse na versão de produção |

F08/F09 são **achados conceituais**: o simulador não expõe eFuses/JTAG; foram deduzidos do firmware/configuração e devem ser confirmados em hardware real.

## Relação com referências
- NIST IR 8259A: configuração/identificação do dispositivo (F01, F07), proteção de dados (F03), controle de acesso a interfaces (F02, F09), atualização (F06).
- ETSI EN 303 645: sem senhas padrão universais (F01, F07); comunicar com segurança (F03); manter software atualizado (F06); proteger parâmetros sensíveis (F01, F08); minimizar superfície de ataque (F02, F09).
- MITRE EMB3D: extração de firmware via interfaces de hardware (F08, F09); interfaces privilegiadas expostas (F02).
- OWASP ISTG: metodologia seguida (superfície → firmware → hardware → rede → relatório).
- Espressif ESP-IDF: Secure Boot v2 e Flash Encryption como mitigações (F06, F08).
