# État courant TR2 — référence reconstruite

Mise à jour : 10 octobre 2026. Mission `T1000-20261010-CLOTURE-001`.

**Ce document est un nouvel état de référence reconstruit à partir de preuves vérifiables. Il n'est pas une restitution du journal historique HP.** Sa création est explicitement autorisée par l'utilisateur pour cette mission, après escalade AGENTS.md §18. Aucun original n'a été retrouvé dans l'historique Git, les répertoires Linux locaux ou les documents Windows accessibles. Les archives d'échanges du 8 octobre mentionnent un journal local HP, sans en fournir le contenu. Aucun état historique inconnu n'est inventé.

## Référence et gouvernance

- Référence des sources au début de mission : `main`, `75eba6b431725b858db3ac5380884934026a1475`.
- AGENTS.md V2-B3 et `Echanges_Codex/PROTOCOLE_ECHANGE_V2.md` applicables ; les spécifications V1 et freezes restent inchangés.
- Autorisations de la présente mission : `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Elles expirent à la clôture. Aucun nouveau développement fonctionnel TR2.
- `Echanges_Codex/MISSION_EN_COURS.md` reste un document historique rédigé par ChatGPT ; le contrat courant est la demande utilisateur du 10 octobre et sa confirmation explicite de dérogation/reconstruction/permissions, pas cette ancienne mission.

## Résultats effectivement établis avant migration

- **VALIDÉ logiciel** : correctif autorisé du setup de `test_time_history_store.c`, `PersistentStorageCore storage = {0};`, sans changement de source de production ou d'assertion. Validation précédente : 102/102 tests firmware et cross-build ; supervision .NET restore/build et 466/466 tests.
- **VALIDÉ physiquement pour le poste** : mission distincte du 10 octobre, flash vérifié, démarrage, GDB, breakpoints matériels, reprise et reset reproductible sur TR2/ST-LINK CN1 `003500463235510B37333439`, STM32U575, Device ID `0x482`, Flash 2 Mio. MicroSD absente, selon confirmation utilisateur. Aucun mass erase/Option Bytes.
- Artefact réellement programmé : BIN 74 412 octets à `0x08000000`, SHA256 `5bb8d47bf370c99596b8e782611375f1ef086b121e11652618274925f376c0f1`. Référence de source ci-dessus. Rapport physique et sauvegardes Flash conservés hors Git dans le dossier de preuves T1000 du 10 octobre.
- **NON VÉRIFIÉ par ces missions** : qualification fonctionnelle complète TR2, stockage microSD/E4 power-loss, communication RS-485 réelle et debug source ligne par ligne. Les séquences E1/E4 peuvent écrire sur microSD au démarrage : toute mission physique future doit recontrôler les prérequis et autorisations.

## Environnement courant et mission de clôture

Clone actif migré dans Linux : `/home/lolo/dev/msm/projects/MSM_vibe`. SDK officiel CubeU5 v1.9.0 : `/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`. Scripts et procédures T1000 dans `00_Environnement_developpement/T1000/`.

L'historique Git et les modifications locales ont été sauvegardés puis conservés à l'identique lors du déplacement ; comparaison des empreintes de 3 408 fichiers et `git fsck` réussis. Aucun nouveau flash/debug n'est nécessaire pour cette migration.

**VALIDÉ depuis la nouvelle architecture** : 102/102 tests firmware et cross-build ARM ; .NET restore/build sans erreur ni warning et 466/466 tests. Après arrêt/redémarrage WSL : 102/102 et 466/466 à nouveau, outils et authentifications opérationnels, binfmt actif et aucun service en échec. Codex natif charge réellement AGENTS.md, compte ChatGPT, défaut read-only/on-request inchangé. Git/gh utilisent un jeton vérifié dans le coffre Windows, retiré du fichier en clair ; helper Git local, aucun paramètre global modifié.

BIN et ELF identiques à la qualification physique précédente ; seule la MAP change avec les chemins. Aucun nouveau flash/debug. Copies anciennes archivées et vérifiées avant nettoyage ; dossiers personnels et preuves préservés. Réserves complémentaires non bloquantes selon décision utilisateur : contrôles Windows administrateur/sécurité séparés ; extraction installateur Windows encore verrouillée, conservée sans arrêt forcé.

**VALIDÉ publication** : correctif de test `dc6dac73bbbdd19ecf921f84808065f816a5bc9c`, première publication globale `61b1217fd8d0a337fdbc07445c056061a7e93c1c` sur main, push normal puis relecture Codex de 39 fichiers et contrôle API du rapport/journal. Git propre. Les commits locaux initialement refusés pour email privé restent sauvegardés hors publication ; identité locale passée à noreply sans désactiver la protection GitHub. Le rapport de clôture est mis à jour après cette preuve ; son SHA final sera communiqué après vérification de la seconde publication.

## Prochaine étape

Clôture technique T1000 : VALIDÉ AVEC RÉSERVES complémentaires non bloquantes. Faire examiner le dossier de clôture et contrôler indépendamment le rapport distant par ChatGPT avant une nouvelle mission de développement fonctionnel MSM/TR2. Si l'original HP est retrouvé ultérieurement : relever sa provenance et son empreinte, comparer les faits et décisions à ce nouvel état, puis intégrer uniquement les éléments vérifiés sous mission autorisée ; ne pas remplacer silencieusement ce journal.
