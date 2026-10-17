---
mission_id: TR2-20261017-R5AFIX-B3-WIRE-ORDER
status: READY
base_ref: main
base_sha: e100770474cf4d43a73f2af2e6e02be91e2cd75c
created_at_utc: 2026-10-17T11:31:13.968Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R5-AFIX — Correction de conformité B3 LIVE STM32 et déblocage supervision R5-A

## Constat indépendant et décision ChatGPT

R5-A a publié un rapport `BLOCKED` malgré une tranche PC fonctionnelle : `1efc4f1500f8401a5996951f8798a8c977019ec2`, 551 tests .NET PASS, 201 tests Service finaux, migration SQLite 7 additive, API `/api/v2/live`, page `/live.html`, replay des ADU avec quarantaine des B3 invalides. Mission/rapport R5-A archivé à l'identique sous `archives/TR2-20261017-R5A-PC-LIVE-PROFILE-SQLITE-WEB/`. Ne pas supprimer les acquis, les archives, ni masquer ce BLOCKED historique.

Le mapping **normatif** `Modbus RTU/02_Validation/LIVE_R0/mapping_live.csv` et la V1 héritée prescrivent `3002=B3_ALARM_FLAGS`, `3003=B3_SEVERITY_GLOBAL`. Pourtant `Modbus RTU/05_Firmware/src/application/live_metrology.c`, lignes 203–204 à la référence R5-A, écrit respectivement la sévérité dans `registers[2]` et les alarmes dans `registers[3]`. Les vrais ADU R4-U/V CRC-conformes montrent `3002=3,3003=240`, valeur de sévérité réservée. Codex a recensé 2068 captures B3 ainsi non conformes, consignées dans `preuves/TR2-20261017-R5A/wire-mismatch.json`. **Décision ChatGPT : corriger le firmware pour rejoindre la norme**, jamais inverser le mapping gelé ni tolérer silencieusement 240 côté PC.

Cette mission est une tranche de remédiation **strictement ciblée** : correctif minimal B3 + oracles indépendants des offsets + validation normative en simulation puis, si justifié et matériellement sûr, nouvelle capture RS-485 réelle avec les registres corrigés, puis reprise de la qualification du replay PC. Pas de refonte R0/B5, du moteur R2, de la supervision ou du runtime FIFO.

## Séquence d'exécution autonome

