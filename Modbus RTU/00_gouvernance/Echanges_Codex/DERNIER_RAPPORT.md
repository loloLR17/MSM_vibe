---
mission_id: TR2-20261016-R4-COOPERATIVE-FRAM-FIFO-RT
status: IN_PROGRESS
base_ref: main
base_sha: 66c5e167b31cbc7baa302843c354760569d1c88d
initial_head: 178f3481e087110ef3f897f02bc8077b4b018028
result_sha: 12e7a1fe6cd6ddee64767bdfecc430d841d1f33d
created_at_utc: 2026-10-15T22:34:08Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4 — checkpoint logiciel et préflight avant qualification physique

**Mission en cours, pas de qualification physique annoncée.** Code coopératif
validé hôte/ARM, préflight cible et sauvegardes acquis ; préparation du probe
physique suivante, sans perdre les résultats déjà démontrés.

Code : `ae3d52f` — `R4 checkpoint: cooperative FRAM CRC SHA service with atomic R2 invariants and load proofs`.
[Architecture et limites](../../05_Firmware/LIVE_R4.md), [preuves](preuves/TR2-20261016-R4/README.md).

- 136tests hôte PASS,57,57s, cross-build ARM PASS ; ASan/UBSan4suites PASS,
  puis reprise ciblée UART renforcée PASS. V1, R1/R2 et métrologie R3-B conservés.
- Les1561oracles de coupure R2 sont réutilisés sans changer les assertions,
  maintenant avec quanta physiques, CRC, SHA et scans coopératifs. START/RENEW/
  STOP/APPLY laissent262144octets identiques à l'ancien chemin, sans format nouveau.
- Modèle indépendant FIFO26667XYZ/s+TS/8, FRAM2,5MHz et SPI3 4MHz : q32 fournit
  620,605s simulées/13776548XYZ/13renouvellements/38804réponses RTU, FIFOmax19,
  RXqueueMax8/overflow0. UART alterne FC03B0 et FC10max255octets refusé02 ;
  drain8événements/service. Contrôle négatif sans coopération déborde.
- Urgence pendant NVM : arrêt observé en272us modèle, erreur sticky ; panne FRAM
  arrêt indépendant. Pilote STM32 réel sous HALmock : six/deux positions d'erreur,
  CS fermé, option64octets/timeoutHAL1ms, timeoutV1 inchangé.

## Préflight réel et garde-fous

Utilisateur confirme le16/10local TR2-001 inchangé, alimentation/capteur/FRAM conformes.
ST-LINK003500463235510B37333439, UID004A003D/38425018/20333157 relu. Image actuelle
M016 stockage tentative2,62312octets SHA00ba4f5b6750f3c20b2bd18832c99eaf10ac257449df6df12a53f6500ed81e5d,
préfixe exact. Flash2Mio deux lectures identiques SHA
c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26.
FRAM262144octets deux lectures identiques SHA
d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce ; historique
[0,122880) génération12, LIVE[122880,245760) entièrement0, tail entièrement0.
Registres RCC/SPI1 et SystemCoreClock160MHz lus ; SPI1 prescaler64, sourceSYSCLK,
2,5MHz configurés déduits, pas fréquence électrique mesurée.

GDB qualifié, halt/lecture/detach sans reset, HOTPLUG ni flash. Lecture fraîche
FRAM via pilote existant et RAM de sauvegarde ; aucune écriture FRAM/SD ni émission
RS485 à ce point. Ancien firmware reprend sa boucle d'attente. Pas de modification
OptionBytes/RDP ni mass erase. Backups restent locaux, empreintes publiées.

## Reprise et limites

Préparer probe R4 isolé au WAIT, gardeUID/image et sauvegarde relue, aucune SD.
Vérifier à nouveau contenu de la zone LIVE avant usage ; preuve de propriété par
allocation normative disjointe, absence d'autre owner dans le code, contenu vide
et sauvegarde vérifiée, jamais par simple booléen permissif. Qualifier progressivement
coexistence, temps de quantum/STOP/FIFO/RTU et défauts ; restaurer le firmware hérité
si le successeur n'est pas qualifié, sans effacer l'historique ni les résultats LIVE.
Provider temps capteur calibré absent : B3 LIVE reste invalide, aucun temps SPI
substitué. Composition RTU2 et intake différé restant à prouver, pas de capability
LIVE annoncée. Les scopes coopératifs restent synchrones avec callbacks, non un
nouveau scheduler asynchrone : pas de réentrée du serveur RTU qui dispatcherait
une commande extérieure, et latence commande durable distincte de réponse cached.

Dépôt historique main82169af et autres worktrees préservés ; travail isolé
`MSM_vibe-R4-cooperative-fram`/`codex/r4-cooperative-fram`, push autorisé versmain.
Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 etf3b829e6188f491920edf74a5f9ed0f559d08b87
conservés ;2336fichiers gels/dettes/archives/mission contrôlés. R3-B archivée
intégralement depuis66c5e16. Journal actualisé, mission ChatGPT inchangée.
Publication/relecture de ce checkpoint en cours, puis poursuite physique autorisée.


## Checkpoint physique complémentaire —16/10/2026

Code supplémentaire0e5f495 (probe isolé), f870df0 (quantification HAL),12e7a1f
(premier-mot distinct, garde gap inchangé). Flash standard qualifié, image vérifiée
par CLI et lecture GDB complète ; reset logiciel après programmation selon M013-D.
Aucun OptionBytes/mass erase. Essai1 : timeout1ms réel après256octets, sans write.
Essai2 après correction : SHA complet FRAM conforme, puis aucune donnée avant5ms,
arrêt sûr. Essai3 après politique premier-mot20ms :180826XYZ/22604TS, premier mot
11,043ms, FIFOmax18, gap601us, read132us/service461us/arrêt235us observés.
Le halt d'observation est survenu pendant le contrôle FRAM final à l'arrêt et a
causé un timeout ; ne pas déclarer RUN complet PASS. FRAM intégrale ensuite relue
à l'arrêt avec le pilote existant :262144octets inchangés et SHA initial exact.

R1/R2 viennent d'être déclenchés après reset contrôlé, sans halte pendant acquisition ;
aucun résultat de cette tranche encore revendiqué. Garder M016 sauvegardé pour
restauration.136/136tests hôte et ARM PASS après corrections, ASan/UBSan3suites PASS.
Code canonique BIN identique à ae3d52f ; mise à jour cartographie/LOC/ressources
et détails dans LIVE_R4.md. Aucun trafic RS485 ni SD, B3/protocole2 nonqualifiés.
Publication de ce checkpoint puis poursuite des mesures/restauration/rapport final.
