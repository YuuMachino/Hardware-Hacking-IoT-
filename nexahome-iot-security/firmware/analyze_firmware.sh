#!/usr/bin/env bash

set -euo pipefail
cd "$(dirname "$0")"
echo "== Integridade da evidência original"; (cd original && sha256sum -c ../hashes.txt)
echo "== Cópia de trabalho"; mkdir -p working && cp original/nexasense-v0.3.1-lab.app.bin working/
echo "== Cabeçalho/segmentos/hash de validação"; esptool image-info working/nexasense-v0.3.1-lab.app.bin | tee analysis/image-info.txt
echo "== Partições"; python3 gen_esp32part.py original/partitions.bin 2>/dev/null | tee analysis/partitions.txt || echo "(use tools/gen_esp32part.py do framework Arduino-ESP32)"
echo "== Strings relevantes"
strings -n 6 working/nexasense-v0.3.1-lab.app.bin > analysis/strings-all.txt
grep -n -i -E "nexa|wokwi|hivemq|Nexa@|nx_lab|http://|mqtt|\[DBG\]|\[OTA\]|telemetry|/cmd|api_token" analysis/strings-all.txt | tee analysis/strings-interesting.txt
