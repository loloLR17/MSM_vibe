---
mission_id: TR2-20261013-M015-U-INTEGRATION-TR2-001
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-U — Intégration et qualification FIFO IIS3DWB sur TR2-001

## Objectif et contexte normatif

Reprendre le développement sur le **TR2 original TR2-001**, et non sur la seconde NUCLEO expérimentale, pour intégrer au firmware STM32 principal le chemin d'acquisition FIFO IIS3DWB validé de manière bornée par M015-T (4096 XYZ + 512 timestamps, CPU160/SPI3 4 et 8 MHz). Poursuivre **M015 — acquisition vibratoire physique**, jalon J3 de `Modbus RTU/00_gouvernance/ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md`, sans anticiper ni déclarer M016–M020 accomplis. Cible : preuve d'acquisition physique de fenêtre V1 exploitable sur TR2-001, avec absence de régression démontrée de M014-H dans la portée testée. Ne pas déclarer validation industrielle, perte zéro globale, endurance ou métrologie calibrée.

Lire en priorité AGENTS.md, roadmap, ETAT_COURANT_TR2.md, spécifications/mapping Modbus V1, rapport M015-T et preuves brutes, état M014-H, instructions matérielles/flash et gouvernance d'échange. Vérifier HEAD distant réel de main, branche, état local, worktrees, stashes et sauvegardes. Ne pas partir d'archives obsolètes ; préserver les originaux. Archiver le contrat précédent avant remplacement suivant la procédure de gouvernance.

## Phase 0 — Préflight matériel obligatoire

L'utilisateur va **rebrancher physiquement TR2-001**. Cela ne constitue pas encore une preuve que la connexion est terminée, ni une autorisation de modifier son câblage. Codex doit attendre la disponibilité effective et identifier sans ambiguïté **TR2-001 par ses UID/SN documentés dans le dépôt et les lectures de cible**, sans réutiliser l'UID de la seconde NUCLEO M015-T (SN 0032001F3235510C37333439 / UID 001B00463842501820333157). Si TR2-001 est absent, incertain, si plusieurs sondes sont présentes ou si le montage ne correspond pas aux données connues : poursuivre les travaux logiciels possibles, mais **STOP avant flash/debug intrusif** et demander uniquement l'intervention physique indispensable.

Avant flash : vérifier identité, alimentation/liaisons observables sans manipulation, méthode ST-LINK qualifiée, état actuel de la cible, image M014-H, hash, sauvegarde Flash 2 Mio et données persistantes concernées selon AGENTS.md ; contrôler et consigner l'intégrité des sauvegardes. **FRAM génération 12, identité/provisioning, partition microSD (LBA 65536), état SD OFF et services Modbus sont à préserver.** Ne pas supposer que les valeurs historiques sont toujours celles de la carte connectée : mesurer sans écriture si nécessaire. Aucune écriture FRAM/SD, formatage, réinitialisation d'identité, effacement massif, changement Option Bytes/RDP, déplacement de jumper ou rebranchement automatisé. Si la protection ne peut être garantie, STOP matériel, documenter.

## Phase 1 — Audit de transposition (sans modification du matériel)

Comparer sources et artefacts de M015-T au firmware principal M014-H : driver **officiel ST IIS3DWB**, wrappers et corrections éventuelles, SPI3 (mode 3, CS PC9 si confirmé sur TR2-001), source d'horloge et diviseur réels, CPU/PLL/VOS/Flash, HAL SPI/RX/OVR, FIFO/tag/timestamp, buffers, IRQ, pile/RAM, délais, nettoyage et ressources partagées. Identifier explicitement quelles fonctions sont officielles, lesquelles sont des adaptations, et les bugs/contournements allégués ; ne pas présumer qu'un driver officiel est sans défaut ni que les anomalies antérieures sont dues à un driver maison. Préserver SPI1/FRAM, SDMMC2, UART/RS-485/Modbus, supervision et autorité des services M014. Réutiliser au maximum les composants qualifiés ; pas de réécriture générale, DMA, refactoring opportuniste, changement mapping V1 ou modifications SDMMC 4 bits.

Produire matrice de compatibilité, risques, mémoire/temps et plan de tests. Les configurations 4/8 MHz du banc ne se transposent PAS par simple valeur de diviseur : vérifier RCC/clocktree effectif. Choisir la configuration initiale la moins risquée justifiée par les preuves ; 4 MHz qualifié borné sur banc, 8 MHz alternative avec marge supérieure, aucun des deux qualifié automatiquement sur TR2-001.

## Phase 2 — Implémentation incrémentale et validations logicielles

Raccorder le chemin FIFO officiel ST + adaptations strictement nécessaires à un profil d'acquisition **borné** dans le firmware principal, sans compromettre le chemin M014-H. Prévoir états explicites READY/INVALID, restauration capteur et ressources, arrêt sûr, détection erreurs HAL/OVR/FULL/latch, niveau FIFO, parité/tags/compteurs, timestamps, saturation, continuité mesurable et absence de données fabriquées. Ne pas faire croire qu'un compteur modulo 4 prouve à lui seul zéro perte. Garantir que les données ne sont publiées comme valides qu'après contrôles.

