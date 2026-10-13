---
mission_id: TR2-20261013-M016-GPIO-DAT123-UNIT
status: READY
base_ref: main
base_sha: 025e46367a2185401d47866f5cfe790936f1ec03
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Diagnostic GPIO statique indépendant DAT1 DAT2 DAT3

## Contexte et décision
L'utilisateur confirme le second banc NUCLEO-U575ZI-Q / Adafruit 4682 câblé par fils Dupont et dispose d'un multimètre Brymen BM869s. La lecture SDMMC 1 bit fonctionne, le 4 bits échoue CRC, fait connu avant la dernière mission. Il accepte les tests GPIO unitaires. **Ne pas refaire les essais SDMMC ni modifier empiriquement le portage ST** dans cette tranche.

## Démarrage
Lire HEAD main réel, AGENTS.md, gouvernance, dernier rapport BLOCKED, pinout H3h-B, schémas et manuel ST de la Nucleo, et exemples GPIO officiels ST du STM32U575 / NUCLEO-U575ZI-Q. Préserver preuves, stashes et worktrees. Vérifier qu'aucune autre session n'utilise la sonde.

## Mission technique
1. Identifier sans ambiguïté **la seconde** Nucleo (ST-LINK historique 0032001F3235510C37333439 ; UID 001B0046/38425018/20333157) avant flash ; refuser la principale. Confirmer le brochage physique CN12-26 PB15/DAT1, CN12-31 PB3/DAT2, CN12-27 PB4/DAT3, masse commune et fonctions partagées selon révision.
2. Rechercher et documenter l'exemple GPIO officiel ST le plus proche, version, licence et adaptations minimales. Créer un firmware GPIO **isolé** dérivé de cette référence, sans SDMMC actif, DMA, Modbus, FRAM, capteur ou écriture microSD. Ne pas prétendre que l'adaptation est validée constructeur.
3. **Avant toute commutation**, exiger carte microSD de test retirée physiquement par l'utilisateur, alimentation coupée pour retrait, et confirmer absence d'autre charge/pilotage sur les lignes. Ne jamais piloter en sortie un signal auquel une carte SD insérée pourrait répondre. Les gestes physiques restent à l'utilisateur.
4. Implémenter une séquence de sortie GPIO lente, bornée, sans ambiguïté : PB15, PB3, PB4 en sorties push-pull 3,3 V, les autres à niveau bas, puis une seule ligne haute à la fois ; phase tout-bas, phase DAT1 haut, tout-bas, DAT2 haut, tout-bas, DAT3 haut, tout-bas, avec pauses longues (par exemple 15–30 s) permettant mesures au BM869s. Préférer un déclenchement/avancement par debug contrôlé ou séquence stable observable ; consigner l'état réel. Ne pas produire d'impulsions rapides, ne pas modifier les autres GPIO, prévoir un arrêt sûr et aucun flash principal.
5. Codex compile, flashe et débogue de façon autonome la **seconde** Nucleo identifiée ; vérifie les états des registres GPIO et la correspondance attendue. Il fournit à l'utilisateur un protocole concret de mesure **tension DC** : pointe noire sur GND breakout, pointe rouge successivement sur D1, DAT2, D3, et grille des tensions attendues (bas proche 0 V, haut proche 3,3 V). Le BM869s ne doit **jamais** être mis en mode ohmmètre/continuité sur un circuit alimenté. Mesures physiques effectuées uniquement par l'utilisateur. En cas de risque de court-circuit avec pointes de touche, proposer sondes adaptées et arrêt.
6. Si les lignes ne sont pas libres ou l'état du câblage incertain, **ne pas flasher/commuter** et publier un rapport BLOCKED avec la raison et la vérification physique requise. Ne pas considérer les lectures GPIO au niveau MCU comme preuve de continuité jusqu'au breakout. Aucun test d'injection directe 3,3 V sur DATx.

## Critères d'acceptation
- Firmware test autonome traçable, cross-build et préflight sonde, états GPIO contrôlés et preuve de non-activation SDMMC.
- Tableau mesurable des états attendus DAT1/DAT2/DAT3, méthode de mesure BM869s et collecte des résultats physiques de l'utilisateur ; distinguer contrôles logiciels et continuité réellement mesurée.
- Aucun changement au TR2 principal, spécifications V1, stockage ou carte SD. Autorisations commit/push/flash/debug true. Rapport DERNIER_RAPPORT.md publié sur main avec identités, SHA, preuves, état du banc, et attente éventuelle des mesures humaines.

## Garde-fous
Flash/debug seulement seconde Nucleo identifiée, sans mass erase, Option Bytes/RDP. SD de test retirée avant sortie GPIO. Pas d'injection de tension externe. Aucun geste physique automatique. Arrêter en cas de risque matériel non maîtrisé.

## Sortie
Après rapport, ChatGPT examinera les mesures et déterminera si défaut de connexion, de ligne partagée, ou investigation dynamique nécessaire.
