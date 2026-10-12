---
mission_id: TR2-20261011-M014-G-COMMISSIONING
status: BLOCKED
base_ref: main
base_sha: 7f9e57b87527a0864a6e4b0eae468b6c940a95c7
initial_head: 2bbf9d66d3cb62fc30acbdc5f01de1ce14f64fbe
result_sha: 1507dcbe3b68dd1b6a4579d78a53b5072eb9cf6e
created_at_utc: 2026-10-11T22:54:58.736509+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-G — Rapport intermédiaire de conservation et reprise

Publication demandée prioritairement par l'utilisateur. **M014 IN PROGRESS, non clôturée.** Le statut BLOCKED signifie critères physiques et relecture distante non encore acquis ; aucun blocage matériel actuel ni validation intermédiaire utilisateur nécessaire. Mission active inchangée. Ce rapport ne présente pas la tranche logicielle comme une qualification physique.

## Lots acquis

1. Inspection Git, contrat/protocole/journal/roadmap, audit des collisions ; registre versionné RESERVED puis ASSIGNED. DeviceId 1, unit 1, serial TR2-000001, fabricant MSM, nom MSM-TR2-000001. UID96 réellement lu : `004A003D 38425018 20333157`. Attribution acquise, **identité persistante NON PROVISIONNÉE**.
2. Préservation diagnostic E BIN/ELF et Flash STM32 complète 2 Mio, double sauvegarde identique ; RAM conservée. ST-LINK `003500463235510B37333439`, STM32U575/U585, Device ID 0x482. Flash initial SHA256 `0ebb31d951a24bd0b609d38f8180ebe1e2976072a7f853a3d116799a48916a7b`. Diagnostic E BIN SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`.
3. Probe isolé en lecture seule, deux flashs standards vérifiés puis reset logiciel GDB suivant procédure qualifiée ; **aucun HOTPLUG**, aucune émission Modbus, aucune écriture FRAM ou de données SD. FRAM RDID `04 7F 48 03`, sauvegarde intégrale 262144 octets et deuxième lecture identique, SHA256 `59d32d12ebe26db36d8552ae522b2704632c6ccfde9e1a57200c3d25c77b4737`. Image physique V1 valide génération 10, ancienne génération 9 ; préfixe métier contenant marqueur 0xAC mais aucune configuration valide. Média non EMPTY, aucun format autorisé.
4. Capteur IIS3DWB WHO_AM_I réellement lu `0x7B`. SD initialement inaccessible (timeout CMD55), puis utilisateur a indiqué insertion ; observation ultérieure : 999743488 blocs de 512 octets, soit 511868665856 octets, CID/CSD conservés. Lecture des 8192 premiers octets seulement ; MBR partition type 0x07, LBA 65536 (32 Mio), jusqu'à la fin de carte. **Partition existante intégralement préservée ; sauvegarde intégrale de la carte non réalisée, aucune écriture SD permise.** Fenêtre en lecture seule [8 Mio,24 Mio) seulement envisagée dans l'espace hors partition : pas encore sauvegardée, affectée ou qualifiée.
5. Inspecteur métier strict de tous domaines historiques ; contrôle d'attribution UID96/DeviceId ; préparation explicite de configuration dans un slot véritablement vide, protection du préfixe et des générations. Callback réel de disponibilité stockage utilisé par B1 ; dépendance obligatoire en production, aucune disponibilité inventée. Interface SDMMC en lecture seule refusant les écritures. Ces briques sont **validées logiciel**, pas encore composées en firmware production physique.
6. Outil PC `--commissioning-check` : OFFLINE par défaut, B0 attendu explicite, maximum neuf FC03, zéro retry, trois B1 avant/après, timeout prévu puis fermeture/invalidation/reconnexion explicites. Valeur et date antérieures conservées indisponibles ; B0 seul ne réactive pas les données. **Tests simulés uniquement ; aucune ouverture COM7 ni campagne physique G exécutée.**

## Validation conservée, non répétée sans motif

- Firmware (depuis `Modbus RTU/05_Firmware`) : `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`, **113/113 tests et cross-build réussis**, source `69d523bf81baf92b0849d63d750c3aca75ee17ba`. Validation complète justifiée par nouvelles interfaces et cœur fonctionnel. Profile probe capteur/SD cross-build réussi ; source et SHA BIN/ELF dans `sensor-probe-artifacts.json`.
- PC : build service zéro warning/erreur, 15 tests ciblés réussis. Puis `dotnet test 'Modbus RTU/Supervision TR2/TR2.Supervision.sln' --no-restore`, **539/539 réussis, retour 0** sur les nouveaux fichiers maintenant committés à `1507dcbe3b68dd1b6a4579d78a53b5072eb9cf6e`. Suite complète justifiée par nouvelle orchestration et dispatch service ; aucun nouveau changement ensuite.
- Échecs initiaux conservés et corrigés : wrappers de régions relatives, contrôle strict des allocateurs, initialisations positionnelles des nouvelles dépendances, arguments de tests PC conformes au parser réel (`--timeout-ms`, OFFLINE sans log). Assertions non affaiblies. Deux erreurs de scripts GDB corrigées (type absent dans ELF E ; syntaxe d'adresses dump), sans écriture NVM.
- Publication et relecture distante : en attente au moment de ce premier commit documentaire ; complément après push normal.

## État cible et limites

Le firmware présent est **le probe isolé lecture seule**, BIN 45348 octets dans `build-readonly-sensor`, source firmware `69d523bf81baf92b0849d63d750c3aca75ee17ba`. Il reste en attente ; ce n'est ni le diagnostic E ni un serveur production. Pas de FRAM provisionnée, pas de B0/B1 production physique, pas de polling ni perte/reprise physiques démontrés. READY/stockage/acquisition ne sont pas qualifiés. M013 conserve ses preuves historiques limitées ; turnaround DE/TC, timings, contention et robustesse industrielle restent ouverts. L'essai unique M014-C utilisateur et son SHA ne prouvent pas que le journal HP intégral ait été vérifié.

## Point de reprise opératoire

1. Relire ce rapport, état Git, contrat et journal ; conserver les deux stashes et preuves. Ne pas relancer les validations ci-dessus sur code inchangé.
2. Construire un manifeste authoritative registre/UID/B0/versions/configuration/géométrie/backups/générations/rollback et génération vérifiée de l'identité. La fenêtre SD proposée n'est pas attribuée : lecture/sauvegarde vérifiée complète de la seule fenêtre envisagée si nécessaire ; **aucune écriture, aucun accès hors bornes, aucun risque sur la partition**. Si ownership incertain, refuser l'allocation et poursuivre seulement les travaux indépendants.
3. Terminer strong binding STM32 : vraies dépendances durables FRAM, capteur, SD, transport, disponibilité réelle. Image production distincte du harness ; aucun fallback fixture. Tester seulement les nouvelles modifications.
4. Avant toute écriture FRAM, revalider UID/backup/ownership/préfixe/plan. Plan préparatoire envisagé : config dans slot vide génération 10→11 puis migration F V1→V2 et identité 11→12, deux transactions explicites hors boot normal, arrêt sans retry si divergence. **Non exécuté, manifeste et image nécessaires avant autorisation opératoire concrète.** Aucun downgrade vers ancienne V1 après migration.
5. Produire image identifiée et restaurable, préflight GDB sûr, provisioning explicite puis boot production/readback. Préparer paquet HP vérifié OFFLINE, maîtrise exclusive, logs neufs, budget requêtes fixé et aucun GDB pendant trafic. La fenêtre de perte côté cible n'est pas implémentée : ne pas exécuter l'outil physique avant préconditions effectives.
6. Exécuter uniquement lorsque GO réel les B0/B1/polling/perte/reprise autorisés par G, jamais rejouer C/I. Publier observations et compteurs ; M014 DONE seulement sur preuves physiques. M015 reste à faire.

## Traçabilité, Git et restauration

Preuves locales : `/home/lolo/dev/msm/reports/TR2-mission014-G-20261011T220027Z` ; index SHA256 `proof-index-intermediate.json`, logs de validation, scripts/sorties GDB, commandes flash, sauvegardes et JSON observations. Ces fichiers volumineux restent hors Git ; ce rapport ne garantit pas leur disponibilité sur un autre poste. Le registre versionné est `00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json`.

Commits logiciels acquis : `db373cf` attribution RESERVED ; `8ddf18e` association UID/probe ; `69d523b` inspecteur/stockage ; `1507dcbe3b68dd1b6a4579d78a53b5072eb9cf6e` outil PC borné. Main initialement propre après ff-only de `7f9e57b` vers `2bbf9d6`. Deux stashes historiques conservés, aucun reset/clean/rebase/force push. Archives F copiées exactement depuis `7f9e57b`. Journal et roadmap actualisés par cette publication. Permissions exercées : commit, flash standard et debug ; push normal et relecture à consigner après exécution.

Restauration fonctionnelle possible du diagnostic E via artefact préservé et procédure standard qualifiée ; ce n'est pas une autorisation de rollback V1 après futur provisioning. Aucun effacement massif, Option Bytes, changement de boot persistant, modification de configuration Codex ou intervention physique automatisée.

## Publication intermédiaire vérifiée

Push normal sur main réussi, SHA distant `895ae413fe130697a55a1c6561885b63fac6ef58`. Relecture intégrale UTF-8 et comparaison byte-à-byte des 29 livrables depuis `origin/main` récupéré : toutes identiques ; preuve locale `remote-readback-intermediate.json`. Archives F exactes contrôlées contre 7f9e57b. Manifeste `COMMISSIONING_M014_G.json` publié au statut AUDIT_ONLY_NOT_PROVISIONING_AUTHORITY : observations acquises seulement, aucune autorisation opérationnelle de provisioning. État Git propre après cette publication, deux stashes historiques inchangés. M014 toujours IN PROGRESS ; la seule relecture distante ne satisfait pas les critères physiques.

## Contrôle de conservation complémentaire et limites de reprise

Contrôle hôte uniquement des preuves déjà acquises : hashes/taille des BIN/ELF/MAP du probe capteur conformes à `sensor-probe-artifacts.json`, dump Flash sauvegardé conforme au BIN et octets après 0x0800C000 inchangés par rapport au backup initial, deux backups FRAM identiques, MBR et quatre entrées de partition contrôlés. Sortie `conservation-final-host-check.json`. Ce contrôle n'a lancé ni test, ni session cible, ni lecture SD supplémentaire. SHA BIN actuellement documenté `12bd87d731acd1c3d335ca7e965672f34a10293d37aa48a9eca210a6bfc1291c`, ELF `ae459e0df07e7ba9df6ac49e610ef82d72b881f65bb4f997e312e721bcc91064`. Manifeste d'audit complété avec artefacts et backups vérifiés.

Pour la reprise, les API existantes sont sous `Modbus RTU/05_Firmware/` : `include/tr2/application/production_commissioning.h`, `include/tr2/application/persistent_identity.h`, `platform/stm32/stm32_production_runtime.h`. La fonction `stm32_production_binding_acquire` reste weak et retourne indisponible par défaut ; il faut une composition forte avant de prétendre au serveur production. Ne pas reprendre par un flash du harness général, qui comporte des écritures de qualification.

Aucune version hardware/firmware ni capacité B0 de production n'est encore attribuée dans ce manifeste. Aucun timeout planifié côté cible n'est encore implémenté. Le programme PC ne doit donc pas être lancé avec `--execute` à ce stade. La présence de la SD, les tests et l'attribution ne constituent pas le GO physique. La séquence de reprise demeure un travail logiciel autorisé, pas une attente d'approbation utilisateur.