Tests hôte avec faux périphérique, cas normaux et injectés (FIFO vide/partielle, timeout, OVR, tags erronés, discontinu, CS/clock, erreur cleanup, ressources occupées), builds ARM et validation complète `./tr2_validate.sh` si interfaces partagées touchées. Conserver et contrôler les acquis de M014-H, B0/B1 et perte/reprise ; pas de modification silencieuse de la V1, pas d'activation stockage ou FRAM/SD sans contrat spécifique. Commits atomiques, push sur main autorisés après tests et vérification des diffs.

## Phase 3 — Qualification physique progressive sur TR2-001

Après Phase 0 satisfaisante et compilation/validation, flasher uniquement **TR2-001 identifié**, image issue des sources du commit vérifié, BIN/ELF SHA256 consignés, programmation normale ST-LINK qualifiée avec verify/readback. Flash/debug explicitement autorisés ici, sans validation répétitive. Limiter l'effacement aux pages nécessaires, préserver données et image de retour. Tester progressivement : WHO/config/readbacks ; petite fenêtre FIFO si utile ; puis **une fenêtre de 4096 XYZ avec timestamps**, contrôles bruts wire↔RAM, ordre XYZ/TS, erreurs, saturation, niveaux, budget de service et cleanup. Toute répétition physique doit être justifiée par une incertitude mesurable et rester bornée ; pas de stress/endurance non prévu, ni progression automatique à 32768 si non démontrée.

Sur anomalie critique (mauvaise cible, désaccord d'identité, erreur d'intégrité sauvegarde, OVR, HAL timeout, défaut de cleanup, divergence de configuration, risque média/Modbus), STOP acquisitions, mettre le capteur dans l'état sûr si possible et restaurer le firmware M014-H **seulement par voie qualifiée et sans risque pour les données**, puis vérifier le retour B0/B1 si le banc PC est disponible. Ne pas masquer un échec ni modifier les guards pour obtenir un PASS. Si un blocage nécessite un geste physique, demander à l'utilisateur uniquement ce geste précis.

Après succès, tester la coexistence opérationnelle raisonnablement accessible avec M014-H (lecture Modbus B0/B1, état liaison, ressources, absence de régression), documenter ce qui reste non testé (FRAM, microSD, continu, FFT, stockage, endurance). Le retour à M014-H ou le maintien de l'image intégrée doit être **délibéré et justifié par preuves** ; ne pas laisser la cible dans un état ambigu. L'acquisition RAM bornée ne vaut pas à elle seule traitement et publication V1 (M016).

## Critères d'acceptation et décision de fin

1. Source exacte, historique Git, identités TR2-001/second banc, hashes image et sauvegardes tracés.
2. Audit ST/firmware M014-H, choix horloges/SPI et ressources documentés ; aucun changement hors périmètre.
3. Builds/tests réels publiés, régressions analysées et corrigées sans affaiblir tests/guards.
4. Si matériel disponible et préflight OK : capture physique bornée 4096 XYZ sur TR2-001, bruts et buffers comparés, 512 timestamps attendus selon configuration, métriques temporelles, flags, cleanup, absence d'erreur observée ; sinon statut **BLOCKED PHYSICAL** explicite avec travaux logiciels livrés.
5. Préservation vérifiée de M014-H, FRAM, SD, identité et services Modbus selon les moyens réellement disponibles ; absence de preuve = NON VÉRIFIÉ, pas PASS.
6. Conclusion motivée : M015 DONE **uniquement si les critères M015 de la roadmap sont réellement satisfaits**, sinon M015 IN PROGRESS avec lacunes, prochain plus petit pas, sans glissement implicite vers M016.

## Autorités, limites et publication

`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`. Ces autorisations couvrent corrections logicielles, tests, commits/push normaux et flash/debug **TR2-001 seulement** selon AGENTS.md. Ne pas accéder à la seconde NUCLEO pour un nouvel essai, sauf lecture de preuves existantes. Aucun mass erase, Option Bytes/RDP, changement de câblage, écriture persistante non autorisée, force push, reset destructif ou effacement d'archives. Les décisions structurantes ou risques non maîtrisés sont escaladés ; ne pas redemander les validations courantes.

Publier le code et les preuves dans le dépôt avec SHA/traçabilité, mettre à jour `DERNIER_RAPPORT.md`, `ETAT_COURANT_TR2.md` et la roadmap **uniquement à hauteur des preuves**, archiver le contrat précédent et les rapports conformément à la gouvernance. Vérifier à distance tous les commits et livrables publiés, rapporter les limitations et l'état final matériel. ChatGPT procédera ensuite à la revue indépendante. **Ne pas lancer M016 dans cette mission.**
