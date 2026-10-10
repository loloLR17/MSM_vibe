---
mission_id: TR2-20261010-DEV-RTU-INTEGRATION-004
status: DONE
base_ref: main
base_sha: 142452b6171bc13f078eace64ae57e9e25565e3c
initial_head: 3f5706780b76707a3be339ac4fb424997d7e794b
result_sha: 92c75f6a387ae4b266cfb7a372d51987a4683f12
created_at_utc: 2026-10-10T15:11:05Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission 004 — chaîne RTU applicative et raccordement STM32 explicite

**DONE pour le périmètre portable utile : code intégré, tests réussis, publication et relecture distante établies au SHA `789cd50c12fc60bd0a26bd6d920c259eea60272b`.** La chaîne applicative est intégrée au build STM32 et raccordable depuis sa boucle principale ; le firmware livré reste en diagnostic RX par défaut, sans réponse applicative physique, car le binding de production/RS-485 n'est pas établi. Aucun résultat matériel n'est revendiqué.

## Références et préservation

Session native Debian, clone `/home/lolo/dev/msm/projects/MSM_vibe`, SDK Linux CubeU5 v1.9.0. État initial propre sur main `3f5706780b76707a3be339ac4fb424997d7e794b`. Fetch puis pull fast-forward vers la mission publiée `d04e580f8113b3a76f9122a64eb0106a4b1112c6`. Le base_sha du contrat `142452b6171bc13f078eace64ae57e9e25565e3c` est bien ancêtre du code utilisé. Stash historique 002 conservé. Aucun audit T1000, réinstallation, refonte des freezes ou travail E4.

Commit code/tests/plan/journal : `92c75f6a387ae4b266cfb7a372d51987a4683f12` — `Firmware: compose RTU application server with explicit STM32 binding`.

## Code intégré et décisions

- `05_Firmware/include/tr2/application/modbus_system_server.h` et `src/application/modbus_system_server.c` : composition empruntant un SystemRuntime booté, un transport, une adresse explicite 1..247, une identité B0 optionnelle et des autorités d'écriture optionnelles. Réutilisation complète des récepteurs/codecs/serveurs/adapters existants, aucune machine Modbus parallèle. Readiness contrôlée ; démarrage explicite et arrêt de la réception applicative si readiness perdue.
- `include/tr2/modbus/rtu_server_runtime.h`, `src/modbus/rtu_server_runtime.c` : port d'actualisation applicative après acceptation longueur/CRC/adresse, avant PDU. Évite les lectures d'autorités/journal par octet ou sur une trame à rejeter. Init remet ce port optionnel à NULL pour les compositions existantes.
- `include/tr2/modbus/pdu_server.h`, `src/modbus/pdu_server.c` : port synchrone de soumission B5, y compris identités invalides, et notification d'invalidation du workflow B4. Correction du classement des écritures : validation de l'adresse avant vérification d'autorité ; adresse valide avec autorité absente → exception 04, adresse interdite → 02.
- B0 : identité injectée, pas de device_id inventé. B1/B3/B7 : projections disponibles du runtime. B2 : autorité TimeService actuelle et staging existant. B4 : workflow fourni, staging et état projetés ; écriture invalide sa validation. B6 : même image autoritaire lue et actualisée par sélection. B5 : mailbox et snapshot moteur actuels, pas de perte silencieuse des soumissions dans cette composition ; écritures indisponibles sans gestionnaire.
- `platform/stm32/stm32_modbus_application.h/.c`, `platform/stm32/main.c`, CMake STM32 : hook de carte explicite, diagnostics de binding et erreurs, chemin init/start/poll du serveur applicatif. Le hook faible par défaut renvoie NOT_AVAILABLE et conserve le diagnostic RX 003. Un binding fourni doit posséder les dépendances durables et le transport half-duplex qualifié ; le UART brut ne suffit pas. Pas de mapping DE//RE supposé.
- `platform/host/rtu_request.c`, CMake firmware : outil hors ligne de génération hex et décodage fichier via le codec réel, sans ouverture série.
- `tests/integration/test_modbus_system_server.c` : nouvelle intégration avec SystemRuntime réellement booté sur HostPlatform, médias persistants hôte existants et transport d'événements simulé. `tests/unit/test_p12c_pdu_server.c` : couverture des autorités absentes B2/B4/B5/B6 et maintien du refus RO.
- `00_gouvernance/PLAN_ESSAI_RTU_INTEGRATION_004.md` : contrat de binding et plan d'essai RS-485 avec commandes, observations et critères. `ETAT_COURANT_TR2.md` actualisé pour les progrès et limites avérés.

L'adressage/broadcast, les fonctions FC03/FC16, exceptions et invariants transactionnels existants sont préservés. Aucun retry TX automatique n'est ajouté : un échec d'émission après exécution métier ne rejoue pas la commande. Les ports sont synchrones, hors ISR ; les objets empruntés doivent vivre pendant toute l'utilisation. Aucune garantie de latence/débit matériel n'est déduite du test.

## Validation effectivement exécutée

Validation complète choisie selon AGENTS.md §6 : le cœur et des interfaces partagées sont modifiés. Commande finale :

```bash
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  bash ./tr2_validate.sh
```

**Code retour 0 : 103/103 tests hôte, CROSS-BUILD VALIDATED.** Dernière suite hôte : 3,50 s. Pas de campagne supervision .NET ni essai matériel. GCC ARM 14.2.1, options de compilation Werror. Linkage : quatre avertissements newlib `_close`, `_lseek`, `_read`, `_write` non implémentés, conservés et non masqués.

