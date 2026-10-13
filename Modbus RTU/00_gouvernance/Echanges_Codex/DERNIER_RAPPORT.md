---
mission_id: TR2-20261013-A04-02-FRAM-MODBUS
status: IN_PROGRESS
base_ref: main
base_sha: ba6f34c4e536f430be74b2e6c27f6c8c9cb873e2
initial_head: 2cbb9bbc88e6ee3a9392b662c23a33b6b64e3d34
result_sha: 8bcb8436822ce7444074865617cb3696f877a6ad
created_at_utc: 2026-10-13T15:40:54.641816+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# A04-02 — checkpoint logiciel avant essai physique

**IN_PROGRESS, sans blocage.** Correction logicielle et validation complète acquises ; essai physique de coexistence restant. Code `8bcb8436822ce7444074865617cb3696f877a6ad`, commit `A04-02: guard synchronous B5 FRAM I/O during cooperative capture`. Worktree isolé `MSM_vibe-A04-02`, branche `codex/a04-02-fram-modbus`. Base du contrat vérifiée ancêtre du HEAD initial distant. Dépôt historique/stashes/worktrees préservés ; protections A04-01 intactes. Rapport A04-01 déjà archivé byte-identique par ChatGPT, non écrasé. Dette non clôturée.

## Tranche réalisée

B5 est préparé uniquement pour les requêtes mappées FC03/FC10 qui le visent ; aucun scan inutile pour B0/B1/B3 et autres blocs ou requêtes invalides. Contexte reconstruit à chaque requête, sans cache de journal. Un guard d'admission injecté par la composition cumulative retire les autorités B5 lors de configuration, armement en attente, capture et cleanup non vérifié. L'adaptateur PDU existant renvoie **exception04**, avant snapshot ou mutation du mailbox : aucun ACK de succès, identité admise, file différée ni modification persistante. Après cleanup vérifié, journal frais et chemins transactionnels existants disponibles. Erreur de snapshot => B5 retiré/exception04 à la requête ; init/start gardent le refus fail-fast. Aucun formatage ni modification des écritures/durabilité/recovery. Hors périmètre : A04-03/M016 et boot synchrone A04-07.

Instrumentation DWT passive : max service Modbus global/en phase protégée, max gap entre polls FIFO pendant démarrage/capture/cleanup ; compteurs refus/fautes snapshot. Rien n'impose un RTOS ou nouveau code Modbus.

## Tests et preuves acquis

`STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`, retour0 : **126/126 tests hôte** et cross-build cumulatif réussi. Un test supplémentaire de coût I/O compile réellement RTU/PDU, dispatcher/journal et moteur FIFO ; média host réel enveloppé avec coût explicite 2,5MHz +20µs/transaction, commit51914*2 octets, producteur30words/ms, profondeur512 et coût SPI4MHz. **Simulation**, pas mesure physique.

Résultat : scan B5 hors capture **131584µs** ; capture protégée32768XYZ+4096TS, niveau max212, OVR0,17 refus ; zéro lecture/écriture/commit média induit par les B0/B1/B3/B5/FC10 testés en phase protégée. Même transaction après cleanup admise, retry sans nouvelle écriture/commit, collision04, défaut04 sans stale, recovery réel puis dernier résultat disponible. Contrôle négatif sans guard dans le fixture : OVR1 et FIFO INVALID, prouvant que le modèle ne masque pas le blocage.

Test RTU existant renforcé : B3 avec fixture host de supervision et vrai guard de séquence, fraîcheur/retrait ; erreur snapshot auparavant silencieuse désormais exception04 avec CRC ; callbacks retirés. Les oracles existants de persistance/atomicité restent dans la suite complète, aucun test désactivé/affaibli. Échecs initiaux des nouveaux fixtures (nom de type recovery et cohérence de génération B3) analysés avec sources réelles et corrigés, logs conservés ; aucun bypass des autorités.

[Preuves logicielles](preuves/TR2-20261013-A04-02/manifest.json), commandes et sorties complètes. BIN169948 octets SHA-256 **b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b**, ELF SHA **3a1310a77ba379bbc513dd57d0fbfdb7cab89aac50ab2e67400abddc903f40a1**. Runtime modifié (+568octetsBIN), donc pas de revendication d'identité golden ou de validation physique. Cumulatif SPI4/Debug/TX/observer/FIFO et autorité M014-H ; ELF sans SD write/bringup. Images locales préservées dans `/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/build-artifacts/`.

## Matériel et reprise

Listing ST-LINK uniquement : SN003500463235510B37333439, NUCLEO-U575ZI-Q ; SSH HP Probe réussi, identité LAPTOP-3F6J7STJ. **Pas encore de flash/debug/transmission Modbus** ; état actuel MCU/médias non relu. Dernier firmware attendu cumulatif M015-V, pas restauration M014-H automatique. Permissions matérielles true restent conditionnées par identité UID/Flash, procédure qualifiée et préservation.

Suite autonome : préflight MCU/HP sans émission, vérifier image/profil/backup ; flash standard utile et lecture de contrôle via GDB qualifié sans HOTPLUG ; baseline B5 puis une campagne bornée de reads B0/B1/B3/B5 sans debugger pendant trafic ; confirmer cleanup, compteursDWT/FIFO/UART/CRC, FRAM/gen12 et limites ; publier rapport final/relecture distante. Aucune écriture FRAM métier physique prévue, aucun accès SD/Option Bytes/mass erase. Les coûts boot et post-cleanup restent explicitement hors garantie FIFO. Le refus temporaire B5 est une politique de qualification ; M016 devra spécifier son propre ordonnancement métier/STOP, sans être engagé ici.

Publication de ce checkpoint et relecture distante à effectuer ; poursuite physique après protection des acquis. Aucun blocage extérieur identifié à ce stade.
