---
mission_id: TR2-20261011-RTU-TX-012
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Mission 012 — Cadrage et integration progressive de la reponse Modbus RTU physique

## Base et resultat acquis

Lire AGENTS.md, gouvernance V2-B3, dernier rapport011 et preuves historiques, specification Modbus RTU V1 gelee, architecture firmware et tests. Travailler sur HEAD reel de main. La mission011 a demontre une reception unique HP COM7 vers STM32 sous BIN008 : 8 octets, 1 trame et 1 ADU valide, zero erreur, octets exacts 01 03 03 E8 00 01 04 7A. RX est valide pour ce cas seulement. Aucun TX physique ni serveur PDU n'est encore qualifie. Ne pas refaire le test RX simplement pour revalider cet acquis.

## Objectif

Identifier la tranche minimale sure pour raccorder le serveur Modbus RTU de production deja implemente au transport STM32 LPUART1 et a la commande DE RS485. Preserver strictement mapping V1, comportement du serveur, absence de float et freezes existants. Ne pas inventer de nouveaux registres ou fonctions. Verifier les connexions, la topologie Click ADM2867E, J2 et les conditions de direction avant tout essai TX. Les anciennes mesures de l'utilisateur et les observations firmware011 sont des acquis documentaires ; ne pas redemander des controles deja faits sans raison technique nouvelle.

## Travail Codex

1. Faire un audit cible des couches serveur Modbus, transport STM32, IRQ, timers, file RX, generation reponse, controle DE et temporisations RTU. Identifier precisement le plus petit raccordement necessaire et ses risques, notamment collision bus, activation prematuree DE, latence turnaround, TX complete versus buffer empty, et retour RX.
2. Implementer une tranche incrementale testable sur hote : parsing ADU -> dispatch serveur V1 existant -> construction reponse -> transport TX avec controle explicite de DE et retour reception. Prevoir un garde-fou fail-safe qui laisse DE bas au boot, reset, erreur et quand aucun TX n'est autorise. Pas de TX non sollicite. Ne pas modifier le mapping gele.
3. Ajouter tests unitaires/integration couvrant CRC, trame valide/invalide, unit id, silence, exception, pas de reponse si adresse non concernee, ordre DE/TX complete/DE bas, erreurs et timeout. Compiler et executer tests hote et STM32, enregistrer preuves.
4. Ne flasher et ne debugger la carte que si le nouveau firmware est suffisamment qualifie et si les preconditions physiques de direction et d'alimentation sont etablies. Autorisation permanente utilisateur : allow_flash true, allow_debug true. Avant programmation sauvegarder/verifier image et identite de cible, conserver preuve precedente et flash ciblé uniquement. Si la surete TX n'est pas suffisamment etablie, ne pas emettre sur le bus : documenter le blocage et preparer une etape physique minimale, sans modifier les ponts/cablage.
5. Ne pas acceder au HP COM7 ni emettre une nouvelle trame sans possibilite d'execution controlee et preuves. Les essais RX precedents ne qualifient pas automatiquement TX. Ne pas pretendre avoir valide une reponse physique sans mesures.

## Autorisations et limites

allow_commit true, allow_push true, allow_flash true, allow_debug true sur main, conformement a AGENTS.md et a la decision permanente du 11 octobre. Flash et debug autonomes autorises quand necessaires, sans sollicitation repetitive. Aucune modification physique, mass erase, Option Bytes/RDP, deprotection, operation Git destructive ou force push. Suspendre toute emission TX physique si la configuration DE et /RE ou les tensions restent incertaines. Preserver stashes, freezes et sources existantes.

## Livrables

Publier DERNIER_RAPPORT.md mission012 avec audit, modifications, tests, etats distincts IMPLEMENTATION / HOST TESTS / STM32 BUILD / FLASH / TX PHYSIQUE, preuves, limites et prochaine action. Commit/push normaux main et verification distante.
