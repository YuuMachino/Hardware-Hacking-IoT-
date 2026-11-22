# Plano de hardening — NexaSense versão comercial

| Prioridade | Medida | Mitiga | Referência |
|---|---|---|---|
| P1 | Remover segredos do código; credencial e certificado únicos por dispositivo, provisionados na fábrica | F01, F07 | ETSI 5.1/5.4; NIST 8259A |
| P1 | MQTT sobre TLS (8883) com validação de certificado do servidor; mTLS por dispositivo | F03 | ETSI 5.5 |
| P1 | Broker com ACL por device-id; sem acesso anônimo; comandos autorizados/assinados | F04, F05 | NIST 8259A (acesso lógico) |
| P1 | OTA por HTTPS + assinatura de firmware (Secure Boot v2) + anti-rollback + partição A/B com recuperação | F06 | ETSI 5.3; Espressif |
| P2 | Build de produção: sem logs `[DBG]`, console desativado, nível de log mínimo | F02 | ETSI 5.6 |
| P2 | Flash Encryption (modo release); NVS criptografada; coredump desabilitado ou criptografado | F08 | Espressif; MITRE EMB3D |
| P2 | Desabilitar JTAG e restringir/desativar modo de download via eFuse; revisar ordem de gravação das eFuses | F09 | Espressif |
| P3 | Processo: modelo de ameaças (EMB3D), testes por checklist OWASP ISTG a cada release, política de divulgação | todos | OWASP ISTG |

Observações: eFuses são irreversíveis; foram **apenas propostas**, não gravadas. Antes de habilitar Secure Boot/Flash Encryption em produção, validar em placas de desenvolvimento descartáveis e manter processo de recuperação.

Exemplo real relacionado: CVE-2019-15894 (Espressif) — mostra que, mesmo com Secure Boot, a combinação com Flash Encryption e atualização do ESP-IDF é necessária (ver relatório).
