---
mission_id: TR2-20261010-DEV-RTU-PRODUCTION-005
status: BLOCKED
base_ref: main
base_sha: ff19cf40a017233b967b95fea3365bad0f631e11
initial_head: 966737ee122fb4fb04489496691667f659607a23
result_sha: 66edf2d17c5183003b3e5fcd7b5fe29a0bde4784
created_at_utc: 2026-10-10T15:57:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission 005 — runtime durable, B5 et préparation RS-485

**Rapport provisoire : BLOCKED porte uniquement sur la publication/relecture distante encore à obtenir. Développement et validations logiciels terminés.** `VALIDÉ hôte` / `COMPILÉ STM32` / `PRÉPARÉ matériel` / `NON QUALIFIÉ physique`. Le firmware conserve son diagnostic RX par défaut, sans émission applicative ou boot de production activé implicitement.

## Références et préservation

Clone canonique Debian `/home/lolo/dev/msm/projects/MSM_vibe`, SDK CubeU5 v1.9.0 Linux. HEAD initial propre main `966737ee122fb4fb04489496691667f659607a23`. Fetch/pull fast-forward de la mission `ee95b8e3c0359f79cd6d208aa2ce0e9fae3d5b2b`, base du contrat ancêtre vérifiée.

Pendant le contrôle avant commit, GitHub a reçu le rectificatif d'inventaire `463d69a8a6a2a36fe8a88f2905c6cad3ff5c3fac`. Après inspection de son diff (journal uniquement), sauvegarde 005 dans un stash conservé, fast-forward sur arbre propre puis restauration délibérée des livrables et intégration du journal corrigé. Chaque fichier logiciel restauré a été comparé octet pour octet à la sauvegarde du code évalué. Aucun merge/rebase/reset, aucune divergence résolue artificiellement. Stash historique 002 conservé, désormais derrière la sauvegarde 005. Mission READY inchangée.

Commit code/tests/documentation/journal : `66edf2d17c5183003b3e5fcd7b5fe29a0bde4784` — `Firmware: compose owned production runtime and durable B5 dispatch`, parent `463d69a8a6a2a36fe8a88f2905c6cad3ff5c3fac`. 23 fichiers dans le périmètre. Archive 004 créée sans écraser d'archive : mission copiée depuis son SHA publié, rapport précédent copié intégralement sans réécriture.

## Code réellement livré

- `include/tr2/application/production_application.h` et `src/application/production_application.c` : propriétaire durable en RAM du SystemRuntime, des descripteurs d'interface/environnement et du dispatcher. Les contextes matériels/backends et capacités optionnelles restent explicitement empruntés. Boot unique, recovery réel et readiness avant binding, sans format ni retry automatique. B4 optionnel possède staging/workflow/adapter contre le ConfigurationStore de ce runtime, avec callback explicite d'allocation des métadonnées ; aucun compteur persistent inventé.
- `platform/stm32/stm32_production_runtime.{h,c}` et `stm32_modbus_application.{h,c}` : runtime statique distinct des fixtures de qualification ; vraie composition des adapters FRAM, média transactionnel/candidat, IIS3DWB, horloges/reset/continuité et application. Géométrie/SPI/CS/backend de campagnes/provisionnement fournis explicitement ; FRAM EMPTY/non-VALID refuse, aucun format implicite. Le port faible `stm32_production_binding_acquire` renvoie NOT_AVAILABLE **avant** préparation ; aucune sortie DE//RE ajoutée et aucune dépendance absente simulée.
- `system_command_dispatcher.{h,c}` : lookup/retry/collision avant redispatch, validation métier après réservation durable des nouvelles identités valides, dispatch vers les services/exécuteurs existants, publication de résultats réellement journalisés. Refus fonctionnel ID0 → 14 sans identité persistante, annulation V1 consommée → 15 sans annuler/terminaliser la transaction métier, concurrence → 13 sans admission supplémentaire. Last durable préservé, y compris lors du rejeu d'une ancienne transaction. La collision produit une erreur d'infrastructure/exception 04 dans ce binding ; ce choix ne définit pas une nouvelle règle normative de collision V1.
- `command_apply_configuration` / `command_synchronize_time` : nouvelles entrées `*_execute_bound` utilisent l'autorité journal du moteur, dont le backend borné réel ; wrappers historiques conservés avec leur contrat. APPLY : recalcul CRC, refus 4 si configuration invalide, 20 si absente, 5 si acquisition active ; activation réelle via workflow et ConfigurationStore. SYNC : staging B2 réel, journal/time history et source centrale Modbus 1, paramètres B5 non réinterprétés comme source. Politique de paramètres existante préservée.
- Codes 3/4 et 5..10 : raccordement des chemins SystemRuntime existants. Sans autorité SELFTEST/RESET, exception 04 avant réservation ; code11 sans service de statistiques → 04 avant réservation. Les commandes indisponibles ne fabriquent pas un succès ou une exécution.
- `modbus_system_server`, `b5_projection`, `pdu_server` : port de snapshot du dispatcher et consommation des annulations même sans submit ; vue courante ID0 visible sans fausse terminaison historique. FC16 fonctionnellement refusé reste acquitté si le refus a réellement été traité. Les erreurs de persistance restent des échecs ; lecture de B5 refuse après recovery_required au lieu de publier un succès non durable.
- CMake hôte/ARM et `tests/integration/test_production_commands.c` : raccordements compilés et scénario utilisant SystemRuntime, journal borné, vrai PDU/RTU et adapters existants.

## Validations effectivement exécutées

Validation complète finale, code retour **0** :

```bash
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  bash ./tr2_validate.sh
```

