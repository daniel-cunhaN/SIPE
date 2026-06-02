import os
import serial
import requests
import time
import sys
from dotenv import load_dotenv

# Carrega as chaves do arquivo .env
load_dotenv()
TOKEN = os.getenv("TELEGRAM_TOKEN")
CHAT_ID = os.getenv("TELEGRAM_CHAT_ID")
URL_TELEGRAM = f"https://api.telegram.org/bot{TOKEN}/sendMessage"

# AJUSTE DINÂMICO: Detecta se está no Linux (WSL) ou Windows
PORTA_COM = 'COM8' 

def conectar_arduino():
    try:
        # Aumentamos o timeout para garantir a leitura estável[cite: 2]
        return serial.Serial(PORTA_COM, 9600, timeout=1)
    except:
        return None

arduino = conectar_arduino()

if arduino:
    print(f"--- SISTEMA ONLINE: Escutando Arduino na {PORTA_COM} ---")
else:
    print(f"--- AVISO: Arduino não detectado na {PORTA_COM}. Modo manual ativo. ---")

try:
    while True:
        comando = ""

        # Prioridade 1: Ler do Arduino
        if arduino and arduino.in_waiting > 0:
            try:
                comando = arduino.readline().decode('utf-8').strip()
                if comando:
                    print(f"Sinal do Arduino: {comando}")
            except:
                pass 

        if "Card UID:" in comando:
            tag_id = comando.split("Card UID:")[1].strip()
            print(f"RFID Detectado: {tag_id}. Enviando para Telegram...")
            payload = {
                "chat_id": CHAT_ID, 
                "text": f"O Sistema registrou acesso. \nTAG RFID ID: {tag_id}"
            }
            
            try:
                r = requests.post(URL_TELEGRAM, data=payload)
                if r.status_code == 200:
                    print("✅ Notificação enviada com sucesso!")
                else:
                    print(f"❌ Falha no Telegram. Status: {r.status_code}")
            except Exception as e:
                print(f"Erro de conexão: {e}")
        
        time.sleep(0.1) # Evita uso excessivo da CPU[cite: 2]

except KeyboardInterrupt:
    print("\nEncerrando monitoramento...")
finally:
    if arduino:
        arduino.close()