1. Consulter `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, état courant, roadmap, `PROFIL_LIVE.md`, mapping `mapping_live.csv`, règles B3 V1/V1.1, rapports et preuves R4-T/U/V et R5-A. Vérifier HEAD réel de `main`, origine exacte des index B3 0..47, et tous les chemins de projection, notamment `live_metrology_project_b3`, `modbus_project_b3`, tests, UART et décodeur PC. Rechercher d'autres inversions adjacentes par **revue de table ciblée**, sans étendre la mission sans anomalie démontrée.
2. Corriger la projection des champs `B3_ALARM_FLAGS` et `B3_SEVERITY_GLOBAL` **aux offsets normatifs**. Conserver l'intégralité de la signification et du masque d'alarmes (y compris latch/exceed), et de la sévérité (0..5 selon contrat). Ajouter des oracles testant directement les ADU/mots 3002/3003 **indépendamment de l'encodeur testé**, avec au moins alarmes nulles, alarme active, dépassement, UTC absente, B3 invalid, FRESH, HELD après STOP, latch et erreur. Vérifier qu'aucune valeur de sévérité réservée ne peut être émise ; ne pas permuter à nouveau les mots dans les tests pour les rendre verts. Conserver les fixtures historiques corrompues comme preuves négatives.
3. Compiler et exécuter les suites hôte affectées, non-régression firmware V1/protocole1, R0/B3, profils LIVE, sanitizers ciblés, cross-build ARM canonique et probe dédié NON-PRODUCTION ; exécuter les tests .NET de lecture/SQLite/Web pertinents. Ne pas présenter le vieux replay de captures R4-U/V comme redevenu conforme : il doit rester refusé/quarantainé ; créer une fixture positive normative indépendante et, idéalement, une capture physique **nouvelle** du firmware corrigé.
4. Si l'essai physique apporte une preuve nécessaire, vérifier TR2-001 NUCLEO-U575ZI-Q, ST-LINK `003500463235510B37333439`, UID `004A003D/38425018/20333157`, T1000, COM4 FTDI réellement affecté, alimentation/capteur arrêté, backups Flash/FRAM complets et récupérables. Référence matérielle **dernière rapportée R4-V**, à confirmer fraîchement : M016 CommissioningReadOnly WAIT, Flash 2097152 octets SHA256 `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`, FRAM 262144 octets SHA256 `70e273972a83e61283a6e572db3187af002fd97ba8de63f8c709d3e30af73587`, journal62/root75/epoch5/run9/config3gen3. Ne restaurer **aucune FRAM historique**, ne réappliquer aucune retraite/RENEW R4-T, ne recycler aucune clé/run/epoch. Privilégier un probe **readonly**, sans nouvelles transactions ni FRAM writes, qui valide B3 statique invalid/HELD et, si nécessaire, une fenêtre réelle V/F en acquisition contrôlée non autoritative seulement si elle est explicitement marquée diagnostic. Pour annoncer B3 V/F autoritatif, respecter les vraies préconditions R2, admission nouvelle identité, garde FIFO/FRAM et budget ; aucune autorité inventée. Minimiser les écritures et la consommation des identités, et documenter la portée exacte de l'essai. Un essai sur banc indisponible reste `PARTIAL`, jamais une prétention de preuve physique.
5. Tester un ADU RS-485 FC03 / B3 réel après correction, CRC/longueur, 3002 alarmes/3003 sévérité, status V/F/UTC/HELD et cohérence des valeurs RMS/peak si échantillonnées ; rapprocher indépendamment des bytes, de la projection STM32 et du décodeur PC. Si une fenêtre réellement valide sur fil n'est pas démontrable sans élargissement du contrat, arrêter à la preuve disponible et documenter exactement la limite. Les budgets work ≤1000 µs, service ≤2500 µs, gap FIFO ≤5000 µs restent inchangés ; STOP sûr sur anomalie. Toute défaillance persistante hors ce défaut exige `BLOCKED`, pas de workaround PC dissimulé.
6. Si flash de qualification, restaurer M016 à l'identique et recontrôler par relecture complète Flash, FRAM complète sans rollback, partitions historique/tail, capteur arrêté, COM4/GDB détachés ; garder toutes les traces initiales et finales. Pas de SD, HP, câblage, mass erase, Option Bytes/RDP, service produit LIVE persistant ni modification de la norme. Préserver worktrees/stashes/backups et contrôler risques matériels ; escalader seulement risque incontrôlé, opération destructive inhabituelle ou changement structurant.

## Livraison et acceptation

Livrer note technique `LIVE_R5AFIX.md`, correction source minimale, tests d'offsets réellement indépendants, rapports builds/sanitizers/ARM/.NET, oracles positifs et anciens négatifs, ADU physique si réalisé, SHA firmware et état final exact, analyse des éventuelles autres incohérences, preuves traçables. Exiger : **(A)** mapping 3002/3003 conforme au contrat, **(B)** pas de régression de qualité/alarme/severity ou du V1, **(C)** voie PC R5-A capable d'accepter la nouvelle fixture conforme et de rejeter les anciennes, **(D)** aucune fausse prétention de qualification live physique/produit, **(E)** préservation matérielle et journal. `DONE` pour correctif/tests démontrés dans ce périmètre ; `BLOCKED` si correction/validation empêchées et `IN_PROGRESS` pour checkpoint sûr. R5-A n'est pas réécrite rétroactivement DONE. Après preuve et revue indépendante ChatGPT, poursuivre R5-B (transport PC COM4 → SQLite → Web) sous contrat séparé.

Conformément à `AGENTS.md`, permissions : `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`. Codex opère et publie code/preuves/journal factuel/rapport `DERNIER_RAPPORT.md` sur `main` puis vérifie à distance. ChatGPT contrôle indépendamment et garde la propriété de la gouvernance. Ne pas demander de validations répétitives pour les opérations déjà autorisées.