Intégration vérifiée : lecture B1 et actualisation de vue, identité B0 absente/présente, CRC invalide sans effet, autre adresse sans effet, broadcast lecture ignoré/écriture exécutée sans réponse, fonction non supportée 01, adresse illégale 02, autorité absente 04, staging B2, staging/lecture B4 et invalidation du workflow, lecture/sélection B6, B5 jusqu'à l'exécuteur production REFRESH_INDICATORS et son journal, submit auto-clear, handoff d'ID 0/code 0 au gestionnaire, contrôle réservé atomique, échecs start/RX/TX, reprise après erreur, absence de replay automatique après TX échoué, perte de readiness et rejet des adresses locales 0/248.

Les cas ID 0/code 0 vérifient la transmission au gestionnaire d'une soumission invalide, **pas le refus fonctionnel complet en production** : le gestionnaire de la fixture retourne une indisponibilité sur ces cas. Le dispatcher universel B5 (dont refus code 14, annulation, commandes 1..11) n'est pas livré par cette tranche et doit être fourni avant activation des écritures B5 en production.

Premier cycle : 102/103 tests passaient ; le nouveau test révélait l'exception 02 erronée pour un mailbox absent. Diagnostic du test via GDB **hôte uniquement**, lecture réelle du dispatch, correction de production puis tests ciblés et suite complète réussis. La fixture d'adresse illégale utilise l'adresse inexistante 999 plutôt que le réservé 20, que le modèle permet en lecture. Aucun ancien test affaibli ; ajout de couvertures, pas de suppression d'assertion.

Outil hors ligne : build `tr2_rtu_request` réussi ; génération/décodage des cas b1/unsupported/other-address, rejet CRC corrompu, adresses 0/248/texte invalide refusées, fichier vide/surdimensionné refusé. Exemple b1 adresse 17 : `110303e8000106ea`. Aucune ouverture de port série. Tests ciblés exécutés via cmake/ctest pendant le diagnostic ; la dernière suite complète contient les tests finaux.

Artefacts non ajoutés à Git :

- BIN SHA256 `4479d4e257273308b9dbe0a82da912e93a7d91918b5ec165d06f2a57541c3c77`.
- ELF SHA256 `337b982313e8651ebbaf13dc602d3951e2ad40fa06461b19139bb83aa732ccd8`.
- ELF : text 95 180, data 124, bss 71 336 octets ; init/start/poll du serveur et PDU présents dans la table de symboles, hook de binding faible présent. Ceci prouve le linkage, pas l'exécution du chemin applicatif.
- Logs locaux temporaires : `/tmp/TR2-004-validation.log` (premier échec), `-validation-final.log` (cycle intermédiaire), `-validation-delivery.log` (validation finale), avec le préfixe `/tmp/TR2-004`.

Diff/index contrôlés, `git diff --check` et `git diff --cached --check` réussis. Seuls les 16 fichiers de tranche sont dans le commit fonctionnel ; aucun artefact/secret ajouté. État propre après commit.

## Limites et suite pour ChatGPT

**VALIDÉ logiciel** : chaîne portable et interfaces de composition, tests hôte, compilation/linkage STM32. **PRÉPARÉ** : raccordement et essai physique. **NON VÉRIFIÉ** : exécution STM32 de cette composition, UART/RS-485 réels, DE//RE, précision TIM6, turnaround et débit.

Le main de qualification possède encore des instances temporaires de périphériques/médias, pas un SystemRuntime de production durable. Leur transformation en boot production n'est pas effectuée par ce hook. Restent à fournir : boot/dépendances durables et environnement de validation réel, identité/adresse provisionnées, workflow/gestionnaire B5 nécessaires, puis transport DE//RE qualifié. PG4/PG5 sont des candidats, pas des affectations gelées. Domaine VDDIO2 et transceiver réel à confirmer : freeze historique ADM2587E versus plan Click ADM2867E. Les observations matérielles requises et commandes sont dans le plan publié.

E4 microSD/power-loss reste ouverte et distincte. Aucun flash, debug sur cible, câblage, microSD, effacement, Option Bytes ni configuration permanente modifiée. Les séquences de stockage présentes au boot restent inchangées ; ce nouvel artefact n'a pas été programmé.

Prochaine étape : contrôle indépendant du rapport par ChatGPT, raccordement des dépendances applicatives durables/gestionnaire B5, décision traçable adresse/transceiver/DE//RE/VDDIO2, puis qualification physique sous autorisations distinctes. Le livrable est le périmètre portable utile permis par la mission, pas une qualification du firmware industriel complet.

## Publication selon V2-B3

Permissions : commit/push exercés sur main pour cette tranche et son rapport, aucune permission matérielle exercée. Destination `origin HEAD:refs/heads/main`, push normal uniquement. Publication normale réussie de `d04e580f8113b3a76f9122a64eb0106a4b1112c6` à `789cd50c12fc60bd0a26bd6d920c259eea60272b` (code `92c75f6a387ae4b266cfb7a372d51987a4683f12` et rapport provisoire `789cd50c12fc60bd0a26bd6d920c259eea60272b` — `Report: publish RTU integration 004 results pending remote verification`).

Après `git fetch origin main`, SHA distant égal au HEAD local ; les 17 livrables ont été lus intégralement par `git show <SHA distant>:<chemin>` puis comparés octet par octet aux fichiers de travail, tous identiques. Arbre propre à cette étape, stash historique 002 conservé. Cette clôture DONE documente la preuve obtenue ; elle sera publiée puis contrôlée de la même manière, et son propre SHA fourni dans la réponse finale. Aucun statut DONE ne qualifie la réponse physique ou le raccordement production encore indisponible. Le contrôle indépendant ChatGPT reste à effectuer par ChatGPT.
