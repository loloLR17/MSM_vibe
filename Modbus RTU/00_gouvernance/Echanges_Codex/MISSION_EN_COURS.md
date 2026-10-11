---
mission_id: TR2-20261011-RX-011
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: true
---

# Mission 011 - Analyse des compteurs de reception RS485

L'utilisateur a lance une seule fois sur le PC HP le script Send-Tr2RxProbe.ps1 -Transmit. Sortie obtenue : Request: 01 03 03 E8 00 01 04 7A; COM7 115200 8E1; CRC/self-check OK. Puis : No response in ~1 s: expected possibility for RX-only firmware; inspect MCU counters. Ne pas renouveler cet essai.

Codex doit lire AGENTS.md, le rapport de mission010 et les procedures RX existantes, puis relever par debug en lecture seule les compteurs RX du STM32 avec le firmware et les symboles de la mission010. Comparer au dernier snapshot de mission010 (ready 1, octets 0, trames 0, ADU valides 0, erreurs 0). Chercher une progression de 8 octets et une ADU valide, avec zero erreur et les octets exacts. Le firmware est deja flashe et verifie : aucun nouveau flash requis.

Conserver les preuves et relever les limites. Si aucun octet n'est observe, documenter les observations et un diagnostic cible sans nouvelle emission ni repetition des mesures utilisateur. Terminer la session debug et laisser le firmware fonctionner.

Publier DERNIER_RAPPORT.md mission011 avec le verdict, les mesures et les preuves. Commit et push normaux sur main, puis verification distante. Aucune modification physique, effacement massif, Option Bytes ou force push.