**104/104 tests hôte réussis**, compilation et link Cortex-M33 STM32U575 réussis. Validation complète justifiée par les changements du cœur et des interfaces partagées. Quatre avertissements newlib `_close`, `_lseek`, `_read`, `_write` non implémentés, déjà présents ; aucune erreur finale. Taille ARM : text 157 604, data 124, bss 128 808 octets. Pas de mesure de charge ou de marge dynamique sur MCU.

Preuves supplémentaires ciblées réellement obtenues via `cmake --build ... --target test_production_commands` puis `ctest --test-dir ... -R production_commands --output-on-failure` : requêtes RTU/PDU et lecture B5, invalid ID0 y compris code nul, refus de paramètres persisté, inconnu, retries/collision sans double effet, synchronisation réelle, capacités absentes, configuration activée/CRC invalide, maintenance, refus start sans configuration et stop sans acquisition, annulation seule et concurrence sur transaction réservée, reboot/configuration/refus restitués et historique préservé. Injection de panne RESERVED : aucun dispatch ; panne COMPLETED après REFRESH : transaction non finalisée, recovery_required, pas de vue succès et snapshot refusé jusqu'à récupération explicite.

SHA256 des artefacts construits, conservés dans les répertoires de build ignorés :

- BIN : `115b8fedd062af4a465893514bb331f9c323a14d1d9ab48f23466be0e42ba8b4`.
- ELF : `e95be47518151d9b42545a3427b0b40e54b5dc48f105bcfcd721f5607c61b3b2`.

`git diff` et contenu indexé examinés ; `git diff --check` et `git diff --cached --check` réussis. Aucun fichier hors périmètre/secrets ou artefact build indexé. Tests supervision .NET non relancés : aucun code .NET modifié ni risque d'interface supervision nouveau démontré.

## Erreurs rencontrées et corrections

Premier cycle : une restriction ajoutée aux paramètres SYNC contredisait le test existant de politique. Retrait de cette restriction dans la source ; aucun test existant ni assertion modifié/affaibli. SYNC utilise une source Modbus explicite, sans nouvelle interprétation des paramètres capturés.

Le nouveau test d'injection supposait initialement une réutilisation du runtime et un snapshot après défaut de commit. Lecture du backend borné : recovery_required interdit ces opérations. Fixture corrigée avec boot explicite après la première panne et assertion de refus de snapshot après la seconde ; preuves d'absence de dispatch/succès renforcées. La validation finale complète passe après ces corrections. Inspection finale : CRC invalide APPLY raccordé au code4, distinct du préparé absent code20, avec assertion et absence d'activation vérifiées.

Les outils locaux curl/pdftoppm n'étaient pas disponibles pour une inspection auxiliaire du PDF ; documentation constructeur consultée via outil web. Aucune installation/changement du poste effectué et aucune liaison VIO supposée à partir d'un simple libellé.

## Matériel, rectificatif et préparation des essais

`PREPARATION_RTU_PRODUCTION_005.md` fournit une matrice prouvé documentaire / à mesurer / inconnu, les références constructeur, procédure Windows PowerShell d'identification USB/pilote/COM et ouverture 115200/8E1 sans émission sur PC seul, puis jalons électrique/un TR2 et deux TR2/supervision. Toutes ces commandes série et manipulations sont **préparées, non exécutées** sous cette mission.

La [fiche MIKROE-3863](https://www.mikroe.com/rs485-isolator-2-click), son schéma v102 et Analog Devices désignent ADM2867E. Incohérence signalée puis levée par la correction utilisateur distante `463d69a` : **ADM2867E / MIKROE-3863 sur le TR2 actuel, deux ADM2867E au total et un ADM2587E sans carte support identifiée**. Inventaire rapporté, pas identification électrique observée par Codex. ADM2587E et ADM2867E distingués explicitement.

PG7/PG8 : UART existant ; PG4/PG5 : candidats seulement. VDDIO2, niveaux, DE//RE, duplex, jumpers, isolation/masses, terminaison/bias et sens A/B encore à qualifier sur le montage réel. Aucun adapter de direction activé : ses prérequis électriques ne sont pas établis malgré le rectificatif d'identité. Câble reçu jamais branché au moment de la déclaration ; second TR2 non présumé qualifié.

## Limites et prochaine étape

**Aucun flash, debug MCU, connexion, émission ou test physique.** Pas de qualification RS-485, exécution sur cible, précision temporelle/turnaround/écho ou validation multi-TR2. E4 microSD/power-loss reste ouverte.

L'activation de production nécessite un transport qualifié, backend de campagnes SD durable et allocation réelle, FRAM/géométrie provisionnées, adresse/B0 et métadonnées de configuration. Les fenêtres sacrificielles E3/E4 ne sont pas recyclées ; la cohabitation harness/production doit être arbitrée avant activation. SELFTEST/RESET et statistiques restent tributaires de leurs vraies autorités. Le raccordement livré est un incrément compilable/testable et documenté, pas une qualification industrielle complète.

Journal actualisé : progrès logiciel, résultats 104/104 + ARM, composition durable et capacités encore absentes, rectificatif d'inventaire, prérequis électriques, essais futurs et dette E4. À transmettre à ChatGPT : contrôle indépendant du rapport, puis mission PC câble seul, contrôle électrique/révision et qualification des autorités de production avant essais sur cible.

## Publication et preuve distante

Commit/push autorisés uniquement pour cette mission sur main ; commit code créé et vérifié. Push et relecture distante restent à effectuer. L'état local après commit code était propre, main en avance d'un commit ; rapport provisoire est la seule modification suivante. Ce paragraphe sera remplacé par les faits de publication/relecture avant DONE.
