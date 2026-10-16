---
mission_id: TR2-20261016-R4R-PHYSICAL-TIME-B3
status: READY
base_ref: main
base_sha: 650aac856b3f912dfbe63ee4163bfe02489a8853
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-R — Temps événement physique et métrologie LIVE B3 sur STM32

## Objectif et état initial
Après R4-P (coexistence FRAM/FIFO réelle, 60s) et R4-Q (FIFO→R3-B→B3 RAM→RTU validé en simulation ; cible readonly sans FIFO), qualifier un fournisseur de temps événement physique fiable puis exercer sur TR2-001 une chaîne d'acquisition réelle jusqu'aux mesures B3, avec lecture RTU loopback RAM et charge FRAM maîtrisée si les préconditions le permettent. Ne pas confondre temps scheduler DWT/HAL, horodatage de lecture et temps réel des échantillons ; ne pas fabriquer une grille de temps physique. Préserver la production V1 et les normes LIVE R0.

## Gouvernance et analyse
Lire sur main AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, état courant, roadmap, spécifications temps/qualité R0, R3-B, R4-P, R4-Q, rapports et preuves. Vérifier HEAD réel et statut R4-Q DONE ; archiver exactement mission/rapport R4-Q avant remplacement selon protocole. Analyser datasheet et drivers IIS3DWB/ST existants, FIFO et tags timestamp, ODR, rollover, ordre des mots, cadence, latence de lecture et domaine d'horloge ; identifier une méthode observable/calibrable et ses limites. Choisir le raccordement minimal, sans inventer un temps par échantillon depuis le temps de polling ni prétendre UTC synchronisée.

## Exécution autonome par incréments
1. Concevoir et implémenter le fournisseur d'événements horodatés à partir de preuves du capteur et/ou d'une méthode physique mesurée, avec continuité monotone, résolution, incertitude, rollover, FIFO backlog, première fenêtre, reset et défauts explicites. Si l'information disponible ne permet pas d'attribuer des instants conformes aux XYZ, fournir un diagnostic démontrant le blocage et ne pas publier de mesures trompeuses. Respecter le contrat R3-B et les indicateurs de qualité/UTC absente.
2. Composer sur profil de banc NON-PRODUCTION le propriétaire FIFO réel, métrologie R3-B, snapshots RAM B3 cohérents et lecture FC03/RTU loopback, sans réentrée ni FRAM depuis PDU/ISR. Vérifier fenêtre complète RMS/peak, état VALID/FRESH/HELD, N=4096/8192/16384/32768 selon couverture réaliste, horodatage/âge et invalidations, cohérence par ADU, aucune capacité produit LIVE annoncée prématurément. Le B5 différé et ses invariants R2 doivent rester inchangés.
3. Réaliser d'abord tests hôte/oracles et ARM, puis un essai physique progressif sur TR2-001 si sûr : vérification UID/ST-LINK, état du capteur, Flash/FRAM et backups récupérables ; FRAM finale attendue R4-Q epoch4/run4 SHA256 a13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9, Flash M016 SHA256 c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26. Flash normal d'image dédiée identifiée et hashée, debug qualifié. Première étape capteur+temps seul ; ensuite B3 et lectures loopback ; enfin coexistence FRAM sous charge bornée seulement si journal vérifié et autorité R2 authentique. Ne pas consommer de nouveaux runs/epochs sans nécessité technique démontrée. Aucune action microSD/HP, aucune modification de câblage, Option Bytes/RDP ou mass erase.
4. Mesurer et publier sur cible DWT/HCLK, work continu <=1000us, service <=2500us, gap FIFO <=5ms en régime continu (premier mot séparé), FIFO max, overrun/pertes détectées, précision/dérive/rollover des timestamps, RMS/peak et qualité B3, coûts de projection et PDU, latence STOP et ressources RAM/pile si observables. Ne pas additionner les maxima R4-P et R4-Q pour conclure à la coexistence. Détecter et conserver les échecs ; STOP sûr sur anomalie, aucun seuil relâché.
5. Vérifier tests de non-régression V1 et R0-R4, sanitizers ciblés, cross-build ARM ; préserver snapshots historiques et journaux FRAM. Restaurer après probe le firmware initial M016 par méthode qualifiée et relecture intégrale Flash ; vérifier FRAM complète et partitions avant/après. Ne pas rembobiner des identités transactionnelles consommées ni effacer des preuves.

## Critères d'acceptation et limites
La mission peut être DONE dans son périmètre seulement si le fournisseur temps physique est justifié par des observations, les valeurs B3 réelles sont correctement datées/qualifiées, la cohérence et les budgets sont mesurés sous la charge réellement exercée, et l'état matériel final est prouvé. Si la physique du capteur ne permet pas de démontrer l'horodatage, conclure BLOCKED ou PARTIAL avec preuve et proposition ciblée ; ne pas remplacer l'horloge par une hypothèse. Distinguer acquisition+calcul+B3+loopback RAM de RS-485/UART électrique et du PC/SQLite/Web ; R4 global/R6 restent ouverts. Risque matériel non maîtrisé, état FRAM indéterminé, violation de budget, perte silencieuse ou identité inattendue : arrêter l'essai concerné et préserver les preuves.

## Livrables
Code minimal, tests et logs bruts, mesures horloge/échantillons/B3, empreintes des images, journal de Flash/FRAM et état final, documentation LIVE_R4R, état courant et roadmap factuels, commits/push main, DERNIER_RAPPORT.md publié et vérifié. Autonomie permanente autorisée : allow_commit/push/flash/debug true avec garde-fous AGENTS ; interventions physiques et décisions structurantes à l'utilisateur uniquement si indispensables. ChatGPT contrôlera indépendamment le rapport et les preuves.
